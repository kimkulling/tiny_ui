#pragma once

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_MouseButtonEvent;

namespace tinyui {

struct tui_widget;

struct tui_color4 {
    int r,g,b,a;
};

struct tui_rect {
    int x1, y1, width, height, x2, y2;

    tui_rect() :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {}
    tui_rect(int x, int y, int w, int h) :
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

struct tui_style {
    tui_color4 mFg;
    tui_color4 mBg;
    tui_color4 mTextColor;
};

enum class tui_mouseState {
    Unknown = -1,
    LeftButton = 0,
    MiddleButton,
    RightButton,
    NumStates
};

struct tui_loggerBackend {
    void (*logMessage) (const char *message);
};

struct tui_callbackI {
    int (*funcCallback) (unsigned int id, void *data);
};

struct tui_sdlContext {
    SDL_Window *mWindow;
    SDL_Surface *mSurface;
    SDL_Renderer *mRenderer;
    bool mOwner;

    tui_sdlContext() :
            mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr), mOwner(true) {}
};

struct tui_context {
    bool mCreated;
    const char *title;
    tui_sdlContext mSDLContext;
    tui_style mStyle;
    tui_widget *mRoot;

    tui_context() :
            mCreated(false), title(nullptr), mSDLContext(), mRoot(nullptr) {}
    ~tui_context() = default;
};

int tui_init(tui_context &ctx);
int tui_release(tui_context &ctx);

    } // Namespace TinyUi
