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

#include "tinyui.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <vector>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace tinyui {
    
struct SurfaceImpl {
    SDL_Surface *mSurface;

    ~SurfaceImpl() {
        clear();
    }

    void clear() {
        if (mSurface != nullptr) {
            SDL_FreeSurface(mSurface);
            mSurface = nullptr;
        }

    }
};

struct FontImpl {
    TTF_Font *mFontImpl;
    
    void clear() {
        if (mFontImpl != nullptr) {
            TTF_CloseFont(mFontImpl);
            mFontImpl = nullptr;
        }
    }
    
    ~FontImpl() {
        clear();
    }
};

/// @brief The renderer implementation using the SDL2 library.
struct Renderer {
    static ret_code initRenderer(Context &ctx);
    static ret_code releaseRenderer(Context &ctx);
    static ret_code initScreen(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h);
    static ret_code initScreen(Context &ctx, SDL_Window *mWindow, SDL_Renderer *mRenderer);
    static ret_code drawText(Context &ctx, const char *string, Font *font, const Rect &r, const Color4 &fgC, const Color4 &bgC, Alignment alignment);
    static ret_code drawRect(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h, bool filled, Color4 fg);
    static ret_code drawImage(Context &ctx, int32_t x, int32_t y, int32_t w, int32_t h, Image *image);
    static ret_code beginRender(Context &ctx, Color4 bg, SDL_Texture *renderTarget = nullptr);
    static ret_code endRender(Context &ctx);
    static ret_code createRenderTexture(Context &ctx, int w, int h, SDL_Texture **texture);
    static ret_code closeScreen(Context &ctx);
    static bool update(Context &ctx);
    static SurfaceImpl *createSurfaceImpl(unsigned char *data, int w, int h, int bytesPerPixel, int pitch);
};

} //  namespace tinyui
