#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "NoLifeNx/file.hpp"
#include "AudioPlayerWidget.hpp"
#include <QMainWindow>
#include <QLabel>
#include <QTreeWidget>
#include <QMenu>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent_ = 0);
    ~MainWindow();

private slots:
    void open();
    void handleCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void handleItemActivated(QTreeWidgetItem *widgetItem, int column);
    void handleItemExpanded(QTreeWidgetItem *widgetItem);
    void copyPath_slash();
    void copyPath_array();
    void saveCurrentNodeToFile();

private:
    Ui::MainWindow* ui;
    nl::file* file;
    bool m_looping = true;
    QLabel *statusBarLabel;
    QTreeWidget *m_treeWidget;
    AudioPlayerWidget *m_audioPlayerWidget;
    QMenu *m_fileMenu, *m_playbackMenu, *m_nodeMenu;
};

#endif // MAINWINDOW_HPP
