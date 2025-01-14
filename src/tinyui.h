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

#include <cstdint>
#include <vector>
#include <list>
#include <string>
#include <map>

#include "stb_image.h"

/*
 *  Changelog:
 *  ==========
 *  0.0.1: Initial version.
 */

// Forward declarations
struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_MouseButtonEvent;

namespace tinyui {

struct SurfaceImpl;
struct FontImpl;
struct Widget;

/// @brief The return code.
using ret_code = int32_t;

/// @brief The invalid handle.
static constexpr ret_code InvalidHandle = -2;
/// @brief The error code.
static constexpr ret_code ErrorCode = -1;
/// @brief The ok code.
static constexpr ret_code ResultOk  = 0;

/// @brief The default color type with 4 components.
struct Color4 {
    uint8_t r;      ///< The red component.
    uint8_t g;      ///< The green component.
    uint8_t b;      ///< The blue component.
    uint8_t a;      ///< The alpha component.
};

/// @brief The image data.
struct Image {
    SurfaceImpl *mSurfaceImpl;
    int32_t mX;     ///< The width of the image.
    int32_t mY;     ///< The height of the image.
    int32_t mComp;  ///< The number of components.

    /// @brief The default class constructor
    Image() : mSurfaceImpl(nullptr), mX(0), mY(0), mComp(0) {}
};

/// @brief The image cache.
using ImageCache = std::map<const char*, Image*>;

/// @brief The point in 2D.
template<class T>
struct Point2 {
    T x; ///< The x-coordinate.
    T y; ///< The y-coordinate.

    /// @brief The default class constructor.
    Point2() : x(0), y(0) {}

    /// @brief The class constructor
    /// @param x_ The x-coordinate.
    /// @param y_ The y-coordinate.
    Point2(T x_, T y_) : x(x_), y(y_) {}

    /// @brief The class destructor.
    ~Point2() = default;

    /// @brief Set the point.
    /// @param x_ The x-coordinate.
    /// @param y_ The y-coordinate.
    void set(T x_, T y_) {
        x = x_;
        y = y_;
    }
};

/// @brief The point in 2D with integer values.
using Point2i = Point2<int32_t>;

/// @brief The rectangle in 2D.
struct Rect {
    int32_t x1;     ///< The x-coordinate of the upper left corner.
    int32_t y1;     ///< The y-coordinate of the upper left corner.
    int32_t width;  ///< The width of the rectangle.
    int32_t height; ///< The height of the rectangle.
    int32_t x2;     ///< The x-coordinate of the lower right corner.
    int32_t y2;     ///< The y-coordinate of the lower right corner.

    /// @brief The default class constructor
    Rect() :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {}

    /// @brief The class constructor
    Rect(int32_t x, int32_t y, int32_t w, int32_t h) :
            x1(-1), y1(-1), width(-1), height(-1), x2(-1), y2(-1) {
        set(x, y, w, h);
    }

    /// @brief The class destructor.
    ~Rect() = default;

    /// @brief Check if a point is inside the rectangle.
    /// @param pt The point to check.
    /// @return true if the point is inside the rectangle, false if not.
    bool isIn(const Point2i &pt) const {
        return isIn(pt.x, pt.y);
    }

    /// @brief Check if a point is inside the rectangle.
    /// @param x_ The x-coordinate of the point.
    /// @param y_ The y-coordinate of the point.
    /// @return true if the point is inside the rectangle, false if not.
    bool isIn(int x_, int y_) const {
        if (x_ >= x1 && y_ >= y1 && x_ <=x2 && y_ <= y2) {
            return true;
        }
        return false;
    }

    /// @brief Set the rectangle.
    /// @param x The x-coordinate of the upper left corner.
    /// @param y The y-coordinate of the upper left corner.
    /// @param w The width of the rectangle.
    /// @param h The height of the rectangle.
    void set(int32_t x, int32_t y, int32_t w, int32_t h) {
        x1 = x;
        y1 = y;
        width = w;
        height = h;
        x2 = x + w;
        y2 = y + h;
    }

