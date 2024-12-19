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
#include "sdl2_renderer.h"
#include "sdl2_iodevice.h"
#include "widgets.h"

#include <cassert>
#include <iostream>

namespace tinyui {
namespace {

static SDL_Color getSDLColor(const Color4 &col) {
    SDL_Color sdl_col = {};
    sdl_col.r = col.r;
    sdl_col.g = col.g;
    sdl_col.b = col.b;
    sdl_col.a = col.a;

    return sdl_col;
}

static void printDriverInfo(SDL_RendererInfo &info) {
    printf("Driver : %s\n", info.name);
}

static void listAllRenderDivers(Context &ctx) {
    const int numRenderDrivers = SDL_GetNumRenderDrivers();
    ctx.mLogger(LogSeverity::Message, "Available drivers:");
    for (int i = 0; i < numRenderDrivers; ++i) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        printDriverInfo(info);
    }
}

static void showDriverInUse(Context &ctx) {
    ctx.mLogger(LogSeverity::Message, "Driver in use:");
    SDL_RendererInfo info;
    SDL_GetRendererInfo(ctx.mSDLContext.mRenderer, &info);
    printDriverInfo(info);
}

static int queryDriver(Context &ctx, const char *type) {
    if (type == nullptr) {
        return -1;
    }

    const int numRenderDrivers = SDL_GetNumRenderDrivers();
    int found = -1;
    for (int i = 0; i < numRenderDrivers; ++i) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        if (strncmp(type, info.name, strlen(type)) == 0) {
            found = i;
            break;
        }
    }

    return found;
}

static void loadFont(Context &ctx) {
    ctx.mSDLContext.mDefaultFont = new Font;
    ctx.mSDLContext.mDefaultFont->mSize = ctx.mStyle.mFont.mSize;
    ctx.mSDLContext.mDefaultFont->mFont = TTF_OpenFont(ctx.mStyle.mFont.mName, ctx.mStyle.mFont.mSize);
}

static MouseState getButtonState(const SDL_MouseButtonEvent &b) {
    MouseState state = MouseState::Invalid;
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

static int32_t getEventType(Uint32 sdlType) {
    switch (sdlType) {
        case SDL_QUIT:
            return Events::QuitEvent;
        case SDL_MOUSEBUTTONDOWN:
            return Events::MouseButtonDownEvent;
        case SDL_MOUSEBUTTONUP:
            return Events::MouseButtonUpEvent;
    }
    return Events::InvalidEvent;
}

} // namespace

ret_code Renderer::initRenderer(Context &ctx) {
    if (ctx.mCreated) {
        ctx.mLogger(LogSeverity::Error, "Renderer already initialized.");
        return ErrorCode;
    }

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        ctx.mLogger(LogSeverity::Error, "Error while SDL_Init for video subsystem.");
        ctx.mCreated = false;
        return ErrorCode;
    }

    ctx.mCreated = true;

    const int numRenderDrivers = SDL_GetNumRenderDrivers();
    ctx.mLogger(LogSeverity::Message, "Available drivers:");
    for (int i=0; i<numRenderDrivers; ++i) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        printDriverInfo(info);
    }

    int imgFlags = IMG_INIT_PNG;
    if (!IMG_Init(imgFlags))  {
        ctx.mLogger(LogSeverity::Error, "Error while IMG_Init for PNG support.");
        return ErrorCode;
    }

    return ResultOk;
}

ret_code Renderer::releaseRenderer(Context &ctx) {
    if (!ctx.mCreated) {
        ctx.mLogger(LogSeverity::Error, "Not initialized.");
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

    return ResultOk;
}

ret_code Renderer::drawText(Context &ctx, const char *string, Font *font, const Rect &r, const Color4 &fgC, const Color4 &bgC, Alignment alignment) {
    if (string == nullptr) {
        return InvalidHandle;
    }

    if (ctx.mSDLContext.mDefaultFont == nullptr) {
        if (ctx.mStyle.mFont.mName != nullptr) {
            loadFont(ctx);
            if (font == nullptr) {
                font = ctx.mSDLContext.mDefaultFont;
            }
        }
    }

    if (font == nullptr) {
        return InvalidHandle;
    }


    SDL_Color text_color = getSDLColor(fgC);
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font->mFont, string, text_color);
    if (surfaceMessage == nullptr) {
        const std::string msg = "Cannot create message surface." + std::string(SDL_GetError()) + ".";
        ctx.mLogger(LogSeverity::Error, msg.c_str());
        return ErrorCode;
    }

    SDL_Texture *message = SDL_CreateTextureFromSurface(ctx.mSDLContext.mRenderer, surfaceMessage);
    if (message == nullptr) {
        const std::string msg = "Cannot create texture: " + std::string(SDL_GetError()) + ".";
        ctx.mLogger(LogSeverity::Error, msg.c_str());
        return ErrorCode;
    }
    
    int32_t margin = ctx.mStyle.mMargin;
    SDL_Rect Message_rect = {};
    switch (alignment) {
        case Alignment::Left:
            Message_rect.x = r.x1 + margin;
            Message_rect.y = r.y1 + margin;
            Message_rect.w = font->mSize;
            Message_rect.h = font->mSize;
            break;
        case Alignment::Center:
            Message_rect.x = r.x1 + 2 * margin + surfaceMessage->clip_rect.w / 2;
            Message_rect.y = r.y1 + margin;
            Message_rect.w = font->mSize;
            Message_rect.h = font->mSize;
            break;
        case Alignment::Right:
            Message_rect.x = r.x1 + surfaceMessage->clip_rect.w - font->mSize * strlen(string);
            Message_rect.y = r.y1 + margin;
            Message_rect.w = font->mSize;
            Message_rect.h = font->mSize;
            break;
    }

    SDL_RenderCopy(ctx.mSDLContext.mRenderer, message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);

    return ResultOk;
}

