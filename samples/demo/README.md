```cpp
#include "widgets.h"
#include <cmath>
#include <iostream>

using namespace tinyui;

int quit(Id, void *instance) {
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

int updateProgressbar(Id, void *instance) {
    if (instance == nullptr) {
        return ErrorCode;
    }
    
    if (LastTick == 0) {
        LastTick = TinyUi::getTicks();
        return ResultOk;
    }

    auto *widget = (Widget*) instance;
    auto *eventPayload = (EventPayload*) widget->mContent;
    if (eventPayload->type == EventDataType::FillState) {
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
    Style style = TinyUi::getDefaultStyle();
    if (!TinyUi::createContext("Sample-Screen",  style)) {
        return -1;
    }

    auto &ctx = TinyUi::getContext();
    if (TinyUi::initScreen(20, 20, 1024, 768) == -1) {
        ctx.mLogger(LogSeverity::Error, "Cannot init screen");
        return ErrorCode;
    }

    constexpr int32_t ButtonHeight = 20;
    const WidgetHandle panel = Widgets::panel(WidgetHandle::getRootHandle(), "Sample-Dialog", Rect(90, 5, 120, 600), nullptr);
    Widgets::label(panel, "Title", Rect(100, 10, 100, 20), Alignment::Center);
    Widgets::button(panel, "Test 1", Rect(100, 50, 100, ButtonHeight), nullptr);
    Widgets::button(panel, "Test 2", Rect(100, 100, 100, ButtonHeight), nullptr);
    Widgets::button(panel, "Test 3", Rect(100, 150, 100, ButtonHeight), nullptr);
    Widgets::imageButton(panel, "button_test.png", Rect(100, 200, 100, ButtonHeight), nullptr);

    // Allocate callbacks dynamically to ensure they persist during event handling
    CallbackI *dynamicQuitCallback = new CallbackI(quit, (void*) &ctx);
    CallbackI *dynamicUpdateProgressBarCallback = new CallbackI(updateProgressbar, nullptr, Events::UpdateEvent);

    Widgets::button(panel, "Quit", Rect(100, 250, 100, ButtonHeight), dynamicQuitCallback);
    Widgets::progressBar(panel, Rect(100, 300, 100, ButtonHeight), 50, dynamicUpdateProgressBarCallback);

    Widgets::inputText(panel, Rect(100, 350, 100, ButtonHeight), Alignment::Left);

    WidgetHandle tree = Widgets::treeView(panel, "tree", Rect(100, 400, 100, ButtonHeight));
    WidgetHandle item_1 = Widgets::treeItem(tree, "Item 1");
    //Widgets::treeItem(item_1, "Item 1.1");
    WidgetHandle item_2 = Widgets::treeItem(tree, "Item 2");
    Widgets::treeItem(item_2, "Item 2.1");

    while (TinyUi::run()) {
        TinyUi::render();
    }

    TinyUi::release();

    return 0;
}
```
