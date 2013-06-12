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
    sound.setLoop(true);
    ui->treeView->setVisible(false);
    timer = new QTimer;
    timer->setInterval(250);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeInfo()));
    nowPlaying = new QLabel("NoLifeExplorer version 1.2");
    ui->statusBar->addWidget(nowPlaying);
    connect(ui->horizontalSlider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
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

    ui->treeView->setVisible(true);
    ui->treeView->setEnabled(true);
}

void MainWindow::handleItemClicked(QModelIndex index)
{
    if (auto* item = dynamic_cast<SoundItem*>(model.itemFromIndex(index)))
    {
        sound.open(*item);
        currentItemTitle = item->text();
        play();
    }
}

void MainWindow::handleNode(const NL::Node& node, QStandardItem* parent)
{
    auto parentItem = (parent ? parent : model.invisibleRootItem());

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

void MainWindow::on_actionLoop_toggled(bool arg1)
{
    sound.setLoop(arg1);
}

void MainWindow::updateTimeInfo()
{
    // Check if sound is stopped
    if (sound.getStatus() == sf::SoundStream::Stopped)
    {
        stop();
    }
    int playingOffset = sound.getPlayingOffset().asSeconds();
    int length = sound.lengthTime.asSeconds();
    int oMinutes = playingOffset / 60;
    int oSeconds = playingOffset % 60;
    int lMinutes = length / 60;
    int lSeconds = length % 60;
    std::ostringstream ss;
    ss << std::setfill('0');
    ss << std::setw(2) << oMinutes << ':' << std::setw(2) << oSeconds << " / " << std::setw(2) << lMinutes << ':' << std::setw(2) << lSeconds;
    ui->label->setText(QString::fromStdString(ss.str()));
    ui->horizontalSlider->setValue(sound.getPlayingOffset().asMilliseconds());
}

void MainWindow::seek(int where)
{
    sound.setPlayingOffset(sf::milliseconds(where));
}

void MainWindow::stop()
{
    ui->horizontalSlider->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton->setText("Play");
    ui->pushButton_2->setText("Pause");
    m_stopped = true;
    nowPlaying->setText("Stopped " + currentItemTitle);
}

void MainWindow::on_pushButton_clicked()
{
    if (!stopped())
    {
        sound.stop();
        stop();
    }
    else
    {
        play();
    }
}

bool MainWindow::stopped()
{
    return m_stopped;
}


void MainWindow::play()
{
    sound.play();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
    ui->label->setEnabled(true);
    ui->horizontalSlider->setMaximum(sound.lengthTime.asMilliseconds());
    updateTimeInfo();
    timer->start();
    nowPlaying->setText("Now playing " + currentItemTitle);
    m_stopped = false;
    ui->pushButton->setText("Stop");
}

void MainWindow::togglePaused()
{
    if (sound.getStatus() == sf::SoundStream::Paused)
    {
        sound.play();
        ui->horizontalSlider->setEnabled(true);
        ui->pushButton_2->setText("Pause");
        nowPlaying->setText("Now playing " + currentItemTitle);
    }
    else
    {
        sound.pause();
        ui->horizontalSlider->setEnabled(false);
        ui->pushButton_2->setText("Resume");
        nowPlaying->setText("Paused " + currentItemTitle);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    togglePaused();
}
