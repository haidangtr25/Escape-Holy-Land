#ifndef DEFS_H
#define DEFS_H
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "audio.h"
#include "objects.h"
constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 800;
constexpr int GROUND_HEIGHT = 100;
//score
extern int score;
extern int maxScore;
extern int roundNumber;
// Player
constexpr int PLAYER_SIZE = 40;
constexpr int PLAYER_SPEED = 9;
constexpr int JUMP = -15;


constexpr int GRAVITY = 1;
constexpr double PI = 3.14159265358979323846;

//objects
constexpr int OBJECT_SIZE = 35;
constexpr int BOSS_SIZE = 130;
constexpr int BOWER_SIZE = 170;
constexpr int ARROW_SIZE = 25;
constexpr int BALL_SIZE = 50;
constexpr int POWERUP_SIZE = 30;
constexpr int PAUSE_SIZE = 60;

// time
constexpr int FALL_DELAY = 300;
constexpr int POWER_UP_INTERVAL = 20000;
constexpr int APPLE_DURATION = 12000;
constexpr int IMMORTAL_DURATION = 10000;
constexpr int CLOUD_DURATION= 2500;
constexpr int LIGHTNING_DURATION = 600;
constexpr int BOSS_DURATION = 6000;
constexpr int WING_DURATION = 10000;
constexpr int BOWER_DURATION = 10000;
constexpr int ARROW_INTERVAL = 1500;
constexpr int BALL_DURATION = 4000;

extern int ARROW_SPEED;
extern int BALL_SPEED;
extern int NUMBER_ARROW;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern SDL_Texture* texPlr;
extern SDL_Texture* dropTex;
extern SDL_Texture* grounddropTex;
extern SDL_Texture* texBoss;
extern SDL_Texture* appleImg;
extern SDL_Texture* shTex;
extern SDL_Texture* wingImg;
extern SDL_Texture* bgCloud;
extern SDL_Texture* boltTex;
extern SDL_Texture* bwrTex;
extern SDL_Texture* texArr;
extern SDL_Texture* bgTex;
extern SDL_Texture* ballTex;
extern SDL_Texture* pauseButtonTexture;
extern SDL_Texture* pauseButtonHoverTexture;

// Audio
extern Mix_Music* bgm;
extern Mix_Chunk* sfxGameOver;
extern Mix_Chunk* sfxZap;
extern Mix_Chunk* sfxJump;
extern Mix_Chunk* sfxBoost;
extern Mix_Chunk* sfxWhoosh;
extern bool mainMenuOn;
extern bool levelMenuOn;
extern bool gameOverMenuOn ;
extern int currentObjectIndex;
extern Uint32 lastObjectSpawnTime;
extern Uint32 lastPowerUpSpawnTime;
extern bool spawnFromLeft;
extern int fallingObjectSpeed;
extern bool isPaused;
extern bool isHoveringPauseButton;

#endif // DEFS_H
