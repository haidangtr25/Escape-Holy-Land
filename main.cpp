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
// Global variable.s

TTF_Font* font;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* playerTexture = nullptr;
SDL_Texture* objectTexture = nullptr;
SDL_Texture* groundObjectTexture = nullptr;
SDL_Texture* BossTexture = nullptr;
SDL_Texture* foodTexture = nullptr;
SDL_Texture* shieldTexture = nullptr;
SDL_Texture* wingTexture = nullptr;
SDL_Texture* cloudTexture = nullptr;
SDL_Texture* lightningTexture = nullptr;
SDL_Texture* bowerTexture = nullptr;
SDL_Texture* arrowTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* poolBallTexture = nullptr;
SDL_Texture* pauseButtonTexture = nullptr;
SDL_Texture* pauseButtonHoverTexture = nullptr;

Mix_Music* bgMusic = nullptr;
Mix_Chunk* gameOverSound = nullptr;
Mix_Chunk* lightningSound = nullptr;
Mix_Chunk* jumpSound = nullptr;
Mix_Chunk* powerUpSound = nullptr;
Mix_Chunk* whooshSound = nullptr;
//score related variable
int score = 0;
int maxScore = 0;
int roundNumber = 1;
// adding these in order to change speed when level arises
int POOL_BALL_SPEED;
int ARROW_SPEED;

// Game state
Player player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT, 0, false, false, false, false, 0, 0, 0, true};
std::vector<FallingObject> fallingObjects;
GroundObject groundObject = {-OBJECT_SIZE, SCREEN_HEIGHT - OBJECT_SIZE - GROUND_HEIGHT, 3, false};
PowerUp food = {0, 0, 2, false, foodTexture};
PowerUp shield = {0, 0, 3, false, shieldTexture};
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Dodge the Falling Objects", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

bool loadTextures() {
    BossTexture = IMG_LoadTexture(renderer,"imgs/boss.png");
    playerTexture = IMG_LoadTexture(renderer, "imgs/player.png");
    objectTexture = IMG_LoadTexture(renderer, "imgs/object.png");
    groundObjectTexture = IMG_LoadTexture(renderer, "imgs/demon.png");
    foodTexture = IMG_LoadTexture(renderer, "imgs/apple.png");
    shieldTexture = IMG_LoadTexture(renderer, "imgs/shield.png");
    wingTexture = IMG_LoadTexture(renderer, "imgs/wing.png");
    cloudTexture = IMG_LoadTexture(renderer, "imgs/cloud.png");
    lightningTexture = IMG_LoadTexture(renderer, "imgs/lightning.png");
    bowerTexture = IMG_LoadTexture(renderer, "imgs/bower.png");
    arrowTexture = IMG_LoadTexture(renderer, "imgs/arrow.png");
    backgroundTexture = IMG_LoadTexture(renderer, "imgs/background.png");
    poolBallTexture = IMG_LoadTexture(renderer, "imgs/poolball.png");
    pauseButtonTexture = IMG_LoadTexture(renderer, "imgs/pause_button.png");
    pauseButtonHoverTexture = IMG_LoadTexture(renderer, "imgs/pause_button_hover.png");

    if (!playerTexture || !objectTexture || !groundObjectTexture || !foodTexture || !shieldTexture ||
        !wingTexture || !cloudTexture || !lightningTexture || !bowerTexture || !arrowTexture ||
        !backgroundTexture || !poolBallTexture || !pauseButtonTexture || !pauseButtonHoverTexture) {
        std::cerr << "Failed to load textures!" << std::endl;
        return false;
    }
    return true;
}

