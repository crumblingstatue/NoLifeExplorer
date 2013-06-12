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
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleItemClicked(QModelIndex)));
    connect(ui->treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(handleItemExpanded(QModelIndex)));
    ui->treeView->setVisible(false);
    ui->statusBar->addWidget(ui->soundPlayerWidget->nowPlaying);
    ui->treeView->setModel(&model);
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
        handleNode(n, model.invisibleRootItem());
    }

    ui->treeView->setVisible(true);
    ui->treeView->setEnabled(true);
}

void MainWindow::handleItemClicked(QModelIndex index)
{
    auto item = static_cast<NodeItem*>(model.itemFromIndex(index));
    auto node = item->node;

    switch (node.T())
    {
    case NL::Node::ireal:
    case NL::Node::dreal:
    case NL::Node::string:
        qDebug() << QString::fromStdString(node.GetString());
        break;
    case NL::Node::vector:
        qDebug() << node.GetVector().first << ", " << node.GetVector().second;
        break;
    case NL::Node::bitmap:
        qDebug() << "Bitmap";
        break;
    case NL::Node::audio:
        ui->soundPlayerWidget->play(*item);
    default:
        break;
    }
}

void MainWindow::handleItemExpanded(QModelIndex index)
{
    auto item = static_cast<NodeItem*>(model.itemFromIndex(index));
    for (int i = 0; i < item->rowCount(); ++i)
    {
        auto child = static_cast<NodeItem*>(item->child(i));

        for (auto childNode : child->node)
        {
            resolveNode(childNode, child);
        }
    }
}

// Resolve node, + direct children
void MainWindow::handleNode(const NL::Node& node, QStandardItem* parent)
{
    auto item = resolveNode(node, parent);

    for (auto child : node)
    {
        resolveNode(child, item);
    }
}

QStandardItem* MainWindow::resolveNode(const NL::Node &node, QStandardItem *parent)
{
    QStandardItem* item = new NodeItem(node);
    parent->appendRow(item);
    return item;
}
