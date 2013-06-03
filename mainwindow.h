#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <mpg123.h>
#include <SFML/Audio.hpp>

#include "NoLifeNx/NX.hpp"

struct SoundItem : public QStandardItem {
    SoundItem(const QString& text) :
        QStandardItem(text) {}
    u_int32_t length;
    const void* data;
};

class Sound : public sf::SoundStream {
public:
    Sound() {
        if (mpg123_init() != MPG123_OK)
        {
            throw;
        }
        handle = mpg123_new(nullptr, nullptr);
        if (!handle)
        {
            throw;
        }
    }

    ~Sound() {
        mpg123_delete(handle);
    }

    void open(const SoundItem& item) {
        stop();
        if (mpg123_close(handle) != MPG123_OK)
        {
            throw;
        }
        if (mpg123_open_feed(handle) != MPG123_OK)
        {
            throw;
        }
        if (mpg123_feed(handle, (const unsigned char*)item.data, item.length) != MPG123_OK)
        {
            throw;
        }
        long rate = 0;
        int channels = 0, encoding  = 0;
        if (mpg123_getformat(handle, &rate, &channels, &encoding) != MPG123_OK)
        {
            throw;
        }
        buf.resize(mpg123_outblock(handle));
        initialize(channels, rate);
    }
private:

    bool onGetData(Chunk &data)
    {
        size_t done;
        mpg123_read(handle, buf.data(), buf.size(), &done);
        data.samples = (sf::Int16 *)buf.data();
        data.sampleCount = done / sizeof(sf::Int16);
        return data.sampleCount > 0;
    }

    void onSeek(sf::Time timeOffset)
    {
        mpg123_seek(handle, timeOffset.asSeconds(), SEEK_SET);
    }

    mpg123_handle* handle = nullptr;
    std::vector<unsigned char> buf;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_action_Open_triggered();
    void handleItemClicked(QModelIndex index);

private:
    void handleNode(const NL::Node& node, QStandardItem *parent = nullptr);
    Ui::MainWindow *ui;
    NL::File* file;
    QStandardItemModel model;
    Sound sound;
};

#endif // MAINWINDOW_H