void close() {
    SDL_DestroyTexture(BossTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(objectTexture);
    SDL_DestroyTexture(groundObjectTexture);
    SDL_DestroyTexture(foodTexture);
    SDL_DestroyTexture(shieldTexture);
    SDL_DestroyTexture(wingTexture);
    SDL_DestroyTexture(cloudTexture);
    SDL_DestroyTexture(lightningTexture);
    SDL_DestroyTexture(bowerTexture);
    SDL_DestroyTexture(arrowTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(poolBallTexture);
    SDL_DestroyTexture(pauseButtonTexture);
    SDL_DestroyTexture(pauseButtonHoverTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();

    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(gameOverSound);
    Mix_FreeChunk(lightningSound);
    Mix_FreeChunk(jumpSound);
    Mix_FreeChunk(powerUpSound);
    Mix_FreeChunk(whooshSound);

    Mix_CloseAudio();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }
if (!initAudio(bgMusic, gameOverSound, lightningSound, jumpSound, powerUpSound, whooshSound)) {
    std::cerr << "Audio initialization failed!" << std::endl;
    return -1;
}

    if (!loadTextures()) {
        std::cerr << "Failed to load textures!" << std::endl;
        return -1;
    }

    loadMaxScore(maxScore);
    srand(static_cast<unsigned int>(time(0)));
    Mix_PlayMusic(bgMusic, -1);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else {
                // handle mouse clicks over the menu, the start menu and the ending menu
                handleEvents(e, quit, showMainMenu, showLevelMenu, showGameOverMenu,
                             isPaused, isHoveringPauseButton, fallingObjectSpeed,
                             POOL_BALL_SPEED, ARROW_SPEED);
            }
        }

        if (showMainMenu || showLevelMenu || showGameOverMenu) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            renderMenu();
            SDL_RenderPresent(renderer);
            continue;
        }

        if (isPaused) {
            continue;
        }

        Uint32 currentTime = SDL_GetTicks();
        const Uint8* keystates = SDL_GetKeyboardState(nullptr);

        // Update all game objects
        player.update(keystates, currentTime);

        for (auto& obj : fallingObjects) {
            obj.update();
        }

        groundObject.update();
        food.update();
        shield.update();
        wing.update();
        cloud.update(currentTime);
        lightning.update(currentTime);
        boss.update(currentTime, player.x);
        bower.update(currentTime, arrows);

        for (auto& arrow : arrows) {
            arrow.update();
        }

        poolBall.update(currentTime);

        // Spawning logic
        SpawningLogic(currentTime);

        // Power-up spawning
        if (currentTime - lastPowerUpSpawnTime >= POWER_UP_INTERVAL) {
            lastPowerUpSpawnTime = currentTime;
            if (!food.active && !shield.active && !wing.active) {
                switch (rand() % 3) {
                    case 0:
                        food.spawn(SCREEN_WIDTH);
                        break;
                    case 1:
                        shield.spawn(SCREEN_WIDTH);
                        break;
                    case 2:
                        wing.spawn(SCREEN_WIDTH, currentTime);
                        break;
                }
            }
        }

        // Pool ball spawning
        if (score % 200 == 0 && score != 0 && !poolBall.active) {
            poolBall.spawn(SCREEN_WIDTH, SCREEN_HEIGHT, currentTime);
        }

        // Cloud spawning
        if (score % 100 == 0 && score != 0 && !cloud.active && !lightning.active) {
            cloud.spawn(SCREEN_WIDTH, currentTime);
        }

        if (cloud.active) {
            if (currentTime - cloud.spawnTime >= CLOUD_WARNING_DURATION) {
                // Cloud disappears, lightning appears
                cloud.active = false;
                lightning.x = cloud.x + OBJECT_SIZE / 2; // Center lightning under cloud
                lightning.spawnTime = currentTime;
                lightning.active = true;
                Mix_PlayChannel(-1, lightningSound, 0);
            }
        }

        if (lightning.active) {
            if (currentTime - lightning.spawnTime >= LIGHTNING_DURATION) {
                lightning.active = false;
            }
        }

        // Boss spawning
        if (score % 250 == 0 && score != 0 && !boss.active) {
            boss.spawn(SCREEN_WIDTH, currentTime);
        }

        // Bower spawning
        if (score % 300 == 0 && score != 0 && !bower.active) {
            bower.spawn(SCREEN_WIDTH, currentTime);
        }

        // Ground object spawning
        if (!groundObject.active && score % 400 == 0) {
            groundObject.spawn(spawnFromLeft, SCREEN_WIDTH, SCREEN_HEIGHT);
            spawnFromLeft = !spawnFromLeft;
        }

        // Check power-up collisions
        if (food.active && player.checkCollisionWith(food)) {
            food.active = false;
            player.speedBoostActive = true;
            player.speedBoostEndTime = currentTime + SPEED_BOOST_DURATION;
            Mix_PlayChannel(-1, powerUpSound, 0);
        }
        if (shield.active && player.checkCollisionWith(shield)) {
            shield.active = false;
            player.immortalEndTime = currentTime + IMMORTAL_DURATION;
            player.isImmortal = true;
            Mix_PlayChannel(-1, powerUpSound, 0);
        }
        if (wing.active && player.checkCollisionWith(wing)) {
            wing.active = false;
            player.wingActive = true;
            player.wingEndTime = currentTime + WING_DURATION;
            Mix_PlayChannel(-1, powerUpSound, 0);
        }

        // Check for game over conditions
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
            Mix_PlayChannel(-1, gameOverSound, 0);
            showGameOverMenu = true;
        }

        // Render everything
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 205);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

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

        renderText("Score: " + std::to_string(score), SCREEN_WIDTH - 200, 10);
        renderText("Max Score: " + std::to_string(maxScore), SCREEN_WIDTH - 200, 40);
        renderText("Round: " + std::to_string(roundNumber), SCREEN_WIDTH - 200, 70);

        renderPowerUpStatus();
        renderPauseButton(renderer,pauseButtonTexture,
                       pauseButtonHoverTexture,isHoveringPauseButton, PAUSE_BUTTON_SIZE, SCREEN_WIDTH);

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
    food = {0, 0, 2, false, foodTexture};
    shield = {0, 0, 3, false, shieldTexture};
    wing = {0, 0, 15, false, 0};
    cloud = {0, 0, 0, false};
    lightning = {0, 0, false};
    boss = {0, 0, 0, false};
    bower = {0, 0, 0, 0, false};
    arrows.clear();
    poolBall = {0, 0, 0.0, 0, false};
    score = 0;
    currentObjectIndex = 0;
    lastObjectSpawnTime = 0;
    spawnFromLeft = true;
    showGameOverMenu = false;
    isPaused = false;
    if (!Mix_PlayingMusic()) {
        Mix_PlayMusic(bgMusic, -1);
    }
}

