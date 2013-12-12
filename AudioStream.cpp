#include "AudioStream.hpp"
#include <sstream>

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
    auto typemagic = static_cast<const unsigned char*>(audio.data()) + 0x33;
    if (typemagic[0] == 0x1E && typemagic[1] == 0x55)
        m_type = Mp3;
    else if (typemagic[0] == 0x12 && typemagic[1] == 0x01)
        m_type = Raw_S16LE_44100;
    else {
        std::ostringstream stream;
        stream << "Unkown audio type: " << std::hex
               << static_cast<int>(typemagic[0]) << ' ' << static_cast<int>(typemagic[1]);
        throw std::runtime_error(stream.str());
    }

    m_begin = static_cast<const unsigned char*>(audio.data()) + 82;
    m_length = audio.length() - 82;
    stop();
    if (m_type == Mp3) {
        mpg123assert(mpg123_close(m_handle));
        mpg123assert(mpg123_open_feed(m_handle));
        mpg123assert(mpg123_set_filesize(m_handle, m_length));
        mpg123assert(mpg123_feed(m_handle, m_begin, m_length));
        int channels;
        mpg123assert(mpg123_getformat(m_handle, &m_rate, &channels, nullptr));
        m_buf.resize(mpg123_outblock(m_handle));
        initialize(channels, m_rate);
        lengthTime = sf::seconds(mpg123_length(m_handle) / (double)m_rate);
    }
    else if (m_type == Raw_S16LE_44100) {
        initialize(1, 44100);
        lengthTime = sf::seconds((m_length / 2) / 44100);
    }
}

bool AudioStream::onGetData(Chunk& data)
{
    if (m_type == Mp3) {
        size_t done;
        mpg123_read(m_handle, m_buf.data(), m_buf.size(), &done);
        data.samples = reinterpret_cast<const sf::Int16*>(m_buf.data());
        data.sampleCount = done / sizeof(sf::Int16);
        return data.sampleCount > 0;
    }
    else if (m_type == Raw_S16LE_44100) {
        // TODO: Maybe implement streaming instead of reading the whole data at once
        data.samples = reinterpret_cast<const sf::Int16*>(m_begin);
        data.sampleCount = m_length / 2;
        return false;
    }
    return false;
}

void AudioStream::onSeek(sf::Time timeOffset)
{
    if (m_type == Mp3) {
        off_t offset;
        mpg123_feedseek(m_handle, timeOffset.asSeconds() * m_rate , SEEK_SET, &offset);
        mpg123_feed(m_handle, m_begin + offset, m_length - offset);
    }
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
