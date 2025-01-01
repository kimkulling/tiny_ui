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
#include "backends/sdl2_renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <cassert>

namespace tinyui {

static Image *findImage(Context &ctx, const char *filename) {
    if (filename == nullptr) {
        return nullptr;
    }

    ImageCache::iterator it = ctx.mImageCache.find(filename);
    if (it == ctx.mImageCache.end()) {
        return nullptr;
    }

    return it->second;
}

static Image *loadIntoImageCache(Context &ctx, const char *filename) {
    if (filename == nullptr) {
        return nullptr;
    }

    Image *image = findImage(ctx, filename);
    if (image != nullptr) {
        return image;
    }

    image = new Image;
    if (image == nullptr) {
        return nullptr;
    }

    int w, h, bytesPerPixel;
    unsigned char *data = stbi_load(filename, &w, &h, &bytesPerPixel, 0);
    if (data == nullptr) {
        return nullptr;
    }


    int pitch = w * bytesPerPixel;
    pitch = (pitch + 3) & ~3;
    image->mSurfaceImpl = Renderer::createSurfaceImpl(data, w, h, bytesPerPixel, pitch);
    image->mX = w;
    image->mY = h;
    image->mComp = bytesPerPixel;

    ctx.mImageCache[filename] = image;

    return image;
}

static Widget *setParent(Context &ctx, Widget *child, Id parentId) {
    Widget *parent = nullptr;
    if (parentId == 0) {
        if (ctx.mRoot == nullptr) {
            ctx.mRoot = new Widget;
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

static Widget *createWidget(Context &ctx, Id id, Id parentId, const Rect &rect, WidgetType type) {
    Widget *widget = new Widget;
    if (widget == nullptr) {
        ctx.mLogger(LogSeverity::Error, "TUI-Widget cannot be created.");
        return nullptr;
    }

    widget->mId = id;
    widget->mType = type;
    widget->mRect = rect;
    widget->mParent = setParent(ctx, widget, parentId);

    return widget;
}

void eventDispatcher() {

}

ret_code Widgets::container(Context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h) {
    if (ctx.mRoot != nullptr) {
        return ErrorCode;
    }
    Rect r(x, y, w, h);
    Widget *widget = createWidget(ctx, id, parentId, r, WidgetType::Container);
    ctx.mRoot = widget;
    if (text != nullptr) {
        widget->mText.assign(text);
    }

    return ResultOk;
}

Widget *Widgets::findWidget(Id id, Widget *root) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->mId == id) {
        return root;
    }

    for (size_t i=0; i<root->mChildren.size(); ++i) {
        Widget *child = root->mChildren[i];
        if (child == nullptr) {
            continue;
        }
        if (child->mId == id) {
            return child;
        }
        Widget *foundWidget = findWidget(id, child);
        if (foundWidget != nullptr) {
            return foundWidget;
        }
    }

    return nullptr;
}

void Widgets::findSelectedWidget(int x, int y, Widget *currentChild, Widget **found) {
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

ret_code Widgets::label(Context &ctx, Id id, Id parentId, const char *text,
        int x, int y, int w, int h, Alignment alignment) {
    if (ctx.mRoot == nullptr) {
        return ErrorCode;
    }

    Rect r(x, y, w, h);
    Widget *widget = createWidget(ctx, id, parentId, r, WidgetType::Label);
    if (widget == nullptr) {
        return ErrorCode;
    }
    widget->mAlignment = alignment;
    if (text != nullptr) {
        widget->mText.assign(text);
    }

    return ResultOk;
}

ret_code Widgets::button(Context &ctx, Id id, Id parentId, const char *text,
        const char *image, int x, int y, int w, int h, CallbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return ErrorCode;
    }

    Rect r(x, y, w, h);
    Widget *child = createWidget(ctx, id, parentId, r, WidgetType::Button);
    if (child == nullptr) {
        return ErrorCode;
    }

    child->mCallback = callback;
    if (text != nullptr) {
        child->mText.assign(text);
    }

    if (image != nullptr) {
        child->mImage = loadIntoImageCache(ctx, image);
    }

    return ResultOk;
}

ret_code Widgets::box(Context &ctx, Id id, Id parentId, int x, int y, 
        int w, int h, const Color4 &color, bool filled) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return ErrorCode;
    }

    Rect r(x, y, w, h);
    Widget *child = createWidget(ctx, id, parentId, r, WidgetType::Box);
    if (child == nullptr) {
        return ErrorCode;
    }

    child->mFilledRect = filled;

    return ResultOk;
}

ret_code Widgets::panel(Context &ctx, Id id, Id parentId, const char *title, int x, int y, int w, int h, 
        CallbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        ctx.mLogger(LogSeverity::Error, "TUI-Renderer is nullptr.");
        return ErrorCode;
    }

    Rect r(x, y, w, h);
    Widget *child = createWidget(ctx, id, parentId, r, WidgetType::Panel);
    if (child == nullptr) {
        return ErrorCode;
    }

    return ResultOk;
}

ret_code Widgets::treeView(Context& ctx, Id id, Id parentId, const char* title, int x, int y, int w, int h) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        ctx.mLogger(LogSeverity::Error, "TUI-Renderer is nullptr.");
        return ErrorCode;
    }

    Rect r(x, y, w, h);
    Widget *child = createWidget(ctx, id, parentId, r, WidgetType::TreeView);
    if (child == nullptr) {
        return ErrorCode;
    }

    return ResultOk;
}

