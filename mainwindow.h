#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>
#include <QLabel>

#include "soundplayerwidget.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();
    void handleItemClicked(QModelIndex index);
    void handleItemExpanded(QModelIndex index);

private:
    void handleNode(const NL::Node& node, QStandardItem* parent);
    QStandardItem *resolveNode(const NL::Node& node, QStandardItem* parent);
    Ui::MainWindow* ui;
    NL::File* file;
    QStandardItemModel model;
    bool m_looping = true;
};

#endif // MAINWINDOW_H
