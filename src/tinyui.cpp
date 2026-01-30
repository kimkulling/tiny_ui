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
#include "tinyui.h"
#include "widgets.h"
#include "backends/sdl2_renderer.h"
#include "backends/sdl2_iodevice.h"

#include <iostream>
#include <cstring>

namespace tinyui {

static Style DefaultStyle {
    Color4{   0,   0,   0, 0 },
    Color4{ 255, 255, 255, 0 },
    Color4{  20,  20,  20, 0 },
    Color4{ 100, 100, 100, 0 },
    Color4{ 200, 200, 200, 0 },
    2,
    { "Arial.ttf", 35, nullptr }
};

static constexpr char *SeverityToken[] = {
    "",
    "*TRACE*",
    "*DEBUG*",
    "*INFO* ",
    "*WARN* ",
    "*ERROR*"
};

void log_message(LogSeverity severity, const char *message) {
    assert(message != nullptr);
    std::cout << SeverityToken[static_cast<size_t>(severity)] << " " << message <<"\n.";
}

Context *gCtx = nullptr;

Context *Context::create(const char *title, Style &style) {
    Context *ctx = new Context;
    ctx->mLogger = log_message;
    ctx->mAppTitle = title;
    ctx->mWindowsTitle = title;
    ctx->mStyle = style;

    return ctx;
}

void Context::destroy(Context *ctx) {
    delete ctx;
}

bool TinyUi::createContext(const char *title, Style &style) {
    if (gCtx != nullptr) {
        return false;
    }

    gCtx = Context::create(title,  style);

    return true;
}

bool TinyUi::destroyContext() {
    if (gCtx == nullptr) {
        return false;
    }

    Context::destroy(gCtx);
    gCtx = nullptr;

    return true;
}

Context &TinyUi::getContext() {
    assert(nullptr != gCtx);
    return *gCtx;
}

ret_code TinyUi::init() {
    auto &ctx = TinyUi::getContext();
    if (ctx.mCreated) {
        return ErrorCode;
    }

    ctx.mCreated = true;

    return ResultOk;
}

ret_code TinyUi::initScreen(int32_t x, int32_t y, int32_t w, int32_t h) {
    auto &ctx = TinyUi::getContext();
    if (Renderer::initRenderer(ctx) == ErrorCode) {
        printf("Error: Cannot init renderer\n");
        return ErrorCode;
    }

    return Renderer::initScreen(ctx, x, y, w, h);
}

ret_code TinyUi::getSurfaceInfo(int32_t &w, int32_t &h) {
    auto &ctx = TinyUi::getContext();
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

bool TinyUi::run() {
    auto &ctx = TinyUi::getContext();
    if (!ctx.mUpdateCallbackList.empty()) {
        for (auto it = ctx.mUpdateCallbackList.begin(); it != ctx.mUpdateCallbackList.end(); ++it) {
            (*it)->mfuncCallback[Events::UpdateEvent](1, (*it)->mInstance);
        }
    }
    return Renderer::update(ctx);
}

ret_code TinyUi::beginRender(Color4 bg) {
    auto &ctx = TinyUi::getContext();
    return Renderer::beginRender(ctx, bg);
}

ret_code TinyUi::endRender() {
    auto &ctx = TinyUi::getContext();
    return Renderer::endRender(ctx);
}

void TinyUi::render() {
    auto &ctx = TinyUi::getContext();
    TinyUi::beginRender(ctx.mStyle.mClearColor);
    Widgets::renderWidgets();
    TinyUi::endRender();
}

ret_code TinyUi::release() {
    auto &ctx = TinyUi::getContext();
    if (!ctx.mCreated) {
        return ErrorCode;
    }
    Renderer::releaseRenderer(ctx);
    Renderer::releaseScreen(ctx);

    ctx.mCreated = false;

    return ResultOk;
}

const Style &TinyUi::getDefaultStyle() {
    return DefaultStyle;
}

void TinyUi::setDefaultStyle(const Style &style) {
    DefaultStyle.mFg = style.mFg;
    DefaultStyle.mBg = style.mBg;
    DefaultStyle.mTextColor = style.mTextColor;
    DefaultStyle.mBorder = style.mBorder;
    DefaultStyle.mMargin = style.mMargin;
}

void TinyUi::setDefaultFont(const char *defaultFont) {
    auto &ctx = TinyUi::getContext();
    if (defaultFont == nullptr) {
        return;
    }

    if (strncmp( ctx.mStyle.mFont.mName, defaultFont, std::strlen(defaultFont)) == 0) {
        return;
    }

    ctx.mStyle.mFont.mName = defaultFont;
}

uint32_t TinyUi::getTicks() {
    return IODevice::getTicks();
}

} // namespace tinyui
