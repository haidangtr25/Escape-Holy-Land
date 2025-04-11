#include "pause.h"
#include "defs.h"
#include <cstdlib>
#include <ctime>
#include "audio.h"
#include <vector>
#include "score.h"
#include "objects.h"
#include <cmath>
#include "menu.h"
#include "objects.h"

TTF_Font* font;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* texPlr = nullptr;
SDL_Texture* dropTex = nullptr;
SDL_Texture* grounddropTex = nullptr;
SDL_Texture* texBoss = nullptr;
SDL_Texture* appleImg = nullptr;
SDL_Texture* shTex = nullptr;
SDL_Texture* wingImg = nullptr;
SDL_Texture* bgCloud = nullptr;
SDL_Texture* boltTex = nullptr;
SDL_Texture* bwrTex = nullptr;
SDL_Texture* texArr = nullptr;
SDL_Texture* bgTex = nullptr;
SDL_Texture* ballTex = nullptr;
SDL_Texture* pauseButtonTexture = nullptr;
SDL_Texture* pauseButtonHoverTexture = nullptr;

Mix_Music* bgm = nullptr;
Mix_Chunk* sfxGameOver = nullptr;
Mix_Chunk* sfxZap = nullptr;
Mix_Chunk* sfxJump = nullptr;
Mix_Chunk* sfxBoost = nullptr;
Mix_Chunk* sfxWhoosh = nullptr;

int score = 0;
int maxScore = 0;
int roundNumber = 1;

int BALL_SPEED;
int ARROW_SPEED;
int NUMBER_ARROW;

// khởi tạo objects đầu
Player player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT, 0, false, false, false, false, 0, 0, 0, true};
std::vector<FallingObject> fallingObjects;
GroundObject groundObject = {-OBJECT_SIZE, SCREEN_HEIGHT - OBJECT_SIZE - GROUND_HEIGHT, 3, false};
PowerUp food = {0, 0, 2, false, appleImg};
PowerUp shield = {0, 0, 3, false, shTex};
Wing wing = {0, 0, 15, false, 0};
Cloud cloud = {0, 0, 0, false};
Lightning lightning = {0, 0, false};
Boss boss = {0, 0, 0, false};
Bower bower = {0, 0, 0, 0, false};
std::vector<Arrow> arrows;
PoolBall poolBall = {0, 0, 0.0, 0, false};

int currentObjectIndex = 0;
Uint32 lastObjectSpawnTime = 0;
Uint32 lastPowerUpSpawnTime = 0;
bool spawnFromLeft = true;
int fallingObjectSpeed = 3;
bool isPaused = false;
bool isHoveringPauseButton = false;


