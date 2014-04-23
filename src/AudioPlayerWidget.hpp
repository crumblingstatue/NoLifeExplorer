#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QHBoxLayout>

#include "AudioStream.hpp"

class AudioPlayerWidget : public QWidget {
    Q_OBJECT
public:
    explicit AudioPlayerWidget(QWidget* parent = 0);
    void play(nl::audio audio);
    void setLoop(bool arg1);

private:
    void play();

    void updateTimeInfo();
    void seek(int where);
    void stop();

    void onStopClicked();
    void onPauseClicked();
    bool stopped();
    QSlider* m_slider;
    QLabel* m_label;
    QPushButton* m_playButton;
    QPushButton* m_pauseButton;
    AudioStream m_audioStream;
    bool m_stopped = true;
    QTimer* m_timer;
    QHBoxLayout* m_layout;
};
