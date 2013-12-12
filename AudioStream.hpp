#ifndef AUDIOSTREAM_HPP
#define AUDIOSTREAM_HPP

#include "NoLifeNx/audio.hpp"
#include <mpg123.h>
#include <SFML/Audio.hpp>

class AudioStream : public sf::SoundStream
{
public:
    AudioStream();
    ~AudioStream();
    void open(const nl::audio& audio);
    sf::Time lengthTime;
private:
    bool onGetData(Chunk& data);
    void onSeek(sf::Time timeOffset);
    void mpg123assert(int result);
    void die();
    mpg123_handle* handle = nullptr;
    std::vector<unsigned char> buf;
    const unsigned char* begin;
    uint32_t length;
    long rate;
};

#endif // AUDIOSTREAM_HPP
