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

    bool isIn(int x_, int y_) const {
        if (x_ >= x && y_ >= y && x_ <=(x+width) && y_ <= (y+height)) {
            return true;
        }
        return false;
    }
};

struct style {
    color4 mFg;
    color4 mBg;
    color4 mTextColor;
};

enum class MouseState {
    Unknown = -1,
    LeftButton = 0,
    MiddleButton,
    RightButton,
    NumStates
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
    ~Context() = default;
};

} // Namespace TinyUi
