#pragma once

#include "tinyui.h"
#include <SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <vector>

namespace tinyui {

struct Renderer {
    static int initRenderer(tui_context &ctx);
    static int releaseRenderer(tui_context &ctx);

    static int initScreen(tui_context &ctx, int x, int y, int w, int h);
    static int initScreen(tui_context &ctx, SDL_Window *mWindow, SDL_Renderer *mRenderer);
    static int drawText(tui_context &ctx, const char *string, int size, const tui_rect &r, const SDL_Color &fgC, const SDL_Color &bgC);
    static int draw_rect(tui_context &ctx, int x, int y, int w, int h, bool filled, tui_color4 fg);
    static int beginRender(tui_context &ctx, tui_color4 bg);
    static int endRender(tui_context &ctx);
    static int closeScreen(tui_context &ctx);
    static bool run(tui_context &ctx);
};

} //  namespace tinyui
