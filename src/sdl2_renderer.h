#pragma once

#include <SDL.h>

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;

struct color4 {
    int r,g,b,a;
};

struct Context {
    bool mCreated;
    const char *title;
    SDL_Window *mWindow;
    SDL_Surface *mSurface;
    SDL_Renderer *mRenderer;
};

int initRenderer(Context &ctx);
int releaseRenderer(Context &ctx);

int initScreen(Context &ctx, int x, int y, int w, int h);
int draw_rect(Context &ctx, int x, int y, int w, int h, bool filled, color4 fg, color4 bg);
int closeScreen(Context &ctx);
