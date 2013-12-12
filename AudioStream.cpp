#include "AudioStream.hpp"

AudioStream::AudioStream()
{
    mpg123assert(mpg123_init());
    handle = mpg123_new(nullptr, nullptr);

    if (!handle)
        die();
}

AudioStream::~AudioStream()
{
    mpg123_delete(handle);
}

void AudioStream::open(const nl::audio &audio)
{
    begin = static_cast<const unsigned char*>(audio.data()) + 82;
    length = audio.length() - 82;
    stop();
    mpg123assert(mpg123_close(handle));
    mpg123assert(mpg123_open_feed(handle));
    mpg123assert(mpg123_set_filesize(handle, length));
    mpg123assert(mpg123_feed(handle, begin, length));
    int channels = 0, encoding  = 0;
    mpg123assert(mpg123_getformat(handle, &rate, &channels, &encoding));
    buf.resize(mpg123_outblock(handle));
    initialize(channels, rate);
    lengthTime = sf::seconds(mpg123_length(handle) / (double)rate);
}

bool AudioStream::onGetData(Chunk& data)
{
    size_t done;
    mpg123_read(handle, buf.data(), buf.size(), &done);
    data.samples = (sf::Int16*)buf.data();
    data.sampleCount = done / sizeof(sf::Int16);
    return data.sampleCount > 0;
}

void AudioStream::onSeek(sf::Time timeOffset)
{
    off_t offset;
    mpg123_feedseek(handle, timeOffset.asSeconds() * rate , SEEK_SET, &offset);
    mpg123_feed(handle, begin + offset, length - offset);
}

void AudioStream::mpg123assert(int result)
{
    if (result != MPG123_OK)
        die();
}

void AudioStream::die()
{
    throw std::runtime_error(mpg123_strerror(handle));
}
