#ifndef OBJECTS_H
#define OBJECTS_H
#include "defs.h"
#include <vector>
#include <cmath>
#include <cstdlib>
bool checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

// Struct declarations
struct FallingObject {
    int x, y, speed;
    bool active;
    void update();
    void spawn(int screenWidth);
    void render() const;
};
extern std::vector<FallingObject> fallingObjects;

struct GroundObject {
    int x, y, speed;
    bool active, facingright;
    void update();
    void spawn(bool fromLeft, int screenWidth, int screenHeight);
    void render() const;
};

struct PowerUp {
    int x, y, speed;
    bool active;
    SDL_Texture* texture;
    void update();
    void spawn(int screenWidth);
    void render() const;
};

struct Wing {
    int x, y, speed;
    bool active;
    Uint32 spawnTime;
    void update();
    void spawn(int screenWidth, Uint32 currentTime);
    void render() const;
};

struct Cloud {
    int x, y;
    Uint32 spawnTime;
    bool active;
    void update(Uint32 currentTime);
    void spawn(int screenWidth, Uint32 currentTime);
    void render() const;
};

struct Lightning {
    int x;
    Uint32 spawnTime;
    bool active;
    void update(Uint32 currentTime);
    void render() const;
};

struct Boss {
    int x, y;
    Uint32 spawnTime;
    bool active;
    void update(Uint32 currentTime, int playerX);
    void spawn(int screenWidth, Uint32 currentTime);
    void render() const;
};

struct Arrow {
    int x, y;
    double angle;
    bool active;
    void update();
    void render() const;
};

struct Bower {
    int x, y;
    Uint32 spawnTime, lastShootTime;
    bool active;
    void update(Uint32 currentTime, std::vector<Arrow>& arrows);
    void spawn(int screenWidth, Uint32 currentTime);
    void render() const;
    void spawnArrows(std::vector<Arrow>& arrows, int centerX, int centerY);
};

struct PoolBall {
    int x, y;
    double angle;
    Uint32 spawnTime;
    bool active;
    void update(Uint32 currentTime);
    void spawn(int screenWidth, int screenHeight, Uint32 currentTime);
    void render() const;
};

struct Player {
    int x, y, velocity;
    bool isJumping, isImmortal, speedBoostActive, wingActive, facingRight;
    Uint32 speedBoostEndTime, immortalEndTime, wingEndTime;
    void update(const Uint8* keystates, Uint32 currentTime);
    void render() const;
    bool checkCollisionWith(const Wing& obj) const;
    bool checkCollisionWith(const FallingObject& obj) const;
    bool checkCollisionWith(const GroundObject& obj) const;
    bool checkCollisionWith(const PowerUp& obj) const;
    bool checkCollisionWith(const Lightning& obj) const;
    bool checkCollisionWith(const Boss& obj) const;
    bool checkCollisionWith(const PoolBall& obj) const;
    bool checkCollisionWith(const Arrow& obj) const;
};
extern Player player;
// Global functions
void renderPowerUpStatus();
void SpawningLogic(const Uint32 &currentTime);

#endif
