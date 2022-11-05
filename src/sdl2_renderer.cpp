#include "sdl2_renderer.h"
#include "widgets.h"
#include <cassert>
#include <iostream>

namespace tinyui {

static constexpr int ErrorCode = -1;

void logError(const char *message) {
    assert(message != nullptr);

    std::cerr << "*ERR* " << message <<", error " << SDL_GetError() << "\n";
}
    
void printDriverInfo(SDL_RendererInfo &info) {
    printf("Driver : %s\n", info.name);
}

int Renderer::initRenderer(tui_context &ctx) {
    if (ctx.mCreated) {
        logError("Renderer already initialized.");
        return ErrorCode;
    }

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        logError("Error while SDL_Init.");
        ctx.mCreated = false;
        return ErrorCode;
    }

    ctx.mCreated = true;

    const int numRenderDrivers = SDL_GetNumRenderDrivers();
    printf("Available drivers:\n");
    for (int i=0; i<numRenderDrivers; ++i) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        printDriverInfo(info);
    }

    return 0;
}

int Renderer::releaseRenderer(tui_context &ctx) {
    if (!ctx.mCreated) {
        logError("Not initialized.");
        return ErrorCode;
    }
    if (ctx.mSDLContext.mDefaultFont != nullptr) {
        TTF_CloseFont(ctx.mSDLContext.mDefaultFont);
        ctx.mSDLContext.mDefaultFont = nullptr;
    }

    SDL_DestroyRenderer(ctx.mSDLContext.mRenderer);
    ctx.mSDLContext.mRenderer = nullptr;
    SDL_DestroyWindow(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mWindow = nullptr;
    SDL_Quit();

    return 0;
}

int Renderer::drawText(tui_context &ctx, const char *string, int size, const tui_rect &r, const SDL_Color &fgC, const SDL_Color &bgC) {
    if (ctx.mSDLContext.mDefaultFont == nullptr) {
        ctx.mSDLContext.mDefaultFont = TTF_OpenFont("Arial.ttf", 32);
    }

    TTF_Font *font = ctx.mSDLContext.mDefaultFont;
    if (font == nullptr) {
        printf("[ERROR] TTF_OpenFont() Failed with: %s\n", TTF_GetError());
        return ErrorCode;
    }

    SDL_Color text_color = {};
    text_color.r = ctx.mStyle.mTextColor.r;
    text_color.g = ctx.mStyle.mTextColor.g;
    text_color.b = ctx.mStyle.mTextColor.b;
    text_color.a = ctx.mStyle.mTextColor.a;
    
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, string, text_color); 
    if (surfaceMessage == nullptr) {
        printf("[ERROR] Cannot create message surface..\n");
        return ErrorCode;
    }
    
    SDL_Texture* Message = SDL_CreateTextureFromSurface(ctx.mSDLContext.mRenderer, surfaceMessage);
    if (Message == nullptr) {
        printf("[ERROR] Cannot create texture.\n");
        return ErrorCode;
    }
    SDL_Rect Message_rect = {}; 
    Message_rect.x = r.x1;  
    Message_rect.y = r.y1; 
    Message_rect.w = r.width;
    Message_rect.h = r.height;

    SDL_RenderCopy(ctx.mSDLContext.mRenderer, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);

    return 0;
}

static void showDriverInUse(tui_context &ctx) {
    printf("Driver in use:\n");
    SDL_RendererInfo info;
    SDL_GetRendererInfo(ctx.mSDLContext.mRenderer, &info);
    printDriverInfo(info);
}
    
static void listAllRenderDivers(tui_context &ctx) {
    const int numRenderDrivers = SDL_GetNumRenderDrivers();
    printf("Available drivers:\n");
    for (int i = 0; i < numRenderDrivers; ++i) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        printDriverInfo(info);
    }
}