bool init() {
     window = SDL_CreateWindow("Escape Holy Land", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) std::cerr << "không thể khởi tạo SDL"  << std::endl;
    if (!window) std::cerr << "không thể khởi tạo SDL" << std::endl;
    if (!renderer) std::cerr << "không thể tạo renderer "  << std::endl;
    if (TTF_Init() == -1) std::cerr << "không thể khởi tạo TTF"  << std::endl;
       font = TTF_OpenFont("arial.ttf", 24);
    if (!font)std::cerr << "không thể load font"  << std::endl;
    if(!font || TTF_Init() == -1 || !window || !renderer || SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;
    return true;
}

bool loadTextures() {
    texBoss = IMG_LoadTexture(renderer,"imgs/boss.png");
    texPlr = IMG_LoadTexture(renderer, "imgs/player.png");
    dropTex = IMG_LoadTexture(renderer, "imgs/object.png");
    grounddropTex = IMG_LoadTexture(renderer, "imgs/demon.png");
    appleImg = IMG_LoadTexture(renderer, "imgs/apple.png");
    shTex = IMG_LoadTexture(renderer, "imgs/shield.png");
    wingImg = IMG_LoadTexture(renderer, "imgs/wing.png");
    bgCloud = IMG_LoadTexture(renderer, "imgs/cloud.png");
    boltTex = IMG_LoadTexture(renderer, "imgs/lightning.png");
    bwrTex = IMG_LoadTexture(renderer, "imgs/bower.png");
    texArr = IMG_LoadTexture(renderer, "imgs/arrow.png");
    bgTex = IMG_LoadTexture(renderer, "imgs/background.png");
    ballTex = IMG_LoadTexture(renderer, "imgs/poolball.png");
    pauseButtonTexture = IMG_LoadTexture(renderer, "imgs/pause_button.png");
    pauseButtonHoverTexture = IMG_LoadTexture(renderer, "imgs/pause_button_hover.png");

    if (!texPlr || !dropTex || !grounddropTex || !appleImg || !shTex ||
        !wingImg || !bgCloud || !boltTex || !bwrTex || !texArr ||
        !bgTex || !ballTex || !pauseButtonTexture || !pauseButtonHoverTexture) {
        std::cerr << "không load đc textures!" << std::endl;
        return false;
    }
    return true;
}
void close() {
    if (texBoss) SDL_DestroyTexture(texBoss);
    if (texPlr) SDL_DestroyTexture(texPlr);
    if (dropTex) SDL_DestroyTexture(dropTex);
    if (grounddropTex) SDL_DestroyTexture(grounddropTex);
    if (appleImg) SDL_DestroyTexture(appleImg);
    if (shTex) SDL_DestroyTexture(shTex);
    if (wingImg) SDL_DestroyTexture(wingImg);
    if (bgCloud) SDL_DestroyTexture(bgCloud);
    if (boltTex) SDL_DestroyTexture(boltTex);
    if (bwrTex) SDL_DestroyTexture(bwrTex);
    if (texArr) SDL_DestroyTexture(texArr);
    if (bgTex) SDL_DestroyTexture(bgTex);
    if (ballTex) SDL_DestroyTexture(ballTex);
    if (pauseButtonTexture) SDL_DestroyTexture(pauseButtonTexture);
    if (pauseButtonHoverTexture) SDL_DestroyTexture(pauseButtonHoverTexture);

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    if (font) TTF_CloseFont(font);
    TTF_Quit();

    if (bgm) Mix_FreeMusic(bgm);
    if (sfxGameOver) Mix_FreeChunk(sfxGameOver);
    if (sfxZap) Mix_FreeChunk(sfxZap);
    if (sfxJump) Mix_FreeChunk(sfxJump);
    if (sfxBoost) Mix_FreeChunk(sfxBoost);
    if (sfxWhoosh) Mix_FreeChunk(sfxWhoosh);

    Mix_CloseAudio();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) return -1;
    if (!nhac(bgm, sfxGameOver, sfxZap, sfxJump, sfxBoost, sfxWhoosh)) return -1;
    if (!loadTextures())return -1;
// load score từ file
    loadMaxScore(maxScore);
    srand((time(0)));
    Mix_PlayMusic(bgm, -1);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else {
                handleEvents(e, quit, mainMenuOn, levelMenuOn, gameOverMenuOn,
                             isPaused, isHoveringPauseButton, fallingObjectSpeed,
                             BALL_SPEED, ARROW_SPEED);
            }
        }

        if (mainMenuOn || levelMenuOn || gameOverMenuOn) {
            SDL_RenderClear(renderer);
            renderMenu();
            SDL_RenderPresent(renderer);
            continue;
        }

        if (isPaused) {
            continue;
        }
        if(score > maxScore) maxScore = score;

        Uint32 realtime = SDL_GetTicks();
        const Uint8* keystates = SDL_GetKeyboardState(nullptr);

        // update x y
        player.update(keystates, realtime);

        for (auto& obj : fallingObjects) {
            obj.update();
        }

        groundObject.update();
        food.update();
        shield.update();
        wing.update();
        lightning.update(realtime);
        boss.update(realtime, player.x);
        bower.update(realtime, arrows);

        for (auto& arrow : arrows) {
            arrow.update();
        }

        poolBall.update(realtime);
        SpawningLogic(realtime);

        // spawn powerup
        if (realtime - lastPowerUpSpawnTime >= POWER_UP_INTERVAL) {
            lastPowerUpSpawnTime = realtime;
            if (!food.active && !shield.active && !wing.active) {
                int RNG = rand() % 3;
                if (RNG == 0) food.spawn(SCREEN_WIDTH);
                else if (RNG == 1)     shield.spawn(SCREEN_WIDTH);
                else wing.spawn(SCREEN_WIDTH, realtime);
            }
        }

        // spawn bóng
        if (score % 200 == 0 && score != 0 && !poolBall.active) {
            poolBall.spawn(SCREEN_WIDTH, SCREEN_HEIGHT, realtime);
        }

        // spawn mây
        if (score % 100 == 0 && score != 0 && !cloud.active && !lightning.active) {
            cloud.spawn(SCREEN_WIDTH, realtime);
        }

        if (cloud.active) {
            if (realtime - cloud.spawnTime >= CLOUD_DURATION) {
                cloud.active = false;
                lightning.x = cloud.x + OBJECT_SIZE / 2;
                lightning.spawnTime = realtime;
                lightning.active = true;
                Mix_PlayChannel(-1, sfxZap, 0);
            }
        }

        if (lightning.active) {
            if (realtime - lightning.spawnTime >= LIGHTNING_DURATION) {
                lightning.active = false;
            }
        }

        // spawn boss
        if (score % 250 == 0 && score != 0 && !boss.active) {
            boss.spawn(SCREEN_WIDTH, realtime);
        }

        // spawn cung thủ
        if (score % 300 == 0 && score != 0 && !bower.active) {
            bower.spawn(SCREEN_WIDTH, realtime);
        }

        // spawn quỷ g
        if (!groundObject.active && score % 400 == 0) {
            groundObject.spawn(spawnFromLeft, SCREEN_WIDTH, SCREEN_HEIGHT);
            spawnFromLeft = !spawnFromLeft;
        }

        // buff tăng lực
        if (food.active && player.checkCollisionWith(food)) {
            food.active = false;
            player.speedBoostActive = true;
            player.speedBoostEndTime = realtime + APPLE_DURATION;
            Mix_PlayChannel(-1, sfxBoost, 0);
        }
        if (shield.active && player.checkCollisionWith(shield)) {
            shield.active = false;
            player.immortalEndTime = realtime + IMMORTAL_DURATION;
            player.isImmortal = true;
            Mix_PlayChannel(-1, sfxBoost, 0);
        }
        if (wing.active && player.checkCollisionWith(wing)) {
            wing.active = false;
            player.wingActive = true;
            player.wingEndTime = realtime + WING_DURATION;
            Mix_PlayChannel(-1, sfxBoost, 0);
        }

        //va chạm
        bool gameOver = false;
        if (!player.isImmortal) {
            for (const auto& obj : fallingObjects) {
                if (player.checkCollisionWith(obj)) {
                    gameOver = true;
                    break;
                }
            }
            if (!gameOver && player.checkCollisionWith(groundObject)) {
                gameOver = true;
            }
            if (!gameOver && player.checkCollisionWith(lightning)) {
                gameOver = true;
            }
            if (!gameOver && player.checkCollisionWith(boss)) {
                gameOver = true;
            }
            if (!gameOver && player.checkCollisionWith(poolBall)) {
                gameOver = true;
            }
            if (!gameOver) {
                for (const auto& arrow : arrows) {
                    if (player.checkCollisionWith(arrow)) {
                        gameOver = true;
                        break;
                    }
                }
            }
        }

        if (gameOver) {
            saveMaxScore(maxScore);
            Mix_HaltMusic();
            Mix_PlayChannel(-1, sfxGameOver, 0);
            gameOverMenuOn = true;
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 205);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTex, nullptr, nullptr);

        player.render();

        for (const auto& obj : fallingObjects) {
            obj.render();
        }

        groundObject.render();
        food.render();
        shield.render();
        wing.render();
        cloud.render();
        lightning.render();
        boss.render();
        bower.render();

        for (const auto& arrow : arrows) {
            arrow.render();
        }

        poolBall.render();

        renderChat("Score: " + std::to_string(score), SCREEN_WIDTH - 200, 10);
        renderChat("Max Score: " + std::to_string(maxScore), SCREEN_WIDTH - 200, 40);
        renderChat("Round: " + std::to_string(roundNumber), SCREEN_WIDTH - 200, 70);

        renderPowerUpStatus();
        renderPauseBtn(renderer,pauseButtonTexture,
                       pauseButtonHoverTexture,isHoveringPauseButton, PAUSE_SIZE, SCREEN_WIDTH);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    close();
    return 0;
}


