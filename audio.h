
#ifndef AUDIO_H
#define AUDIO_H

#include "defs.h"
#include <SDL_mixer.h>
#include <iostream>


bool nhac(Mix_Music*& bgm, Mix_Chunk*& sfxGameOver, Mix_Chunk*& sfxZap,
               Mix_Chunk*& sfxJump, Mix_Chunk*& sfxBoost, Mix_Chunk*& sfxWhoosh);
#endif // AUDIO_H
