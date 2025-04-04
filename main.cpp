#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fstream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SIZE = 50;
const int OBJECT_SIZE = 30;
const int GRAVITY = 1;
const int PLAYER_SPEED = 5;
const int JUMP_STRENGTH = -15;
const int FALLING_OBJECT_DELAY = 2000; // 2-second delay between falling objects

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* playerTexture = nullptr;
SDL_Texture* objectTexture = nullptr;
SDL_Texture* groundObjectTexture = nullptr;
TTF_Font* font = nullptr;

struct Player {
    int x, y;
    int velocity;
    bool isJumping;
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

Player player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT - PLAYER_SIZE, 0, false};
std::vector<FallingObject> fallingObjects;
GroundObject groundObject = {-OBJECT_SIZE, SCREEN_HEIGHT - OBJECT_SIZE, 3, false};

int score = 0;
int maxScore = 0;
int roundNumber = 1;
int currentObjectIndex = 0; // Tracks which object is currently falling
Uint32 lastObjectSpawnTime = 0; // Tracks the last time a falling object was spawned
bool spawnFromLeft = true; // Toggle for ground object spawn side

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
    if (!playerTexture || !objectTexture || !groundObjectTexture) {
        std::cerr << "Failed to load textures!" << std::endl;
        return false;
    }
    return true;
}

void close() {
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(objectTexture);
    SDL_DestroyTexture(groundObjectTexture);
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

    // Initialize the first falling object for round 1
    fallingObjects.push_back({rand() % (SCREEN_WIDTH - OBJECT_SIZE), 0, 3, false});

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Handle player movement
        const Uint8* keystates = SDL_GetKeyboardState(nullptr);
        if (keystates[SDL_SCANCODE_A] && player.x > 0) { // Move left with "A"
            player.x -= PLAYER_SPEED;
        }
        if (keystates[SDL_SCANCODE_D] && player.x < SCREEN_WIDTH - PLAYER_SIZE) { // Move right with "D"
            player.x += PLAYER_SPEED;
        }
        if (keystates[SDL_SCANCODE_W] && !player.isJumping) { // Jump with "W"
            player.velocity = JUMP_STRENGTH;
            player.isJumping = true;
        }

        // Apply gravity
        player.velocity += GRAVITY;
        player.y += player.velocity;

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
                fallingObjects[currentObjectIndex].x = rand() % (SCREEN_WIDTH - OBJECT_SIZE);
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
                fallingObjects.push_back({rand() % (SCREEN_WIDTH - OBJECT_SIZE), 0, 3, false}); // Add a new object
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

        // Update ground object
        if (groundObject.active) {
            groundObject.x += groundObject.speed;
            if (groundObject.x > SCREEN_WIDTH || groundObject.x < -OBJECT_SIZE) {
                groundObject.active = false;
            }
        } else {
            if (rand() % 100 < 2) { // Make ground object appear less frequently
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

        // Check for collisions
        bool gameOver = false;
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

        if (gameOver) {
            // Save maxScore to file before quitting
            saveMaxScore();
            quit = true;
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