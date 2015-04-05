#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QHBoxLayout>
#include <QMouseEvent>

#include "AudioStream.hpp"

// "Stolen" from http://stackoverflow.com/a/11133022
class StreamSeekSlider : public QSlider {
    using QSlider::QSlider;

protected:
    void mousePressEvent(QMouseEvent * event_) {
        if (event_->button() == Qt::LeftButton) {
            if (orientation() == Qt::Vertical) {
                auto value_ =
                    minimum() +
                    ((maximum() - minimum()) * (height() - event_->y())) /
                        height();
                setValue(value_);
                emit sliderMoved(value_);
            } else {
                auto value_ = minimum() +
                              ((maximum() - minimum()) * event_->x()) / width();
                setValue(value_);
                emit sliderMoved(value_);
            }

            event_->accept();
        }
    }
};

class AudioPlayerWidget : public QWidget {
    Q_OBJECT
public:
    explicit AudioPlayerWidget(QWidget * parent = 0);
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
    StreamSeekSlider * m_slider{nullptr};
    QLabel * m_label{nullptr};
    QPushButton * m_playButton{nullptr};
    QPushButton * m_pauseButton{nullptr};
    AudioStream m_audioStream;
    bool m_stopped{true};
    QTimer * m_timer{nullptr};
    QHBoxLayout * m_layout{nullptr};
};