    /// @brief Merge the rectangle with another rectangle.
    /// @param r The rectangle to merge with.
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

enum class TextureAccessType {
    Invalid = -1,  ///< The invalid access type.
    Static = 0,     ///< The static access type.
    Streaming,      ///< The streaming access type.
    Target,         ///< The target access type.
    Count           ///< The number of access types.
};

/// @brief The alignment enum.
enum class Alignment : int32_t {
    Invalid = -1,   ///< The invalid alignment.
    Left = 0,       ///< The left alignment.
    Center,         ///< The center alignment.
    Right,          ///< The right alignment.
    Count           ///< The number of alignments.
};

/// @brief The font description struct.
struct Font {
    const char *mName;  ///< The name of the font.
    uint32_t mSize;     ///< The size of the font.
    FontImpl *mFont;    ///< The font implementation.
};

/// @brief The font cache.
using FontCache = std::map<const char*, Font*>;

/// @brief The style struct.
///
/// The style struct is used to describe the style of the tiny ui.
struct Style {
    Color4  mClearColor;    ///< The clear color.
    Color4  mFg;            ///< The foreground color.
    Color4  mBg;            ///< The background color.
    Color4  mTextColor;     ///< The text color.
    Color4  mBorder;        ///< The border color.
    int32_t mMargin;        ///< The margin.
    Font    mFont;          ///< The font.
};

/// @brief The mouse state
enum class MouseState {
    Invalid = -1,       ///< The invalid state
    LeftButton = 0,     ///< The left button
    MiddleButton,       ///< The middle button
    RightButton,        ///< The right button
    Count               ///< The number of mouse states
};

/// @brief The log severity.
enum class LogSeverity {
    Invalid = -1,   ///< Marks an invalid entry
    Message = 0,    ///< A message
    Trace,          ///< A trace message
    Debug,          ///< A debug message
    Info,           ///< An info message
    Warn,           ///< A warning message
    Error,          ///< An error message
    Count           ///< The number of log severities
};

/// @brief The tiny ui events.
struct Events {
    static constexpr int32_t InvalidEvent = -1;
    static constexpr int32_t QuitEvent = 0;
    static constexpr int32_t MouseButtonDownEvent = 1;
    static constexpr int32_t MouseButtonUpEvent = 2;
    static constexpr int32_t MouseMoveEvent = 3;
    static constexpr int32_t MouseHoverEvent = 4;
    static constexpr int32_t UpdateEvent = 5;
    static constexpr int32_t NumEvents = UpdateEvent + 1;
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
        clear();
    }

    /// @brief The class constructor
    /// @param callbackFunc The callback function.
    /// @param instance The instance to use.
    /// @param eventType The event type to use.
    CallbackI(funcCallback callbackFunc, void *instance, size_t eventType = Events::MouseButtonDownEvent) :
            mfuncCallback{ nullptr }, mInstance(instance) {
        clear();
        mfuncCallback[eventType] = callbackFunc;
    }

    /// @brief The class destructor.
    ~CallbackI() = default;

    /// @brief Will clear all callback functions.
    void clear() {
        for (size_t i = 0; i < Events::NumEvents; ++i) {
            mfuncCallback[i] = nullptr;
        }
    }
};

/// @brief The event callback array.
using EventCallbackArray = std::vector<CallbackI*>;

/// @brief The event dispatch map.
using EventDispatchMap = std::map<int32_t, EventCallbackArray>;

/// @brief Function pointer declaration for callbacks.
typedef void (*tui_log_func) (LogSeverity severity, const char *message);

/// @brief The SDL context.
struct SDLContext {
    SDL_Window   *mWindow;          ///< The window.
    SDL_Surface  *mSurface;         ///< The surface.
    SDL_Renderer *mRenderer;        ///< The renderer.
    Font         *mDefaultFont;     ///< The default font.
    Font         *mSelectedFont;    ///< The selected font.
    bool          mOwner;           ///< The owner state.

    /// @brief The default class constructor
    SDLContext() :
            mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr),
        mDefaultFont(nullptr), mSelectedFont(nullptr), mOwner(true) {}
};

/// @brief The update callback list.
using UpdateCallbackList = std::list<CallbackI*>;

