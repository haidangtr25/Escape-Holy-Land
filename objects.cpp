#include "objects.h"
#include "defs.h"
#include <vector>
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

// FallingObject implementations
void FallingObject::update() {
    if (active) {
        y += speed;
        if (y > SCREEN_HEIGHT - GROUND_HEIGHT) {
            score += 10;
            active = false;
        }
    }
}

void FallingObject::spawn(int screenWidth) {
    x = rand() % (screenWidth - OBJECT_SIZE);
    y = 0;
    active = true;
}

void FallingObject::render() const {
    if (active) {
        SDL_Rect objectRect = {x, y, OBJECT_SIZE, OBJECT_SIZE};
        SDL_RenderCopy(renderer, objectTexture, nullptr, &objectRect);
    }
}

// GroundObject implementations
void GroundObject::update() {
    if (active) {
        x += speed;
        if (x > SCREEN_WIDTH || x < -OBJECT_SIZE) {
            active = false;
            score += 10;
        }
    }
}

void GroundObject::spawn(bool fromLeft, int screenWidth, int screenHeight) {
    y = screenHeight - OBJECT_SIZE - GROUND_HEIGHT;
    if (fromLeft) {
        x = -OBJECT_SIZE;
        speed = 3;
        facingright = false;
    } else {
        x = screenWidth;
        speed = -3;
        facingright = true;
    }
    active = true;
}

void GroundObject::render() const {
    if (active) {
        SDL_Rect groundObjectRect = {x, y-10, OBJECT_SIZE+10, OBJECT_SIZE+10};
        SDL_Point center = {PLAYER_SIZE / 2, PLAYER_SIZE / 2};
        if (facingright) {
            SDL_RenderCopyEx(renderer, groundObjectTexture, nullptr, &groundObjectRect, 0, &center, SDL_FLIP_HORIZONTAL);
        } else {
            SDL_RenderCopyEx(renderer, groundObjectTexture, nullptr, &groundObjectRect, 0, nullptr, SDL_FLIP_NONE);
        }
    }
}

// PowerUp implementations
void PowerUp::update() {
    if (active) {
        y += speed;
        if (y > SCREEN_HEIGHT - GROUND_HEIGHT) {
            active = false;
            score += 10;
        }
    }
}

void PowerUp::spawn(int screenWidth) {
    x = rand() % (screenWidth - OBJECT_SIZE);
    y = 0;
    active = true;
}

void PowerUp::render() const {
    if (active) {
        SDL_Rect rect = {x, y, OBJECT_SIZE, OBJECT_SIZE};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}

// Wing implementations
void Wing::update() {
    if (active) {
        y += speed;
        if (y > SCREEN_HEIGHT - GROUND_HEIGHT) {
            active = false;
        }
    }
}

void Wing::spawn(int screenWidth, Uint32 currentTime) {
    x = rand() % (screenWidth - OBJECT_SIZE);
    y = 0;
    active = true;
    spawnTime = currentTime;
}

void Wing::render() const {
    if (active) {
        SDL_Rect wingRect = {x, y, OBJECT_SIZE, OBJECT_SIZE};
        SDL_RenderCopy(renderer, wingTexture, nullptr, &wingRect);
    }
}

// Cloud implementations
void Cloud::update(Uint32 currentTime) {
    // Cloud doesn't need to do anything in update
}

void Cloud::spawn(int screenWidth, Uint32 currentTime) {
    x = rand() % (screenWidth - OBJECT_SIZE);
    y = 0;
    spawnTime = currentTime;
    active = true;
}

void Cloud::render() const {
    if (active) {
        SDL_Rect cloudRect = {x, y, OBJECT_SIZE+70, OBJECT_SIZE+40};
        SDL_RenderCopy(renderer, cloudTexture, nullptr, &cloudRect);
    }
}

// Lightning implementations
void Lightning::update(Uint32 currentTime) {
    if (active && currentTime - spawnTime >= LIGHTNING_DURATION) {
        active = false;
    }
}

void Lightning::render() const {
    if (active) {
        SDL_Rect lightningRect = {x, 0, OBJECT_SIZE, SCREEN_HEIGHT - GROUND_HEIGHT};
        SDL_RenderCopy(renderer, lightningTexture, nullptr, &lightningRect);
    }
}

// Boss implementations
void Boss::update(Uint32 currentTime, int playerX) {
    if (active) {
        // Follow player horizontally
        if (x < playerX) {
            x += 1;
        } else if (x > playerX) {
            x -= 1;
        }

        // Fall down
        y += 2;

        // Check duration
        if (currentTime - spawnTime >= BOSS_DURATION) {
            active = false;
        }
    }
}

void Boss::spawn(int screenWidth, Uint32 currentTime) {
    x = rand() % (screenWidth - BOSS_SIZE);
    y = 0;
    spawnTime = currentTime;
    active = true;
}

void Boss::render() const {
    if (active) {
        SDL_Rect bossRect = {x, y, BOSS_SIZE, BOSS_SIZE};
        SDL_RenderCopy(renderer, BossTexture, nullptr, &bossRect);
    }
}

// Arrow implementations
void Arrow::update() {
    if (active) {
        x += static_cast<int>(ARROW_SPEED * cos(angle));
        y += static_cast<int>(ARROW_SPEED * sin(angle));

        if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT - GROUND_HEIGHT) {
            active = false;
        }
    }
}

