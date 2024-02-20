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

int main(int argc, char *argv[]) {
    tui_style style = get_default_style();
    tui_context &ctx = tui_context::create("Sample-Screen",  style);

    if (tui_init_screen(ctx, 20, 20, 1024, 768) == -1) {
        printf("Error: Cannot init screen\n");
        return ErrorCode;
    }

    Widgets::createPanel(ctx, RootPanelId, 0, 90, 5, 120, 250, nullptr);
    Widgets::createLabel(ctx, 2, RootPanelId, "Title", 100, 10, 100, 20);
    Widgets::createButton(ctx, 3, RootPanelId, "Test 1", nullptr, 100, 50, 100, 20, nullptr);
    Widgets::createButton(ctx, 4, RootPanelId, "Test 2", nullptr, 100, 90, 100, 20, nullptr);
    Widgets::createButton(ctx, 5, RootPanelId, "Test 3", nullptr, 100, 130, 100, 20, nullptr);
    Widgets::createButton(ctx, 6, RootPanelId, nullptr,  nullptr, 100, 170, 100, 20, nullptr);
    Widgets::createButton(ctx, 7, RootPanelId, "Quit",   nullptr, 100, 210, 100, 20, nullptr);

    while (tui_run(ctx)) {
        tui_begin_render(ctx, style.mClearColor);
        Widgets::renderWidgets(ctx);
        tui_end_render(ctx);
    }

    tui_release(ctx);

    return 0;
}
