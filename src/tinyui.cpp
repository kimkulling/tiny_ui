#include "tinyui.h"
#include "widgets.h"

#include <cassert>
#include <iostream>
#include <cstring>

namespace tinyui {

static tui_style DefaultStyle = {
    tui_color4{ 120, 120, 120, 0 }, 
    tui_color4{ 50,  50,  50,  0 }, 
    tui_color4{ 230, 230, 230, 0 }, 
    tui_color4{ 0,   0,   255, 0 }, 
    tui_color4{ 200, 200, 200, 0 }, 
    2,
    { "Arial.ttf", 10 }
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

tui_context &tui_context::create() {
    tui_context *ctx = new tui_context;
    ctx->mLogger = log_message;
    
    return *ctx;
}

void tui_context::destroy(tui_context &ctx) {
    tui_context *ptr = &ctx;
    delete ptr;
}

void tui_context::enableExtensions(tui_context &ctx, const std::vector<tui_extensions> &extensions) {
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

tui_context *create_context() {
    tui_context *ctx = &tui_context::create();
    
    return ctx;
}

void set_default_font(tui_context &ctx, const char *defaultFont) {
    if (defaultFont == nullptr) {
        return;
    }

    if (0 == strncmp( ctx.mStyle.mFont.mName, defaultFont, std::strlen( defaultFont ))) {
        return;
    }

    ctx.mStyle.mFont.mName = defaultFont;
}

} // namespace tinyui
