```cpp
#include "widgets.h"

using namespace tinyui;

static constexpr Id RootPanelId = 1;

static constexpr Id NextPanelId = 100;

int quit(Id, void *instance) {
    if (instance == nullptr) {
        return ErrorCode;
    }
    
    auto *ctx = static_cast<Context *>(instance);
    ctx->mRequestShutdown = true;

    return ResultOk;
}

int main(int argc, char *argv[]) {
    Style style = TinyUi::getDefaultStyle();
    if (!TinyUi::createContext("Sample-Screen",  style)) {
        return -1;
    }

    if (TinyUi::initScreen(20, 20, 1024, 768) == -1) {
        auto &ctx = TinyUi::getContext();
        ctx.mLogger(LogSeverity::Error, "Cannot init screen");
        return ErrorCode;
    }

    Widgets::panel(RootPanelId, 0, "Sample-Dialog", Rect(90, 5, 220, 60), nullptr);
    auto &ctx = TinyUi::getContext();
    CallbackI *dynamicQuitCallback = new CallbackI(quit, (void*) &ctx);

    constexpr int32_t ButtonHeight = 20;
    Widgets::label(2, RootPanelId, "Hi, World!", Rect(100, 10, 200, ButtonHeight), Alignment::Center);
    Widgets::button(3, RootPanelId, "Quit", Rect(100, 30, 200, ButtonHeight), dynamicQuitCallback);
    while (TinyUi::run()) {
        TinyUi::render();
    }

    TinyUi::release();

    return 0;
}
```
