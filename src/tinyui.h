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
    color4 mTextColor;
};

enum class MouseState {
    Unknown = -1,
    LeftButton = 0,
    MiddleButton,
    RightButton,
    NumStates
};

struct LoggerBackend {
    void (*logMessage) (const char *message);
};

struct CallbackI {
    int (*funcCallback) (unsigned int id, void *data);
};

struct SDLContext {
    SDL_Window *mWindow;
    SDL_Surface *mSurface;
    SDL_Renderer *mRenderer;
    bool mOwner;

    SDLContext() : mWindow(nullptr), mSurface(nullptr), mRenderer(nullptr), mOwner(true) {}
};
struct Context {
    bool mCreated;
    const char *title;
    SDLContext mSDLContext;
    style mStyle;
    Widget *mRoot;

    Context() : mCreated(false), title(nullptr), mSDLContext(), mRoot(nullptr) {}
    ~Context() = default;
};

} // Namespace TinyUi
