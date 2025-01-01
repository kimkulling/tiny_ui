/*
MIT License

Copyright (c) 2022-2024 Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <iostream>
#include "widgets.h"

#ifdef main
#  undef main
#endif

using namespace tinyui;

static constexpr Id RootPanelId = 1;

static constexpr Id NextPanelId = 100;

static void renderDialog(const char *title, Context &ctx) {
    Rect r(240, 90, 120, 250);
    Widgets::panel(ctx, NextPanelId, 0, title, r, nullptr);
}

int quit(uint32_t id, void *instance) {
    if (instance == nullptr) {
        return ErrorCode;
    }
    
    auto *ctx = static_cast<Context *>(instance);
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

    Widgets::panel(ctx, RootPanelId, 0, "Sample-Dialog", Rect(90, 5, 120, 400), nullptr);
    Widgets::label(ctx, 2, RootPanelId, "Title", Rect(100, 10, 100, 20), Alignment::Center);
    Widgets::button(ctx, 3, RootPanelId, "Test 1", nullptr, Rect(100, 50, 100, 40), nullptr);
    Widgets::button(ctx, 4, RootPanelId, "Test 2", nullptr, Rect(100, 100, 100, 40), nullptr);
    Widgets::button(ctx, 5, RootPanelId, "Test 3", nullptr, Rect(100, 150, 100, 40), nullptr);
    Widgets::button(ctx, 6, RootPanelId, nullptr,  "button_test.png", Rect(100, 200, 100, 40), nullptr);

    CallbackI quitCallback(quit, (void*) &ctx);
    Widgets::button(ctx, 7, RootPanelId, "Quit", nullptr, Rect(100, 250, 100, 40), &quitCallback);

    Widgets::progressBar(ctx, 8, RootPanelId, Rect(100, 300, 100, 40), 50, nullptr);

    while (TinyUi::run(ctx)) {
        TinyUi::beginRender(ctx, style.mClearColor);
        Widgets::renderWidgets(ctx);
        TinyUi::endRender(ctx);
    }

    TinyUi::release(ctx);

    return 0;
}
