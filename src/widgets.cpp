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

    int32_t pitch = w * bytesPerPixel;
    pitch = (pitch + 3) & ~3;
    image->mSurfaceImpl = Renderer::createSurfaceImpl(data, w, h, bytesPerPixel, pitch);
    image->mX = w;
    image->mY = h;
    image->mComp = bytesPerPixel;

    ctx.mImageCache[filename] = image;

    return image;
}

static Widget *setParent(Context &ctx, Widget *child, Id parentId) {
    Widget *parent{nullptr};
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

void eventDispatcher(Context &ctx, int32_t eventId, EventPayload *eventPayload) {
    if (ctx.mFocus == nullptr) {
        return;
    }

    if (eventId == Events::KeyDownEvent || eventId == Events::KeyUpEvent) {
        if (eventPayload != nullptr) {
            if (ctx.mFocus->mType == WidgetType::InputField) {
                ctx.mFocus->mText.append((const char*)eventPayload->payload[0]);
            }
        }
    }
}

ret_code Widgets::container(Id id, Id parentId, const char *text, const Rect &rect) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mRoot != nullptr) {
        return ErrorCode;
    }

    Widget *widget = createWidget(ctx, id, parentId, rect, WidgetType::Container);
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

ret_code Widgets::label(Id id, Id parentId, const char *text, const Rect &rect, Alignment alignment) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    if (ctx.mRoot == nullptr) {
        return ErrorCode;
    }

    Widget *widget = createWidget(ctx, id, parentId, rect, WidgetType::Label);
    if (widget == nullptr) {
        return ErrorCode;
    }
    widget->mAlignment = alignment;
    if (text != nullptr) {
        widget->mText.assign(text);
    }

    return ResultOk;
}

ret_code Widgets::inputText(Id id, Id parentId, const Rect &rect, Alignment alignment) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    if (ctx.mRoot == nullptr) {
        return InvalidRenderHandle;
    }

    Widget *widget = createWidget(ctx, id, parentId, rect, WidgetType::InputField);
    if (widget == nullptr) {
        return ErrorCode;
    }

    widget->mAlignment = alignment;

    return ResultOk;
}

ret_code Widgets::button(Id id, Id parentId, const char *text, const Rect &rect, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    if (ctx.mRoot == nullptr) {
        return InvalidRenderHandle;
    }

    Widget *child = createWidget(ctx, id, parentId, rect, WidgetType::Button);
    if (child == nullptr) {
        return ErrorCode;
    }

    child->mCallback = callback;
    if (text != nullptr) {
        child->mText.assign(text);
    }

    return ResultOk;
}

ret_code Widgets::imageButton(Id id, Id parentId, const char *image, const Rect &rect, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    if (ctx.mRoot == nullptr) {
        return InvalidRenderHandle;
    }

    Widget *child = createWidget(ctx, id, parentId, rect, WidgetType::Button);
    if (child == nullptr) {
        return ErrorCode;
    }
    
    child->mCallback = callback;
    if (image != nullptr) {
        child->mImage = loadIntoImageCache(ctx, image);
    }
    
    return ResultOk;
}

ret_code Widgets::box(Id id, Id parentId, const Rect &rect, bool filled) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    if (ctx.mRoot == nullptr) {
        return InvalidRenderHandle;
    }

    Widget *child = createWidget(ctx, id, parentId, rect, WidgetType::Box);
    if (child == nullptr) {
        return ErrorCode;
    }

    child->mFilledRect = filled;

    return ResultOk;
}

ret_code Widgets::panel(Id id, Id parentId, const char *title, const Rect &rect, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    Widget *child = createWidget(ctx, id, parentId, rect, WidgetType::Panel);
    if (child == nullptr) {
        return ErrorCode;
    }

    return ResultOk;
}

