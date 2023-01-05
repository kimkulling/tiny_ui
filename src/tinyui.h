#pragma once

#include <SDL_ttf/SDL_ttf.h>

#include <vector>
#include <string>

#include "stb_image.h"

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_MouseButtonEvent;

namespace tinyui {

static constexpr int ErrorCode = -1;

struct tui_widget;

struct tui_color4 {
    int r,g,b,a;

    ~tui_color4() = default;
};

struct tui_image {
    unsigned char *mImage;

    tui_image() : mImage(nullptr) {}
    
    ~tui_image() {
        if (mImage != nullptr) {
            stbi_image_free(mImage);
            mImage = nullptr;
        }
    }
};

struct tui_rect {
    int x1, y1, width, height, x2, y2;

    tui_rect() :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {}
    
    tui_rect(int x, int y, int w, int h) :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {
        set(x, y, w, h);
    }
    
    ~tui_rect() = default;

    bool isIn(int x_, int y_) const {
        if (x_ >= x1 && y_ >= y1 && x_ <=x2 && y_ <= y2) {
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

    void mergeWithRect(const tui_rect &r) {
        if (x1 > r.x1 || x1 == -1) {
            x1 = r.x1;
        }
        
        if (y1 > r.y1 || y1 == -1) {
            y1 = r.y1;
        }
        
        const int x2_ = r.x1 + r.width;
        if (x2 < x2_) {
            x2 = x2_;
            width = r.width;
        }

        const int y2_ = r.y1 + r.height;
        if (y2 < y2_) {
            y2 = y2_;
            height = r.height;
        }
    }
};

struct tui_style {
    tui_color4 mFg;
    tui_color4 mBg;
    tui_color4 mTextColor;
    tui_color4 mBorder;
    int mMargin;
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

struct tui_events {
    static constexpr int QuitEvent = 0;
    static constexpr int MouseButtorDownEvent = 1;
    static constexpr int MouseButtorUpEvent = 2;
    static constexpr int MouseMoveEvent = 3;
    static constexpr int NumEvents = MouseMoveEvent + 1;
    static constexpr int InvalidEvent = NumEvents + 1;
};

struct tui_callbackI {
    typedef int (*funcCallback) (unsigned int id, void *data);
    funcCallback mfuncCallback[tui_events::NumEvents];
    void *mInstance;
    
    tui_callbackI() :
            mfuncCallback{ nullptr }, mInstance(nullptr) {}
    
    tui_callbackI(funcCallback mbDownFunc, void *instance) :
            mfuncCallback{ nullptr }, mInstance(instance) {
        mfuncCallback[tui_events::MouseButtorDownEvent] = mbDownFunc;
    }

    ~tui_callbackI() = default;
};

struct tui_sdlContext {
    SDL_Window *mWindow;
    SDL_Surface *mSurface;
    SDL_Renderer *mRenderer;
    TTF_Font *mDefaultFont;
    bool mOwner;

    tui_sdlContext() : mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr), 
        mDefaultFont(nullptr), mOwner(true) {}

    ~tui_sdlContext() = default;
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
const tui_style &get_default_style();
void set_default_style(const tui_style &style);
tui_context *create_context();

} // Namespace TinyUi
