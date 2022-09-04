#pragma once

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_MouseButtonEvent;

namespace TinyUi {

struct Widget;

struct color4 {
    int r,g,b,a;
};

struct rect {
    int x, y, width, height;
};

struct style {
    color4 mFg;
    color4 mBg;
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
    style mStyle;
    Widget *mRoot;

    Context() : mCreated(false), title(nullptr), mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr), mRoot(nullptr) {}
};

} // Namespace TinyUi
