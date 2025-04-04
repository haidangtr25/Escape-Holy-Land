#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 680;
const int PLAYER_SIZE = 50;
const int OBJECT_SIZE = 30;
const int GRAVITY = 1;
const int PLAYER_SPEED = 5;
const int JUMP_STRENGTH = -15;
const int FALLING_OBJECT_DELAY = 400; // 2-second delay between falling objects
const int POWER_UP_INTERVAL = 20000; // 60 seconds
const int SPEED_BOOST_DURATION = 12000; // 12 seconds
const int IMMORTAL_DURATION = 10000; // 10 seconds
const int WING_DURATION = 15000; // 15 seconds
const int CLOUD_WARNING_DURATION = 1500; // 1.5 seconds
const int LIGHTNING_DURATION = 1000; // 1 second
const int BOSS_SIZE = 100; // Boss size
const int BOSS_DURATION = 6000; // 5 seconds

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* playerTexture = nullptr;
SDL_Texture* objectTexture = nullptr;
SDL_Texture* groundObjectTexture = nullptr;
SDL_Texture* foodTexture = nullptr;
SDL_Texture* shieldTexture = nullptr;
SDL_Texture* wingTexture = nullptr; // New texture for wing power-up
SDL_Texture* cloudTexture = nullptr;
SDL_Texture* lightningTexture = nullptr;
TTF_Font* font = nullptr;

struct Player {
    int x, y;
    int velocity;
    bool isJumping;
    bool isImmortal;
    bool speedBoostActive; // Track if speed boost is active
    bool wingActive; // Track if wing power-up is active
    Uint32 speedBoostEndTime;
    Uint32 immortalEndTime;
    Uint32 wingEndTime; // Track when the wing power-up expires
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
};

struct PowerUp {
    int x, y;
    int speed;
    bool active;
};

Player player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - PLAYER_SIZE, 0, false, false, false, false, 0, 0, 0};
std::vector<FallingObject> fallingObjects;
GroundObject groundObject = {-OBJECT_SIZE, SCREEN_HEIGHT - OBJECT_SIZE, 3, false};
PowerUp food = {0, 0, 3, false};
PowerUp shield = {0, 0, 3, false};
PowerUp wing = {0, 0, 3, false}; // New wing power-up
Cloud cloud = {0, 0, 0, false};
Lightning lightning = {0, 0, false};
Boss boss = {0, 0, 0, false};

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
    playerTexture = IMG_LoadTexture(renderer, "player.png"); // Replace with your player image
    objectTexture = IMG_LoadTexture(renderer, "object.png"); // Replace with your object image
    groundObjectTexture = IMG_LoadTexture(renderer, "gift.png"); // Replace with your ground object image
    foodTexture = IMG_LoadTexture(renderer, "food.png"); // Replace with your food image
    shieldTexture = IMG_LoadTexture(renderer, "shield.png"); // Replace with your shield image
    wingTexture = IMG_LoadTexture(renderer, "wing.png"); // New texture for wing power-up
    cloudTexture = IMG_LoadTexture(renderer, "cloud.png"); // Replace with your cloud image
    lightningTexture = IMG_LoadTexture(renderer, "lightning.png"); // Replace with your lightning image
    if (!playerTexture || !objectTexture || !groundObjectTexture || !foodTexture || !shieldTexture || !wingTexture || !cloudTexture || !lightningTexture) {
        std::cerr << "Failed to load textures!" << std::endl;
        return false;
    }
    return true;
}

