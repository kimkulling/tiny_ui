# tiny_ui
This repo contains a small library to generate simple UIs on different platforms. I am using it for my own games.

Currently this works on Windows and Linux. The renderer is based on SDL2. 

A small example:
```cpp

#include <iostream>

#include "sdl2_renderer.h"
#include "widgets.h"

#define main main

using namespace tinyui;

int main(int argc, char *argv[]) {
    tui_context &ctx = tui_context::create();
    ctx.title = "Sample-Screen";
    if (Renderer::initRenderer(ctx) == -1) {
        printf("Error: Cannot init renderer\n");
        return -1;
    }

    if (Renderer::initScreen(ctx, 20, 20, 1024, 768) == -1) {
        printf("Error: Cannot init screen\n");
        return -1;
    }

    tui_style currentStyle = get_default_style();

    ctx.mStyle = currentStyle;
    Widgets::createPanel(ctx, 1, 0, 90, 5, 120, 200, nullptr);
    Widgets::createLabel(ctx, 2, 1, "Title", 100, 10, 100, 20);
    Widgets::createButton(ctx, 3, 1, "Test 1", nullptr, 100, 50, 100, 20, nullptr);
    Widgets::createButton(ctx, 4, 1, "Test 2", nullptr, 100, 90, 100, 20, nullptr);
    Widgets::createButton(ctx, 5, 1, "Test 3", nullptr, 100, 130, 100, 20, nullptr);
    Widgets::createButton(ctx, 6, 1, nullptr, nullptr, 100, 170, 100, 20, nullptr);
    while (Renderer::run(ctx)) {
        Renderer::beginRender(ctx, bg);
        Widgets::renderWidgets(ctx);
        Renderer::endRender(ctx);
    }
    Renderer::releaseRenderer(ctx);
    
    return 0;
}

```

Build Status: [![CMake](https://github.com/kimkulling/tiny_ui/actions/workflows/cmake.yml/badge.svg)](https://github.com/kimkulling/tiny_ui/actions/workflows/cmake.yml)
