#ifndef AUDIOPLAYERWIDGET_HPP
#define AUDIOPLAYERWIDGET_HPP

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QHBoxLayout>

#include "AudioStream.hpp"
#include "NodeItem.hpp"

class AudioPlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AudioPlayerWidget(QWidget *parent = 0);
    void play(const NodeItem& item);

public slots:
    void toggleLoop(bool arg1);
private slots:
    void play();

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
    AudioStream sound;
    bool m_stopped = true;
    QString currentItemTitle;
    QTimer *timer;
    QHBoxLayout* layout;
};

#endif // AUDIOPLAYERWIDGET_HPP