void close() {
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(objectTexture);
    SDL_DestroyTexture(groundObjectTexture);
    SDL_DestroyTexture(foodTexture);
    SDL_DestroyTexture(shieldTexture);
    SDL_DestroyTexture(wingTexture); // Destroy wing texture
    SDL_DestroyTexture(cloudTexture);
    SDL_DestroyTexture(lightningTexture);
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
        renderText("Level 1", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 100);
        renderText("Level 2", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderText("Level 3", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
        renderText("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 50);
    } else if (showGameOverMenu) {
        renderText("Play Again", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderText("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
    }
}

void resetGame() {
    player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - PLAYER_SIZE, 0, false, false, false, false, 0, 0, 0};
    fallingObjects.clear();
    fallingObjects.push_back({rand() % (SCREEN_WIDTH - OBJECT_SIZE), 0, fallingObjectSpeed, false});
    groundObject = {-OBJECT_SIZE, SCREEN_HEIGHT - OBJECT_SIZE, 3, false};
    food = {0, 0, 3, false};
    shield = {0, 0, 3, false};
    wing = {0, 0, 3, false}; // Reset wing power-up
    cloud = {0, 0, 0, false};
    lightning = {0, 0, false};
    boss = {0, 0, 0, false};
    score = 0;
    currentObjectIndex = 0;
    lastObjectSpawnTime = 0;
    spawnFromLeft = true;
    showGameOverMenu = false;
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
                            fallingObjectSpeed = 3 * 2; // Level 1: Default speed (3) multiplied by 2
                            showLevelMenu = false;
                            resetGame();
                        } else if (mouseY >= SCREEN_HEIGHT / 2 - 50 && mouseY <= SCREEN_HEIGHT / 2 - 10) {
                            fallingObjectSpeed = 5 * 2; // Level 2: Default speed (5) multiplied by 2
                            showLevelMenu = false;
                            resetGame();
                        } else if (mouseY >= SCREEN_HEIGHT / 2 && mouseY <= SCREEN_HEIGHT / 2 + 40) {
                            fallingObjectSpeed = 7 * 2; // Level 3: Default speed (7) multiplied by 2
                            showLevelMenu = false;
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

        // Handle player movement
        const Uint8* keystates = SDL_GetKeyboardState(nullptr);
        int currentPlayerSpeed = player.speedBoostActive ? PLAYER_SPEED * 2 : PLAYER_SPEED; // Double speed if speed boost is active

        if (player.wingActive) {
            // Wing power-up is active: allow free movement up and down
            if (keystates[SDL_SCANCODE_W] && player.y > 0) { // Move up with "W"
                player.y -= PLAYER_SPEED;
            }
            if (keystates[SDL_SCANCODE_S] && player.y < SCREEN_HEIGHT - PLAYER_SIZE) { // Move down with "S"
                player.y += PLAYER_SPEED;
            }
        } else {
            // Normal movement with gravity
            if (keystates[SDL_SCANCODE_A] && player.x > 0) { // Move left with "A"
                player.x -= currentPlayerSpeed;
            }
            if (keystates[SDL_SCANCODE_D] && player.x < SCREEN_WIDTH - PLAYER_SIZE) { // Move right with "D"
                player.x += currentPlayerSpeed;
            }
            if (keystates[SDL_SCANCODE_W] && !player.isJumping) { // Jump with "W"
                player.velocity = JUMP_STRENGTH;
                player.isJumping = true;
            }
        }

        // Apply gravity (only if wing power-up is not active)
        if (!player.wingActive) {
            player.velocity += GRAVITY;
            player.y += player.velocity;
        }

        // Prevent player from falling through the ground
        if (player.y > SCREEN_HEIGHT - PLAYER_SIZE) {
            player.y = SCREEN_HEIGHT - PLAYER_SIZE;
            player.velocity = 0;
            player.isJumping = false;
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
                if (obj.y > SCREEN_HEIGHT) {
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
                } else if (randomChoice == 2) {
                    wing.x = rand() % (SCREEN_WIDTH - OBJECT_SIZE);
                    wing.y = 0;
                    wing.active = true;
                }
            }
        }

        // Update power-ups
        if (food.active) {
            food.y += food.speed;
            if (food.y > SCREEN_HEIGHT) {
                food.active = false; // Food has fallen off the screen
            }
        }
        if (shield.active) {
            shield.y += shield.speed;
            if (shield.y > SCREEN_HEIGHT) {
                shield.active = false; // Shield has fallen off the screen
            }
        }
        if (wing.active) {
            wing.y += wing.speed;
            if (wing.y > SCREEN_HEIGHT) {
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
            player.wingActive = true; // Activate wing power-up
            player.wingEndTime = currentTime + WING_DURATION; // Set wing power-up end time
        }

        // Reset speed boost if the duration has expired
        if (player.speedBoostActive && currentTime >= player.speedBoostEndTime) {
            player.speedBoostActive = false; // Reset speed boost
        }

        // Reset immortality if the duration has expired
        if (player.isImmortal && currentTime >= player.immortalEndTime) {
            player.isImmortal = false; // Reset immortality
        }

        // Reset wing power-up if the duration has expired
        if (player.wingActive && currentTime >= player.wingEndTime) {
            player.wingActive = false; // Reset wing power-up
        }

        // Update ground object
        if (groundObject.active) {
            groundObject.x += groundObject.speed;
            if (groundObject.x > SCREEN_WIDTH || groundObject.x < -OBJECT_SIZE) {
                groundObject.active = false;
            }
        } else {
            if (rand() % 100 < 1) { // Make ground object appear less frequently
                groundObject.y = SCREEN_HEIGHT - OBJECT_SIZE;
                if (spawnFromLeft) {
                    groundObject.x = -OBJECT_SIZE;
                    groundObject.speed = 3;
                } else {
                    groundObject.x = SCREEN_WIDTH;
                    groundObject.speed = -3;
                }
                groundObject.active = true;
                spawnFromLeft = !spawnFromLeft; // Alternate spawn side
            }
        }

        // Spawn cloud and lightning every 50 points
        if (score % 50 == 0 && score != 0 && !cloud.active && !lightning.active) {
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
                boss.x += 2; // Move right
            } else if (boss.x > player.x) {
                boss.x -= 2; // Move left
            }

            // Boss falls down
            boss.y += 2;

            // Check if boss duration has expired
            if (currentTime - boss.spawnTime >= BOSS_DURATION) {
                boss.active = false; // Boss disappears
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
            if (!gameOver && lightning.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, lightning.x, 0, OBJECT_SIZE, SCREEN_HEIGHT)) {
                std::cout << "Game Over! Hit by lightning!" << std::endl;
                gameOver = true;
            }
            if (!gameOver && boss.active && checkCollision(player.x, player.y, PLAYER_SIZE, PLAYER_SIZE, boss.x, boss.y, BOSS_SIZE, BOSS_SIZE)) {
                std::cout << "Game Over! Hit by boss!" << std::endl;
                gameOver = true;
            }
        }

        if (gameOver) {
            // Save maxScore to file before quitting
            saveMaxScore();
            showGameOverMenu = true;
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render player
        SDL_Rect playerRect = {player.x, player.y, PLAYER_SIZE, PLAYER_SIZE};
        SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);

        // Render falling objects
        for (const auto& obj : fallingObjects) {
            if (obj.active) {
                SDL_Rect objectRect = {obj.x, obj.y, OBJECT_SIZE, OBJECT_SIZE};
                SDL_RenderCopy(renderer, objectTexture, nullptr, &objectRect);
            }
        }

        // Render ground object
        if (groundObject.active) {
            SDL_Rect groundObjectRect = {groundObject.x, groundObject.y, OBJECT_SIZE, OBJECT_SIZE};
            SDL_RenderCopy(renderer, groundObjectTexture, nullptr, &groundObjectRect);
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
            SDL_Rect cloudRect = {cloud.x, cloud.y, OBJECT_SIZE, OBJECT_SIZE};
            SDL_RenderCopy(renderer, cloudTexture, nullptr, &cloudRect);
        }

        // Render lightning
        if (lightning.active) {
            SDL_Rect lightningRect = {lightning.x, 0, OBJECT_SIZE, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, lightningTexture, nullptr, &lightningRect);
        }

        // Render boss
        if (boss.active) {
            SDL_Rect bossRect = {boss.x, boss.y, BOSS_SIZE, BOSS_SIZE};
            SDL_RenderCopy(renderer, objectTexture, nullptr, &bossRect);
        }

        // Render score, max score, and round
        renderText("Score: " + std::to_string(score), SCREEN_WIDTH - 200, 10);
        renderText("Max Score: " + std::to_string(maxScore), SCREEN_WIDTH - 200, 40);
        renderText("Round: " + std::to_string(roundNumber), SCREEN_WIDTH - 200, 70);

        // Update screen
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    close();
    return 0;
}