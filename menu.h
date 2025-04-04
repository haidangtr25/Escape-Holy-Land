#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "audio.h"  // Để sử dụng âm thanh trong menu


void renderText(const std::string& text, int x, int y);
void renderMenu();
void handleEvents(SDL_Event& e, bool& quit, bool& showMainMenu, bool& showLevelMenu,
                 bool& showGameOverMenu, bool& isPaused, bool isHoveringPauseButton,
                 int& fallingObjectSpeed, int& POOL_BALL_SPEED, int& ARROW_SPEED);

#endif // MENU_H
