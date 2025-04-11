#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "audio.h"

void renderChat(const std::string& text, int x, int y);
void renderMenu();
void handleEvents(SDL_Event& e, bool& quit, bool& mainMenuOn, bool& levelMenuOn,
                 bool& gameOverMenuOn, bool& isPaused, bool isHoveringPauseButton,
                 int& fallingObjectSpeed, int& BALL_SPEED, int& ARROW_SPEED);

#endif // MENU_H