void Arrow::render() const {
    if (active) {
        SDL_Rect arrowRect = {x, y, ARROW_SIZE, ARROW_SIZE};
        double degrees = angle * 180 / PI;
        SDL_RenderCopyEx(renderer, arrowTexture, nullptr, &arrowRect, degrees, nullptr, SDL_FLIP_NONE);
    }
}

// Bower implementations
void Bower::update(Uint32 currentTime, std::vector<Arrow>& arrows) {
    if (active) {
        if (currentTime - lastShootTime >= ARROW_SHOOT_INTERVAL) {
            x = rand() % (SCREEN_WIDTH - BOWER_SIZE);
            y = rand() % 200;
            spawnArrows(arrows, x + BOWER_SIZE / 2, y + BOWER_SIZE / 2);
            Mix_PlayChannel(-1, whooshSound, 0);
            lastShootTime = currentTime;
        }

        if (currentTime - spawnTime >= BOWER_DURATION) {
            active = false;
        }
    }
}

void Bower::spawn(int screenWidth, Uint32 currentTime) {
    x = rand() % (screenWidth - BOWER_SIZE);
    y = rand() % 200;
    spawnTime = currentTime;
    lastShootTime = currentTime;
    active = true;
}

void Bower::render() const {
    if (active) {
        SDL_Rect bowerRect = {x, y, BOWER_SIZE, BOWER_SIZE};
        SDL_RenderCopy(renderer, bowerTexture, nullptr, &bowerRect);
    }
}

void Bower::spawnArrows(std::vector<Arrow>& arrows, int centerX, int centerY) {
    for (double angle = 0; angle < 2 * PI; angle += PI / 6) {
        arrows.push_back({centerX, centerY, angle, true});
    }
}

// PoolBall implementations
void PoolBall::update(Uint32 currentTime) {
    if (!active) return;

    if (currentTime - spawnTime >= POOL_BALL_DURATION) {
        active = false;
        return;
    }

    x += static_cast<int>(POOL_BALL_SPEED * cos(angle));
    y += static_cast<int>(POOL_BALL_SPEED * sin(angle));

    // Bounce off walls
    if (x <= 0) {
        x = 0;
        angle = PI - angle;
    }
    else if (x >= SCREEN_WIDTH - POOL_BALL_SIZE) {
        x = SCREEN_WIDTH - POOL_BALL_SIZE;
        angle = PI - angle;
    }

    if (y <= 0) {
        y = 0;
        angle = -angle;
    }
    else if (y >= SCREEN_HEIGHT - POOL_BALL_SIZE - GROUND_HEIGHT) {
        y = SCREEN_HEIGHT - POOL_BALL_SIZE - GROUND_HEIGHT;
        angle = -angle;
    }
}

