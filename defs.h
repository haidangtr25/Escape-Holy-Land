#ifndef DEFS_H
#define DEFS_H
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "audio.h"
#include "objects.h"
// Screen constants
constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 800;
constexpr int GROUND_HEIGHT = 100;
//score
extern int score;
extern int maxScore;
extern int roundNumber;
// Player constants
constexpr int PLAYER_SIZE = 40;
constexpr int PLAYER_SPEED = 9;
constexpr int JUMP_STRENGTH = -15;

// Physics constants
constexpr int GRAVITY = 1;
constexpr double PI = 3.14159265358979323846;

// Game objects constants
constexpr int OBJECT_SIZE = 35;
constexpr int BOSS_SIZE = 120;
constexpr int BOWER_SIZE = 150;
constexpr int ARROW_SIZE = 25;
constexpr int POOL_BALL_SIZE = 50;
constexpr int POWER_UP_ICON_SIZE = 30;
constexpr int PAUSE_BUTTON_SIZE = 60;

// Timing constants (in milliseconds)
constexpr int FALLING_OBJECT_DELAY = 200;
constexpr int POWER_UP_INTERVAL = 20000;
constexpr int SPEED_BOOST_DURATION = 12000;
constexpr int IMMORTAL_DURATION = 10000;
constexpr int CLOUD_WARNING_DURATION = 2500;
constexpr int LIGHTNING_DURATION = 600;
constexpr int BOSS_DURATION = 6000;
constexpr int WING_DURATION = 10000;
constexpr int BOWER_DURATION = 10000;
constexpr int ARROW_SHOOT_INTERVAL = 1500;
constexpr int POOL_BALL_DURATION = 4000;

// Modifiable variables (extern declarations)
extern int ARROW_SPEED;
extern int POOL_BALL_SPEED;
// SDL Window and Renderer
extern SDL_Window* window;
extern SDL_Renderer* renderer;

// Texture Resources
extern SDL_Texture* playerTexture;
extern SDL_Texture* objectTexture;
extern SDL_Texture* groundObjectTexture;
extern SDL_Texture* BossTexture;
extern SDL_Texture* foodTexture;
extern SDL_Texture* shieldTexture;
extern SDL_Texture* wingTexture;
extern SDL_Texture* cloudTexture;
extern SDL_Texture* lightningTexture;
extern SDL_Texture* bowerTexture;
extern SDL_Texture* arrowTexture;
extern SDL_Texture* backgroundTexture;
extern SDL_Texture* poolBallTexture;
extern SDL_Texture* pauseButtonTexture;
extern SDL_Texture* pauseButtonHoverTexture;

// Audio Resources
extern Mix_Music* bgMusic;
extern Mix_Chunk* gameOverSound;
extern Mix_Chunk* lightningSound;
extern Mix_Chunk* jumpSound;
extern Mix_Chunk* powerUpSound;
extern Mix_Chunk* whooshSound;
extern bool showMainMenu;
extern bool showLevelMenu;
extern bool showGameOverMenu ;
extern int currentObjectIndex;
extern Uint32 lastObjectSpawnTime;
extern Uint32 lastPowerUpSpawnTime;
extern bool spawnFromLeft;
extern int fallingObjectSpeed;
extern bool isPaused;
extern bool isHoveringPauseButton;

#endif // DEFS_H
