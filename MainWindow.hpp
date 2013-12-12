#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "NoLifeNx/file.hpp"
#include "AudioPlayerWidget.hpp"
#include <QMainWindow>
#include <QLabel>
#include <QTreeWidget>
#include <QMenu>
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent_ = 0);
    ~MainWindow();

private slots:
    void openFromFile(QString filename);
    void handleCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    void handleItemActivated(QTreeWidgetItem *widgetItem, int column);
    void handleItemExpanded(QTreeWidgetItem *widgetItem);
    void copyPath_slash();
    void copyPath_array();
    void saveCurrentNodeToFile();
    void updateRecentFilesList();

private:
    nl::file *m_file;
    bool m_looping = true;
    QLabel *m_statusBarLabel;
    QTreeWidget *m_treeWidget;
    AudioPlayerWidget *m_audioPlayerWidget;
    QMenu *m_fileMenu, *m_playbackMenu, *m_nodeMenu, *m_helpMenu, *m_recentFilesMenu;
    QStringList m_recentFiles;
    QSettings m_settings;
};

#endif // MAINWINDOW_HPP