void PoolBall::spawn(int screenWidth, int screenHeight, Uint32 currentTime) {
    x = rand() % (screenWidth - POOL_BALL_SIZE);
    y = rand() % (screenHeight / 2);
    angle = (rand() % 360) * PI / 180.0;
    spawnTime = currentTime;
    active = true;
}

void PoolBall::render() const {
    if (active) {
        SDL_Rect ballRect = {x, y, POOL_BALL_SIZE, POOL_BALL_SIZE};
        double degrees = (SDL_GetTicks() / 10) % 360;
        SDL_RenderCopyEx(renderer, poolBallTexture, nullptr, &ballRect, degrees, nullptr, SDL_FLIP_NONE);
    }
}

// Player implementations
void Player::update(const Uint8* keystates, Uint32 currentTime) {
    int currentPlayerSpeed = speedBoostActive ? PLAYER_SPEED * 2 : PLAYER_SPEED;

    if (keystates[SDL_SCANCODE_A] && x > 0) {
        x -= currentPlayerSpeed;
        facingRight = false;
    }
    if (keystates[SDL_SCANCODE_D] && x < SCREEN_WIDTH - PLAYER_SIZE) {
        x += currentPlayerSpeed;
        facingRight = true;
    }
    if (keystates[SDL_SCANCODE_W] && !isJumping && !wingActive) {
        velocity = JUMP_STRENGTH;
        isJumping = true;
        Mix_PlayChannel(-1, jumpSound, 0);
    }

    if (!wingActive) {
        velocity += GRAVITY;
        y += velocity;

        if (y > SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT) {
            y = SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT;
            velocity = 0;
            isJumping = false;
        }

        if (keystates[SDL_SCANCODE_S] && isJumping) {
            velocity = 10;
        }
    } else {
        if (keystates[SDL_SCANCODE_W]) {
            y -= currentPlayerSpeed;
            if (y < 0) y = 0;
        }
        if (keystates[SDL_SCANCODE_S]) {
            y += currentPlayerSpeed;
            if (y > SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT) {
                y = SCREEN_HEIGHT - PLAYER_SIZE - GROUND_HEIGHT;
            }
        }
    }

    // Reset power-ups if duration has expired
    if (speedBoostActive && currentTime >= speedBoostEndTime) {
        speedBoostActive = false;
    }
    if (isImmortal && currentTime >= immortalEndTime) {
        isImmortal = false;
    }
    if (wingActive && currentTime >= wingEndTime) {
        wingActive = false;
        velocity = 0;
    }
}

void Player::render() const {
    SDL_Rect playerRect = {x, y, PLAYER_SIZE, PLAYER_SIZE};
    SDL_Point center = {PLAYER_SIZE / 2, PLAYER_SIZE / 2};
    if (facingRight) {
        SDL_RenderCopyEx(renderer, playerTexture, nullptr, &playerRect, 0, &center, SDL_FLIP_HORIZONTAL);
    } else {
        SDL_RenderCopyEx(renderer, playerTexture, nullptr, &playerRect, 0, nullptr, SDL_FLIP_NONE);
    }
}

bool Player::checkCollisionWith(const Wing& obj) const {
    return obj.active && checkCollision(x, y, PLAYER_SIZE, PLAYER_SIZE, obj.x, obj.y, OBJECT_SIZE, OBJECT_SIZE);
}

bool Player::checkCollisionWith(const FallingObject& obj) const {
    return obj.active && checkCollision(x, y, PLAYER_SIZE, PLAYER_SIZE, obj.x, obj.y, OBJECT_SIZE, OBJECT_SIZE);
}

bool Player::checkCollisionWith(const GroundObject& obj) const {
    return obj.active && checkCollision(x, y, PLAYER_SIZE, PLAYER_SIZE, obj.x, obj.y, OBJECT_SIZE, OBJECT_SIZE);
}

bool Player::checkCollisionWith(const PowerUp& obj) const {
    return obj.active && checkCollision(x, y, PLAYER_SIZE, PLAYER_SIZE, obj.x, obj.y, OBJECT_SIZE, OBJECT_SIZE);
}

