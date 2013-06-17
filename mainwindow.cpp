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

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
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
        handleNode(n, ui->treeWidget->invisibleRootItem());
    }

    ui->treeWidget->setVisible(true);
}

void MainWindow::handleCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    statusBarLabel->setText(getPathString(current, Slash));
}

void MainWindow::handleItemActivated(QTreeWidgetItem* widgetItem, int column)
{
    NodeItem* item;
    if (!(item = dynamic_cast<NodeItem*>(widgetItem)))
    {
        throw;
    }
    auto node = item->node;

    switch (node.T())
    {
    case NL::Node::audio:
        ui->soundPlayerWidget->play(*item);
        break;
    case NL::Node::bitmap:
    {
        auto bm = node.GetBitmap();
        QImage image((uchar*)bm.Data(), bm.Width(), bm.Height(), QImage::Format_ARGB32);
        QPixmap pm = QPixmap::fromImage(image);
        QLabel *label = new QLabel;
        label->setPixmap(pm);
        label->show();
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

        for (auto childNode : child->node)
        {
            resolveNode(childNode, child);
        }
    }
}

// Resolve node, + direct children
void MainWindow::handleNode(const NL::Node& node, QTreeWidgetItem* parent)
{
    auto item = resolveNode(node, parent);

    for (auto child : node)
    {
        resolveNode(child, item);
    }
}

NodeItem* MainWindow::resolveNode(const NL::Node &node, QTreeWidgetItem *parent)
{
    auto item = new NodeItem(node);
    parent->addChild(item);
    return item;
}

void MainWindow::on_actionCopy_path_triggered()
{
    QApplication::clipboard()->setText(getPathString(ui->treeWidget->currentItem(), Slash));
}

QStringList MainWindow::getPath(QTreeWidgetItem *widgetItem)
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

QString MainWindow::getPathString(QTreeWidgetItem *widgetItem, PathFormat format)
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

void MainWindow::on_actionCopy_path_NoLifeNx_triggered()
{
    QApplication::clipboard()->setText(getPathString(ui->treeWidget->currentItem(), Array));
}

void MainWindow::on_actionSave_to_file_triggered()
{
    auto node = static_cast<NodeItem*>(ui->treeWidget->currentItem())->node;

    QString type;
    const char* data = nullptr;
    int len = 0;

    switch (node.T())
    {
    case NL::Node::bitmap:
        type = "bitmap";
        data = static_cast<const char*>(node.GetBitmap().Data());
        len = node.GetBitmap().Length();
        break;
    case NL::Node::audio:
        type = "audio";
        data = static_cast<const char*>(node.GetAudio().Data());
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
        QFile file;
        file.setFileName(filename);
        file.open(QIODevice::WriteOnly);
        file.write(data, len);
        file.close();
    }
    else if (type == "bitmap")
    {
        QImage image((uchar*)data, node.GetBitmap().Width(), node.GetBitmap().Height(), QImage::Format_ARGB32);
        image.save(filename);
    }
}
