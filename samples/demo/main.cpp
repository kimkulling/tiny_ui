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
#include <cmath>
#include <iostream>

using namespace tinyui;

int quit(WidgetHandle, void *instance) {
    if (instance == nullptr) {
        return ErrorCode;
    }
    
    auto *ctx = static_cast<Context *>(instance);
    ctx->mRequestShutdown = true;

    return ResultOk;
}

static uint32_t LastTick = 0;
static uint32_t Diff = 0;
static constexpr uint32_t TimeDiff = 10;

int updateProgressbar(WidgetHandle, void *instance) {
    if (instance == nullptr) {
        return ErrorCode;
    }
    
    if (LastTick == 0) {
        LastTick = TinyUi::getTicks();
        return ResultOk;
    }

    auto *widget = (Widget*) instance;
    if (auto *eventPayload = (EventPayload *)widget->mContent; eventPayload->type == EventDataType::FillState) {
        FilledState *state = (FilledState*) (eventPayload->payload);
        uint32_t tick = TinyUi::getTicks();
        uint32_t diff = tick - LastTick;
        Diff += diff;
        if (Diff > TimeDiff) {
            state->filledState++;
            Diff = 0;
            if (state->filledState > 100) {
                state->filledState = 0;
            }
        }
        LastTick = tick;
    }

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

    constexpr int32_t ButtonHeight = 25;
    WidgetHandle panel = Widgets::panel(WidgetHandle::getRootHandle(), "Sample-Dialog", Rect(90, 5, 120, 600), nullptr);
    Widgets::label(panel, "Title", Rect(100, 10, 100, 20), Alignment::Center);
    Widgets::textButton(panel, "Test 1", Rect(100, 50, 100, ButtonHeight), Alignment::Center, nullptr);
    Widgets::textButton(panel, "Test 2", Rect(100, 100, 100, ButtonHeight), Alignment::Center, nullptr);
    Widgets::textButton(panel, "Test 3", Rect(100, 150, 100, ButtonHeight), Alignment::Center, nullptr);
    Widgets::imageButton(panel, "button_test.png", Rect(100, 200, 100, ButtonHeight), nullptr);

    auto &ctx = TinyUi::getContext();

    // Allocate callbacks dynamically to ensure they persist during event handling
    CallbackI *dynamicQuitCallback = new CallbackI(quit, (void*) &ctx);
    CallbackI *dynamicUpdateProgressBarCallback = new CallbackI(updateProgressbar, nullptr, Events::UpdateEvent);

    Widgets::textButton(panel, "Quit", Rect(100, 250, 100, ButtonHeight), Alignment::Center, dynamicQuitCallback);
    Widgets::progressBar(panel, Rect(100, 300, 100, ButtonHeight), 50, dynamicUpdateProgressBarCallback);

    Widgets::inputText(panel, Rect(100, 350, 100, ButtonHeight), Alignment::Left, KeyInputType::Character, "");

    WidgetHandle tree = Widgets::treeView(panel, "tree", Rect(100, 400, 100, ButtonHeight));
    Widgets::treeItem(tree, "Item 1");
    //Widgets::treeItem(12, 11, "Item 1.1");
    WidgetHandle view = Widgets::treeItem(tree, "Item 2");
    Widgets::treeItem(view, "Item 2.1");

    while (TinyUi::run()) {
        TinyUi::render();
    }

    TinyUi::release();

    return 0;
}