int Renderer::initScreen(tui_context &ctx, int x, int y, int w, int h) {
    if (!ctx.mCreated) {
        logError("Not initialized.");
        return ErrorCode;
    }

    if (ctx.mSDLContext.mWindow != nullptr ) {
        logError("Already created.");
        return ErrorCode;
    }
    TTF_Init();

    const char *title = ctx.title;
    if (ctx.title == nullptr) {
        title = "untitled";
    }

    ctx.mSDLContext.mWindow = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (ctx.mSDLContext.mWindow == nullptr) {
        logError("Error while SDL_CreateWindow.");
        return ErrorCode;
    }
    
    ctx.mSDLContext.mRenderer = SDL_CreateRenderer(ctx.mSDLContext.mWindow, 2, SDL_RENDERER_ACCELERATED);
    if (nullptr == ctx.mSDLContext.mRenderer) {
        logError("Error while SDL_CreateRenderer.");
        return ErrorCode;
    }

    listAllRenderDivers(ctx);

    showDriverInUse(ctx);

    ctx.mSDLContext.mSurface = SDL_GetWindowSurface(ctx.mSDLContext.mWindow);
    if (ctx.mSDLContext.mSurface == nullptr) {
        logError("Surface pointer from window is nullptr.");
        return ErrorCode;
    }

    return 0;
}

int Renderer::initScreen(tui_context &ctx, SDL_Window *window, SDL_Renderer *renderer) {
    if (ctx.mCreated) {
        logError("Renderer already initialized.");
        return ErrorCode;
    }

    if (window == nullptr || renderer == nullptr) {
        logError("Invalid render pointer detected.");
        return ErrorCode;
    }
    TTF_Init();

    ctx.mSDLContext.mRenderer = renderer;
    ctx.mSDLContext.mWindow = window;

    showDriverInUse(ctx);

    ctx.mSDLContext.mSurface = SDL_GetWindowSurface(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mOwner = false;
    ctx.mCreated = true;

    return 0;
}

int Renderer::beginRender(tui_context &ctx, tui_color4 bg) {
    if (!ctx.mCreated) {
        logError("Not initialized.");
        return ErrorCode;
    }

    SDL_SetRenderDrawColor(ctx.mSDLContext.mRenderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(ctx.mSDLContext.mRenderer);        

    return 0;
}

int Renderer::draw_rect(tui_context &ctx, int x, int y, int w, int h, bool filled, tui_color4 fg) {
    SDL_Rect r = {};
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    SDL_SetRenderDrawColor(ctx.mSDLContext.mRenderer, fg.r, fg.g, fg.b, fg.a);
    if (filled) {
        SDL_RenderFillRect(ctx.mSDLContext.mRenderer, &r);
    } else {
        SDL_RenderDrawRect(ctx.mSDLContext.mRenderer, &r);
    }

    return 0;
}

int Renderer::closeScreen(tui_context &ctx) {
    if (ctx.mSDLContext.mWindow == nullptr) {
        return ErrorCode;
    }
    TTF_Quit();

    SDL_DestroyWindow(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mWindow = nullptr;

    return 0;
}

int Renderer::endRender(tui_context &ctx) {
    SDL_RenderPresent(ctx.mSDLContext.mRenderer);

    return 0;
}

static tui_mouseState getButtonState(const SDL_MouseButtonEvent &b) {
    tui_mouseState state = tui_mouseState::Unknown;
    switch (b.button) {
        case SDL_BUTTON_LEFT:
            state = tui_mouseState::LeftButton;
            break;
        case SDL_BUTTON_MIDDLE:
            state = tui_mouseState::MiddleButton;
            break;
        case SDL_BUTTON_RIGHT:
            state = tui_mouseState::RightButton;
            break;
        case SDL_BUTTON_X1:
        case SDL_BUTTON_X2:
        default:
            break;
    }
    return state;
}

static int getEventType(Uint32 sdlType) {
    switch (sdlType) {
        case SDL_QUIT:
            return tui_events::QuitEvent;
        case SDL_MOUSEBUTTONDOWN:
            return tui_events::MouseButtorDownEvent;
        case SDL_MOUSEBUTTONUP:
            return tui_events::MouseButtorUpEvent;
    }
    return tui_events::InvalidEvent;
}

bool Renderer::run(tui_context &ctx) {
    if (!ctx.mCreated) {
        return false;
    }

    bool running = true;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                {
                    const int x = event.button.x;
                    const int y = event.button.y;
                    Widgets::onMouseButton(x, y, getEventType(event.type), getButtonState(event.button), ctx);
                } break;

                {
                } break;

            default:
                break;
        }
    }

    return running;
}

} // namespace tinyui
