#pragma once

#include "tinyui.h"
#include <SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <vector>

namespace tinyui {

struct IODevice {
    static bool update(SDL_Event &event);
};

struct Renderer {
    static tui_ret_code initRenderer(tui_context &ctx);
    static tui_ret_code releaseRenderer(tui_context &ctx);
    static tui_ret_code initScreen(tui_context &ctx, int32_t x, int32_t y, int32_t w, int32_t h);
    static tui_ret_code initScreen(tui_context &ctx, SDL_Window *mWindow, SDL_Renderer *mRenderer);
    static tui_ret_code drawText(tui_context &ctx, const char *string, tui_font *font, const tui_rect &r, const SDL_Color &fgC, const SDL_Color &bgC);
    static tui_ret_code drawRect(tui_context &ctx, int32_t x, int32_t y, int32_t w, int32_t h, bool filled, tui_color4 fg);
    static tui_ret_code drawImage(tui_context &ctx, tui_image *image);
    static tui_ret_code beginRender(tui_context &ctx, tui_color4 bg);
    static tui_ret_code endRender(tui_context &ctx);
    static tui_ret_code closeScreen(tui_context &ctx);
    static bool update(tui_context &ctx);
};

} //  namespace tinyui
