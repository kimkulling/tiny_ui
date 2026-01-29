/*
MIT License

Copyright (c) 2022-2026 Kim Kulling

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

#include "tinyui_config.h"

#include <cstdint>
#include <cassert>
#include <vector>
#include <list>
#include <string>
#include <map>

#include "stb_image.h"

/*
 ==============================================================================
 *  Changelog:
 *  ==========
 *  0.0.2: New features:
 *         - Added mouse hover event support.
 *         - Added progress bar widget.
 *  0.0.1: Initial version.
 ==============================================================================
 */

// Compiler configurations ----------------------------------------------------
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#   define _CRT_SECURE_NO_WARNINGS
#endif

// Forward declarations -------------------------------------------------------
struct SDL_Window;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_MouseButtonEvent;

namespace tinyui {

struct SurfaceImpl;
struct FontImpl;
struct Widget;

// Type declarations ----------------------------------------------------------

/// @brief The return code type used in the ui library.
using ret_code = int32_t;

/// @brief The invalid render handle return code.
static constexpr ret_code InvalidRenderHandle = -3;
/// @brief The invalid handle return code.
static constexpr ret_code InvalidHandle = -2;
/// @brief The general error code. This will indicate an error, which was not related to a special operation.
static constexpr ret_code ErrorCode = -1;
/// @brief The ok code, everythink was fine.
static constexpr ret_code ResultOk  = 0;

/// @brief The default color type with 4 components.
struct Color4 {
    uint8_t r{ 1 };                         ///< The red component.
    uint8_t g{ 1 };                         ///< The green component.
    uint8_t b{ 1 };                         ///< The blue component.
    uint8_t a{ 1 };                         ///< The alpha component.
};

/// @brief The image data.
struct Image {
    SurfaceImpl *mSurfaceImpl{ nullptr };   ///< The surface implementation. 
    int32_t mX{ 0 };                        ///< The width of the image.
    int32_t mY{ 0 };                        ///< The height of the image.
    int32_t mComp{ 0 };                     ///< The number of components.
};

/// @brief The image cache.
using ImageCache = std::map<const char*, Image*>;

/// @brief  A 2-dimensional vector 
/// @tparam T The pod template type
template<class T>
struct Vec2 {
    T x{ -1 };     ///< The x-component
    T y{ -1 };     ///< The y-component

    /// @brief The default class constructor.
    Vec2() = default;

    /// @brief The constructor with the components.
    /// @param[in] x_    The x-component
    /// @param[in] y_    The y-component
    Vec2(T x_, T y_) : x(x_), y(y_) {}
};

/// @brief 2D vector for signed 32-bit integer.
using Vec2i = Vec2<int32_t>;

/// @brief The point in 2D.
/// @tparam T The pod template type
template<class T>
struct Point2 {
    T x{ 0 };           ///< The x-coordinate.
    T y{ 0 };           ///< The y-coordinate.

    /// @brief The default class constructor.
    Point2() = default;

    /// @brief The class constructor
    /// @param x_ The x-coordinate.
    /// @param y_ The y-coordinate.
    Point2(T x_, T y_) : x(x_), y(y_) {}

    /// @brief The class destructor.
    ~Point2() = default ;

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
    Vec2i   top;        ///< The upper left corner.
    int32_t width{-1};  ///< The width of the rectangle.
    int32_t height{-1}; ///< The height of the rectangle.
    Vec2i   bottom;     ///< The lower right corner.

    /// @brief The default class constructor
    Rect() = default;

    /// @brief The class constructor
    /// @param x    The x-coordinate of the upper left corner.
    /// @param y    The y-coordinate of the upper left corner.
    /// @param w    The width of the rectangle.
    /// @param h    The height of the rectangle.
    Rect(int32_t x, int32_t y, int32_t w, int32_t h) {
        set(x, y, w, h);
    }

    /// @brief The class destructor.
    ~Rect() = default;

    /// @brief Check if a point is inside the rectangle.
    /// @param pt   The point to check.
    /// @return true if the point is inside the rectangle, false if not.
    bool isIn(const Point2i &pt) const {
        return isIn(pt.x, pt.y);
    }

