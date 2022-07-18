#include "sdl2_renderer.h"

#include <iostream>

int initRenderer(Context &ctx) {
    if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)) {
        std::cerr << "Could not initialize SDL: " << SDL_GetError() << "\n";
        ctx.mCreated = false;
        return -1;
    }

    ctx.mCreated = true;

    return 0;
}

int releaseRenderer(Context &ctx) {
    if (!ctx.mCreated) {
        return -1;
    }
    
    SDL_DestroyWindow(ctx.mWindow);
    ctx.mWindow = nullptr;
    SDL_Quit();

    return 0;
}

int initScreen(Context &ctx, int x, int y, int w, int h) {
    SDL_Surface *screen = nullptr;
    ctx.mWindow = SDL_CreateWindow(ctx.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    if (ctx.mWindow == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    ctx.mSurface = SDL_GetWindowSurface(ctx.mWindow);
    ctx.mRenderer = SDL_CreateRenderer(ctx.mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (nullptr == ctx.mRenderer) {
        return -1;
    }

    return 0;
}

int draw_rect(Context &ctx, int x, int y, int w, int h, bool filled, color4 fg, color4 bg){
    return 0;
}

int closeScreen(Context &ctx) {
    return 0;
}
