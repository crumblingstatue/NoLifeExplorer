#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <sstream>
#include <QDebug>
#include <iomanip>
#include <QClipboard>
#include <QFileDialog>
#include <QFile>
#include <QPlainTextEdit>

NodeItem* addNode(const NL::Node &node, QTreeWidgetItem *parent)
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
        throw;
    }

    list.append(QString::fromStdString(item->node.Name()));

     for (auto itm = item->parent(); itm; itm = itm->parent())
     {
         NodeItem* nitm;
         if (!(nitm = dynamic_cast<NodeItem*>(itm)))
         {
             throw;
         }
         list.prepend(QString::fromStdString(nitm->node.Name()));
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
    connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(handleItemActivated(QTreeWidgetItem*,int)));
    connect(ui->treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(handleItemExpanded(QTreeWidgetItem*)));
    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(handleCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    ui->treeWidget->setVisible(false);
    statusBarLabel = new QLabel("NoLifeExplorer v0.85");
    ui->statusBar->addWidget(statusBarLabel);
    ui->soundPlayerWidget->hide();
    ui->treeWidget->header()->resizeSection(0, 300);
    ui->treeWidget->header()->resizeSection(1, 70);
}

MainWindow::~MainWindow()
{
    mpg123_exit();
    delete file;
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select data file", QString(), "*.nx");

    if (filename.isNull())
    {
        return;
    }

    file = new NL::File(filename.toLocal8Bit().data());

    for (NL::Node n : file->Base())
    {
        auto item = addNode(n, ui->treeWidget->invisibleRootItem());

        for (auto child : n)
        {
            addNode(child, item);
        }

        item->childrenAdded = true;
    }

    ui->treeWidget->setVisible(true);
}

void MainWindow::handleCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem */*previous*/)
{
    statusBarLabel->setText(getPathString(current, Slash));
    ui->menuNode->setEnabled(true);
}

void MainWindow::handleItemActivated(QTreeWidgetItem* widgetItem, int /*column*/)
{
    NodeItem* item;
    if (!(item = dynamic_cast<NodeItem*>(widgetItem)))
    {
        throw;
    }
    auto node = item->node;

    switch (node.T())
    {
    case NL::Node::Type::Audio:
        ui->soundPlayerWidget->play(*item);
        break;
    case NL::Node::Type::Bitmap:
    {
        auto bm = node.GetBitmap();
        QImage image((uchar*)bm.Data(), bm.Width(), bm.Height(), QImage::Format_ARGB32);
        QPixmap pm = QPixmap::fromImage(image);
        QLabel *label = new QLabel;
        label->setPixmap(pm);
        label->show();
        break;
    }
    case NL::Node::Type::String:
    {
        QPlainTextEdit* edit = new QPlainTextEdit;
        edit->setReadOnly(true);
        edit->setPlainText(QString::fromStdString(node.GetString()));
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

void MainWindow::on_actionCopy_path_triggered()
{
    QApplication::clipboard()->setText(getPathString(ui->treeWidget->currentItem(), Slash));
}

void MainWindow::on_actionCopy_path_NoLifeNx_triggered()
{
    QApplication::clipboard()->setText(getPathString(ui->treeWidget->currentItem(), Array));
}

void MainWindow::on_actionSave_to_file_triggered()
{
    auto node = static_cast<NodeItem*>(ui->treeWidget->currentItem())->node;

    QString type;
    const char* data_ = nullptr;
    int len = 0;

    switch (node.T())
    {
    case NL::Node::Type::Bitmap:
        type = "bitmap";
        data_ = static_cast<const char*>(node.GetBitmap().Data());
        len = node.GetBitmap().Length();
        break;
    case NL::Node::Type::Audio:
        type = "audio";
        data_ = static_cast<const char*>(node.GetAudio().Data());
        len = node.GetAudio().Length();
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
        QImage image((uchar*)data_, node.GetBitmap().Width(), node.GetBitmap().Height(), QImage::Format_ARGB32);
        image.save(filename);
    }
}
