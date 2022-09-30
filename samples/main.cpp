#include <iostream>

#include "sdl2_renderer.h"
#include "widgets.h"

#define main main

using namespace TinyUi;

int main(int argc, char *argv[]) {
    Context ctx;
    ctx.title = "Sample-Screen";
    if (Renderer::initRenderer(ctx)==-1) {
        printf("Error: Cannot init renderer\n");
        return -1;
    }

    if (Renderer::initScreen(ctx, 20, 20, 1024, 768) == -1) {
        printf("Error: Cannot init screen\n");
        return -1;
    }

    std::cout << "Test\n";
    color4 fg = {125, 125, 125, 0};
    color4 bg = {0, 0, 0, 0};
    color4 tc = {0, 0, 200, 0};
    style currentStyle;
    currentStyle.mFg = fg;
    currentStyle.mBg = bg;
    currentStyle.mTextColor = tc;
    ctx.mStyle = currentStyle;
    Widgets::create_button(ctx, 1, 0, "Test 1", 100, 10, 100, 20, nullptr);
    Widgets::create_button(ctx, 2, 0, "Test 2", 100, 50, 100, 20, nullptr);
    Widgets::create_button(ctx, 2, 0, "Test 3", 100, 90, 100, 20, nullptr);
    while (Renderer::run(ctx)) {
        Renderer::beginRender(ctx, bg);
        Widgets::render_widgets(ctx);
        //Renderer::draw_rect(ctx, 10, 10, 20, 20, true, fg);
        //Renderer::draw_rect(ctx, 10, 40, 20, 20, false, fg);
        Renderer::endRender(ctx);
    }
    Renderer::releaseRenderer(ctx);
    
    return 0;
}
