#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    file = nullptr;
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleItemClicked(QModelIndex)));
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
    NL::Node sounds = file->Base()["Sound"];
    for (NL::Node n : sounds)
    {
        handleNode(n);
    }

}

void MainWindow::handleItemClicked(QModelIndex index)
{
    if (auto* item = dynamic_cast<SoundItem*>(model.itemFromIndex(index)))
    {
        sound.open(*item);
        sound.play();
    }
}

void MainWindow::handleNode(const NL::Node &node, QStandardItem* parent)
{
    auto parentItem = (parent? parent : model.invisibleRootItem());
    switch (node.T())
    {
    case NL::Node::none:
    {
        QStandardItem* item = new QStandardItem(QString::fromStdString(node.Name()));
        parentItem->appendRow(item);
        for (NL::Node n : node)
        {
            handleNode(n, item);
        }
        break;
    }
    case NL::Node::audio:
    {
        auto item = new SoundItem(QString::fromStdString(node.Name()));
        item->length = node.GetAudio().Length();
        item->data = node.GetAudio().Data();
        parentItem->appendRow(item);
        break;
    }
    default:
        throw;
    }

    ui->treeView->setModel(&model);
}
