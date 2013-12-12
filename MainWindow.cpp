#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "NoLifeNx/bitmap.hpp"
#include "NoLifeNx/audio.hpp"
#include "config.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QClipboard>
#include <QFileDialog>
#include <QFile>
#include <QPlainTextEdit>
#include <QStatusBar>
#include <QMenuBar>

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
    QMainWindow(parent_),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    file = nullptr;
    m_treeWidget = new QTreeWidget;
    m_treeWidget->setHeaderLabels({"Name", "Type", "Value"});
    ui->centralWidget->layout()->addWidget(m_treeWidget);
    m_audioPlayerWidget = new AudioPlayerWidget;
    ui->centralWidget->layout()->addWidget(m_audioPlayerWidget);
    QAction *action;
    m_fileMenu = new QMenu("&File");
    action = m_fileMenu->addAction("&Open...");
    connect(action, &QAction::triggered, this, &MainWindow::open);
    m_playbackMenu = new QMenu("&Playback");
    action = m_playbackMenu->addAction("&Loop");
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::triggered, m_audioPlayerWidget, &AudioPlayerWidget::toggleLoop);
    m_nodeMenu = new QMenu("&Node");
    action = m_nodeMenu->addAction("Copy path (/)");
    connect(action, &QAction::triggered, this, &MainWindow::copyPath_slash);
    action = m_nodeMenu->addAction("Copy path ([])");
    connect(action, &QAction::triggered, this, &MainWindow::copyPath_array);
    menuBar()->addMenu(m_fileMenu);
    menuBar()->addMenu(m_playbackMenu);
    menuBar()->addMenu(m_nodeMenu);
    connect(m_treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(handleItemActivated(QTreeWidgetItem*,int)));
    connect(m_treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(handleItemExpanded(QTreeWidgetItem*)));
    connect(m_treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(handleCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    m_treeWidget->setVisible(false);
    statusBarLabel = new QLabel("NoLifeExplorer release " NOLIFEEXPLORER_RELEASE " \"" NOLIFEEXPLORER_CODENAME "\"");
    statusBar()->addWidget(statusBarLabel);
    m_audioPlayerWidget->hide();
    m_treeWidget->header()->resizeSection(0, 300);
    m_treeWidget->header()->resizeSection(1, 70);
}

MainWindow::~MainWindow()
{
    mpg123_exit();
    delete file;
    delete ui;
}

void MainWindow::open()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select data file", QString(), "*.nx");

    if (filename.isNull())
    {
        return;
    }

    file = new nl::file(filename.toLocal8Bit().data());

    for (nl::node n : file->root())
    {
        auto item = addNode(n, m_treeWidget->invisibleRootItem());

        for (auto child : n)
        {
            addNode(child, item);
        }

        item->childrenAdded = true;
    }

    m_treeWidget->setVisible(true);
}

void MainWindow::handleCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem */*previous*/)
{
    statusBarLabel->setText(getPathString(current, Slash));
    m_nodeMenu->setEnabled(true);
}

static const void* getBitmapData(nl::node n)
{
    if (n["source"]) {
        std::string src = n["source"];
        n = n.root().resolve(src.substr(src.find_first_of('/') + 1));
    }

    return n.get_bitmap().data();
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
            m_audioPlayerWidget->play(*item);
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

    QString type;
    const char* data_ = nullptr;
    int len = 0;

    switch (node.data_type())
    {
    case nl::node::type::bitmap:
        type = "bitmap";
        data_ = static_cast<const char*>(getBitmapData(node));
        len = node.get_bitmap().length();
        break;
    case nl::node::type::audio:
        type = "audio";
        data_ = static_cast<const char*>(node.get_audio().data());
        len = node.get_audio().length();
        break;
    default:
        return;
    }

    auto filename = QFileDialog::getSaveFileName(this, "Save " + type + " to ");
    if (filename.isNull())
    {
        return;
    }

    if (type == "audio")
    {
        QFile f;
        f.setFileName(filename);
        f.open(QIODevice::WriteOnly);
        f.write(data_, len);
        f.close();
    }
    else if (type == "bitmap")
    {
        QImage image((uchar*)data_, node.get_bitmap().width(), node.get_bitmap().height(), QImage::Format_ARGB32);
        image.save(filename);
    }
}
