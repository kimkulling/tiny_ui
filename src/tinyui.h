/*
MIT License

Copyright (c) 2022-2024 Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <SDL_ttf/SDL_ttf.h>
#include <cstdint>
#include <vector>
#include <string>
#include <map>

#include "stb_image.h"

struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_MouseButtonEvent;

namespace tinyui {

struct tui_widget;

using tui_ret_code = int32_t;

static constexpr tui_ret_code ErrorCode = -1;
static constexpr tui_ret_code ResultOk = 0;

struct tui_color4 {
    uint8_t r,g,b,a;

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

using tui_ImageCache = std::map<const char*, tui_image*>;

template<class T>
struct tui_point {
    T x, y;

    tui_point() : x(0), y(0) {}
    tui_point(T x_, T y_) : x(x_), y(y_) {}
    ~tui_point() = default;

    void set(T x_, T y_) {
        x = x_;
        y = y_;
    }
};

using tui_pointi = tui_point<int32_t>;

struct tui_rect {
    int32_t x1, y1, width, height, x2, y2;

    tui_rect() :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {}

    tui_rect(int32_t x, int32_t y, int32_t w, int32_t h) :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {
        set(x, y, w, h);
    }

    ~tui_rect() = default;

    bool isIn(const tui_pointi &pt) const {
        return isIn(pt.x, pt.y);
    }

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

struct tui_font {
    const char *mName;
    uint32_t mSize;
    TTF_Font *mFont;

    ~tui_font() {
        if (mFont != nullptr) {
            TTF_CloseFont(mFont);
        }
    }
};

using tui_FontCache = std::map<const char*, tui_font*>;

struct tui_style {
    tui_color4 mClearColor;
    tui_color4 mFg;
    tui_color4 mBg;
    tui_color4 mTextColor;
    tui_color4 mBorder;
    int32_t mMargin;
    tui_font mFont;
};

enum class tui_mouseState {
    Unknown = -1,
    LeftButton = 0,
    MiddleButton,
    RightButton,
    NumStates
};

enum class tui_log_severity {
    Message = 0,
    Trace,
    Debug,
    Info,
    Warn,
    Error
};

enum class tui_extensions {
    InvalidExtension = -1,
    VerboseMode,
    NumExtensions
};

struct tui_events {
    static constexpr int32_t InvalidEvent = -1;
    static constexpr int32_t QuitEvent = 0;
    static constexpr int32_t MouseButtonDownEvent = 1;
    static constexpr int32_t MouseButtonUpEvent = 2;
    static constexpr int32_t MouseMoveEvent = 3;
    static constexpr int32_t MouseHoverEvent = 4;

    static constexpr int32_t NumEvents = MouseHoverEvent + 1;
};

struct tui_callbackI {
    typedef int (*funcCallback) (uint32_t id, void *data);
    funcCallback mfuncCallback[tui_events::NumEvents];
    void *mInstance;

    tui_callbackI() :
            mfuncCallback{ nullptr }, mInstance(nullptr) {
        for (size_t i=0; i<tui_events::NumEvents; ++i) {
            mfuncCallback[i] = nullptr;
        }
    }

    tui_callbackI(funcCallback mbDownFunc, void *instance) :
            mfuncCallback{ nullptr }, mInstance(instance) {
        for (size_t i=0; i<tui_events::NumEvents; ++i) {
            mfuncCallback[i] = nullptr;
        }
        mfuncCallback[tui_events::MouseButtonDownEvent] = mbDownFunc;
    }

    ~tui_callbackI() = default;
};

typedef void (*tui_log_func) (tui_log_severity severity, const char *message);

struct tui_sdlContext {
    SDL_Window   *mWindow;
    SDL_Surface  *mSurface;
    SDL_Renderer *mRenderer;
    tui_font     *mDefaultFont;
    tui_font     *mSelectedFont;
    bool          mOwner;

    tui_sdlContext() : mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr),
        mDefaultFont(nullptr), mSelectedFont(nullptr), mOwner(true) {}

    ~tui_sdlContext() = default;
};

struct tui_context {
    bool mCreated;
    const char *title;
    tui_sdlContext mSDLContext;
    tui_style mStyle;
    tui_widget *mRoot;
    tui_log_func mLogger;
    tui_FontCache mFontCache;
    tui_ImageCache mImageCache;

    static tui_context &create(const char *title, tui_style &style);
    static void destroy(tui_context &ctx);
    ~tui_context() = default;

private:
    tui_context() :
            mCreated(false), title(nullptr), mSDLContext(), mStyle(), mRoot(nullptr), mLogger(nullptr) {
        // empty
    }
};

tui_ret_code tui_init(tui_context &ctx);
tui_ret_code tui_init_screen(tui_context &ctx, int32_t x, int32_t y, int32_t w, int32_t h);
tui_ret_code tui_get_surface_info(tui_context &ctx, int32_t &w, int32_t &h);
bool tui_run(tui_context &ctx);
tui_ret_code tui_begin_render(tui_context &ctx, tui_color4 bg);
tui_ret_code tui_end_render(tui_context &ctx);
tui_ret_code tui_release(tui_context &ctx);
const tui_style &tui_get_default_style();
void tui_set_default_style(const tui_style &style);
void tui_set_default_font(tui_context &ctx, const char *defaultFont);
tui_context *tui_create_context(const char *title, tui_style &style);

} // Namespace TinyUi
