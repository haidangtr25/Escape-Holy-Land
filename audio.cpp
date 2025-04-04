
#include "audio.h"
bool initAudio(Mix_Music*& bgMusic, Mix_Chunk*& gameOverSound, Mix_Chunk*& lightningSound,
               Mix_Chunk*& jumpSound, Mix_Chunk*& powerUpSound, Mix_Chunk*& whooshSound) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)) {
        std::cerr << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
        return false;
    }

    // Load music and sounds
 bgMusic = Mix_LoadMUS("sounds/music/background.mp3");
    gameOverSound = Mix_LoadWAV("sounds/sfx/game_over.wav");
    lightningSound = Mix_LoadWAV("sounds/sfx/lightning.wav");
    jumpSound = Mix_LoadWAV("sounds/sfx/jump.wav");
    powerUpSound = Mix_LoadWAV("sounds/sfx/powerup.wav");
    whooshSound = Mix_LoadWAV("sounds/sfx/whoosh.wav");

    if (!bgMusic) {
    std::cerr << "Failed to load background music! Error: " << Mix_GetError() << std::endl;
    return false;
}
if (!gameOverSound) {
    std::cerr << "Failed to load game over sound! Error: " << Mix_GetError() << std::endl;

}
if (!lightningSound) {
    std::cerr << "Failed to load lightning sound! Error: " << Mix_GetError() << std::endl;

}
if (!jumpSound) {
    std::cerr << "Failed to load jump sound! Error: " << Mix_GetError() << std::endl;

}
if (!powerUpSound) {
    std::cerr << "Failed to load power-up sound! Error: " << Mix_GetError() << std::endl;

}
if (!whooshSound) {
    std::cerr << "Failed to load whoosh sound! Error: " << Mix_GetError() << std::endl;
    return false;
}

    // Set volumes (0-128)
 Mix_VolumeMusic(64);  // 50% volume for music
    Mix_VolumeChunk(gameOverSound, 90);
    Mix_VolumeChunk(lightningSound, 100);
    Mix_VolumeChunk(jumpSound, 60);
    Mix_VolumeChunk(powerUpSound, 80);
    Mix_VolumeChunk(whooshSound, 70);


    return true;
}
