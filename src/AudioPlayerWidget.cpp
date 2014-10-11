#include "AudioPlayerWidget.hpp"

#include <sstream>
#include <iomanip>
#include <boost/numeric/conversion/cast.hpp>

using boost::numeric_cast;

AudioPlayerWidget::AudioPlayerWidget(QWidget * parent_) : QWidget(parent_) {
    m_slider = new StreamSeekSlider(Qt::Horizontal);
    m_playButton = new QPushButton("Play");
    m_pauseButton = new QPushButton("Pause");
    m_timer = new QTimer(this);
    m_label = new QLabel("Wot");
    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_slider);
    m_layout->addWidget(m_pauseButton);
    m_layout->addWidget(m_playButton);
    setLayout(m_layout);

    connect(m_slider, &QSlider::sliderMoved, this, &AudioPlayerWidget::seek);
    connect(m_timer, &QTimer::timeout, this,
            &AudioPlayerWidget::updateTimeInfo);
    connect(m_pauseButton, &QPushButton::clicked, this,
            &AudioPlayerWidget::onPauseClicked);
    connect(m_playButton, &QPushButton::clicked, this,
            &AudioPlayerWidget::onStopClicked);

    m_timer->setInterval(250);
}

void AudioPlayerWidget::play(nl::audio audio) {
    m_audioStream.open(audio);
    show();
    play();
}

void AudioPlayerWidget::setLoop(bool looping) {
    m_audioStream.setLoop(looping);
}

void AudioPlayerWidget::updateTimeInfo() {
    // Check if sound is stopped
    if (m_audioStream.getStatus() == sf::SoundStream::Stopped) {
        stop();
    }
    auto const playingOffset = m_audioStream.getPlayingOffset().asSeconds();
    auto const length = m_audioStream.lengthTime.asSeconds();
    auto const oMinutes = numeric_cast<int>(playingOffset) / 60;
    auto const oSeconds = numeric_cast<int>(playingOffset) % 60;
    auto const lMinutes = numeric_cast<int>(length) / 60;
    auto const lSeconds = numeric_cast<int>(length) % 60;
    std::ostringstream ss;
    ss << std::setfill('0');
    ss << std::setw(2) << oMinutes << ':' << std::setw(2) << oSeconds << " / "
       << std::setw(2) << lMinutes << ':' << std::setw(2) << lSeconds;
    m_label->setText(QString::fromStdString(ss.str()));
    m_slider->setValue(m_audioStream.getPlayingOffset().asMilliseconds());
}

void AudioPlayerWidget::seek(int where) {
    m_audioStream.setPlayingOffset(sf::milliseconds(where));
}

void AudioPlayerWidget::stop() {
    m_slider->setEnabled(false);
    m_pauseButton->setEnabled(false);
    m_playButton->setText("Play");
    m_pauseButton->setText("Pause");
    m_stopped = true;
}

void AudioPlayerWidget::onStopClicked() {
    if (!stopped()) {
        m_audioStream.stop();
        stop();
    } else {
        play();
    }
}

bool AudioPlayerWidget::stopped() { return m_stopped; }

void AudioPlayerWidget::play() {
    m_audioStream.play();
    m_playButton->setEnabled(true);
    m_pauseButton->setEnabled(true);
    m_slider->setEnabled(true);
    m_label->setEnabled(true);
    m_slider->setMaximum(m_audioStream.lengthTime.asMilliseconds());
    updateTimeInfo();
    m_timer->start();
    m_stopped = false;
    m_playButton->setText("Stop");
}

void AudioPlayerWidget::onPauseClicked() {
    if (m_audioStream.getStatus() == sf::SoundStream::Paused) {
        m_audioStream.play();
        m_slider->setEnabled(true);
        m_pauseButton->setText("Pause");
    } else {
        m_audioStream.pause();
        m_slider->setEnabled(false);
        m_pauseButton->setText("Resume");
    }
}
