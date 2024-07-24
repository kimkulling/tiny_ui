/*
MIT License

Copyright (c) 2022-2023 Kim Kulling

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
#include <vector>

namespace tinyui {

struct Renderer {
    static tui_ret_code initRenderer(tui_context &ctx);
    static tui_ret_code releaseRenderer(tui_context &ctx);
    static tui_ret_code initScreen(tui_context &ctx, int32_t x, int32_t y, int32_t w, int32_t h);
    static tui_ret_code initScreen(tui_context &ctx, SDL_Window *mWindow, SDL_Renderer *mRenderer);
    static tui_ret_code drawText(tui_context &ctx, const char *string, tui_font *font, const tui_rect &r, const tui_color4 &fgC, const tui_color4 &bgC);
    static tui_ret_code drawRect(tui_context &ctx, int32_t x, int32_t y, int32_t w, int32_t h, bool filled, tui_color4 fg);
    static tui_ret_code drawImage(tui_context &ctx, tui_image *image);
    static tui_ret_code beginRender(tui_context &ctx, tui_color4 bg, SDL_Texture *renderTarget = nullptr);
    static tui_ret_code endRender(tui_context &ctx);
    static tui_ret_code closeScreen(tui_context &ctx);
    static bool update(tui_context &ctx);
};

} //  namespace tinyui