ret_code Renderer::initScreen(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h) {
    if (!ctx.mCreated) {
        ctx.mLogger(LogSeverity::Error, "Not initialzed.");
        return ErrorCode;
    }

    if (ctx.mSDLContext.mWindow != nullptr ) {
        ctx.mLogger(LogSeverity::Error, "Already created.");
        return ErrorCode;
    }

    TTF_Init();
    const char *title = ctx.mWindowsTitle;
    if (ctx.mWindowsTitle == nullptr) {
        title = "untitled";
    }

    ctx.mSDLContext.mWindow = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (ctx.mSDLContext.mWindow == nullptr) {
        const std::string msg = "Error while SDL_CreateWindow: " + std::string(SDL_GetError()) + ".";
        ctx.mLogger(LogSeverity::Error, msg.c_str());
        return ErrorCode;
    }

    const int driverIndex = queryDriver(ctx, "opengl");
    if (driverIndex == -1) {
        ctx.mLogger(LogSeverity::Error, "Cannot open opengl driver");
        return ErrorCode;
    }

    ctx.mSDLContext.mRenderer = SDL_CreateRenderer(ctx.mSDLContext.mWindow, driverIndex, SDL_RENDERER_ACCELERATED);
    if (nullptr == ctx.mSDLContext.mRenderer) {
        const std::string msg = "Error while SDL_CreateRenderer: " + std::string(SDL_GetError()) + ".";
        ctx.mLogger(LogSeverity::Error, msg.c_str());
        return ErrorCode;
    }

    listAllRenderDivers(ctx);

    showDriverInUse(ctx);

    ctx.mSDLContext.mSurface = SDL_GetWindowSurface(ctx.mSDLContext.mWindow);
    if (ctx.mSDLContext.mSurface == nullptr) {
        ctx.mLogger(LogSeverity::Error, "Surface pointer from window is nullptr.");
        return ErrorCode;
    }

    return ResultOk;
}

ret_code Renderer::initScreen(Context &ctx, SDL_Window *window, SDL_Renderer *renderer) {
    if (!ctx.mCreated) {
        ctx.mLogger(LogSeverity::Error, "Renderer already initialized.");
        return ErrorCode;
    }

    if (window == nullptr || renderer == nullptr) {
        ctx.mLogger(LogSeverity::Error, "Invalid render pointer detected.");
        return ErrorCode;
    }

    TTF_Init();

    ctx.mSDLContext.mRenderer = renderer;
    ctx.mSDLContext.mWindow = window;

    showDriverInUse(ctx);

    ctx.mSDLContext.mSurface = SDL_GetWindowSurface(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mOwner = false;
    ctx.mCreated = true;

    return ResultOk;
}

ret_code Renderer::beginRender(Context &ctx, Color4 bg, SDL_Texture *renderTarget) {
    if (!ctx.mCreated) {
        ctx.mLogger(LogSeverity::Error, "Not initialized.");
        return ErrorCode;
    }

    const SDL_Color sdl_bg = getSDLColor(bg);
    SDL_SetRenderDrawColor(ctx.mSDLContext.mRenderer, sdl_bg.r, sdl_bg.g, sdl_bg.b, sdl_bg.a);
    SDL_RenderClear(ctx.mSDLContext.mRenderer);

    return 0;
}

ret_code Renderer::drawRect(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h, bool filled, Color4 fg) {
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

    return ResultOk;
}

ret_code Renderer::drawImage(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h, Image *image) {
    if (image == nullptr) {
        return ErrorCode;
    }

    SDL_Rect imageRect = {x,y, w,h};
    
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ctx.mSDLContext.mRenderer, image->mSurface);
    SDL_RenderCopy(ctx.mSDLContext.mRenderer, tex, nullptr, &imageRect);
    SDL_DestroyTexture(tex);

    return ResultOk;
}

ret_code Renderer::closeScreen(Context &ctx) {
    if (ctx.mSDLContext.mWindow == nullptr) {
        return ErrorCode;
    }

    TTF_Quit();

    SDL_DestroyWindow(ctx.mSDLContext.mWindow);
    ctx.mSDLContext.mWindow = nullptr;

    return ResultOk;
}

ret_code Renderer::endRender(Context &ctx) {
    SDL_RenderPresent(ctx.mSDLContext.mRenderer);

    return ResultOk;
}

bool Renderer::update(Context &ctx) {
    if (!ctx.mCreated) {
        return false;
    }

    bool running = true;
    SDL_Event event = {};
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

            case SDL_KEYDOWN:
                {
                    const char *key = SDL_GetKeyName(event.key.keysym.sym);
                    assert(key != nullptr);
                    Widgets::onKey(key, true, ctx);
                } break;

            case SDL_KEYUP: 
                {
                    const char *key = SDL_GetKeyName(event.key.keysym.sym);
                    assert(key != nullptr);
                    Widgets::onKey(key, false, ctx);
                } break;

            default:
                break;
        }
    }

    return running;
}

} // namespace tinyui
