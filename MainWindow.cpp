#include "MainWindow.hpp"
#include "NoLifeNx/bitmap.hpp"
#include "NoLifeNx/audio.hpp"
#include "config.hpp"
#include "nodeUtil.hpp"
#include "NodeItem.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QClipboard>
#include <QFileDialog>
#include <QFile>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QMenuBar>
#include <QHeaderView>
#include <QApplication>

NodeItem* addNode(const nl::node &node, QTreeWidgetItem *parent)
{
    auto item = new NodeItem(node);
    parent->addChild(item);
    return item;
}

QStringList getPath(QTreeWidgetItem *widgetItem)
{
    QStringList list;
    NodeItem* item;
    if (!(item = dynamic_cast<NodeItem*>(widgetItem)))
    {
        throw std::runtime_error("Item is not a NodeItem");
    }

    list.append(QString::fromStdString(item->node.name()));

     for (auto itm = item->parent(); itm; itm = itm->parent())
     {
         NodeItem* nitm;
         if (!(nitm = dynamic_cast<NodeItem*>(itm)))
         {
             throw std::runtime_error("Item is not a NodeItem");
         }
         list.prepend(QString::fromStdString(nitm->node.name()));
     }
     return list;
}

enum PathFormat {
    Slash,
    Array
};

QString getPathString(QTreeWidgetItem *widgetItem, PathFormat format)
{
    QString path;
    auto list = getPath(static_cast<NodeItem*>(widgetItem));

    if (format == Slash)
    {

        for (QString s : list)
        {
            path.append(s + '/');
        }

        // Remove last '/'
        path.remove(path.length() - 1, 1);
    }
    else if (format == Array)
    {
        for (QString s : list)
        {
            path.append("[\"" + s + "\"]");
        }
    }
    return path;
}

MainWindow::MainWindow(QWidget* parent_) :
    QMainWindow(parent_)
{
    setWindowTitle("NoLifeExplorer");
    m_file = nullptr;
    m_treeWidget = new QTreeWidget;
    m_treeWidget->setHeaderLabels({"Name", "Type", "Value"});
    setCentralWidget(new QWidget);
    centralWidget()->setLayout(new QVBoxLayout);
    centralWidget()->layout()->addWidget(m_treeWidget);
    m_audioPlayerWidget = new AudioPlayerWidget;
    centralWidget()->layout()->addWidget(m_audioPlayerWidget);
    QAction *action;
    m_fileMenu = new QMenu("&File");
    action = m_fileMenu->addAction("&Open...");
    connect(action, &QAction::triggered, [=]() {
        QString opendir;
        if (!m_recentFiles.empty())
            opendir = QFileInfo(m_recentFiles.first()).absoluteDir().path();
        QString filename = QFileDialog::getOpenFileName(this, "Select data file", opendir, "*.nx");
        if (!filename.isNull())
            openFromFile(filename);
    });
    m_recentFilesMenu = m_fileMenu->addMenu("&Recent files");
    m_recentFiles = m_settings.value("recentFiles").toStringList();
    updateRecentFilesList();
    action = m_fileMenu->addAction("&Quit");
    connect(action, &QAction::triggered, this, &QMainWindow::close);
    m_playbackMenu = new QMenu("&Playback");
    action = m_playbackMenu->addAction("&Loop");
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::triggered, m_audioPlayerWidget, &AudioPlayerWidget::toggleLoop);
    m_nodeMenu = new QMenu("&Node");
    m_nodeMenu->setEnabled(false);
    action = m_nodeMenu->addAction("Copy path (/)");
    connect(action, &QAction::triggered, this, &MainWindow::copyPath_slash);
    action = m_nodeMenu->addAction("Copy path ([])");
    connect(action, &QAction::triggered, this, &MainWindow::copyPath_array);
    action = m_nodeMenu->addAction("&Save to file...");
    connect(action, &QAction::triggered, this, &MainWindow::saveCurrentNodeToFile);
    m_helpMenu = new QMenu("&Help");
    action = m_helpMenu->addAction("&About NoLifeExplorer...");
    connect(action, &QAction::triggered, [=]() {
        QMessageBox::about(this, "About NoLifeExplorer", "NoLifeExplorer release " NOLIFEEXPLORER_RELEASE " \"" NOLIFEEXPLORER_CODENAME "\"");
    });
    action = m_helpMenu->addAction("About Qt...");
    connect(action, &QAction::triggered, [=]() {
        QMessageBox::aboutQt(this);
    });
    menuBar()->addMenu(m_fileMenu);
    menuBar()->addMenu(m_playbackMenu);
    menuBar()->addMenu(m_nodeMenu);
    menuBar()->addMenu(m_helpMenu);
    connect(m_treeWidget, &QTreeWidget::itemActivated, this, &MainWindow::handleItemActivated);
    connect(m_treeWidget, &QTreeWidget::itemExpanded, this, &MainWindow::handleItemExpanded);
    connect(m_treeWidget, &QTreeWidget::currentItemChanged, this, &MainWindow::handleCurrentItemChanged);
    m_treeWidget->setVisible(false);
    m_statusBarLabel = new QLabel;
    statusBar()->addWidget(m_statusBarLabel);
    m_audioPlayerWidget->hide();
    m_treeWidget->header()->resizeSection(0, 300);
    m_treeWidget->header()->resizeSection(1, 70);
}

