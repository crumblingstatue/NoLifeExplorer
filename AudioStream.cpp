#include "AudioStream.hpp"

AudioStream::AudioStream()
{
    mpg123assert(mpg123_init());
    m_handle = mpg123_new(nullptr, nullptr);

    if (!m_handle)
        die();
}

AudioStream::~AudioStream()
{
    mpg123_delete(m_handle);
}

void AudioStream::open(const nl::audio &audio)
{
    m_begin = static_cast<const unsigned char*>(audio.data()) + 82;
    m_length = audio.length() - 82;
    stop();
    mpg123assert(mpg123_close(m_handle));
    mpg123assert(mpg123_open_feed(m_handle));
    mpg123assert(mpg123_set_filesize(m_handle, m_length));
    mpg123assert(mpg123_feed(m_handle, m_begin, m_length));
    int channels = 0, encoding  = 0;
    mpg123assert(mpg123_getformat(m_handle, &m_rate, &channels, &encoding));
    m_buf.resize(mpg123_outblock(m_handle));
    initialize(channels, m_rate);
    lengthTime = sf::seconds(mpg123_length(m_handle) / (double)m_rate);
}

bool AudioStream::onGetData(Chunk& data)
{
    size_t done;
    mpg123_read(m_handle, m_buf.data(), m_buf.size(), &done);
    data.samples = (sf::Int16*)m_buf.data();
    data.sampleCount = done / sizeof(sf::Int16);
    return data.sampleCount > 0;
}

void AudioStream::onSeek(sf::Time timeOffset)
{
    off_t offset;
    mpg123_feedseek(m_handle, timeOffset.asSeconds() * m_rate , SEEK_SET, &offset);
    mpg123_feed(m_handle, m_begin + offset, m_length - offset);
}

void AudioStream::mpg123assert(int result)
{
    if (result != MPG123_OK)
        die();
}

void AudioStream::die()
{
    throw std::runtime_error(mpg123_strerror(m_handle));
}
