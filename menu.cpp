#include "defs.h"
#include "menu.h"
bool mainMenuOn = true;
bool levelMenuOn = false;
bool gameOverMenuOn = false;
void renderChat(const std::string& text, int x, int y) {

    TTF_Font* arialFont = TTF_OpenFont("arial.ttf", 24);
    if (!arialFont) {
        std::cerr << "Failed to load Arial font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* s = TTF_RenderText_Solid(arialFont, text.c_str(), color);
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect rect = {x, y, s->w, s->h};
    SDL_RenderCopy(renderer, t, nullptr, &rect);

    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
    TTF_CloseFont(arialFont);
}

void renderMenu(){
    if (mainMenuOn) {
        renderChat("New Game", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderChat("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
    } else if (levelMenuOn) {
        renderChat("Use WASD to dodging the object! The longer you survives the more score u would gain.",SCREEN_WIDTH / 2 -500,SCREEN_HEIGHT / 2 - 250);
        renderChat("Level: Easy", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 150);
        renderChat("Level: Medium", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 100);
        renderChat("Level: Hard", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderChat("Level: Hell", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
        renderChat("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 50);
    } else if (gameOverMenuOn) {
        renderChat("Play Again", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50);
        renderChat("Exit", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2);
    }
}


