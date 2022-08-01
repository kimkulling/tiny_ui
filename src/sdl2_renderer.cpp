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
        std::cerr << "Not initialized.\n";
        return -1;
    }
    
    SDL_DestroyWindow(ctx.mWindow);
    ctx.mWindow = nullptr;
    SDL_Quit();

    return 0;
}

int initScreen(Context &ctx, int x, int y, int w, int h) {
    SDL_Surface *screen = nullptr;
    const char *title = ctx.title;
    if (ctx.title == nullptr) {
        title = "untitled";
    }

    ctx.mWindow = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (ctx.mWindow == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    ctx.mSurface = SDL_GetWindowSurface(ctx.mWindow);
    if (ctx.mSurface == nullptr) {
        return -1;
    }
    
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
    if (ctx.mWindow == nullptr) {
        return -1;
    }
    
    SDL_DestroyWindow(ctx.mWindow);
    ctx.mWindow = nullptr;

    return 0;
}

bool run(Context &ctx) {
    if (!ctx.mCreated) {
        return false;
    }

    bool running = true;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type)
        {
            case SDL_QUIT:
                running = false;
                break;
        }
    }

    return running;
}
