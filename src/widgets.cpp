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
#include "widgets.h"
#include "sdl2_renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cassert>

namespace tinyui {

static tui_image *findImage(tui_context &ctx, const char *filename) {
    if (filename == nullptr) {
        return nullptr;
    }

    tui_ImageCache::iterator it = ctx.mImageCache.find(filename);
    if (it == ctx.mImageCache.end()) {
        return nullptr;
    }

    return it->second;
}

static tui_image *loadIntoImageCache(tui_context &ctx, const char *filename) {
    if (filename == nullptr) {
        return nullptr;
    }

    tui_image *image = findImage(ctx, filename);
    if (image != nullptr) {
        return image;
    }

    image = new tui_image;
    if (image == nullptr) {
        return nullptr;
    }

    int32_t x, y, comp;
    unsigned char *data = stbi_load(filename, &x, &y, &comp, 4);
    if (data == nullptr) {
        return nullptr;
    }
    int32_t format = SDL_PIXELFORMAT_RGBA32;
    image->mSurface = SDL_CreateRGBSurfaceWithFormatFrom(data, x, y, comp, 1, format);
    image->mX = x;
    image->mY = y;
    image->mComp = comp;

    ctx.mImageCache[filename] = image;

    return image;
}

static tui_widget *createWidget(tui_context &ctx, Id id) {
    tui_widget *widget = new tui_widget;
    widget->mId = id;

    return widget;
}

void event_dispatcher() {

}

tui_widget *setParent(tui_context &ctx, tui_widget *child, Id parentId) {
    tui_widget *parent = nullptr;
    if (parentId == 0) {
        if (ctx.mRoot == nullptr) {
            ctx.mRoot = new tui_widget;
            ctx.mRoot->mType = WidgetType::Container;
        }
        parent = ctx.mRoot;
    } else {
        parent = Widgets::findWidget(parentId, ctx.mRoot);
    }

    parent->mChildren.emplace_back(child);
    parent->mRect.mergeWithRect(child->mRect);

    return parent;
}

tui_ret_code Widgets::Container(tui_context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h) {
    if (ctx.mRoot != nullptr) {
        return ErrorCode;
    }

    tui_widget *widget = createWidget(ctx, id);
    ctx.mRoot = widget;
    widget->mRect.set(x, y, w, h);
    if (text != nullptr) {
        widget->mText.assign(text);
    }
    widget->mParent = setParent(ctx, widget, parentId);

    return ResultOk;
}

tui_widget *Widgets::findWidget(Id id, tui_widget *root) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->mId == id) {
        return root;
    }

    for (size_t i=0; i<root->mChildren.size(); ++i) {
        tui_widget *child = root->mChildren[i];
        if (child == nullptr) {
            continue;
        }
        if (child->mId == id) {
            return child;
        }
        tui_widget *foundWidget = findWidget(id, child);
        if (foundWidget != nullptr) {
            return foundWidget;
        }
    }

    return nullptr;
}

void Widgets::findSelectedWidget(int x, int y, tui_widget *currentChild, tui_widget **found) {
    if (found == nullptr) {
        return;
    }

    if (currentChild == nullptr) {
        return;
    }

    if (currentChild->mRect.isIn(x, y)) {
        *found = currentChild;
        for ( auto &child : currentChild->mChildren) {
            if (child->mRect.isIn(x, y)){
                findSelectedWidget(x, y, child, found);
            }
        }
    }
}

tui_ret_code Widgets::Label(tui_context &ctx, Id id, Id parentId, const char *text,
        int x, int y, int w, int h) {
    if (ctx.mRoot == nullptr) {
        return ErrorCode;
    }

    tui_widget *widget = createWidget(ctx, id);
    widget->mRect.set(x, y, w, h);
    widget->mType = WidgetType::Label;
    if (text != nullptr) {
        widget->mText.assign(text);
    }
    widget->mParent = setParent(ctx, widget, parentId);

    return ResultOk;
}

tui_ret_code Widgets::Button(tui_context &ctx, Id id, Id parentId, const char *text,
        tui_image *image, int x, int y, int w, int h, tui_callbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return ErrorCode;
    }

    tui_widget *childWidget = createWidget(ctx, id);
    if (childWidget == nullptr) {
        return ErrorCode;
    }

    childWidget->mType = WidgetType::Button;
    childWidget->mRect.set(x, y, w, h);
    childWidget->mCallback = callback;
    if (text != nullptr) {
        childWidget->mText.assign(text);
    }

    if (image != nullptr) {
        childWidget->mImage = image;
    }

    childWidget->mParent = setParent(ctx, childWidget, parentId);

    return ResultOk;
}

tui_ret_code Widgets::Box(tui_context &ctx, Id id, Id parentId, int x, int y, 
        int w, int h, const tui_color4 &color, bool filled) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return ErrorCode;
    }

    tui_widget *childWidget = createWidget(ctx, id);
    if (childWidget == nullptr) {
        return ErrorCode;
    }

    childWidget->mRect.set(x, y, w, h);
    childWidget->mType = WidgetType::Box;
    childWidget->mParent = setParent(ctx, childWidget, parentId);
    childWidget->mFilledRect = filled;

    return ResultOk;
}

