```cpp
#include "widgets.h"

using namespace tinyui;

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

    const WidgetHandle panel = Widgets::panel(WidgetHandle::getRootHandle(), "Sample-Dialog", Rect(90, 5, 220, 60), nullptr);
    if (!panel.isValid()) {
        TinyUi::release();
        return -1;
    }

    auto &ctx = TinyUi::getContext();
    CallbackI *dynamicQuitCallback = new CallbackI(quit, (void*) &ctx);

    constexpr int32_t ButtonHeight = 20;
    Widgets::label(panel, "Hi, World!", Rect(100, 10, 200, ButtonHeight), Alignment::Center);
    Widgets::button(panel, "Quit", Rect(100, 30, 200, ButtonHeight), dynamicQuitCallback);
    while (TinyUi::run()) {
        TinyUi::render();
    }

    TinyUi::release();

    return 0;
}
```