ret_code Widgets::treeView(Id id, Id parentId, const char *title, const Rect &rect, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    if (ctx.mRoot == nullptr) {
        return InvalidRenderHandle;
    }

    Widget *widget = createWidget(ctx, id, parentId, rect, WidgetType::TreeView);
    if (widget == nullptr) {
        return ErrorCode;
    }

    if (title != nullptr) {
        widget->mText.assign(title);
    }   

    return ResultOk;
}

ret_code Widgets::treeItem(Id id, Id parentItemId, const char *text) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    if (ctx.mRoot == nullptr) {
        return InvalidRenderHandle;
    }

    Widget *parentWidget = Widgets::findWidget(parentItemId, ctx.mRoot);
    if (parentWidget == nullptr) {
        return ErrorCode;
    }

    auto &parentRect = parentWidget->mRect;
    const int32_t margin = ctx.mStyle.mMargin;
    int32_t w = parentRect.width;
    if (text != nullptr) {
        const size_t numGlyphs = strlen(text);
        w = numGlyphs * ctx.mSDLContext.mDefaultFont->mSize;
    }

    const int32_t h = parentRect.height;
    const Rect rect(parentRect.top.x + margin, parentRect.top.y + margin, w, h);
    Widget *child = createWidget(ctx, id, parentItemId, rect, WidgetType::Label);
    if (child == nullptr) {
        return ErrorCode;
    }

    if (text != nullptr) {
        child->mText.assign(text);
    }

    return ResultOk;
}

ret_code Widgets::progressBar(Id id, Id parentId, const Rect &rect, int fillRate, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return InvalidRenderHandle;
    }

    if (ctx.mRoot == nullptr) {
        return InvalidRenderHandle;
    }

    Widget *child = createWidget(ctx, id, parentId, rect, WidgetType::ProgressBar);
    if (child == nullptr) {
        return ErrorCode;
    }

    clamp(0, 100, fillRate);
    FilledState state;
    state.filledState = fillRate;
    child->mContent = new uint8_t[sizeof(EventPayload)];
    child->mCallback = callback;
    EventPayload payload;
    payload.type = EventDataType::FillState;
    memcpy(child->mContent, &payload, sizeof(EventPayload));
    if (callback != nullptr) {
        callback->mInstance = child;
        ctx.mUpdateCallbackList.push_back(callback);
    }
 
    return ResultOk;
}

static void render(Context &ctx, Widget *currentWidget) {
    if (currentWidget == nullptr) {
        return;
    }

    if (!currentWidget->mEnabled) {
        return;
    }

    // Render the widget
    const Rect &r = currentWidget->mRect;
    switch( currentWidget->mType) {
        case WidgetType::Button:
            {
                Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, true, ctx.mStyle.mFg);
                if (currentWidget->mImage != nullptr) {
                    Renderer::drawImage(ctx, r.top.x, r.top.y, r.width, r.height, currentWidget->mImage);
                }
                if (!currentWidget->mText.empty()) {
                    Color4 fg = ctx.mStyle.mTextColor, bg = ctx.mStyle.mBg;
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mSDLContext.mDefaultFont, 
                        currentWidget->mRect, fg, bg, currentWidget->mAlignment);
                }
            }
            break;

            case WidgetType::TreeView:
            {
                Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, false, ctx.mStyle.mFg);
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
                Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, false, ctx.mStyle.mBorder);
            }
            break;

        case WidgetType::ProgressBar:
            {
                Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, true, ctx.mStyle.mFg);
                auto *payload = reinterpret_cast<EventPayload *>(currentWidget->mContent);
                if (payload == nullptr) {
                    break;
                }
                FilledState *state = reinterpret_cast<FilledState *>(payload->payload);
                if (state == nullptr) {
                    break;
                }
                const uint32_t fillRate{ state->filledState };
                uint32_t width{ 0 };
                if (fillRate != 0) {
                    width = r.width * fillRate / 100;
                }
                Renderer::drawRect(ctx, r.top.x, r.top.y, width, r.height, true, ctx.mStyle.mTextColor);                       
            } 
            break;

        case WidgetType::InputField:
        {
            Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, true, ctx.mStyle.mFg);
            Renderer::drawRect(ctx, r.top.x+2, r.top.y+2, r.width-4, r.height-4, true, ctx.mStyle.mBorder);
        }
        break;

        case WidgetType::Container:
        case WidgetType::Box:
            {
                Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, currentWidget->mFilledRect, ctx.mStyle.mBorder);
            }
            break;
    }

    for (auto &child : currentWidget->mChildren) {
        render(ctx, child);
    }
}

