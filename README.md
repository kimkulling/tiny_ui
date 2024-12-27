<p align="center">
  <img src="https://github.com/kimkulling/tiny_ui/blob/master/assets/images/tinyui_logo.webp" />
</p>

# tiny_ui
This repo contains a small library to generate simple UIs on different platforms. I am using it for my own games.

Currently this works on Windows and Linux. The renderer is based on SDL2. An OSRE Renderbackend is planned.

# Features
- Elements
  - Panel
  - Buttons
- Platform support for
  - Windows
  - Linux
 
# Planned
- Treeview
- Status Bar
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

static int quit(unsigned int id, void *data) {
    if (data == nullptr) {
        return ErrorCode;
    }
    
    Context *ctx = static_cast<Context*>(data);
    ctx->mRequestShutdown = true;

    return ResultOk;
}

int main(int argc, char *argv[]) {
    Style style = TinyUi::getDefaultStyle();
    Context &ctx = Context::create("Sample-Screen",  style);

    if (TinyUi::initScreen(ctx, 20, 20, 1024, 768) == -1) {
        ctx.mLogger(LogSeverity::Error, "Cannot init screen");
        return ErrorCode;
    }

    Widgets::panel(ctx, RootPanelId, 0, "Sample-Dialog", 90, 5, 120, 300, nullptr);
    Widgets::label(ctx, 2, RootPanelId, "Title", 100, 10, 100, 20, Alignment::Center);
    Widgets::button(ctx, 3, RootPanelId, "Test 1", nullptr, 100, 50, 100, 40, nullptr);
    Widgets::button(ctx, 4, RootPanelId, "Test 2", nullptr, 100, 100, 100, 40, nullptr);
    Widgets::button(ctx, 5, RootPanelId, "Test 3", nullptr, 100, 150, 100, 40, nullptr);
    Widgets::button(ctx, 6, RootPanelId, nullptr,  "button_test.png", 100, 200, 100, 40, nullptr);

    CallbackI quit(quit, &ctx);
    Widgets::button(ctx, 7, RootPanelId, "Quit", nullptr, 100, 250, 100, 40, &quit);

    while (TinyUi::run(ctx)) {
        TinyUi::beginRender(ctx, style.mClearColor);
        Widgets::renderWidgets(ctx);
        TinyUi::endRender(ctx);
    }

    TinyUi::release(ctx);

    return 0;
}

```
results to

![Sample screen](assets/images/sample1.png "The sample screen").

Build Status: [![CMake](https://github.com/kimkulling/tiny_ui/actions/workflows/cmake.yml/badge.svg)](https://github.com/kimkulling/tiny_ui/actions/workflows/cmake.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=kimkulling_tiny_ui&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=kimkulling_tiny_ui)
