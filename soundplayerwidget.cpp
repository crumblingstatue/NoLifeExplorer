#include "soundplayerwidget.h"

#include <sstream>
#include <iomanip>

SoundPlayerWidget::SoundPlayerWidget(QWidget *parent) :
    QWidget(parent)
{
    slider = new QSlider(Qt::Horizontal);
    playButton = new QPushButton("Play");
    pauseButton = new QPushButton("Pause");
    nowPlaying = new QLabel("NoLifeExplorer version 1.2");
    timer = new QTimer(this);
    label = new QLabel("Wot");
    layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(slider);
    layout->addWidget(pauseButton);
    layout->addWidget(playButton);
    setLayout(layout);

    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimeInfo()));
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(onPauseClicked()));
    connect(playButton, SIGNAL(clicked()), this, SLOT(onStopClicked()));

    sound.setLoop(true);
    timer->setInterval(250);
}

void SoundPlayerWidget::play(const NodeItem &item)
{
    sound.open(item.node.GetAudio());
    play();
}

void SoundPlayerWidget::on_actionLoop_toggled(bool arg1)
{
    sound.setLoop(arg1);
}

void SoundPlayerWidget::updateTimeInfo()
{
    // Check if sound is stopped
    if (sound.getStatus() == sf::SoundStream::Stopped)
    {
        stop();
    }
    int playingOffset = sound.getPlayingOffset().asSeconds();
    int length = sound.lengthTime.asSeconds();
    int oMinutes = playingOffset / 60;
    int oSeconds = playingOffset % 60;
    int lMinutes = length / 60;
    int lSeconds = length % 60;
    std::ostringstream ss;
    ss << std::setfill('0');
    ss << std::setw(2) << oMinutes << ':' << std::setw(2) << oSeconds << " / " << std::setw(2) << lMinutes << ':' << std::setw(2) << lSeconds;
    label->setText(QString::fromStdString(ss.str()));
    slider->setValue(sound.getPlayingOffset().asMilliseconds());
}

void SoundPlayerWidget::seek(int where)
{
    sound.setPlayingOffset(sf::milliseconds(where));
}

void SoundPlayerWidget::stop()
{
    slider->setEnabled(false);
    pauseButton->setEnabled(false);
    playButton->setText("Play");
    pauseButton->setText("Pause");
    m_stopped = true;
    nowPlaying->setText("Stopped " + currentItemTitle);
}

void SoundPlayerWidget::onStopClicked()
{
    if (!stopped())
    {
        sound.stop();
        stop();
    }
    else
    {
        play();
    }
}

bool SoundPlayerWidget::stopped()
{
    return m_stopped;
}


void SoundPlayerWidget::play()
{
    sound.play();
    playButton->setEnabled(true);
    pauseButton->setEnabled(true);
    slider->setEnabled(true);
    label->setEnabled(true);
    slider->setMaximum(sound.lengthTime.asMilliseconds());
    updateTimeInfo();
    timer->start();
    nowPlaying->setText("Now playing " + currentItemTitle);
    m_stopped = false;
    playButton->setText("Stop");
}

void SoundPlayerWidget::onPauseClicked()
{
    if (sound.getStatus() == sf::SoundStream::Paused)
    {
        sound.play();
        slider->setEnabled(true);
        pauseButton->setText("Pause");
        nowPlaying->setText("Now playing " + currentItemTitle);
    }
    else
    {
        sound.pause();
        slider->setEnabled(false);
        pauseButton->setText("Resume");
        nowPlaying->setText("Paused " + currentItemTitle);
    }
}
