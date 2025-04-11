#ifndef PAUSE_H
#define PAUSE_H

#include <SDL.h>

void renderPauseBtn(SDL_Renderer* renderer, SDL_Texture* pauseButtonTexture,
                       SDL_Texture* pauseButtonHoverTexture, bool& isHoveringPauseButton,int PAUSE_SIZE, int SCREEN_WIDTH);

#endif // PAUSE_H