MainWindow::~MainWindow()
{
    mpg123_exit();
    m_settings.setValue("recentFiles", m_recentFiles);
    delete m_file;
}

void MainWindow::openFromFile(QString filename)
{
    m_file = new nl::file(filename.toLocal8Bit().data());

    for (nl::node n : m_file->root())
    {
        auto item = addNode(n, m_treeWidget->invisibleRootItem());

        for (auto child : n)
        {
            addNode(child, item);
        }

        item->childrenAdded = true;
    }

    m_treeWidget->setVisible(true);
    m_recentFiles.prepend(filename);
    m_recentFiles.removeDuplicates();
    if (m_recentFiles.size() > 10)
        m_recentFiles.removeLast();
    updateRecentFilesList();
}

void MainWindow::handleCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem */*previous*/)
{
    m_statusBarLabel->setText(getPathString(current, Slash));
    m_nodeMenu->setEnabled(true);
}

void MainWindow::handleItemActivated(QTreeWidgetItem* widgetItem, int /*column*/)
{
    NodeItem* item;
    if (!(item = dynamic_cast<NodeItem*>(widgetItem)))
    {
        QMessageBox::critical(this, "Error", "Item is not a NodeItem. This is a bug.");
        return;
    }
    auto node = item->node;

    switch (node.data_type())
    {
    case nl::node::type::audio:
        try {
            m_audioPlayerWidget->play(item->node.get_audio());
        } catch (std::runtime_error &err) {
            QMessageBox::critical(this, "Error", tr("Error playing audio: %1").arg(err.what()));
            m_audioPlayerWidget->hide();
        }

        break;
    case nl::node::type::bitmap:
    {
        auto bm = node.get_bitmap();
        QImage image((uchar*)getBitmapData(node), bm.width(), bm.height(), QImage::Format_ARGB32);
        QPixmap pm = QPixmap::fromImage(image);
        QLabel *label = new QLabel;
        label->setPixmap(pm);
        label->show();
        break;
    }
    case nl::node::type::string:
    {
        QPlainTextEdit* edit = new QPlainTextEdit;
        edit->setReadOnly(true);
        edit->setPlainText(QString::fromStdString(node.get_string()));
        edit->show();
        break;
    }
    default:
        break;
    }
}

void MainWindow::handleItemExpanded(QTreeWidgetItem* widgetItem)
{
    auto item = static_cast<NodeItem*>(widgetItem);

    for (int i = 0; i < item->childCount(); ++i)
    {
        auto child = static_cast<NodeItem*>(item->child(i));

        if (!child->childrenAdded)
        {

            for (auto grandChildNode : child->node)
            {
                addNode(grandChildNode, child);
            }

            child->childrenAdded = true;
        }
    }
}

void MainWindow::copyPath_slash()
{
    QApplication::clipboard()->setText(getPathString(m_treeWidget->currentItem(), Slash));
}

void MainWindow::copyPath_array()
{
    QApplication::clipboard()->setText(getPathString(m_treeWidget->currentItem(), Array));
}

void MainWindow::saveCurrentNodeToFile()
{
    auto node = static_cast<NodeItem*>(m_treeWidget->currentItem())->node;
    
    if (node.data_type() == nl::node::type::none) {
        QMessageBox::information(this, "LOL WUT", "It doesn't make sense to save a node of type \"none\"");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(this, "Save " + nodeTypeAsString(node) + " to ");
    if (filename.isNull())
    {
        return;
    }
    
    switch (node.data_type()) {
    case nl::node::type::bitmap: {
        QImage image(static_cast<const uchar*>(getBitmapData(node)), node.get_bitmap().width(), node.get_bitmap().height(), QImage::Format_ARGB32);
        image.save(filename);
        break;
    }
    case nl::node::type::audio: {
        QFile f;
        f.setFileName(filename);
        f.open(QIODevice::WriteOnly);
        f.write(static_cast<const char*>(node.get_audio().data()), node.get_audio().length());
        break;
    }
    default: {
        QFile f;
        f.setFileName(filename);
        f.open(QIODevice::WriteOnly);
        f.write(nodeValueAsString(node).toUtf8());
    }
    }
}

void MainWindow::updateRecentFilesList()
{
    m_recentFilesMenu->clear();
    for (auto s : m_recentFiles) {
        QAction *action = m_recentFilesMenu->addAction(s);
        connect(action, &QAction::triggered, [=]() {
           openFromFile(s);
        });
    }
}
