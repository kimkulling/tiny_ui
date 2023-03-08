#include "tinyui.h"
#include "widgets.h"

#include <cassert>
#include <iostream>

namespace tinyui {

static tui_style DefaultStyle = {
    tui_color4{ 255, 255, 255, 0 }, 
    tui_color4{ 0, 0, 0, 0 }, 
    tui_color4{ 0, 0, 255, 0 }, 
    tui_color4{ 200, 200, 200, 0 }, 
    2
};

static constexpr char *SeverityToken[] = {
    "",
    "*TRACE*",
    "*DEBUG*",
    "*INFO* ",
    "*WARN* ",
    "*ERROR*"
};

void log_message(tui_log_severity severity, const char *message) {
    assert(message != nullptr);
    std::cout << SeverityToken[static_cast<size_t>(severity)] << message <<"\n.";
}

tui_ret_code tui_init(tui_context &ctx) {
    if (ctx.mCreated) {
        return -1;
    }

    return 0;
}

tui_ret_code tui_release(tui_context &ctx) {
    if (!ctx.mCreated) {
        return -1;
    }

    return 0;
}

tui_context *tui_context::create() {
    tui_context *ctx = new tui_context;
    ctx->mLogger = log_message;
    
    return ctx;
}


const tui_style &get_default_style() {
    return DefaultStyle;
}

void set_default_style(const tui_style &style) {
    DefaultStyle.mFg = style.mFg;
    DefaultStyle.mBg = style.mBg;
    DefaultStyle.mTextColor = style.mTextColor;
    DefaultStyle.mBorder = style.mBorder;
    DefaultStyle.mMargin = style.mMargin;
}

} // namespace tinyui
