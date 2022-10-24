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
    int x1, y1, width, height, x2, y2;

    rect() : x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {}
    rect(int x, int y, int w, int h) :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {
        set(x, y, w, h);
    }
    bool isIn(int x_, int y_) const {
        if (x_ >= x1 && y_ >= y1 && x_ <=(x2) && y_ <= y2) {
            return true;
        }
        return false;
    }
    
    void set( int x, int y, int w, int h ) {
        x1 = x;
        y1 = y;
        width = w;
        height = h;
        x2 = x + w;
        y2 = y + h;
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