bool Player::checkCollisionWith(const Lightning& obj) const {
    return obj.active && checkCollision(x, y, PLAYER_SIZE, PLAYER_SIZE, obj.x, 0, OBJECT_SIZE, SCREEN_HEIGHT - GROUND_HEIGHT);
}

bool Player::checkCollisionWith(const Boss& obj) const {
    return obj.active && checkCollision(x, y, PLAYER_SIZE, PLAYER_SIZE, obj.x, obj.y, BOSS_SIZE, BOSS_SIZE);
}

bool Player::checkCollisionWith(const PoolBall& obj) const {
    return obj.active && checkCollision(x, y, PLAYER_SIZE, PLAYER_SIZE, obj.x, obj.y, POOL_BALL_SIZE, POOL_BALL_SIZE);
}

bool Player::checkCollisionWith(const Arrow& obj) const {
    return obj.active && checkCollision(x, y, PLAYER_SIZE, PLAYER_SIZE, obj.x, obj.y, ARROW_SIZE, ARROW_SIZE);
}

// Global functions
void renderPowerUpStatus() {
    Uint32 currentTime = SDL_GetTicks();
    int iconY = 10;

    if (player.speedBoostActive) {
        SDL_Rect iconRect = {10, iconY, POWER_UP_ICON_SIZE, POWER_UP_ICON_SIZE};
        SDL_RenderCopy(renderer, foodTexture, nullptr, &iconRect);

        float timeLeft = (player.speedBoostEndTime - currentTime) / (float)SPEED_BOOST_DURATION;
        if (timeLeft > 0) {
            SDL_Rect timeBar = {10 + POWER_UP_ICON_SIZE + 5, iconY + POWER_UP_ICON_SIZE / 2 - 5,
                               static_cast<int>(100 * timeLeft), 10};
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &timeBar);
        }
        iconY += POWER_UP_ICON_SIZE + 10;
    }

    if (player.isImmortal) {
        SDL_Rect iconRect = {10, iconY, POWER_UP_ICON_SIZE, POWER_UP_ICON_SIZE};
        SDL_RenderCopy(renderer, shieldTexture, nullptr, &iconRect);

        float timeLeft = (player.immortalEndTime - currentTime) / (float)IMMORTAL_DURATION;
        if (timeLeft > 0) {
            SDL_Rect timeBar = {10 + POWER_UP_ICON_SIZE + 5, iconY + POWER_UP_ICON_SIZE / 2 - 5,
                               static_cast<int>(100 * timeLeft), 10};
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &timeBar);
        }
        iconY += POWER_UP_ICON_SIZE + 10;
    }

    if (player.wingActive) {
        SDL_Rect iconRect = {10, iconY, POWER_UP_ICON_SIZE, POWER_UP_ICON_SIZE};
        SDL_RenderCopy(renderer, wingTexture, nullptr, &iconRect);

        float timeLeft = (player.wingEndTime - currentTime) / (float)WING_DURATION;
        if (timeLeft > 0) {
            SDL_Rect timeBar = {10 + POWER_UP_ICON_SIZE + 5, iconY + POWER_UP_ICON_SIZE / 2 - 5,
                               static_cast<int>(100 * timeLeft), 10};
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &timeBar);
        }
    }
}

void SpawningLogic(const Uint32 &currentTime) {
    if (currentObjectIndex < (int)fallingObjects.size()) {
        if (!fallingObjects[currentObjectIndex].active &&
            (currentTime - lastObjectSpawnTime >= FALLING_OBJECT_DELAY)) {
            fallingObjects[currentObjectIndex].spawn(SCREEN_WIDTH);
            lastObjectSpawnTime = currentTime;
            currentObjectIndex++;
        }
    } else {
        bool allObjectsFallen = true;
        for (const auto& obj : fallingObjects) {
            if (obj.active) {
                allObjectsFallen = false;
                break;
            }
        }
        if (allObjectsFallen) {
            roundNumber++;
            currentObjectIndex = 0;
            fallingObjects.push_back(FallingObject{0, 0, fallingObjectSpeed, false});
            fallingObjects.back().spawn(SCREEN_WIDTH);
        }
    }
}
