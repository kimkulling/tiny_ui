#pragma once

#include "tinyui.h"
#include <SDL.h>

#include <vector>

namespace TinyUi {

struct Renderer {
    static int initRenderer(Context &ctx);
    static int releaseRenderer(Context &ctx);

    static int initScreen(Context &ctx, int x, int y, int w, int h);
    static int draw_rect(Context &ctx, int x, int y, int w, int h, bool filled, color4 fg);
    static int beginRender(Context &ctx, color4 bg);
    static int endRender(Context &ctx);
    static int closeScreen(Context &ctx);
    static bool run(Context &ctx);
    static void onMousePress(SDL_MouseButtonEvent& b);
};

} //  namespace TinyUi
