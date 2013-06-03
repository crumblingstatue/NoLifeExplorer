#include "Sound.h"

#include <QDebug>

Sound::Sound()
{
    assert(mpg123_init());
    handle = mpg123_new(nullptr, nullptr);

    if (!handle)
    {
        die();
    }
}

Sound::~Sound()
{
    mpg123_delete(handle);
}

void Sound::open(const SoundItem& item)
{
    begin = static_cast<const unsigned char*>(item.data);
    length = item.length;
    stop();
    assert(mpg123_close(handle));
    assert(mpg123_open_feed(handle));
    assert(mpg123_feed(handle, (const unsigned char*)item.data, item.length));
    long rate = 0;
    int channels = 0, encoding  = 0;
    assert(mpg123_getformat(handle, &rate, &channels, &encoding));
    buf.resize(mpg123_outblock(handle));
    initialize(channels, rate);
}

bool Sound::onGetData(Chunk& data)
{
    size_t done;
    mpg123_read(handle, buf.data(), buf.size(), &done);
    data.samples = (sf::Int16*)buf.data();
    data.sampleCount = done / sizeof(sf::Int16);
    return data.sampleCount > 0;
}

void Sound::onSeek(sf::Time timeOffset)
{
    off_t offset;
    mpg123_feedseek(handle, timeOffset.asSeconds(), SEEK_SET, &offset);
    mpg123_feed(handle, begin + offset, length - offset);
}

void Sound::assert(int result)
{
    if (result != MPG123_OK)
    {
        die();
    }
}

void Sound::die()
{
    qDebug() << mpg123_strerror(handle);
    throw;
}
