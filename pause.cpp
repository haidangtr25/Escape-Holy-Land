#include "pause.h"

void renderPauseBtn(SDL_Renderer* renderer, SDL_Texture* pauseButtonTexture,
                       SDL_Texture* pauseButtonHoverTexture, bool& isHoveringPauseButton,int PAUSE_SIZE, int SCREEN_WIDTH) {
    SDL_Rect pauseButtonRect = {SCREEN_WIDTH - PAUSE_SIZE - 10, 100, PAUSE_SIZE, PAUSE_SIZE};

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    isHoveringPauseButton = (mouseX >= pauseButtonRect.x && mouseX <= pauseButtonRect.x + PAUSE_SIZE &&
                             mouseY >= pauseButtonRect.y && mouseY <= pauseButtonRect.y + PAUSE_SIZE);

    if (isHoveringPauseButton) {
        SDL_RenderCopy(renderer, pauseButtonHoverTexture, nullptr, &pauseButtonRect);
    } else {
        SDL_RenderCopy(renderer, pauseButtonTexture, nullptr, &pauseButtonRect);
    }
}
