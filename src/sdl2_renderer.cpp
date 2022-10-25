#include "sdl2_renderer.h"
#include "widgets.h"
#include <cassert>
#include <iostream>

namespace TinyUi {

static constexpr int ErrorCode = -1;

void logError(const char *message) {
    assert(message != nullptr);

    std::cerr << "*ERR* " << message <<", error " << SDL_GetError() << "\n";
}
    
void printDriverInfo(SDL_RendererInfo &info) {
    printf("Driver : %s\n", info.name);
}

int Renderer::initRenderer(Context &ctx) {
    if (ctx.mCreated) {
        logError("Renderer already initialized.");
        return ErrorCode;
    }

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        logError("Error while SDL_Init.");
        ctx.mCreated = false;
        return ErrorCode;
    }
    TTF_Init();

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

int Renderer::releaseRenderer(Context &ctx) {
    if (!ctx.mCreated) {
        logError("Not initialized.");
        return ErrorCode;
    }

    TTF_Quit();
    SDL_DestroyRenderer(ctx.mSDLContext.mRenderer);
    ctx.mSDLContext.mRenderer = nullptr;
    SDL_DestroyWindow(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mWindow = nullptr;
    SDL_Quit();

    return 0;
}

int Renderer::drawText(Context &ctx, const char* string, int size, const rect &r, const SDL_Color &fgC, const SDL_Color &bgC) {
    TTF_Font *font = TTF_OpenFont("Arial.ttf", 24);
    if (font == nullptr) {
        printf("[ERROR] TTF_OpenFont() Failed with: %s\n", TTF_GetError());
        return ErrorCode;
    }

    SDL_Color white;
    white.r = ctx.mStyle.mTextColor.r;
    white.g = ctx.mStyle.mTextColor.g;
    white.b = ctx.mStyle.mTextColor.b;
    white.a = ctx.mStyle.mTextColor.a;
    
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, string, white); 
    if (surfaceMessage == nullptr) {
        printf("[ERROR] Cannot create message surface..\n");
        return ErrorCode;
    }
    
    SDL_Texture* Message = SDL_CreateTextureFromSurface(ctx.mSDLContext.mRenderer, surfaceMessage);
    if (Message == nullptr) {
        printf("[ERROR] Cannot create texture.\n");
        return -1;
    }
    SDL_Rect Message_rect; 
    Message_rect.x = r.x;  
    Message_rect.y = r.y; 
    Message_rect.w = r.width;
    Message_rect.h = r.height;

    SDL_RenderCopy(ctx.mSDLContext.mRenderer, Message, NULL, &Message_rect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
    TTF_CloseFont(font);

    return 0;
}

int Renderer::initScreen(Context &ctx, int x, int y, int w, int h) {
    if (!ctx.mCreated) {
        logError("Not initialized.");
        return ErrorCode;
    }

    if (ctx.mSDLContext.mWindow != nullptr ) {
        logError("Already created.");
        return ErrorCode;
    }

    const char *title = ctx.title;
    if (ctx.title == nullptr) {
        title = "untitled";
    }

    ctx.mSDLContext.mWindow = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (ctx.mSDLContext.mWindow == nullptr) {
        logError("Error while SDL_CreateWindow.");
        return ErrorCode;
    }
    
    ctx.mSDLContext.mRenderer = SDL_CreateRenderer(ctx.mSDLContext.mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (nullptr == ctx.mSDLContext.mRenderer) {
        logError("Eror while SDL_CreateRenderer.");
        return ErrorCode;
    }
    printf("Driver in use:\n");
    SDL_RendererInfo info;
    SDL_GetRendererInfo(ctx.mSDLContext.mRenderer, &info);
    printDriverInfo(info);

    ctx.mSDLContext.mSurface = SDL_GetWindowSurface(ctx.mSDLContext.mWindow);
    if (ctx.mSDLContext.mSurface == nullptr) {
        logError("Surface pointer from window is nullptr.");
        return ErrorCode;
    }

    return 0;
}

int Renderer::initScreen(Context &ctx, SDL_Window *window, SDL_Renderer *renderer) {
    if (ctx.mCreated) {
        logError("Renderer already initialized.");
        return ErrorCode;
    }

    if (window == nullptr || renderer == nullptr) {
        logError("Invalid render pointer detected.");
        return ErrorCode;
    }

    ctx.mSDLContext.mRenderer = renderer;
    ctx.mSDLContext.mWindow = window;
    ctx.mSDLContext.mSurface = SDL_GetWindowSurface(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mOwner = false;
    ctx.mCreated = true;

    return 0;
}

int Renderer::beginRender(Context &ctx, color4 bg) {
    if (!ctx.mCreated) {
        logError("Not initialized.");
        return ErrorCode;
    }

    SDL_SetRenderDrawColor(ctx.mSDLContext.mRenderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(ctx.mSDLContext.mRenderer);        

    return 0;
}

int Renderer::draw_rect(Context &ctx, int x, int y, int w, int h, bool filled, color4 fg){
    SDL_Rect r;
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

int Renderer::closeScreen(Context &ctx) {
    if (ctx.mSDLContext.mWindow == nullptr) {
        return ErrorCode;
    }

    SDL_DestroyWindow(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mWindow = nullptr;

    return 0;
}

int Renderer::endRender(Context &ctx) {
    SDL_RenderPresent(ctx.mSDLContext.mRenderer);

    return 0;
}

static MouseState getButtonState(const SDL_MouseButtonEvent &b) {
    MouseState state = MouseState::Unknown;
    switch (b.button) {
        case SDL_BUTTON_LEFT:
            state = MouseState::LeftButton;
            break;
        case SDL_BUTTON_MIDDLE:
            state = MouseState::MiddleButton;
            break;
        case SDL_BUTTON_RIGHT:
            state = MouseState::RightButton;
            break;
        case SDL_BUTTON_X1:
        case SDL_BUTTON_X2:
        default:
            break;
    }
    return state;
}

bool Renderer::run(Context &ctx) {
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
                {
                    int x = event.button.x;
                    int y = event.button.y;

                    Widgets::onMouseButton(x, y, getButtonState(event.button), ctx);
                }
                break;

            default:
                break;
        }
    }

    return running;
}

} // namespace tinyui
