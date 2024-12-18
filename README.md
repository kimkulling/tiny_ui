# tiny_ui
This repo contains a small library to generate simple UIs on different platforms. I am using it for my own games.

Currently this works on Windows and Linux. The renderer is based on SDL2. An OSRE Renderbackend is planned.

# Features
- Platform support for
  - Windows
  - Linux
 
# Planned
- Treeview
- Default dialogs

# Examples:
## Quickstart

```cpp
#include <iostream>
#include "widgets.h"

#ifdef main
#  undef main
#endif

using namespace tinyui;

int main(int argc, char *argv[]) {
    Style style = getDefaultStyle();
    Context &ctx = Context::create("Sample-Screen",  style);

    if (initScreen(ctx, 20, 20, 1024, 768) == -1) {
        std::cerr<< "Error: Cannot init screen\n";
        return -1;
    }

    Widgets::Panel(ctx, 1, 0, 90, 5, 120, 200, nullptr);
    Widgets::Label(ctx, 2, 1, "Title", 100, 10, 100, 20);
    Widgets::Button(ctx, 3, 1, "Test 1", nullptr, 100, 50, 100, 20, nullptr);
    Widgets::Button(ctx, 4, 1, "Test 2", nullptr, 100, 90, 100, 20, nullptr);
    Widgets::Button(ctx, 5, 1, "Test 3", nullptr, 100, 130, 100, 20, nullptr);
    Widgets::Button(ctx, 6, 1, nullptr, nullptr, 100, 170, 100, 20, nullptr);

    while (tui_run(ctx)) {
        beginRender(ctx, style.mClearColor);
        Widgets::renderWidgets(ctx);
        endRender(ctx);
    }
    release(ctx);

    return 0;
}
```
results to

![Sample screen](assets/images/sample1.png "The sample screen").

Build Status: [![CMake](https://github.com/kimkulling/tiny_ui/actions/workflows/cmake.yml/badge.svg)](https://github.com/kimkulling/tiny_ui/actions/workflows/cmake.yml)
