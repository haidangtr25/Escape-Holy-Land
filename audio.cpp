
#include "audio.h"
bool nhac(Mix_Music*& bgm, Mix_Chunk*& sfxGameOver, Mix_Chunk*& sfxZap,
               Mix_Chunk*& sfxJump, Mix_Chunk*& sfxBoost, Mix_Chunk*& sfxWhoosh) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // load nhạc
 bgm = Mix_LoadMUS("sounds/music/background.mp3");
    sfxGameOver = Mix_LoadWAV("sounds/sfx/game_over.wav");
    sfxZap = Mix_LoadWAV("sounds/sfx/lightning.wav");
    sfxJump = Mix_LoadWAV("sounds/sfx/jump.wav");
    sfxBoost = Mix_LoadWAV("sounds/sfx/powerup.wav");
    sfxWhoosh = Mix_LoadWAV("sounds/sfx/whoosh.wav");

    if (!bgm) {
    std::cerr << "Failed to load background music! Error: " << Mix_GetError() << std::endl;
    return false;
}
if (!sfxGameOver) {
    std::cerr << "Failed to load game over sound! Error: " << Mix_GetError() << std::endl;

}
if (!sfxZap) {
    std::cerr << "Failed to load lightning sound! Error: " << Mix_GetError() << std::endl;

}
if (!sfxJump) {
    std::cerr << "Failed to load jump sound! Error: " << Mix_GetError() << std::endl;

}
if (!sfxBoost) {
    std::cerr << "Failed to load power-up sound! Error: " << Mix_GetError() << std::endl;

}
if (!sfxWhoosh) {
    std::cerr << "Failed to load whoosh sound! Error: " << Mix_GetError() << std::endl;
    return false;
}

    // tùy chỉnh âm
 Mix_VolumeMusic(64);
    Mix_VolumeChunk(sfxGameOver, 90);
    Mix_VolumeChunk(sfxZap, 100);
    Mix_VolumeChunk(sfxJump, 60);
    Mix_VolumeChunk(sfxBoost, 80);
    Mix_VolumeChunk(sfxWhoosh, 70);


    return true;
}
