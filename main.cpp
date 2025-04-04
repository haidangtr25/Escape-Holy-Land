#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>
#include <cmath>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const int PLAYER_SIZE = 40;
const int OBJECT_SIZE = 40;
const int GRAVITY = 1;
const int PLAYER_SPEED = 9;
const int JUMP_STRENGTH = -15;
const int FALLING_OBJECT_DELAY = 400; // 2-second delay between falling objects
const int POWER_UP_INTERVAL = 20000; // 20 seconds
const int SPEED_BOOST_DURATION = 12000; // 12 seconds
const int IMMORTAL_DURATION = 10000; // 10 seconds
const int CLOUD_WARNING_DURATION = 1500; // 1.5 secondsp
const int LIGHTNING_DURATION = 600; // 1 second
const int BOSS_SIZE = 130; // Boss size
const int BOSS_DURATION = 6000; // 5 seconds
const int WING_DURATION = 10000; // 10 seconds
const int BOWER_SIZE = 180; // Bower size
const int BOWER_DURATION = 10000; // 10 seconds
const int ARROW_SIZE = 30; // Arrow size
 int ARROW_SPEED = 6; // Arrow speed
const int ARROW_SHOOT_INTERVAL = 1500; // Shoot arrows every 200ms
const double PI = 3.14159265358979323846; // For rotation calculations
const int GROUND_HEIGHT = 100; // Height of the ground in the background
const int POWER_UP_ICON_SIZE = 30; // Size for power-up status icons
const int POOL_BALL_SIZE =60; // Size of the pool ball
const int POOL_BALL_DURATION = 4000; // 11 seconds
 int POOL_BALL_SPEED = 25; // Speed of the pool ball
const int PAUSE_BUTTON_SIZE = 60; // Size of the pause button

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
SDL_Texture* backgroundTexture = nullptr; // Background texture
SDL_Texture* poolBallTexture = nullptr; // Pool ball texture
SDL_Texture* pauseButtonTexture = nullptr; // Pause button texture
SDL_Texture* pauseButtonHoverTexture = nullptr; // Pause button hover texture
TTF_Font* font = nullptr;

struct Player {
    int x, y;
    int velocity;
    bool isJumping;
    bool isImmortal;
    bool speedBoostActive; // Track if speed boost is active
    bool wingActive; // Track if wing is active
    Uint32 speedBoostEndTime;
    Uint32 immortalEndTime;
    Uint32 wingEndTime;// Track player rotation
    bool facingRight; // Track player direction
};

struct FallingObject {
    int x, y;
    int speed;
    bool active;
};

struct GroundObject {
    int x, y;
    int speed;
    bool active;
    bool facingright;
};

struct PowerUp {
    int x, y;
    int speed;
    bool active;
};

struct Wing {
    int x, y;
    int speed;
    bool active;
    Uint32 spawnTime;
};

struct Cloud {
    int x, y;
    Uint32 spawnTime;
    bool active;
};

struct Lightning {
    int x;
    Uint32 spawnTime;
    bool active;
};

struct Boss {
    int x, y;
    Uint32 spawnTime;
    bool active;
};

struct Bower {
    int x, y;
    Uint32 spawnTime;
    Uint32 lastShootTime; // Track the last time arrows were shot
    bool active;
};

struct Arrow {
    int x, y;
    double angle; // Angle in radians
    bool active;
};

struct PoolBall {
    int x, y;
    double angle; // Angle in radians
    Uint32 spawnTime;
    bool active;
};

Player player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT, 0, false, false, false, false, 0, 0, 0, true};
std::vector<FallingObject> fallingObjects;
GroundObject groundObject = {-OBJECT_SIZE, SCREEN_HEIGHT - OBJECT_SIZE - GROUND_HEIGHT, 3, false};
PowerUp food = {0, 0, 3, false};
PowerUp shield = {0, 0, 3, false};
Wing wing = {0, 0, 15, false, 0};
Cloud cloud = {0, 0, 0, false};
Lightning lightning = {0, 0, false};
Boss boss = {0, 0, 0, false};
Bower bower = {0, 0, 0, 0, false};
std::vector<Arrow> arrows;
PoolBall poolBall = {0, 0, 0.0, 0, false};

int score = 0;
int maxScore = 0;
int roundNumber = 1;
int currentObjectIndex = 0; // Tracks which object is currently falling
Uint32 lastObjectSpawnTime = 0; // Tracks the last time a falling object was spawned
Uint32 lastPowerUpSpawnTime = 0; // Tracks the last time a power-up was spawned
bool spawnFromLeft = true; // Toggle for ground object spawn side
bool showMainMenu = true; // Track if the main menu is shown
bool showLevelMenu = false; // Track if the level menu is shown
bool showGameOverMenu = false; // Track if the game over menu is shown
int fallingObjectSpeed = 3; // Default speed for Level 1
bool isPaused = false; // Track if game is paused
bool isHoveringPauseButton = false; // Track if mouse is hovering over pause button