/// @brief The tiny ui context.
struct Context {
    bool               mCreated;            ///< The created state.
    bool               mRequestShutdown;    ///< The request shutdown state.
    const char        *mAppTitle;           ///< The application title.
    const char        *mWindowsTitle;       ///< The window title.
    SDLContext         mSDLContext;         ///< The SDL context.
    Style              mStyle;              ///< The active style.
    Widget            *mRoot;               ///< The root widget.
    tui_log_func       mLogger = nullptr;   ///< The logger function.
    EventDispatchMap   mEventDispatchMap;   ///< The event dispatch map.
    FontCache          mFontCache;          ///< The font cache.
    ImageCache         mImageCache;         ///< The image cache.
    UpdateCallbackList mUpdateCallbackList; ///< The update callback list.

    /// @brief Will create a new tiny ui context.
    /// @param title The title of the context.
    /// @param style The style to use.
    /// @return The created context.
    static Context &create(const char *title, Style &style);
    
    /// @brief Will destroy a valid tinyui context.
    /// @param ctx  The context to destroy.
    static void destroy(Context &ctx);

private:
    /// @brief The default class constructor
    Context() :
            mCreated(false),
            mRequestShutdown(false),
            mAppTitle(nullptr),
            mWindowsTitle(nullptr),
            mSDLContext(),
            mStyle(),
            mRoot(nullptr) {
        // empty
    }
};

/// @brief The tiny ui interface.
struct TinyUi {
    /// @brief Initialize the tiny ui.
    /// @param ctx The context to initialize.
    /// @return ResultOk if the initialization was successful, ErrorCode if not.
    static ret_code init(Context &ctx);

    /// @brief Initialize the render target.
    /// @param ctx The context to initialize.
    /// @param x The x-coordinate of the render target.
    /// @param y The y-coordinate of the render target.
    /// @param w The width of the render target.
    /// @param h The height of the render target.
    /// @return ResultOk if the initialization was successful, ErrorCode if not.
    static ret_code createRenderTarget(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h);

    /// @brief Initialize the screen.
    /// @param ctx The context to initialize.
    /// @param x The x-coordinate of the screen.
    /// @param y The y-coordinate of the screen.
    /// @param w The width of the screen.
    /// @param h The height of the screen.
    /// @return ResultOk if the initialization was successful, ErrorCode if not.
    static ret_code initScreen(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h);

    /// @brief Get the surface information.
    /// @param ctx The context to get the surface information from.
    /// @param w The width of the surface.
    /// @param h The height of the surface.
    /// @return ResultOk if the information was retrieved, ErrorCode if not.
    static ret_code getSurfaceInfo(Context &ctx, int32_t &w, int32_t &h);

    /// @brief Run the tiny ui.
    /// @param ctx The context to run.
    /// @return true if the tiny ui is running, false if not.
    static bool run(Context &ctx);

    /// @brief Begins the rendering.
    /// @param ctx The context to begin the rendering.
    /// @param bg The background color for clearing.
    /// @return ResultOk if the rendering was started, ErrorCode if not.
    static ret_code beginRender(Context &ctx, Color4 bg);

    /// @brief Ends the rendering.
    /// @param ctx The context to end the rendering.
    static ret_code endRender(Context &ctx);
    
    /// @brief Release the tiny ui context.
    /// @param ctx The context to release.
    /// @return ResultOk if the context was released, ErrorCode if not.
    static ret_code release(Context &ctx);

    /// @brief Get the default style.
    /// @return The default style.
    static const Style &getDefaultStyle();

    /// @brief Set the default style.
    /// @param style The style to set as default.
    static void setDefaultStyle(const Style &style);

    /// @brief Get the default font.
    /// @param ctx The context to end the rendering.
    /// @param defaultFont The default font to set.
    static void setDefaultFont(Context &ctx, const char *defaultFont);
    
    /// @brief Will create a new context.
    /// @param title The title of the context.
    /// @param style The style to use.
    /// @return The created context.
    static Context *createContext(const char *title, Style &style);
};

} // Namespace TinyUi
