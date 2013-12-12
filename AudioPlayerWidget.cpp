#include "AudioPlayerWidget.hpp"
#include "NoLifeNx/audio.hpp"

#include <sstream>
#include <iomanip>

AudioPlayerWidget::AudioPlayerWidget(QWidget *parent_) :
    QWidget(parent_)
{
    slider = new QSlider(Qt::Horizontal);
    playButton = new QPushButton("Play");
    pauseButton = new QPushButton("Pause");
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

void AudioPlayerWidget::play(const NodeItem &item)
{
    sound.open(item.node.get_audio());
    show();
    play();
}

void AudioPlayerWidget::toggleLoop(bool arg1)
{
    sound.setLoop(arg1);
}

void AudioPlayerWidget::updateTimeInfo()
{
    // Check if sound is stopped
    if (sound.getStatus() == sf::SoundStream::Stopped)
    {
        stop();
    }
    const int playingOffset = sound.getPlayingOffset().asSeconds();
    const int length = sound.lengthTime.asSeconds();
    const int oMinutes = playingOffset / 60;
    const int oSeconds = playingOffset % 60;
    const int lMinutes = length / 60;
    const int lSeconds = length % 60;
    std::ostringstream ss;
    ss << std::setfill('0');
    ss << std::setw(2) << oMinutes << ':' << std::setw(2) << oSeconds << " / " << std::setw(2) << lMinutes << ':' << std::setw(2) << lSeconds;
    label->setText(QString::fromStdString(ss.str()));
    slider->setValue(sound.getPlayingOffset().asMilliseconds());
}

void AudioPlayerWidget::seek(int where)
{
    sound.setPlayingOffset(sf::milliseconds(where));
}

void AudioPlayerWidget::stop()
{
    slider->setEnabled(false);
    pauseButton->setEnabled(false);
    playButton->setText("Play");
    pauseButton->setText("Pause");
    m_stopped = true;
}

void AudioPlayerWidget::onStopClicked()
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

bool AudioPlayerWidget::stopped()
{
    return m_stopped;
}

void AudioPlayerWidget::play()
{
    sound.play();
    playButton->setEnabled(true);
    pauseButton->setEnabled(true);
    slider->setEnabled(true);
    label->setEnabled(true);
    slider->setMaximum(sound.lengthTime.asMilliseconds());
    updateTimeInfo();
    timer->start();
    m_stopped = false;
    playButton->setText("Stop");
}

void AudioPlayerWidget::onPauseClicked()
{
    if (sound.getStatus() == sf::SoundStream::Paused)
    {
        sound.play();
        slider->setEnabled(true);
        pauseButton->setText("Pause");
    }
    else
    {
        sound.pause();
        slider->setEnabled(false);
        pauseButton->setText("Resume");
    }
}