tui_ret_code Widgets::Panel(tui_context &ctx, Id id, Id parentId, int x, int y, int w, int h, 
        tui_callbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        ctx.mLogger(tui_log_severity::Error, "TUI-Renderer is nullptr.");
        return ErrorCode;
    }

    tui_widget *child = createWidget(ctx, id);
    if (child == nullptr) {
        ctx.mLogger(tui_log_severity::Error, "TUI-Widget cannot be created.");
        return ErrorCode;
    }

    child->mType = WidgetType::Panel;
    child->mRect.set(x, y, w, h);
    child->mParent = setParent(ctx, child, parentId);

    return ResultOk;
}

static void render(tui_context &ctx, tui_widget *currentWidget) {
    if (!currentWidget->mEnabled) {
        return;
    }

    if (currentWidget == nullptr) {
        return;
    }

    const tui_rect &r = currentWidget->mRect;
    switch( currentWidget->mType) {
        case WidgetType::Button:
            {
                Renderer::drawRect(ctx, r.x1, r.y1, r.width, r.height, true, ctx.mStyle.mFg);
                if (currentWidget->mImage != nullptr) {
                    Renderer::drawImage(ctx, currentWidget->mImage);
                }
                if (!currentWidget->mText.empty()) {
                    tui_color4 fg = { 0x00, 0x00, 0xff }, bg = { 0xff, 0xff, 0xff };
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mSDLContext.mDefaultFont, currentWidget->mRect, fg, bg);
                }
            }
            break;

        case WidgetType::Label:
            {
                if (!currentWidget->mText.empty()) {
                    const tui_color4 fg = { 0x00,0x00,0xff,0x00 }, bg = {0xff,0xff,0xff, 0x00};
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mSDLContext.mDefaultFont, currentWidget->mRect, fg, bg);
                }
            } 
            break;

        case WidgetType::Panel:
            {
                Renderer::drawRect(ctx, r.x1, r.y1, r.width, r.height, false, ctx.mStyle.mBorder);
            }
            break;

        case WidgetType::Box:
            {
                Renderer::drawRect(ctx, r.x1, r.y1, r.width, r.height, currentWidget->mFilledRect, ctx.mStyle.mBorder);
            }
            break;

       default:
            break;
    }

    for (auto &child : currentWidget->mChildren) {
        render(ctx, child);
    }
}

void Widgets::renderWidgets(tui_context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }
    render(ctx, ctx.mRoot);
}

void Widgets::onMouseButton(int x, int y, int eventType, tui_mouseState state, tui_context &ctx) {
    assert(eventType >= 0);
    assert(eventType < tui_events::NumEvents);

    if (ctx.mRoot == nullptr) {
        return;
    }

    tui_widget *found = nullptr;
    findSelectedWidget(x, y, ctx.mRoot, &found);
    if (found != nullptr) {
#ifdef _DEBUG
        printf("Clicked %d\n", found->mId);
#endif // _DEBUG
        if (found->mCallback != nullptr) {
            if (found->mCallback->mfuncCallback[eventType] != nullptr) {
                found->mCallback->mfuncCallback[eventType](found->mId, found->mCallback->mInstance);
            }
        }
    } 
}

void Widgets::onMouseMove(int x, int y, int eventType, tui_mouseState state, tui_context &ctx) {
    assert(eventType >= 0);
    assert(eventType < tui_events::NumEvents);

    if (ctx.mRoot == nullptr) {
        return;
    }

    tui_widget *found = nullptr;
    findSelectedWidget(x, y, ctx.mRoot, &found);
    if (found != nullptr) {
        if (found->mCallback != nullptr) {
            if (found->mCallback->mfuncCallback[eventType] != nullptr) {
                found->mCallback->mfuncCallback[eventType](found->mId, found->mCallback->mInstance);
            }
        }
    }
}

void Widgets::onKey(const char *key, bool isDown, tui_context &ctx) {
    if (key == nullptr) {
        return;
    }

    event_dispatcher();
}

void recursiveClear(tui_widget *current) {
    if (current == nullptr) {
        return;
    }
    if (current->mChildren.empty()) {
        return;
    }

    for (size_t i = 0; i < current->mChildren.size(); ++i) {
        recursiveClear(current->mChildren[i]);
        delete current;
    }
}

void Widgets::clear(tui_context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    tui_widget *current = ctx.mRoot;
    recursiveClear(current);
}


void Widgets::setEnableState(tui_context &ctx, Id id, bool enabled) {
    tui_widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        if (enabled) {
            widget->enable();
        } else {
            widget->disable();
        }
    }
}

bool Widgets::isEnabled(tui_context &ctx, Id id) {
    tui_widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        return widget->isEnabled();
    }

    return false;
}

tui_widget *Widgets::getWidgetById(tui_context &ctx, Id id) {
    tui_widget *widget = findWidget(id, ctx.mRoot);

    return widget;
}

} // namespace tinyui
