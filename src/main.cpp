#include <iostream>

#include "sdl2_renderer.h"

#define main main

int main(int argc, char *argv[]) {
    Context ctx;
    ctx.title = "Sample-Screen";
    TinyUi::initRenderer(ctx);
    TinyUi::initScreen(ctx, 20, 20, 1024, 768);
    std::cout << "Test\n";
    color4 fg = {125, 125, 125, 0};
    color4 bg = {0, 0, 0, 0};
    while (TinyUi::run(ctx)) {
        TinyUi::begin(ctx, bg);
        TinyUi::draw_rect(ctx, 10, 10, 20, 20, true, fg);
        TinyUi::draw_rect(ctx, 10, 40, 20, 20, false, fg);
        TinyUi::end(ctx);
    }
    TinyUi::releaseRenderer(ctx);
    
    return 0;
}
