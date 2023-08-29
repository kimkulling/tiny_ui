#include "tinyui.h"
#include "widgets.h"
#include "sdl2_renderer.h"

#include <cassert>
#include <iostream>
#include <cstring>

namespace tinyui {

static tui_style DefaultStyle = {
    tui_color4{  0,  100,  100, 0 }, 
    tui_color4{ 220, 220, 220,  0 }, 
    tui_color4{ 20, 20, 20, 0 }, 
    tui_color4{ 0,   255,   255, 0 }, 
    tui_color4{ 200, 200, 200, 0 }, 
    2,
    { "Arial.ttf", 20, nullptr }
};

static const char *SeverityToken[] = {
    "",
    "*TRACE*",
    "*DEBUG*",
    "*INFO* ",
    "*WARN* ",
    "*ERROR*"
};

void log_message(tui_log_severity severity, const char *message) {
    assert(message != nullptr);
    std::cout << SeverityToken[static_cast<size_t>(severity)] << " " << message <<"\n.";
}

tui_ret_code tui_init(tui_context &ctx) {
    if (ctx.mCreated) {
        return ErrorCode;
    }

    ctx.mCreated = true;

    return ResultOk;
}

tui_ret_code tui_init_screen(tui_context &ctx, int32_t x, int32_t y, int32_t w, int32_t h) {
    if (Renderer::initRenderer(ctx) == ErrorCode) {
        printf("Error: Cannot init renderer\n");
        return ErrorCode;
    }

    return Renderer::initScreen(ctx, x, y, w, h);
}

tui_ret_code tui_get_surface_info(tui_context &ctx, int32_t &w, int32_t &h) {
    w = h = -1;
    if (!ctx.mCreated) {
        return ErrorCode;
    }

    if (ctx.mSDLContext.mSurface == nullptr) {
        return ErrorCode;
    }

    w = ctx.mSDLContext.mSurface->w;
    h = ctx.mSDLContext.mSurface->h;
    
    return ResultOk;
}

bool tui_run(tui_context &ctx) {
    return Renderer::run(ctx);
}

tui_ret_code tui_begin_render(tui_context &ctx, tui_color4 bg) {
    return Renderer::beginRender(ctx, bg);
}


tui_ret_code tui_end_render(tui_context &ctx) {
    return Renderer::endRender(ctx);
}

tui_ret_code tui_release(tui_context &ctx) {
    if (!ctx.mCreated) {
        return ErrorCode;
    }
    Renderer::releaseRenderer(ctx);
    ctx.mCreated = false;

    return ResultOk;
}

tui_context &tui_context::create(const char *title, tui_style &style) {
    tui_context *ctx = new tui_context;
    ctx->mLogger = log_message;
    ctx->title = title;
    ctx->mStyle = style;
    
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

tui_context *create_context(const char *title, tui_style &style) {
    tui_context *ctx = &tui_context::create(title, style);
    
    return ctx;
}

void set_default_font(tui_context &ctx, const char *defaultFont) {
    if (defaultFont == nullptr) {
        return;
    }

    if (strncmp( ctx.mStyle.mFont.mName, defaultFont, std::strlen(defaultFont)) == 0) {
        return;
    }

    ctx.mStyle.mFont.mName = defaultFont;
}

} // namespace tinyui
