#include "sdl2_renderer.h"
#include "widgets.h"
#include <cassert>
#include <iostream>

namespace tinyui {

void printDriverInfo(SDL_RendererInfo &info) {
    printf("Driver : %s\n", info.name);
}

tui_ret_code Renderer::initRenderer(tui_context &ctx) {
    if (ctx.mCreated) {
        ctx.mLogger(tui_log_severity::Error, "Renderer already initialized.");
        return ErrorCode;
    }

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        ctx.mLogger(tui_log_severity::Error, "Error while SDL_Init.");
        ctx.mCreated = false;
        return ErrorCode;
    }

    ctx.mCreated = true;

    const int numRenderDrivers = SDL_GetNumRenderDrivers();
    ctx.mLogger(tui_log_severity::Message, "Available drivers:");
    for (int i=0; i<numRenderDrivers; ++i) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        printDriverInfo(info);
    }

    return 0;
}

tui_ret_code Renderer::releaseRenderer(tui_context &ctx) {
    if (!ctx.mCreated) {
        ctx.mLogger(tui_log_severity::Error, "Not initialized.");
        return ErrorCode;
    }

    if (ctx.mSDLContext.mDefaultFont != nullptr) {
        delete ctx.mSDLContext.mDefaultFont;
        ctx.mSDLContext.mDefaultFont = nullptr;
    }

    SDL_DestroyRenderer(ctx.mSDLContext.mRenderer);
    ctx.mSDLContext.mRenderer = nullptr;
    SDL_DestroyWindow(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mWindow = nullptr;
    SDL_Quit();

    return 0;
}

tui_ret_code Renderer::drawText(tui_context &ctx, const char *string, tui_font *font, const tui_rect &r, const SDL_Color &fgC, const SDL_Color &bgC) {
    if (ctx.mSDLContext.mDefaultFont == nullptr) {
        if (ctx.mStyle.mFont.mName != nullptr) {
            ctx.mStyle.mFont.mSize = 20;
            ctx.mSDLContext.mDefaultFont = new tui_font;
            ctx.mSDLContext.mDefaultFont->mSize = ctx.mStyle.mFont.mSize;
            ctx.mSDLContext.mDefaultFont->mFont = TTF_OpenFont(ctx.mStyle.mFont.mName, ctx.mStyle.mFont.mSize);
        }
    }

    if (font == nullptr) {
        font = ctx.mSDLContext.mDefaultFont;
    }

    if (font == nullptr) {
        const std::string msg = "TTF_OpenFont() Failed with: " + std::string(TTF_GetError()) + ".";
        ctx.mLogger(tui_log_severity::Error, msg.c_str());
        return ErrorCode;
    }

    SDL_Color text_color = fgC;
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font->mFont, string, text_color); 
    if (surfaceMessage == nullptr) {
        const std::string msg = "Cannot create message surface." + std::string(SDL_GetError()) + ".";
        ctx.mLogger(tui_log_severity::Error, msg.c_str());
        return ErrorCode;
    }

    SDL_Texture *message = SDL_CreateTextureFromSurface(ctx.mSDLContext.mRenderer, surfaceMessage);
    if (message == nullptr) {
        const std::string msg = "Cannot create texture: " + std::string(SDL_GetError()) + ".";
        ctx.mLogger(tui_log_severity::Error, msg.c_str());
        return ErrorCode;
    }
    SDL_Rect Message_rect = {}; 
    Message_rect.x = r.x1;  
    Message_rect.y = r.y1; 
    Message_rect.w = r.width;
    Message_rect.h = r.height;

    SDL_RenderCopy(ctx.mSDLContext.mRenderer, message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);

    return 0;
}

static void showDriverInUse(tui_context &ctx) {
    ctx.mLogger(tui_log_severity::Message, "Driver in use:");
    SDL_RendererInfo info;
    SDL_GetRendererInfo(ctx.mSDLContext.mRenderer, &info);
    printDriverInfo(info);
}
    
static void listAllRenderDivers(tui_context &ctx) {
    const int numRenderDrivers = SDL_GetNumRenderDrivers();
    ctx.mLogger(tui_log_severity::Message, "Available drivers:");
    for (int i = 0; i < numRenderDrivers; ++i) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        printDriverInfo(info);
    }
}