void resetGame() {
    player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT, 0, false, false, false, false, 0, 0, 0, false};
    fallingObjects.clear();
    fallingObjects.push_back({rand() % (SCREEN_WIDTH - OBJECT_SIZE), 0, fallingObjectSpeed, false});
    groundObject = {-OBJECT_SIZE, SCREEN_HEIGHT - OBJECT_SIZE - GROUND_HEIGHT, 3, false};
    food = {0, 0, 2, false, appleImg};
    shield = {0, 0, 3, false, shTex};
    wing = {0, 0, 15, false, 0};
    cloud = {0, 0, 0, false};
    lightning = {0, 0, false};
    boss = {0, 0, 0, false};
    bower = {0, 0, 0, 0, false};
    arrows.clear();
    poolBall = {0, 0, 0.0, 0, false};
    score = 0;
    roundNumber = 1;
    currentObjectIndex = 0;
    lastObjectSpawnTime = 0;
    spawnFromLeft = true;
    gameOverMenuOn = false;
    isPaused = false;
    if (!Mix_PlayingMusic()) {
        Mix_PlayMusic(bgm, -1);
    }
}

void handleEvents(SDL_Event& e, bool& quit, bool& mainMenuOn, bool& levelMenuOn,
                 bool& gameOverMenuOn, bool& isPaused, bool isHoveringPauseButton,
                 int& fallingObjectSpeed, int& BALL_SPEED, int& ARROW_SPEED) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (mainMenuOn) {
            if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                    mainMenuOn = false;
                    levelMenuOn = true;
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                    quit = true;
                }
            }
        }
        else if (levelMenuOn) {
            if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                if (mouseY >= SCREEN_HEIGHT / 2 - 150 && mouseY <= SCREEN_HEIGHT / 2 - 110) {
                    fallingObjectSpeed = 5;
                    BALL_SPEED = 7;
                    ARROW_SPEED = 3;
                    NUMBER_ARROW = 4;
                    levelMenuOn = false;
                    resetGame();
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 - 100 && mouseY <= SCREEN_HEIGHT / 2 - 60) {
                    fallingObjectSpeed = 7;
                    BALL_SPEED = 10;
                    ARROW_SPEED = 4;
                    NUMBER_ARROW = 5;
                    levelMenuOn = false;
                    resetGame();
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                    fallingObjectSpeed = 10;
                    BALL_SPEED = 13;
                    ARROW_SPEED = 6;
                    NUMBER_ARROW = 6;
                    levelMenuOn = false;
                    resetGame();
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                    fallingObjectSpeed = 16;
                    BALL_SPEED = 16;
                    ARROW_SPEED = 9;
                    NUMBER_ARROW = 8;
                    levelMenuOn = false;
                    resetGame();
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 + 50 && mouseY <= SCREEN_HEIGHT / 2 + 90) {
                    quit = true;
                }
            }
        }
        else if (gameOverMenuOn) {
            if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                    gameOverMenuOn = false;
                    levelMenuOn = true;
                    resetGame();
                    Mix_HaltMusic();
                    Mix_PlayMusic(bgm, -1);
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                    quit = true;
                }
            }
        }

        if (!mainMenuOn && !levelMenuOn && !gameOverMenuOn && isHoveringPauseButton) {
            isPaused = !isPaused;
            if (isPaused) {
                Mix_PauseMusic();
            } else {
                Mix_ResumeMusic();
            }
        }
    }
    else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE && !mainMenuOn && !levelMenuOn && !gameOverMenuOn) {
            isPaused = !isPaused;
            if (isPaused) {
                Mix_PauseMusic();
            } else {
                Mix_ResumeMusic();
            }
        }
    }
}