    /// @brief Check if a point is inside the rectangle.
    /// @param x_   The x-coordinate of the point.
    /// @param y_   The y-coordinate of the point.
    /// @return true if the point is inside the rectangle, false if not.
    bool isIn(int x_, int y_) const {
        if (x_ >= top.x && y_ >= top.y && x_ <= bottom.x && y_ <= bottom.y) {
            return true;
        }
        return false;
    }

    /// @brief Set the rectangle.
    /// @param x    The x-coordinate of the upper left corner.
    /// @param y    The y-coordinate of the upper left corner.
    /// @param w    The width of the rectangle.
    /// @param h    The height of the rectangle.
    void set(int32_t x, int32_t y, int32_t w, int32_t h) {
        top.x = x;
        top.y = y;
        width = w;
        height = h;
        bottom.x = x + w;
        bottom.y = y + h;
    }

    /// @brief Merge the rectangle with another rectangle.
    /// @param r The rectangle to merge with.
    void mergeWithRect(const Rect &r) {
        if (top.x > r.top.x || top.x == -1) {
            top.x = r.top.x;
        }

        if (top.y > r.top.y || top.y == -1) {
            top.y = r.top.y;
        }

        const int x2_ = top.x + r.width;
        if (bottom.x < x2_) {
            bottom.x = x2_;
            width = r.width;
        }

        const int y2_ = bottom.y + r.height;
        if (bottom.y < y2_) {
            bottom.y = y2_;
            height = r.height;
        }
    }
};

/// @brief The alignment enum.
enum class Alignment : int32_t {
    Invalid = -1,           ///< The invalid alignment.
    Left = 0,               ///< The left alignment.
    Center,                 ///< The center alignment.
    Right,                 ///< The right alignment.
    Count                   ///< The number of alignments.
};

/// @brief The font description struct.
struct Font {
    const char *mName;      ///< The name of the font.
    uint32_t mSize;         ///< The size of the font.
    FontImpl *mFont;        ///< The font implementation.
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
    int32_t mMargin{2};     ///< The margin.
    Font    mFont;          ///< The font.
};

/// @brief The mouse state, used to describe different mouse events.
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
    static constexpr int32_t InvalidEvent = -1;             ///< The invalid event.
    static constexpr int32_t QuitEvent = 0;                 ///< The quit event.
    static constexpr int32_t MouseButtonDownEvent = 1;      ///< The mouse button down event.
    static constexpr int32_t MouseButtonUpEvent = 2;        ///< The mouse button up event.
    static constexpr int32_t MouseMoveEvent = 3;            ///< The mouse move event.
    static constexpr int32_t MouseHoverEvent = 4;           ///< The mouse hover event.
    static constexpr int32_t KeyDownEvent = 5;              ///< The key down event.
    static constexpr int32_t KeyUpEvent = 6;                ///< The key up event.
    static constexpr int32_t UpdateEvent = 7;               ///< The update event.
    static constexpr int32_t NumEvents = UpdateEvent + 1;   ///< The number of events.
};

/// @brief The payload identifier for the events.
enum class EventDataType : int32_t {
    Invalid = -1,   ///< The invalid event data type.
    FillState,      ///< The fill state.    
    KeyDownState,   ///< The key down state.
    KeyUpState,     ///< The key up state.
    Count           ///< The number of event data        
};

/// @brief The event data struct.
struct EventPayload {
    static constexpr size_t EventDataSize = 16;     ///< The size of the event data.
    EventDataType type{ EventDataType::Invalid };   ///< The event data type.
    uint8_t payload[EventDataSize] = {};            ///< The event data payload.
};

/// @brief This interface is used to store all neede message handlers.
struct CallbackI {
    /// The function callback
    typedef int (*funcCallback) (uint32_t id, void *data);
    /// The function callback array, not handled callbacks are marked as a nullptr.
    funcCallback mfuncCallback[Events::NumEvents];
    /// The data instance.
    void *mInstance{nullptr};

