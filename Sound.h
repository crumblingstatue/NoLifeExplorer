#ifndef SOUND_H
#define SOUND_H

#include "SoundItem.h"

#include <mpg123.h>
#include <SFML/Audio.hpp>

class Sound : public sf::SoundStream {
public:
    Sound();
    ~Sound();
    void open(const SoundItem& item);
private:
    bool onGetData(Chunk &data);
    void onSeek(sf::Time timeOffset);
    void assert(int result);
    void die();
    mpg123_handle* handle = nullptr;
    std::vector<unsigned char> buf;
    const unsigned char* begin;
    u_int32_t length;
};

#endif // SOUND_H