// Function to load maxScore from a file
void loadMaxScore() {
    std::ifstream file("max_score.txt");
    if (file.is_open()) {
        file >> maxScore;
        file.close();
    } else {
        maxScore = 0; // If the file doesn't exist, start with 0
    }
}

// Function to save maxScore to a file
void saveMaxScore() {
    std::ofstream file("max_score.txt");
    if (file.is_open()) {
        file << maxScore;
        file.close();
    } else {
        std::cerr << "Failed to save max score!" << std::endl;
    }
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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

    font = TTF_OpenFont("arial.ttf", 24); // Replace with your font file
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

bool loadTextures() {
    BossTexture = IMG_LoadTexture(renderer,"gift.png");
    playerTexture = IMG_LoadTexture(renderer, "player.png"); // Replace with your player image
    objectTexture = IMG_LoadTexture(renderer, "object.png"); // Replace with your object image
    groundObjectTexture = IMG_LoadTexture(renderer, "bull.png"); // Replace with your ground object image
    foodTexture = IMG_LoadTexture(renderer, "apple.png"); // Replace with your food image
    shieldTexture = IMG_LoadTexture(renderer, "shield.png"); // Replace with your shield image
    wingTexture = IMG_LoadTexture(renderer, "wing.png"); // Replace with your wing image
    cloudTexture = IMG_LoadTexture(renderer, "cloud.png"); // Replace with your cloud image
    lightningTexture = IMG_LoadTexture(renderer, "lightning.png"); // Replace with your lightning image
    bowerTexture = IMG_LoadTexture(renderer, "bower.png"); // Replace with your bower image
    arrowTexture = IMG_LoadTexture(renderer, "arrow.png"); // Replace with your arrow image
    backgroundTexture = IMG_LoadTexture(renderer, "background.png"); // Load background image
    poolBallTexture = IMG_LoadTexture(renderer, "poolball.png"); // Load pool ball image
    pauseButtonTexture = IMG_LoadTexture(renderer, "pause_button.png"); // Load pause button image
    pauseButtonHoverTexture = IMG_LoadTexture(renderer, "pause_button_hover.png"); // Load pause button hover image

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
    SDL_Quit();
}

bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

void renderText(const std::string& text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderMenu() {
    if (showMainMenu) {
        renderText("New Game", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderText("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
    } else if (showLevelMenu) {
        renderText("Level: Easy", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 100);
        renderText("Level: Hard", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderText("Level: Hell", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
        renderText("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 50);
    } else if (showGameOverMenu) {
        renderText("Play Again", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderText("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
    }
}

void renderPauseButton() {
    SDL_Rect pauseButtonRect = {SCREEN_WIDTH - PAUSE_BUTTON_SIZE - 10, 100, PAUSE_BUTTON_SIZE, PAUSE_BUTTON_SIZE};

    // Check if mouse is hovering over the button
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    isHoveringPauseButton = (mouseX >= pauseButtonRect.x && mouseX <= pauseButtonRect.x + PAUSE_BUTTON_SIZE &&
                            mouseY >= pauseButtonRect.y && mouseY <= pauseButtonRect.y + PAUSE_BUTTON_SIZE);

    // Render the appropriate texture based on hover state
    if (isHoveringPauseButton) {
        SDL_RenderCopy(renderer, pauseButtonHoverTexture, nullptr, &pauseButtonRect);
    } else {
        SDL_RenderCopy(renderer, pauseButtonTexture, nullptr, &pauseButtonRect);
    }
}

void resetGame() {
    player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT, 0, false, false, false, false, 0, 0, 0, false};
    fallingObjects.clear();
    fallingObjects.push_back({rand() % (SCREEN_WIDTH - OBJECT_SIZE), 0, fallingObjectSpeed, false});
    groundObject = {-OBJECT_SIZE, SCREEN_HEIGHT - OBJECT_SIZE - GROUND_HEIGHT, 3, false};
    food = {0, 0, 3, false};
    shield = {0, 0, 3, false};
    wing = {0, 0, 3, false, 0};
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
}

void spawnArrows(int centerX, int centerY) {
    for (double angle = 0; angle < 2 * PI; angle += PI / 8) { // Shoot arrows in 8 directions
        arrows.push_back({centerX, centerY, angle, true});
    }
}

void updatePoolBall() {
    if (!poolBall.active) return;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - poolBall.spawnTime >= POOL_BALL_DURATION) {
        poolBall.active = false;
        return;
    }

    // Move ball based on angle
    poolBall.x += static_cast<int>(POOL_BALL_SPEED * cos(poolBall.angle));
    poolBall.y += static_cast<int>(POOL_BALL_SPEED * sin(poolBall.angle));

    // Check for collisions with walls and bounce
    if (poolBall.x <= 0) { // Left wall
        poolBall.x = 0;
        poolBall.angle = PI - poolBall.angle; // Reflect horizontally
    }
    else if (poolBall.x >= SCREEN_WIDTH - POOL_BALL_SIZE) { // Right wall
        poolBall.x = SCREEN_WIDTH - POOL_BALL_SIZE;
        poolBall.angle = PI - poolBall.angle; // Reflect horizontally
    }

    if (poolBall.y <= 0) { // Top wall
        poolBall.y = 0;
        poolBall.angle = -poolBall.angle; // Reflect vertically
    }
    else if (poolBall.y >= SCREEN_HEIGHT - POOL_BALL_SIZE - GROUND_HEIGHT) { // Bottom wall (above ground)
        poolBall.y = SCREEN_HEIGHT - POOL_BALL_SIZE - GROUND_HEIGHT;
        poolBall.angle = -poolBall.angle; // Reflect vertically
    }
}

void renderPowerUpStatus() {
    Uint32 currentTime = SDL_GetTicks();
    int iconY = 10;

    // Render speed boost status
    if (player.speedBoostActive) {
        SDL_Rect iconRect = {10, iconY, POWER_UP_ICON_SIZE, POWER_UP_ICON_SIZE};
        SDL_RenderCopy(renderer, foodTexture, nullptr, &iconRect);

        // Calculate remaining time percentage
        float timeLeft = (player.speedBoostEndTime - currentTime) / (float)SPEED_BOOST_DURATION;
        if (timeLeft > 0) {
            SDL_Rect timeBar = {10 + POWER_UP_ICON_SIZE + 5, iconY + POWER_UP_ICON_SIZE / 2 - 5,
                               static_cast<int>(100 * timeLeft), 10};
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &timeBar);
        }
        iconY += POWER_UP_ICON_SIZE + 10;
    }

    // Render immortality status
    if (player.isImmortal) {
        SDL_Rect iconRect = {10, iconY, POWER_UP_ICON_SIZE, POWER_UP_ICON_SIZE};
        SDL_RenderCopy(renderer, shieldTexture, nullptr, &iconRect);

        // Calculate remaining time percentage
        float timeLeft = (player.immortalEndTime - currentTime) / (float)IMMORTAL_DURATION;
        if (timeLeft > 0) {
            SDL_Rect timeBar = {10 + POWER_UP_ICON_SIZE + 5, iconY + POWER_UP_ICON_SIZE / 2 - 5,
                               static_cast<int>(100 * timeLeft), 10};
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &timeBar);
        }
        iconY += POWER_UP_ICON_SIZE + 10;
    }

    // Render wing status
    if (player.wingActive) {
        SDL_Rect iconRect = {10, iconY, POWER_UP_ICON_SIZE, POWER_UP_ICON_SIZE};
        SDL_RenderCopy(renderer, wingTexture, nullptr, &iconRect);

        // Calculate remaining time percentage
        float timeLeft = (player.wingEndTime - currentTime) / (float)WING_DURATION;
        if (timeLeft > 0) {
            SDL_Rect timeBar = {10 + POWER_UP_ICON_SIZE + 5, iconY + POWER_UP_ICON_SIZE / 2 - 5,
                               static_cast<int>(100 * timeLeft), 10};
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &timeBar);
        }
    }
}

int main(int argc, char* argv[]) {
    if (!init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return -1;
    }

    if (!loadTextures()) {
        std::cerr << "Failed to load textures!" << std::endl;
        return -1;
    }

    // Load maxScore from file
    loadMaxScore();

    srand(static_cast<unsigned int>(time(0)));

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Handle main menu, level menu, and game over menu clicks
                if (showMainMenu) {
                    if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                        if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                            showMainMenu = false;
                            showLevelMenu = true;
                        } else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                            quit = true;
                        }
                    }
                } else if (showLevelMenu) {
                    if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                        if (mouseY >= SCREEN_HEIGHT / 2 - 100 && mouseY <= SCREEN_HEIGHT / 2 - 60) {
                            fallingObjectSpeed = 4; // Level 1
                            POOL_BALL_SPEED = 7;
                            showLevelMenu = false;
                            ARROW_SPEED = 3;
                            resetGame();
                        } else if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                            fallingObjectSpeed = 9; // Level 2
                            showLevelMenu = false;
                             POOL_BALL_SPEED = 14;
                             ARROW_SPEED = 5;
                            resetGame();
                        } else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                            fallingObjectSpeed = 15; // Level 3
                            showLevelMenu = false;
                             POOL_BALL_SPEED = 21;
                            ARROW_SPEED = 7;
                            resetGame();
                        } else if (mouseY >= SCREEN_HEIGHT / 2 + 50 && mouseY <= SCREEN_HEIGHT / 2 + 90) {
                            quit = true;
                        }
                    }
                } else if (showGameOverMenu) {
                    if (mouseX >= SCREEN_WIDTH / 2 - 50 && mouseX <= SCREEN_WIDTH / 2 + 50) {
                        if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                            showGameOverMenu = false;
                            showLevelMenu = true;
                        } else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                            quit = true;
                        }
                    }
                }

                // Handle pause button click
                if (!showMainMenu && !showLevelMenu && !showGameOverMenu && isHoveringPauseButton) {
                    isPaused = !isPaused;
                }
            } else if (e.type == SDL_KEYDOWN) {
                // Handle ESC key for pausing
                if (e.key.keysym.sym == SDLK_ESCAPE && !showMainMenu && !showLevelMenu && !showGameOverMenu) {
                    isPaused = !isPaused;
                }
            }
        }

        if (showMainMenu || showLevelMenu || showGameOverMenu) {
            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Render menu
            renderMenu();

            // Update screen
            SDL_RenderPresent(renderer);

            continue;
        }

        // Skip game updates if paused
        if (isPaused) {
            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Render background
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

            // Render all game objects (they won't move while paused)
            SDL_Rect playerRect = {player.x, player.y, PLAYER_SIZE, PLAYER_SIZE};
            SDL_Point center = {PLAYER_SIZE / 2, PLAYER_SIZE / 2};
            if (player.facingRight) {
                SDL_RenderCopyEx(renderer, playerTexture, nullptr, &playerRect, 0, &center, SDL_FLIP_HORIZONTAL);
            } else {
                SDL_RenderCopyEx(renderer, playerTexture, nullptr, &playerRect, 180, nullptr,SDL_FLIP_NONE );
            }

            for (const auto& obj : fallingObjects) {
                if (obj.active) {
                    SDL_Rect objectRect = {obj.x, obj.y, OBJECT_SIZE, OBJECT_SIZE};
                    SDL_RenderCopy(renderer, objectTexture, nullptr, &objectRect);
                }
            }

            if (groundObject.active) {
                SDL_Rect groundObjectRect = {groundObject.x, groundObject.y, OBJECT_SIZE+20, OBJECT_SIZE+20};
                SDL_RenderCopy(renderer, groundObjectTexture, nullptr, &groundObjectRect);
            }

            if (food.active) {
                SDL_Rect foodRect = {food.x, food.y, OBJECT_SIZE, OBJECT_SIZE};
                SDL_RenderCopy(renderer, foodTexture, nullptr, &foodRect);
            }

            if (shield.active) {
                SDL_Rect shieldRect = {shield.x, shield.y, OBJECT_SIZE, OBJECT_SIZE};
                SDL_RenderCopy(renderer, shieldTexture, nullptr, &shieldRect);
            }

            if (wing.active) {
                SDL_Rect wingRect = {wing.x, wing.y, OBJECT_SIZE, OBJECT_SIZE};
                SDL_RenderCopy(renderer, wingTexture, nullptr, &wingRect);
            }

            if (cloud.active) {
                SDL_Rect cloudRect = {cloud.x, cloud.y, OBJECT_SIZE+50, OBJECT_SIZE};
                SDL_RenderCopy(renderer, cloudTexture, nullptr, &cloudRect);
            }

            if (lightning.active) {
                SDL_Rect lightningRect = {lightning.x, 0, OBJECT_SIZE, SCREEN_HEIGHT - GROUND_HEIGHT};
                SDL_RenderCopy(renderer, lightningTexture, nullptr, &lightningRect);
            }

            if (boss.active) {
                SDL_Rect bossRect = {boss.x, boss.y, BOSS_SIZE, BOSS_SIZE};
                SDL_RenderCopy(renderer, BossTexture, nullptr, &bossRect);
            }

            if (bower.active) {
                SDL_Rect bowerRect = {bower.x, bower.y, BOWER_SIZE, BOWER_SIZE};
                SDL_RenderCopy(renderer, bowerTexture, nullptr, &bowerRect);
            }

            for (const auto& arrow : arrows) {
                if (arrow.active) {
                    SDL_Rect arrowRect = {arrow.x, arrow.y, ARROW_SIZE, ARROW_SIZE};
                    double degrees = arrow.angle * 180 / PI;
                    SDL_RenderCopyEx(renderer, arrowTexture, nullptr, &arrowRect, degrees, nullptr, SDL_FLIP_NONE);
                }
            }

            if (poolBall.active) {
                SDL_Rect ballRect = {poolBall.x, poolBall.y, POOL_BALL_SIZE, POOL_BALL_SIZE};
                double degrees = (SDL_GetTicks() / 10) % 360;
                SDL_RenderCopyEx(renderer, poolBallTexture, nullptr, &ballRect, degrees, nullptr, SDL_FLIP_NONE);
            }

            // Render score, max score, and round
            renderText("Score: " + std::to_string(score), SCREEN_WIDTH - 200, 10);
            renderText("Max Score: " + std::to_string(maxScore), SCREEN_WIDTH - 200, 40);
            renderText("Round: " + std::to_string(roundNumber), SCREEN_WIDTH - 200, 70);

            // Render power-up status indicators
            renderPowerUpStatus();

            // Render pause button
            renderPauseButton();

            // Render "PAUSED" text
            renderText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);

            // Update screen
            SDL_RenderPresent(renderer);

            continue;
        }

        // Handle player movement
        const Uint8* keystates = SDL_GetKeyboardState(nullptr);
        int currentPlayerSpeed = player.speedBoostActive ? PLAYER_SPEED * 2 : PLAYER_SPEED; // Double speed if speed boost is active

        if (keystates[SDL_SCANCODE_A] && player.x > 0) { // Move left with "A"
            player.x -= currentPlayerSpeed;
            // Reset rotation when moving left
            player.facingRight = false;
        }
        if (keystates[SDL_SCANCODE_D] && player.x < SCREEN_WIDTH - PLAYER_SIZE) { // Move right with "D"
            player.x += currentPlayerSpeed;
            player.facingRight = true;
                    }
        if (keystates[SDL_SCANCODE_W] && !player.isJumping && !player.wingActive) { // Jump with "W"
            player.velocity = JUMP_STRENGTH;
            player.isJumping = true;
        }

        // Apply gravity if not in wing mode
        if (!player.wingActive) {
            player.velocity += GRAVITY;
            player.y += player.velocity;

            // Prevent player from falling through the ground
            if (player.y > SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT) {
                player.y = SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT;
                player.velocity = 0;
                player.isJumping = false; // Reset rotation when on ground
            }

            // Quick landing with "S" button
            if (keystates[SDL_SCANCODE_S] && player.isJumping) {
                player.velocity = 10; // Increase falling speed
            }
        } else {
            // Wing mode: No gravity, can hover
            if (keystates[SDL_SCANCODE_W]) { // Hover up
                player.y -= currentPlayerSpeed;
                if (player.y < 0) { // Prevent going above the screen
                    player.y = 0;
                }
            }
            if (keystates[SDL_SCANCODE_S]) { // Move down
                player.y += currentPlayerSpeed;
                if (player.y > SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT) { // Prevent going below the ground
                    player.y = SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT;
                }
            }
        }

        // Spawn falling objects with a 2-second delay
        Uint32 currentTime = SDL_GetTicks();
        if (currentObjectIndex < fallingObjects.size()) {
            if (!fallingObjects[currentObjectIndex].active && (currentTime - lastObjectSpawnTime >= FALLING_OBJECT_DELAY)) {
                // Activate the next falling object
                fallingObjects[currentObjectIndex].x = rand() % (SCREEN_WIDTH - OBJECT_SIZE); // Random x-coordinate
                fallingObjects[currentObjectIndex].y = 0;
                fallingObjects[currentObjectIndex].active = true;
                lastObjectSpawnTime = currentTime; // Update the last spawn time
                currentObjectIndex++; // Move to the next object
            }
        } else {
            // All objects in the round have been spawned
            bool allObjectsFallen = true;
            for (const auto& obj : fallingObjects) {
                if (obj.active) {
                    allObjectsFallen = false;
                    break;
                }
            }
            if (allObjectsFallen) {
                // Proceed to the next round
                roundNumber++;
                currentObjectIndex = 0; // Reset the index
                fallingObjects.push_back({rand() % (SCREEN_WIDTH - OBJECT_SIZE), 0, fallingObjectSpeed, false}); // Add a new object
            }
        }

        // Update falling objects
        for (auto& obj : fallingObjects) {
            if (obj.active) {
                obj.y += obj.speed;
                if (obj.y > SCREEN_HEIGHT - GROUND_HEIGHT) {
                    obj.active = false; // Object has fallen
                    score += 10; // Increase score
                    if (score > maxScore) {
                        maxScore = score;
                    }
                }
            }
        }

        // Spawn power-ups every 60 seconds
        if (currentTime - lastPowerUpSpawnTime >= POWER_UP_INTERVAL) {
            lastPowerUpSpawnTime = currentTime;
            // Spawn food, shield, or wing alternately
            if (!food.active && !shield.active && !wing.active) {
                int randomChoice = rand() % 3; // Randomly choose between food, shield, or wing
                if (randomChoice == 0) {
                    food.x = rand() % (SCREEN_WIDTH - OBJECT_SIZE);
                    food.y = 0;
                    food.active = true;
                } else if (randomChoice == 1) {
                    shield.x = rand() % (SCREEN_WIDTH - OBJECT_SIZE);
                    shield.y = 0;
                    shield.active = true;
                } else {
                    wing.x = rand() % (SCREEN_WIDTH - OBJECT_SIZE);
                    wing.y = 0;
                    wing.active = true;
                    wing.spawnTime = currentTime;
                }
            }
        }

        // Spawn pool ball every 20 points
        if (score % 200 == 0 && score != 0 && !poolBall.active) {
            poolBall.x = rand() % (SCREEN_WIDTH - POOL_BALL_SIZE);
            poolBall.y = rand() % (SCREEN_HEIGHT / 2); // Spawn in upper half
            poolBall.angle = (rand() % 360) * PI / 180.0; // Random angle in radians
            poolBall.spawnTime = currentTime;
            poolBall.active = true;
        }

        // Update pool ball
        updatePoolBall();

        // Update power-ups
        if (food.active) {
            food.y += food.speed;
            if (food.y > SCREEN_HEIGHT - GROUND_HEIGHT) {
                food.active = false; // Food has fallen off the screen
            }
        }
        if (shield.active) {
            shield.y += shield.speed;
            if (shield.y > SCREEN_HEIGHT - GROUND_HEIGHT) {
                shield.active = false; // Shield has fallen off the screen
            }
        }
        if (wing.active) {
            wing.y += wing.speed;
            if (wing.y > SCREEN_HEIGHT - GROUND_HEIGHT) {
                wing.active = false; // Wing has fallen off the screen
            }
        }

        // Check for collisions with power-ups
        if (food.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, food.x, food.y, OBJECT_SIZE, OBJECT_SIZE)) {
            food.active = false; // Deactivate food
            player.speedBoostActive = true; // Activate speed boost
            player.speedBoostEndTime = currentTime + SPEED_BOOST_DURATION; // Set speed boost end time
        }
        if (shield.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, shield.x, shield.y, OBJECT_SIZE, OBJECT_SIZE)) {
            shield.active = false; // Deactivate shield
            player.immortalEndTime = currentTime + IMMORTAL_DURATION; // Activate immortality
            player.isImmortal = true;
        }
        if (wing.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, wing.x, wing.y, OBJECT_SIZE, OBJECT_SIZE)) {
            wing.active = false; // Deactivate wing
            player.wingActive = true; // Activate wing mode
            player.wingEndTime = currentTime + WING_DURATION; // Set wing end time
        }

        // Reset speed boost if the duration has expired
        if (player.speedBoostActive && currentTime >= player.speedBoostEndTime) {
            player.speedBoostActive = false; // Reset speed boost
        }

        // Reset immortality if the duration has expired
        if (player.isImmortal && currentTime >= player.immortalEndTime) {
            player.isImmortal = false; // Reset immortality
        }

        // Reset wing mode if the duration has expired
        if (player.wingActive && currentTime >= player.wingEndTime) {
            player.wingActive = false; // Reset wing mode
            player.velocity = 0; // Start falling if in the air
        }

        // Update ground object
        if (groundObject.active) {
            groundObject.x += groundObject.speed;
            if (groundObject.x > SCREEN_WIDTH || groundObject.x < -OBJECT_SIZE) {
                groundObject.active = false;
            }
        } else {
            if (rand() % 100 < 1) { // Make ground object appear less frequently
                groundObject.y = SCREEN_HEIGHT - OBJECT_SIZE - GROUND_HEIGHT;
                if (spawnFromLeft) {
                    groundObject.x = -OBJECT_SIZE;
                    groundObject.speed = 3;
                    groundObject.facingright = false;
                } else {
                    groundObject.x = SCREEN_WIDTH;
                    groundObject.speed = -3;
                    groundObject.facingright = true;
                }
                groundObject.active = true;
                spawnFromLeft = !spawnFromLeft; // Alternate spawn side
            }
        }

        // Spawn cloud and lightning every 50 points
        if (score % 80 == 0 && score != 0 && !cloud.active && !lightning.active) {
            cloud.x = rand() % (SCREEN_WIDTH - OBJECT_SIZE); // Random x-coordinate
            cloud.y = 0;
            cloud.spawnTime = currentTime;
            cloud.active = true;
        }

        // Update cloud and lightning
        if (cloud.active) {
            if (currentTime - cloud.spawnTime >= CLOUD_WARNING_DURATION) {
                // Cloud disappears, lightning appears
                cloud.active = false;
                lightning.x = cloud.x;
                lightning.spawnTime = currentTime;
                lightning.active = true;
            }
        }

        if (lightning.active) {
            if (currentTime - lightning.spawnTime >= LIGHTNING_DURATION) {
                // Lightning disappears
                lightning.active = false;
            }
        }

        // Spawn boss every 5 rounds
        if (roundNumber % 5 == 0 && !boss.active) {
            boss.x = rand() % (SCREEN_WIDTH - BOSS_SIZE); // Random x-coordinate
            boss.y = 0;
            boss.spawnTime = currentTime;
            boss.active = true;
        }

        // Update boss
        if (boss.active) {
            // Boss follows the player
            if (boss.x < player.x) {
                boss.x += 1; // Move right
            } else if (boss.x > player.x) {
                boss.x -= 1; // Move left
            }

            // Boss falls down
            boss.y += 2;

            // Check if boss duration has expired
            if (currentTime - boss.spawnTime >= BOSS_DURATION) {
                boss.active = false; // Boss disappears
            }
        }

        // Spawn bower every 10 rounds (rarer than boss)
        if (roundNumber % 8 == 0 && !bower.active) {
            bower.x = rand() % (SCREEN_WIDTH - BOWER_SIZE); // Random x-coordinate
            bower.y = rand() % 200;
            bower.spawnTime = currentTime;
            bower.lastShootTime = currentTime; // Initialize last shoot time
            bower.active = true;
        }

        // Update bower
        if (bower.active) {
            // Shoot arrows rapidly
            if (currentTime - bower.lastShootTime >= ARROW_SHOOT_INTERVAL) {
                bower.x = rand() % (SCREEN_WIDTH - BOWER_SIZE); // Random x-coordinate
                bower.y = rand() % 200;
                spawnArrows(bower.x + BOWER_SIZE / 2, bower.y + BOWER_SIZE / 2); // Shoot arrows from bower
                bower.lastShootTime = currentTime; // Update last shoot time
            }

            // Check if bower duration has expired
            if (currentTime - bower.spawnTime >= BOWER_DURATION) {
                bower.active = false; // Bower disappears
            }
        }

        // Update arrows
        for (auto& arrow : arrows) {
            if (arrow.active) {
                // Move arrow based on its angle
                arrow.x += static_cast<int>(ARROW_SPEED * cos(arrow.angle));
                arrow.y += static_cast<int>(ARROW_SPEED * sin(arrow.angle));

                // Check if arrow is out of bounds
                if (arrow.x < 0 || arrow.x > SCREEN_WIDTH || arrow.y < 0 || arrow.y > SCREEN_HEIGHT - GROUND_HEIGHT) {
                    arrow.active = false; // Deactivate arrow
                }
            }
        }

        // Check for collisions
        bool gameOver = false;
        if (!player.isImmortal) {
            for (const auto& obj : fallingObjects) {
                if (obj.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, obj.x, obj.y, OBJECT_SIZE, OBJECT_SIZE)) {
                    std::cout << "Game Over!" << std::endl;
                    gameOver = true;
                    break;
                }
            }
            if (!gameOver && groundObject.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, groundObject.x, groundObject.y, OBJECT_SIZE, OBJECT_SIZE)) {
                std::cout << "Game Over!" << std::endl;
                gameOver = true;
            }
            if (!gameOver && lightning.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, lightning.x, 0, OBJECT_SIZE, SCREEN_HEIGHT - GROUND_HEIGHT)) {
                std::cout << "Game Over! Hit by lightning!" << std::endl;
                gameOver = true;
            }
            if (!gameOver && boss.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, boss.x, boss.y, BOSS_SIZE, BOSS_SIZE)) {
                std::cout << "Game Over! Hit by boss!" << std::endl;
                gameOver = true;
            }
            if (!gameOver && poolBall.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, poolBall.x, poolBall.y, POOL_BALL_SIZE, POOL_BALL_SIZE)) {
                std::cout << "Game Over! Hit by pool ball!" << std::endl;
                gameOver = true;
            }
            if (!gameOver) {
                for (const auto& arrow : arrows) {
                    if (arrow.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, arrow.x, arrow.y, ARROW_SIZE, ARROW_SIZE)) {
                        std::cout << "Game Over! Hit by arrow!" << std::endl;
                        gameOver = true;
                        break;
                    }
                }
            }
        }

        if (gameOver) {
            // Save maxScore to file before quitting
            saveMaxScore();
            showGameOverMenu = true;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 205);
        SDL_RenderClear(renderer);

        // Render background
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

        // Render player with rotation when moving right
        SDL_Rect playerRect = {player.x, player.y, PLAYER_SIZE, PLAYER_SIZE};
        SDL_Point center = {PLAYER_SIZE / 2, PLAYER_SIZE / 2};
        if (player.facingRight) {
            SDL_RenderCopyEx(renderer, playerTexture, nullptr, &playerRect, 0, &center,  SDL_FLIP_HORIZONTAL);
        } else {
            SDL_RenderCopyEx(renderer, playerTexture, nullptr, &playerRect, 0, nullptr, SDL_FLIP_NONE);
        }

        // Render falling objects
        for (const auto& obj : fallingObjects) {
            if (obj.active) {
                SDL_Rect objectRect = {obj.x, obj.y, OBJECT_SIZE, OBJECT_SIZE};
                SDL_RenderCopy(renderer, objectTexture, nullptr, &objectRect);
            }
        }

        // Render ground object
        if (groundObject.active) {
            SDL_Rect groundObjectRect = {groundObject.x, groundObject.y-10, OBJECT_SIZE+20, OBJECT_SIZE+20};
           if (groundObject.facingright) {
            SDL_RenderCopyEx(renderer, groundObjectTexture, nullptr, &groundObjectRect, 0, &center,  SDL_FLIP_HORIZONTAL);
        } else {
            SDL_RenderCopyEx(renderer, groundObjectTexture, nullptr, &groundObjectRect, 0, nullptr, SDL_FLIP_NONE);
        }
        }

        // Render food
        if (food.active) {
            SDL_Rect foodRect = {food.x, food.y, OBJECT_SIZE, OBJECT_SIZE};
            SDL_RenderCopy(renderer, foodTexture, nullptr, &foodRect);
        }

        // Render shield
        if (shield.active) {
            SDL_Rect shieldRect = {shield.x, shield.y, OBJECT_SIZE, OBJECT_SIZE};
            SDL_RenderCopy(renderer, shieldTexture, nullptr, &shieldRect);
        }

        // Render wing
        if (wing.active) {
            SDL_Rect wingRect = {wing.x, wing.y, OBJECT_SIZE, OBJECT_SIZE};
            SDL_RenderCopy(renderer, wingTexture, nullptr, &wingRect);
        }

        // Render cloud
        if (cloud.active) {
            SDL_Rect cloudRect = {cloud.x, cloud.y, OBJECT_SIZE+70, OBJECT_SIZE+40};
            SDL_RenderCopy(renderer, cloudTexture, nullptr, &cloudRect);
        }

        // Render lightning
        if (lightning.active) {
            SDL_Rect lightningRect = {lightning.x, 0, OBJECT_SIZE, SCREEN_HEIGHT - GROUND_HEIGHT};
            SDL_RenderCopy(renderer, lightningTexture, nullptr, &lightningRect);
        }

        // Render boss
        if (boss.active) {
            SDL_Rect bossRect = {boss.x, boss.y, BOSS_SIZE, BOSS_SIZE};
            SDL_RenderCopy(renderer, BossTexture, nullptr, &bossRect);
        }

        // Render bower
        if (bower.active) {
            SDL_Rect bowerRect = {bower.x, bower.y, BOWER_SIZE, BOWER_SIZE};
            SDL_RenderCopy(renderer, bowerTexture, nullptr, &bowerRect);
        }

        // Render arrows
        for (const auto& arrow : arrows) {
            if (arrow.active) {
                SDL_Rect arrowRect = {arrow.x, arrow.y, ARROW_SIZE, ARROW_SIZE};
                // Rotate arrow based on its angle
                double degrees = arrow.angle * 180 / PI;
                SDL_RenderCopyEx(renderer, arrowTexture, nullptr, &arrowRect, degrees, nullptr, SDL_FLIP_NONE);
            }
        }

        // Render pool ball
        if (poolBall.active) {
            SDL_Rect ballRect = {poolBall.x, poolBall.y, POOL_BALL_SIZE, POOL_BALL_SIZE};
            // Rotate the ball based on its movement
            double degrees = (SDL_GetTicks() / 10) % 360; // Continuous rotation
            SDL_RenderCopyEx(renderer, poolBallTexture, nullptr, &ballRect, degrees, nullptr, SDL_FLIP_NONE);
        }

        // Render score, max score, and round
        renderText("Score: " + std::to_string(score), SCREEN_WIDTH - 200, 10);
        renderText("Max Score: " + std::to_string(maxScore), SCREEN_WIDTH - 200, 40);
        renderText("Round: " + std::to_string(roundNumber), SCREEN_WIDTH - 200, 70);

        // Render power-up status indicators
        renderPowerUpStatus();

        // Render pause button
        renderPauseButton();

        // Update screen
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    close();
    return 0;
}