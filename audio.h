
#ifndef AUDIO_H
#define AUDIO_H

#include "defs.h"
#include <SDL_mixer.h>
#include <iostream>



// Function declarations
bool initAudio(Mix_Music*& bgMusic, Mix_Chunk*& gameOverSound, Mix_Chunk*& lightningSound,
               Mix_Chunk*& jumpSound, Mix_Chunk*& powerUpSound, Mix_Chunk*& whooshSound);
#endif // AUDIO_H
