#pragma once

#include <SDL.h>

#include <vector>

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_MouseButtonEvent;

struct color4 {
    int r,g,b,a;
};

struct Widget {
    Widget *mParent;
    std::vector<Widget*> mChildren;
}

struct Context {
    bool mCreated;
    const char *title;
    SDL_Window *mWindow;
    SDL_Surface *mSurface;
    SDL_Renderer *mRenderer;

    Context() : mCreated(false), title(nullptr), mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr){}
};

struct TinyUi {
    static int initRenderer(Context &ctx);
    static int releaseRenderer(Context &ctx);

    static int initScreen(Context &ctx, int x, int y, int w, int h);
    static int create_button(Context &ctx, int x, int y, int w, int h);
    static int draw_rect(Context &ctx, int x, int y, int w, int h, bool filled, color4 fg);
    static int begin(Context &ctx, color4 bg);
    static int end(Context &ctx);
    static int closeScreen(Context &ctx);
    static bool run(Context &ctx);
    static void onMousePress(SDL_MouseButtonEvent& b);
};
