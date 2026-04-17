/*
MIT License

Copyright (c) 2022-2026 Kim Kulling

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
#include "widgets.h"

using namespace tinyui;

static constexpr Id RootPanelId = 1;

int quit(Id, void *instance) {
    if (instance == nullptr) {
        return ErrorCode;
    }
    
    auto *ctx = static_cast<Context *>(instance);
    ctx->mRequestShutdown = true;

    return ResultOk;
}

int main(int argc, char *argv[]) {
    if (Style style = TinyUi::getDefaultStyle(); !TinyUi::createContext("Sample-Screen", style)) {
        return -1;
    }

    if (TinyUi::initScreen(20, 20, 1024, 768) == -1) {
        const auto &ctx = TinyUi::getContext();
        ctx.mLogger(LogSeverity::Error, "Cannot init screen");
        return ErrorCode;
    }

    constexpr int32_t ButtonHeight = 18;
    Widgets::panel(RootPanelId, 0, "Sample-Dialog", Rect(90, 5, 220, 60), nullptr);
    auto &ctx = TinyUi::getContext();
    auto *dynamicQuitCallback = new CallbackI(quit, (void*) &ctx);

    Widgets::label(2, RootPanelId, "Hi, World!", Rect(100, 10, 200, ButtonHeight), Alignment::Center);
    Widgets::textButton(3, RootPanelId, "Quit", Rect(100, 30, 200, ButtonHeight), Alignment::Center, dynamicQuitCallback);
    while (TinyUi::run()) {
        TinyUi::render();
    }

    TinyUi::release();

    return 0;
}
