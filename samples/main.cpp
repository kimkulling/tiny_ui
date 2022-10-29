#include <iostream>

#include "sdl2_renderer.h"
#include "widgets.h"

#define main main

using namespace tinyui;

int main(int argc, char *argv[]) {
    tui_context ctx;
    ctx.title = "Sample-Screen";
    if (Renderer::initRenderer(ctx) == -1) {
        printf("Error: Cannot init renderer\n");
        return -1;
    }

    if (Renderer::initScreen(ctx, 20, 20, 1024, 768) == -1) {
        printf("Error: Cannot init screen\n");
        return -1;
    }

    const tui_color4 fg = {125, 125, 125, 0};
    const tui_color4 bg = { 0, 0, 0, 0 };
    const tui_color4 tc = { 0, 0, 200, 0 };
    const tui_color4 border = {200, 200, 200,0};
    
    tui_style currentStyle;
    currentStyle.mFg = fg;
    currentStyle.mBg = bg;
    currentStyle.mTextColor = tc;
    currentStyle.mBorder = border;
    currentStyle.mMargin = 2;

    ctx.mStyle = currentStyle;
    Widgets::create_panel(ctx, 1, 0, 90, 5, 120, 120, nullptr);
    Widgets::create_button(ctx, 2, 1, "Test 1", 100, 10, 100, 20, nullptr);
    Widgets::create_button(ctx, 3, 1, "Test 2", 100, 50, 100, 20, nullptr);
    Widgets::create_button(ctx, 4, 1, "Test 3", 100, 90, 100, 20, nullptr);
    while (Renderer::run(ctx)) {
        Renderer::beginRender(ctx, bg);
        Widgets::render_widgets(ctx);
        Renderer::endRender(ctx);
    }
    Renderer::releaseRenderer(ctx);
    
    return 0;
}
