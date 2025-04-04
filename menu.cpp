#include "defs.h"
#include "menu.h"
bool showMainMenu = true;
bool showLevelMenu = false;
bool showGameOverMenu = false;
void renderText(const std::string& text, int x, int y) {
    // Load Arial font (replace "arial.ttf" with your actual font file path)
    TTF_Font* arialFont = TTF_OpenFont("arial.ttf", 24); // 24 = font size
    if (!arialFont) {
        std::cerr << "Failed to load Arial font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color color = {255, 255, 255, 255}; // White text
    SDL_Surface* surface = TTF_RenderText_Solid(arialFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    // Cleanup
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(arialFont); // Close the font to avoid memory leaks
}

void renderMenu(){
    if (showMainMenu) {
        renderText("New Game", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderText("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
    } else if (showLevelMenu) {
        renderText("Use WASD to dodging the object! The longer you survives the more score u would gain.",SCREEN_WIDTH / 2 -500,SCREEN_HEIGHT / 2 - 250);
        renderText("Level: Easy", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 150);
        renderText("Level: Medium", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 100);
        renderText("Level: Hard", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderText("Level: Hell", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
        renderText("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 50);
    } else if (showGameOverMenu) {
        renderText("Play Again", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderText("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
    }
}