    /// @brief The default class constructor.
    CallbackI() : mfuncCallback{ nullptr } {
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
    SDL_Window      *mWindow{nullptr};              ///< The window.
    SDL_Surface     *mSurface{ nullptr };           ///< The surface.
    SDL_Renderer    *mRenderer{ nullptr };          ///< The renderer.
    Font            *mDefaultFont{ nullptr };       ///< The default font.
    Font            *mSelectedFont{ nullptr };      ///< The selected font.
    bool            mOwner{ false };                ///< The owner state.
};

/// @brief The update callback list.
using UpdateCallbackList = std::list<CallbackI*>;

/// @brief The tiny ui context.
struct Context {
    bool               mCreated{false};             ///< The created state.
    bool               mRequestShutdown{false};     ///< The request shutdown state.
    const char        *mAppTitle{nullptr};          ///< The application title.
    const char        *mWindowsTitle{nullptr};      ///< The window title.
    SDLContext         mSDLContext;                 ///< The SDL context.
    Style              mStyle{};                    ///< The active style.
    Widget            *mRoot{nullptr};              ///< The root widget.
    Widget            *mFocus{nullptr};             ///< The widget which is in focus.
    tui_log_func       mLogger{};                   ///< The logger function.
    EventDispatchMap   mEventDispatchMap;           ///< The event dispatch map.
    FontCache          mFontCache{};                ///< The font cache.
    ImageCache         mImageCache{};               ///< The image cache.
    UpdateCallbackList mUpdateCallbackList{};       ///< The update callback list.

    /// @brief Will create a new tiny ui context.
    /// @param title The title of the context.
    /// @param style The style to use.
    /// @return The created context.
    static Context *create(const char *title, Style &style);
    
    /// @brief Will destroy a valid tinyui context.
    /// @param ctx  The context to destroy.
    static void destroy(Context *ctx);

private:
    /// @brief The default class constructor
    Context() = default;

    /// @brief The class destructor.
    ~Context() = default;
};

/// @brief The tiny ui interface.
struct TinyUi {
    /// @brief Will create the tinyui context.
    /// @param title    The app title.
    /// @param style    The style to use.
    /// @return true if successful.
    static bool createContext(const char *title, Style &style);
    
    /// @brief Will destroy the context.
    /// @return true if successful.
    static bool destroyContext();

    /// @brief Will return the current tiny ui context.
    /// @return The current tiny ui context.
    static Context &getContext();

    /// @brief Initialize the tiny ui.
    /// @param ctx The context to initialize.
    /// @return ResultOk if the initialization was successful, ErrorCode if not.
    static ret_code init();

    /// @brief Initialize the screen.
    /// @param ctx The context to initialize.
    /// @param x The x-coordinate of the screen.
    /// @param y The y-coordinate of the screen.
    /// @param w The width of the screen.
    /// @param h The height of the screen.
    /// @return ResultOk if the initialization was successful, ErrorCode if not.
    static ret_code initScreen(int32_t x, int32_t y, int32_t w, int32_t h);

    /// @brief Get the surface information.
    /// @param ctx The context to get the surface information from.
    /// @param w The width of the surface.
    /// @param h The height of the surface.
    /// @return ResultOk if the information was retrieved, ErrorCode if not.
    static ret_code getSurfaceInfo(int32_t &w, int32_t &h);

    /// @brief Run the tiny ui.
    /// @param ctx The context to run.
    /// @return true if the tiny ui is running, false if not.
    static bool run();

    /// @brief Begins the rendering.
    /// @param ctx The context to begin the rendering.
    /// @param bg The background color for clearing.
    /// @return ResultOk if the rendering was started, ErrorCode if not.
    static ret_code beginRender(Color4 bg);

    /// @brief Ends the rendering.
    /// @param ctx The context to end the rendering.
    static ret_code endRender();
    
    /// @brief Release the tiny ui context.
    /// @param ctx The context to release.
    /// @return ResultOk if the context was released, ErrorCode if not.
    static ret_code release();

    /// @brief Get the default style.
    /// @return The default style.
    static const Style &getDefaultStyle();

    /// @brief Set the default style.
    /// @param style The style to set as default.
    static void setDefaultStyle(const Style &style);

    /// @brief Get the default font.
    /// @param ctx The context to end the rendering.
    /// @param defaultFont The default font to set.
    static void setDefaultFont(const char *defaultFont);
    
    /// @brief Will return the current counted ticks in ms.
    /// @return The ticks in ms.
    static uint32_t getTicks();
};

// Utilities ------------------------------------------------------------------

/// @brief  Function to clamp the value in the given limits.
/// @param min      The lower bound for the value.
/// @param max      The upper bound for the value.
/// @param value    The value to clamp.
template<class T>
inline void clamp(T min, T max, T &value) {
    if (value < min) {
        value = min;
    }
    if (value > max) {
        value = max;
    }
}

} // Namespace TinyUi