void Widgets::renderWidgets() {
    auto &ctx = TinyUi::getContext();
    if (ctx.mRoot == nullptr) {
        return;
    }
    render(ctx, ctx.mRoot);
}

void Widgets::onMouseButton(int x, int y, int eventType, MouseState state) {
    auto &ctx = TinyUi::getContext();
    assert(eventType >= 0);
    assert(eventType < Events::NumEvents);

    if (ctx.mRoot == nullptr) {
        return;
    }

    Widget *found{nullptr};
    findSelectedWidget(x, y, ctx.mRoot, &found);
    if (found != nullptr) {
#ifdef _DEBUG
        if (eventType == Events::MouseButtonDownEvent) {
            std::cout << "Clicked " << found->mId << "\n";
        }  else {
            std::cout << "Released " << found->mId << "\n";
        }
#endif // _DEBUG
        if (found->mCallback != nullptr) {
            if (found->mCallback->mfuncCallback[eventType] != nullptr) {
                found->mCallback->mfuncCallback[eventType](found->mId, found->mCallback->mInstance);
            }
        }
    } 
}

void Widgets::onMouseMove(int x, int y, int eventType, MouseState state) {
    auto &ctx = TinyUi::getContext();
    assert(eventType >= 0);
    assert(eventType < Events::NumEvents);

    if (ctx.mRoot == nullptr) {
        return;
    }

    Widget *found{nullptr};
    findSelectedWidget(x, y, ctx.mRoot, &found);
    if (found != nullptr) {
        if (found->mCallback != nullptr) {
            if (found->mCallback->mfuncCallback[eventType] != nullptr) {
                found->mCallback->mfuncCallback[eventType](found->mId, found->mCallback->mInstance);
            }
        }
    }
}

void Widgets::onKey(const char *key, bool isDown) {
    auto &ctx = TinyUi::getContext();
    if (key == nullptr) {
        return;
    }

    EventPayload eventPayload;
    eventPayload.payload[0] = *key;
    int32_t eventId = -1;
    if (isDown) {
        eventId = Events::KeyDownEvent;
        eventPayload.type = EventDataType::KeyDownState;
    } else {
        eventId = Events::KeyUpEvent;
        eventPayload.type = EventDataType::KeyUpState;
    }
    eventDispatcher(ctx, eventId, &eventPayload);
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

void Widgets::clear() {
    auto &ctx = TinyUi::getContext();
    if (ctx.mRoot == nullptr) {
        return;
    }

    Widget *current{ctx.mRoot};
    recursiveClear(current);
}

void Widgets::setEnableState(Id id, bool enabled) {
    auto &ctx = TinyUi::getContext();
    Widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        if (enabled) {
            widget->enable();
        } else {
            widget->disable();
        }
    }
}

bool Widgets::isEnabled(Id id) {
    auto &ctx = TinyUi::getContext();
    Widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        return widget->isEnabled();
    }

    return false;
}

ret_code Widgets::setFocus(Id id)  {
    auto &ctx = TinyUi::getContext();
    Widget *widget = findWidget(id, ctx.mRoot);
    if (widget == nullptr) {
        return ErrorCode;
    }

    ctx.mFocus = widget;

    return ResultOk;
}

Widget *Widgets::getWidgetById(Id id) {
    auto &ctx = TinyUi::getContext();
    return findWidget(id, ctx.mRoot);
}

} // namespace tinyui
