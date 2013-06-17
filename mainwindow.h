#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void handleItemActivated(QTreeWidgetItem *widgetItem, int column);
    void handleItemExpanded(QTreeWidgetItem *widgetItem);

private:
    void handleNode(const NL::Node& node, QTreeWidgetItem *parent);
    NodeItem *resolveNode(const NL::Node& node, QTreeWidgetItem *parent);
    Ui::MainWindow* ui;
    NL::File* file;
    bool m_looping = true;
};

#endif // MAINWINDOW_H
