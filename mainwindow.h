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
    void handleCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void handleItemActivated(QTreeWidgetItem *widgetItem, int column);
    void handleItemExpanded(QTreeWidgetItem *widgetItem);

    void on_actionCopy_path_triggered();

    void on_actionCopy_path_NoLifeNx_triggered();

    void on_actionSave_to_file_triggered();

private:
    Ui::MainWindow* ui;
    NL::File* file;
    bool m_looping = true;
    QLabel *statusBarLabel;
};

#endif // MAINWINDOW_H
