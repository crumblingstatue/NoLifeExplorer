#ifndef SOUNDPLAYERWIDGET_H
#define SOUNDPLAYERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QHBoxLayout>

#include "Sound.h"


class SoundPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SoundPlayerWidget(QWidget *parent = 0);
    void play(const NodeItem& item);

private slots:
    void play();
    void on_actionLoop_toggled(bool arg1);

    void updateTimeInfo();
    void seek(int where);
    void stop();

    void onStopClicked();
    void onPauseClicked();
private:
    bool stopped();
    QSlider* slider;
    QLabel* label;
    QPushButton* playButton;
    QPushButton* pauseButton;
    Sound sound;
    bool m_stopped = true;
    QString currentItemTitle;
    QTimer *timer;
    QHBoxLayout* layout;
};

#endif // SOUNDPLAYERWIDGET_H
