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
    unsigned int mId;
    Widget *mParent;
    std::vector<Widget*> mChildren;
};

struct CallbackI {
    int (*funcCallback) (unsigned int id, void *data);
};

struct Context {
    bool mCreated;
    const char *title;
    SDL_Window *mWindow;
    SDL_Surface *mSurface;
    SDL_Renderer *mRenderer;
    Widget *mRoot;

    Context() : mCreated(false), title(nullptr), mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr), mRoot(nullptr) {}
};

struct TinyUi {
    static int initRenderer(Context &ctx);
    static int releaseRenderer(Context &ctx);

    static int initScreen(Context &ctx, int x, int y, int w, int h);
    static int create_button(Context &ctx, unsigned int id, unsigned int parentId,int x, int y, int w, int h, CallbackI *callback);
    static int draw_rect(Context &ctx, int x, int y, int w, int h, bool filled, color4 fg);
    static int beginRender(Context &ctx, color4 bg);
    static int endRender(Context &ctx);
    static int closeScreen(Context &ctx);
    static bool run(Context &ctx);
    static void onMousePress(SDL_MouseButtonEvent& b);
};
