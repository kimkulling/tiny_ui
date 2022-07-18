#include <iostream>

#include "sdl2_renderer.h"
#define main main

int main(int argc, char *argv[]) {
    Context ctx;
    initRenderer(ctx);
    initScreen(ctx, 10,10, 100, 100);
    std::cout << "Test\n";
    while (run(ctx)) {
        SDL_RenderClear(ctx.mRenderer);        
        SDL_RenderPresent(ctx.mRenderer);
    }
    releaseRenderer(ctx);
    
    return 0;
}