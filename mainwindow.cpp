#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <sstream>
#include <QDebug>
#include <iomanip>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    file = nullptr;
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(handleItemClicked(QTreeWidgetItem*,int)));
    connect(ui->treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(handleItemExpanded(QTreeWidgetItem*)));
    ui->treeWidget->setVisible(false);
    ui->statusBar->addWidget(ui->soundPlayerWidget->nowPlaying);
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

void MainWindow::handleItemClicked(QTreeWidgetItem* widgetItem, int column)
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
