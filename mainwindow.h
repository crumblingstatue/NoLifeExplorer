#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <bass.h>

#include "NoLifeNx/NX.hpp"

struct SoundItem : public QStandardItem {
    SoundItem(const QString& text) :
        QStandardItem(text) {}
    u_int32_t length;
    const void* data;
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
    DWORD playing = 0;
};

#endif // MAINWINDOW_H
