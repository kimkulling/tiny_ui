#pragma once

#include <SDL_ttf/SDL_ttf.h>
#include <cstdint>
#include <vector>
#include <string>

#include "stb_image.h"

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_MouseButtonEvent;

namespace tinyui {

using tui_ret_code = int32_t;

static constexpr tui_ret_code ErrorCode = -1;

struct tui_widget;

struct tui_color4 {
    int32_t r,g,b,a;

    ~tui_color4() = default;
};


struct tui_image {
    SDL_Surface *mSurface;
    int32_t mX, mY, mComp;
    
    tui_image() : mSurface(nullptr), mX(0), mY(0), mComp(0) {}
    
    ~tui_image() {
        if (mSurface != nullptr) {
            SDL_FreeSurface(mSurface);
        }
    }
};

struct tui_rect {
    int32_t x1, y1, width, height, x2, y2;

    tui_rect() :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {}
    
    tui_rect(int32_t x, int32_t y, int32_t w, int32_t h) :
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
    
    void set( int32_t x, int32_t y, int32_t w, int32_t h ) {
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
    int32_t mMargin;
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
    static constexpr int32_t QuitEvent = 0;
    static constexpr int32_t MouseButtorDownEvent = 1;
    static constexpr int32_t MouseButtorUpEvent = 2;
    static constexpr int32_t MouseMoveEvent = 3;
    static constexpr int32_t NumEvents = MouseMoveEvent + 1;
    static constexpr int32_t InvalidEvent = NumEvents + 1;
};

struct tui_callbackI {
    typedef int (*funcCallback) (uint32_t id, void *data);
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
            mCreated(false), title(nullptr), mSDLContext(), mRoot(nullptr) {
        // empty
    }

    ~tui_context() = default;
};

tui_ret_code tui_init(tui_context &ctx);
tui_ret_code tui_release(tui_context &ctx);
const tui_style &get_default_style();
void set_default_style(const tui_style &style);
tui_context *create_context();

} // Namespace TinyUi