void handleEvents(SDL_Event& e, bool& quit, bool& showMainMenu, bool& showLevelMenu,
                 bool& showGameOverMenu, bool& isPaused, bool isHoveringPauseButton,
                 int& fallingObjectSpeed, int& POOL_BALL_SPEED, int& ARROW_SPEED) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (showMainMenu) {
            if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                    showMainMenu = false;
                    showLevelMenu = true;
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                    quit = true;
                }
            }
        }
        else if (showLevelMenu) {
            if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                if (mouseY >= SCREEN_HEIGHT / 2 - 150 && mouseY <= SCREEN_HEIGHT / 2 - 110) {
                    fallingObjectSpeed = 5;
                    POOL_BALL_SPEED = 7;
                    ARROW_SPEED = 3;
                    showLevelMenu = false;
                    resetGame();
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 - 100 && mouseY <= SCREEN_HEIGHT / 2 - 60) {
                    fallingObjectSpeed = 7;
                    POOL_BALL_SPEED = 10;
                    ARROW_SPEED = 4;
                    showLevelMenu = false;
                    resetGame();
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                    fallingObjectSpeed = 10;
                    POOL_BALL_SPEED = 13;
                    ARROW_SPEED = 5;
                    showLevelMenu = false;
                    resetGame();
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                    fallingObjectSpeed = 16;
                    POOL_BALL_SPEED = 16;
                    ARROW_SPEED = 6;
                    showLevelMenu = false;
                    resetGame();
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 + 50 && mouseY <= SCREEN_HEIGHT / 2 + 90) {
                    quit = true;
                }
            }
        }
        else if (showGameOverMenu) {
            if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                    showGameOverMenu = false;
                    showLevelMenu = true;
                    resetGame();
                    Mix_HaltMusic();
                    Mix_PlayMusic(bgMusic, -1);
                }
                else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                    quit = true;
                }
            }
        }

        if (!showMainMenu && !showLevelMenu && !showGameOverMenu && isHoveringPauseButton) {
            isPaused = !isPaused;
            if (isPaused) {
                Mix_PauseMusic();
            } else {
                Mix_ResumeMusic();
            }
        }
    }
    else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_ESCAPE && !showMainMenu && !showLevelMenu && !showGameOverMenu) {
            isPaused = !isPaused;
            if (isPaused) {
                Mix_PauseMusic();
            } else {
                Mix_ResumeMusic();
            }
        }
    }
}