struct FilledState {
    uint32_t filledState;
};

ret_code Widgets::statusBar(Context& ctx, Id id, Id parentId, int x, int y, int w, int h) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        ctx.mLogger(LogSeverity::Error, "TUI-Renderer is nullptr.");
        return ErrorCode;
    }

    Rect r(x, y, w, h);
    Widget *child = createWidget(ctx, id, parentId, r, WidgetType::StatusBar);
    if (child == nullptr) {
        return ErrorCode;
    }
    FilledState *state = new FilledState;
    state->filledState = 0u;
    child->mContent = new uint8_t[sizeof(FilledState)];
    memcpy(child->mContent, state, sizeof(FilledState));
    
    return ResultOk;
}


static void render(Context &ctx, Widget *currentWidget) {
    if (currentWidget == nullptr) {
        return;
    }

    if (!currentWidget->mEnabled) {
        return;
    }

    if (currentWidget == nullptr) {
        return;
    }

    // Render the widget
    const Rect &r = currentWidget->mRect;
    switch( currentWidget->mType) {
        case WidgetType::Button:
            {
                Renderer::drawRect(ctx, r.x1, r.y1, r.width, r.height, true, ctx.mStyle.mFg);
                if (currentWidget->mImage != nullptr) {
                    Renderer::drawImage(ctx, r.x1, r.y1, r.width, r.height, currentWidget->mImage);
                }
                if (!currentWidget->mText.empty()) {
                    Color4 fg = ctx.mStyle.mTextColor, bg = ctx.mStyle.mBg;
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mSDLContext.mDefaultFont, 
                        currentWidget->mRect, fg, bg, currentWidget->mAlignment);
                }
            }
            break;

        case WidgetType::Label:
            {
                if (!currentWidget->mText.empty()) {
                Color4 fg = ctx.mStyle.mTextColor, bg = ctx.mStyle.mBg;
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mSDLContext.mDefaultFont, 
                        currentWidget->mRect, fg, bg, currentWidget->mAlignment);
                }
            } 
            break;

        case WidgetType::Panel:
            {
                Renderer::drawRect(ctx, r.x1, r.y1, r.width, r.height, false, ctx.mStyle.mBorder);
            }
            break;
        case WidgetType::StatusBar:
            {
                Renderer::drawRect(ctx, r.x1, r.y1, r.width, r.height, true, ctx.mStyle.mFg);
                FilledState *state = reinterpret_cast<FilledState *>(currentWidget->mContent);
                const uint32_t fillRate = state->filledState;
                const uint32_t width = r.width * fillRate / 100;
                Renderer::drawRect(ctx, r.x1, r.y1, width, r.height, true, ctx.mStyle.mFg);                       
            } break;
        case WidgetType::Container:
        case WidgetType::Box:
            {
                Renderer::drawRect(ctx, r.x1, r.y1, r.width, r.height, currentWidget->mFilledRect, ctx.mStyle.mBorder);
            }
            break;
    }

    for (auto &child : currentWidget->mChildren) {
        render(ctx, child);
    }
}

void Widgets::renderWidgets(Context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }
    render(ctx, ctx.mRoot);
}

void Widgets::onMouseButton(Context &ctx, int x, int y, int eventType, MouseState state) {
    assert(eventType >= 0);
    assert(eventType < Events::NumEvents);

    if (ctx.mRoot == nullptr) {
        return;
    }

    Widget *found = nullptr;
    findSelectedWidget(x, y, ctx.mRoot, &found);
    if (found != nullptr) {
#ifdef _DEBUG
        std::cout << "Clicked " << found->mId << "\n";
#endif // _DEBUG
        if (found->mCallback != nullptr) {
            if (found->mCallback->mfuncCallback[eventType] != nullptr) {
                found->mCallback->mfuncCallback[eventType](found->mId, found->mCallback->mInstance);
            }
        }
    } 
}

void Widgets::onMouseMove(Context &ctx, int x, int y, int eventType, MouseState state) {
    assert(eventType >= 0);
    assert(eventType < Events::NumEvents);

    if (ctx.mRoot == nullptr) {
        return;
    }

    Widget *found = nullptr;
    findSelectedWidget(x, y, ctx.mRoot, &found);
    if (found != nullptr) {
        if (found->mCallback != nullptr) {
            if (found->mCallback->mfuncCallback[eventType] != nullptr) {
                found->mCallback->mfuncCallback[eventType](found->mId, found->mCallback->mInstance);
            }
        }
    }
}

void Widgets::onKey(Context &ctx, const char *key, bool isDown) {
    if (key == nullptr) {
        return;
    }

    eventDispatcher();
}

void recursiveClear(Widget *current) {
    if (current == nullptr) {
        return;
    }
    if (current->mChildren.empty()) {
        return;
    }

    for (size_t i = 0; i < current->mChildren.size(); ++i) {
        recursiveClear(current->mChildren[i]);
    }
    delete current;
}

void Widgets::clear(Context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    Widget *current = ctx.mRoot;
    recursiveClear(current);
}


void Widgets::setEnableState(Context &ctx, Id id, bool enabled) {
    Widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        if (enabled) {
            widget->enable();
        } else {
            widget->disable();
        }
    }
}

bool Widgets::isEnabled(Context &ctx, Id id) {
    Widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        return widget->isEnabled();
    }

    return false;
}

Widget *Widgets::getWidgetById(Context &ctx, Id id) {
    return findWidget(id, ctx.mRoot);
}

} // namespace tinyui
