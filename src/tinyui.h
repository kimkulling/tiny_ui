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

#include <SDL_image.h>
#include <SDL_ttf.h>
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

struct Widget;

using ret_code = int32_t;

static constexpr ret_code InvalidHandle = -2;
static constexpr ret_code ErrorCode = -1;
static constexpr ret_code ResultOk  = 0;

struct Color4 {
    uint8_t r,g,b,a;
};

struct Image {
    SDL_Surface *mSurface;
    int32_t mX, mY, mComp;

    Image() : mSurface(nullptr), mX(0), mY(0), mComp(0) {}

    ~Image() {
        if (mSurface != nullptr) {
            SDL_FreeSurface(mSurface);
        }
    }
};

using ImageCache = std::map<const char*, Image*>;

template<class T>
struct Point2 {
    T x, y;

    Point2() : x(0), y(0) {}
    Point2(T x_, T y_) : x(x_), y(y_) {}
    ~Point2() = default;

    void set(T x_, T y_) {
        x = x_;
        y = y_;
    }
};

using Point2i = Point2<int32_t>;

struct Rect {
    int32_t x1, y1, width, height, x2, y2;

    Rect() :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {}

    Rect(int32_t x, int32_t y, int32_t w, int32_t h) :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {
        set(x, y, w, h);
    }

    ~Rect() = default;

    bool isIn(const Point2i &pt) const {
        return isIn(pt.x, pt.y);
    }

    bool isIn(int x_, int y_) const {
        if (x_ >= x1 && y_ >= y1 && x_ <=x2 && y_ <= y2) {
            return true;
        }
        return false;
    }

    void set(int32_t x, int32_t y, int32_t w, int32_t h) {
        x1 = x;
        y1 = y;
        width = w;
        height = h;
        x2 = x + w;
        y2 = y + h;
    }

    void mergeWithRect(const Rect &r) {
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

enum class Alignment : int32_t {
    Invalid = -1,
    Left = 0,
    Center,
    Right,
    Count
};

struct Font {
    const char *mName;
    uint32_t mSize;
    TTF_Font *mFont;

    ~Font() {
        if (mFont != nullptr) {
            TTF_CloseFont(mFont);
        }
    }
};

using tui_FontCache = std::map<const char*, Font*>;

struct Style {
    Color4 mClearColor;
    Color4 mFg;
    Color4 mBg;
    Color4 mTextColor;
    Color4 mBorder;
    int32_t mMargin;
    Font mFont;
};

enum class MouseState {
    Invalid = -1,
    LeftButton = 0,
    MiddleButton,
    RightButton,
    Count
};

enum class LogSeverity {
    Invalid = -1,
    Message = 0,
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Count
};

enum class Extensions {
    Invalid = -1,
    VerboseMode,
    Count
};

/// @brief The tiny ui events.
struct Events {
    static constexpr int32_t InvalidEvent = -1;
    static constexpr int32_t QuitEvent = 0;
    static constexpr int32_t MouseButtonDownEvent = 1;
    static constexpr int32_t MouseButtonUpEvent = 2;
    static constexpr int32_t MouseMoveEvent = 3;
    static constexpr int32_t MouseHoverEvent = 4;
    static constexpr int32_t NumEvents = MouseHoverEvent + 1;
};

/// @brief This interface is used to store all neede message handlers.
struct CallbackI {
    /// The function callback
    typedef int (*funcCallback) (uint32_t id, void *data);
    /// The function callback array, not handled callbacks are marked as a nullptr.
    funcCallback mfuncCallback[Events::NumEvents];
    /// The data instance.
    void *mInstance;

    /// @brief The default class constructor.
    CallbackI() :
            mfuncCallback{ nullptr }, mInstance(nullptr) {
        for (size_t i=0; i<Events::NumEvents; ++i) {
            mfuncCallback[i] = nullptr;
        }
    }

    CallbackI(funcCallback mbDownFunc, void *instance) :
            mfuncCallback{ nullptr }, mInstance(instance) {
        for (size_t i = 0; i < Events::NumEvents; ++i) {
            mfuncCallback[i] = nullptr;
        }
        mfuncCallback[Events::MouseButtonDownEvent] = mbDownFunc;
    }

    ~CallbackI() = default;
};

using EventCallbackArray = std::vector<CallbackI*>;
using EventDispatchMap = std::map<int32_t, EventCallbackArray>;

typedef void (*tui_log_func) (LogSeverity severity, const char *message);

struct SDLContext {
    SDL_Window   *mWindow;
    SDL_Surface  *mSurface;
    SDL_Renderer *mRenderer;
    Font         *mDefaultFont;
    Font         *mSelectedFont;
    bool          mOwner;

    SDLContext() :
            mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr),
        mDefaultFont(nullptr), mSelectedFont(nullptr), mOwner(true) {}
};

struct Context {
    bool             mCreated;
    const char      *mAppTitle;
    const char      *mWindowsTitle;
    SDLContext       mSDLContext;
    Style            mStyle;
    Widget          *mRoot;
    tui_log_func     mLogger;
    EventDispatchMap mEventDispatchMap;
    tui_FontCache    mFontCache;
    ImageCache       mImageCache;

    static Context &create(const char *title, Style &style);
    static void destroy(Context &ctx);
    static void enableExtensions(Context &ctx, const std::vector<tinyui::Extensions> &extensions);

private:
    Context() :
            mCreated(false),
            mAppTitle(nullptr),
            mWindowsTitle(nullptr),
            mSDLContext(),
            mStyle(),
            mRoot(nullptr),
            mLogger(nullptr) {
        // empty
    }
};

ret_code init(Context &ctx);
ret_code initScreen(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h);
ret_code getSurfaceInfo(Context &ctx, int32_t &w, int32_t &h);
bool run(Context &ctx);
ret_code beginRender(Context &ctx, Color4 bg);
ret_code endRender(Context &ctx);
ret_code release(Context &ctx);
const Style &getDefaultStyle();
void setDefaultStyle(const Style &style);
void setDefaultFont(Context &ctx, const char *defaultFont);
Context *createContext(const char *title, Style &style);

} // Namespace TinyUi