tui_ret_code Renderer::initScreen(tui_context &ctx, int32_t x, int32_t y, int32_t w, int32_t h) {
    if (!ctx.mCreated) {
        ctx.mLogger(tui_log_severity::Error, "Not initialzed.");
        return ErrorCode;
    }

    if (ctx.mSDLContext.mWindow != nullptr ) {
        ctx.mLogger(tui_log_severity::Error, "Already created.");
        return ErrorCode;
    }
    TTF_Init();

    const char *title = ctx.title;
    if (ctx.title == nullptr) {
        title = "untitled";
    }

    ctx.mSDLContext.mWindow = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (ctx.mSDLContext.mWindow == nullptr) {
        const std::string msg = "Error while SDL_CreateWindow: " + std::string(SDL_GetError()) + ".";
        ctx.mLogger(tui_log_severity::Error, msg.c_str());
        return ErrorCode;
    }
    
    ctx.mSDLContext.mRenderer = SDL_CreateRenderer(ctx.mSDLContext.mWindow, 2, SDL_RENDERER_ACCELERATED);
    if (nullptr == ctx.mSDLContext.mRenderer) {
        const std::string msg = "Error while SDL_CreateRenderer: " + std::string(SDL_GetError()) + ".";
        ctx.mLogger(tui_log_severity::Error, msg.c_str());
        return ErrorCode;
    }

    listAllRenderDivers(ctx);

    showDriverInUse(ctx);

    ctx.mSDLContext.mSurface = SDL_GetWindowSurface(ctx.mSDLContext.mWindow);
    if (ctx.mSDLContext.mSurface == nullptr) {
        ctx.mLogger(tui_log_severity::Error, "Surface pointer from window is nullptr.");
        return ErrorCode;
    }

    return 0;
}

tui_ret_code Renderer::initScreen(tui_context &ctx, SDL_Window *window, SDL_Renderer *renderer) {
    if (ctx.mCreated) {
        ctx.mLogger(tui_log_severity::Error, "Renderer already initialized.");
        return ErrorCode;
    }

    if (window == nullptr || renderer == nullptr) {
        ctx.mLogger(tui_log_severity::Error, "Invalid render pointer detected.");
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

tui_ret_code Renderer::beginRender(tui_context &ctx, tui_color4 bg) {
    if (!ctx.mCreated) {
        ctx.mLogger(tui_log_severity::Error, "Not initialized.");
        return ErrorCode;
    }

    SDL_SetRenderDrawColor(ctx.mSDLContext.mRenderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(ctx.mSDLContext.mRenderer);

    return 0;
}

tui_ret_code Renderer::drawRect(tui_context &ctx, int32_t x, int32_t y, int32_t w, int32_t h, bool filled, tui_color4 fg) {
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

tui_ret_code Renderer::drawImage(tui_context &ctx, tui_image *image) {
    if (image == nullptr) {
        return ErrorCode;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ctx.mSDLContext.mRenderer, image->mSurface);

    SDL_RenderCopy(ctx.mSDLContext.mRenderer, tex, nullptr, nullptr);

    SDL_DestroyTexture(tex);

    return 0;
}

tui_ret_code Renderer::closeScreen(tui_context &ctx) {
    if (ctx.mSDLContext.mWindow == nullptr) {
        return ErrorCode;
    }
    TTF_Quit();

    SDL_DestroyWindow(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mWindow = nullptr;

    return 0;
}

tui_ret_code Renderer::endRender(tui_context &ctx) {
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

static int32_t getEventType(Uint32 sdlType) {
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

bool IODevice::update(SDL_Event &event) {
    return SDL_PollEvent(&event);
};

bool Renderer::run(tui_context &ctx) {
    if (!ctx.mCreated) {
        return false;
    }

    bool running = true;
    SDL_Event event;
    while (IODevice::update(event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                {
                    const int32_t x = event.button.x;
                    const int32_t y = event.button.y;
                    Widgets::onMouseButton(x, y, getEventType(event.type), getButtonState(event.button), ctx);
                } break;

            case SDL_MOUSEMOTION:
                {
                    const int32_t x = event.button.x;
                    const int32_t y = event.button.y;
                } break;

            default:
                break;
        }
    }

    return running;
}

} // namespace tinyui
