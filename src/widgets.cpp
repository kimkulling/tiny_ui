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
#include <cstring>
#include <algorithm>

namespace tinyui {

static constexpr Id RootHandle = 1;

static Id createHandle() {
    static Id id{RootHandle};
    return ++id;
}

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

    int w, h, bytesPerPixel;
    unsigned char *data = stbi_load(filename, &w, &h, &bytesPerPixel, 0);
    if (data == nullptr) {
        return nullptr;
    }

    image = new Image;
    if (image == nullptr) {
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

static void releaseImageCache(Context &ctx) {
    for (auto it = ctx.mImageCache.begin(); it != ctx.mImageCache.end(); ++it) {
        if (Image *image = it->second; image != nullptr) {
            Renderer::releaseSurfaceImpl(image->mSurfaceImpl);
            delete image;
        }
    }
    ctx.mImageCache.clear();
} 

static Widget *getValidRoot(Context &ctx) {
    if (ctx.mRoot != nullptr) {
        return ctx.mRoot;
    }
    
    ctx.mRoot = new Widget;
    ctx.mRoot->mType = WidgetType::Container;
    
    return ctx.mRoot;
}

static Widget *setParent(Context &ctx, Widget *child, WidgetHandle parentId) {
    Widget *parent{nullptr};
    if (parentId.mId == 0) {
        parent = getValidRoot(ctx);
    } else {
        parent = Widgets::findWidget(parentId, ctx.mRoot);
    }

    if (parent == nullptr) {
        return nullptr;
    }

    parent->mChildren.emplace_back(child);
    parent->mRect.mergeWithRect(child->mRect);

    return parent;
}

static Widget *createWidget(Context &ctx, WidgetHandle parentId, const Rect &rect, WidgetType type) {
    auto *widget = new Widget;
    widget->mHandle = WidgetHandle{createHandle()};
    widget->mType = type;
    widget->mRect = rect;
    widget->mParent = setParent(ctx, widget, parentId);

    return widget;
}

static void deleteKeyFromText(Context &ctx) {    
    ctx.mFocus->mText.erase(ctx.mFocus->mText.size() - 1);
}

static void appendKeyToText(Context &ctx, char *buffer) {
    ctx.mFocus->mText.append(buffer);
}

static void handleInputField(Context &ctx, EventPayload *eventPayload) {
    char buffer[2] = { 
        static_cast<char>(eventPayload->payload[0]), 
        '\0' 
    };
    if (buffer[0] == SDLK_BACKSPACE) {
        deleteKeyFromText(ctx);
    } else {
        appendKeyToText(ctx, buffer);
    }
}

void eventDispatcher(Context &ctx, int32_t eventId, EventPayload *eventPayload) {
    if (ctx.mFocus == nullptr) {
        return;
    }

    if (eventId == Events::KeyDownEvent) {
        if (eventPayload != nullptr) {
            if (ctx.mFocus->mType == WidgetType::InputField) {
                handleInputField(ctx, eventPayload);
            }
        }
    }
}

WidgetHandle Widgets::container(WidgetHandle parentId, const char *text, const Rect &rect) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mRoot != nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    Widget *widget = createWidget(ctx, parentId, rect, WidgetType::Container);
    ctx.mRoot = widget;
    if (text != nullptr) {
        widget->mText.assign(text);
    }

    return widget->mHandle;
}

Widget *Widgets::findWidget(WidgetHandle id, Widget *root) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->mHandle.mId == id.mId) {
        return root;
    }

    for (size_t i=0; i<root->mChildren.size(); ++i) {
        Widget *child = root->mChildren[i];
        if (child == nullptr) {
            continue;
        }
        if (child->mHandle.mId == id.mId) {
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

WidgetHandle Widgets::label(WidgetHandle parentId, const char *text, const Rect &rect, Alignment alignment) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    Widget *widget = createWidget(ctx, parentId, rect, WidgetType::Label);
    if (widget == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }
    widget->mAlignment = alignment;
    if (text != nullptr) {
        widget->mText.assign(text);
    }

    return widget->mHandle;
}

static int inputHandler(WidgetHandle id, void *instance) {
    if (instance == nullptr) {
        return ErrorCode;
    }

    auto *ctx = static_cast<Context *>(instance);
    ctx->mFocus = Widgets::findWidget(id, ctx->mRoot);

    return ResultOk;
}

WidgetHandle Widgets::inputText(WidgetHandle parentId, const Rect &rect, Alignment alignment, KeyInputType type, const char *defaultText) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    Widget *widget = createWidget(ctx, parentId, rect, WidgetType::InputField);
    if (widget == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    widget->mAlignment = alignment;
    widget->mKeyInputType = type;
    if (defaultText != nullptr) {
        widget->mText.assign(defaultText);
    }

    widget->mCallback = new CallbackI(inputHandler, (void *)&ctx, Events::MouseButtonDownEvent);

    return widget->mHandle;
}

WidgetHandle Widgets::textButton(WidgetHandle parentId, const char *text, const Rect &rect, Alignment alignment, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    Widget *child = createWidget(ctx, parentId, rect, WidgetType::Button);
    if (child == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    child->mCallback = callback;
    if (callback != nullptr) {
        callback->incRef();
    }
    if (text != nullptr) {
        child->mText.assign(text);
        child->mAlignment = alignment;
    }

    return child->mHandle;
}

WidgetHandle Widgets::imageButton(WidgetHandle parentId, const char *image, const Rect &rect, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    Widget *child = createWidget(ctx, parentId, rect, WidgetType::Button);
    if (child == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }
    
    child->mCallback = callback;
    if (callback != nullptr) {
        callback->incRef();
    }

    if (image != nullptr) {
        child->mImage = loadIntoImageCache(ctx, image);
    }
    
    return child->mHandle;
}

WidgetHandle Widgets::box(WidgetHandle parentId, const Rect &rect, bool filled) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    Widget *child = createWidget(ctx, parentId, rect, WidgetType::Box);
    if (child == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    child->mFilledRect = filled;

    return child->mHandle;
}

WidgetHandle Widgets::imageBox(WidgetHandle parentId, const char* image, const Rect& rect, bool filled) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }
    Widget *child = createWidget(ctx, parentId, rect, WidgetType::ImageBox);
    if (child == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    child->mFilledRect = filled;
    if (image != nullptr) {
        child->mImage = loadIntoImageCache(ctx, image);
    }

    return child->mHandle;
}

WidgetHandle Widgets::panel(WidgetHandle parentId, const char *title, const Rect &rect, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    const Widget *child = createWidget(ctx, parentId, rect, WidgetType::Panel);
    if ( child == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    return child->mHandle;
}

static int onTreeViewItemClicked(WidgetHandle id, void *data) {
    Widget *treeView = Widgets::findWidget(id, TinyUi::getContext().mRoot);
    if (treeView == nullptr) {
        return ErrorCode;
    }

    for (size_t i = 0; i < treeView->mChildren.size(); ++i ) {
        Widget *child = treeView->mChildren[i];
        if (child == nullptr) {
            continue;
        }
        child->mEnabled = !child->mEnabled;
    }

    std::cout << "TreeView item clicked: " << id.mId << std::endl;

    return 0;
}

WidgetHandle Widgets::treeView(WidgetHandle parentId, const char *title, const Rect &rect) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }
    
    Widget *widget = createWidget(ctx, parentId, rect, WidgetType::TreeView);
    if (widget == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (title != nullptr) {
        widget->mText.assign(title);
    }

    CallbackI *callback = new CallbackI(onTreeViewItemClicked, nullptr, Events::MouseButtonDownEvent);
    widget->mCallback = callback;
    if (callback != nullptr) {
        callback->incRef();
    }

    return widget->mHandle;
}

WidgetHandle Widgets::treeItem(WidgetHandle parentItemId, const char *text) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    const Widget *parentWidget = findWidget(parentItemId, ctx.mRoot);
    if (parentWidget == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }
    
    const auto &parentRect = parentWidget->mRect;
    
    const int32_t margin = ctx.mStyle.mMargin;
    const int32_t w = parentRect.width;
    const int32_t h = parentRect.height;
    size_t numChildren = parentWidget->mChildren.size() + 1;
    const Rect rect(parentRect.top.x + margin, parentRect.top.y + static_cast<int32_t>(numChildren) * margin +
        static_cast<int32_t>(numChildren) * h, w, h);
    Widget *child = createWidget(ctx, parentItemId, rect, WidgetType::Label);
    if (child == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }
    
    child->mIntention = parentWidget->mIntention + 1;
    if (text != nullptr) {
        child->mText.assign(text);
    }

    return child->mHandle;
}

WidgetHandle Widgets::progressBar(WidgetHandle parentId, const Rect &rect, int fillRate, CallbackI *callback) {
    auto &ctx = TinyUi::getContext();
    if (ctx.mBackendCtx == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    if (ctx.mRoot == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
    }

    Widget *child = createWidget(ctx, parentId, rect, WidgetType::ProgressBar);
    if (child == nullptr) {
        return WidgetHandle{WidgetHandle::InvalidId};
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
 
    return child->mHandle;
}

static void render(Context &ctx, const Widget *currentWidget) {
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
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mDefaultFont, 
                        currentWidget->mRect, fg, bg, currentWidget->mAlignment);
                }
            }
            break;

            case WidgetType::TreeView:
            {
                Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, false, ctx.mStyle.mFg);
                if (!currentWidget->mText.empty()) {
                    Color4 fg = ctx.mStyle.mTextColor, bg = ctx.mStyle.mBg;
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mDefaultFont, 
                        currentWidget->mRect, fg, bg, currentWidget->mAlignment);
                }
            }
            break;
            
            case WidgetType::Label:
            {
                if (!currentWidget->mText.empty()) {
                    const Color4 fg = ctx.mStyle.mTextColor;
                    const Color4 bg = ctx.mStyle.mBg;
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mDefaultFont, 
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
                const auto *state = reinterpret_cast<FilledState *>(payload->payload);
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
                if (!currentWidget->mText.empty()) {
                    const Color4 fg = ctx.mStyle.mTextColor;
                    const Color4 bg = ctx.mStyle.mBg;
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), ctx.mDefaultFont,
                            currentWidget->mRect, fg, bg, currentWidget->mAlignment);                
                }
            }
            break;

        case WidgetType::Container:
        case WidgetType::Box:
            {
                Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, currentWidget->mFilledRect, ctx.mStyle.mBorder);
            }
            break;

        case WidgetType::ImageBox:
            {
                Renderer::drawRect(ctx, r.top.x, r.top.y, r.width, r.height, currentWidget->mFilledRect, ctx.mStyle.mBorder);
                if (currentWidget->mImage != nullptr) {
                    Renderer::drawImage(ctx, r.top.x, r.top.y, r.width, r.height, currentWidget->mImage);
                }
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
            std::cout << "Clicked " << found->mHandle.mId << "\n";
        }  else {
            std::cout << "Released " << found->mHandle.mId << "\n";
        }
#endif // _DEBUG
        if (found->mCallback != nullptr) {
            if (found->mCallback->mfuncCallback[eventType] != nullptr) {
                found->mCallback->mfuncCallback[eventType](found->mHandle, found->mCallback->mInstance);
            }
        }
    } 
}

void Widgets::onMouseMove(int x, int y, int eventType, MouseState state) {
    assert(eventType >= 0);
    assert(eventType < Events::NumEvents);

    auto &ctx = TinyUi::getContext();
    if (ctx.mRoot == nullptr) {
        return;
    }

    Widget *found{nullptr};
    findSelectedWidget(x, y, ctx.mRoot, &found);
    if (found == nullptr) {
        return;
    }

    if (found->mCallback != nullptr) {
        if (found->mCallback->mfuncCallback[eventType] != nullptr) {
            found->mCallback->mfuncCallback[eventType](found->mHandle, found->mCallback->mInstance);
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

    for (size_t i = 0; i < current->mChildren.size(); ++i) {
        recursiveClear(current->mChildren[i]);
    }

    if (current->mCallback) {
        delete current->mCallback;
        current->mCallback = nullptr;
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
    ctx.mRoot = nullptr;
    releaseImageCache(ctx);
}

bool Widgets::clearItem(WidgetHandle id, bool recursive) {
    auto &ctx = TinyUi::getContext();
    Widget *widget = findWidget(id, ctx.mRoot);
    if (widget == nullptr) {
        return false;
    }
    
    if (widget->mParent == nullptr) {
        return false;
    }
    
    auto &siblings = widget->mParent->mChildren;
    auto it = std::find(siblings.begin(), siblings.end(), widget);
    bool result{ false };
    if (it != siblings.end()) {
        siblings.erase(it);
        result = true;
    }
    
    for (size_t i = 0; i < widget->mChildren.size(); ++i) {
        recursiveClear(widget->mChildren[i]);
    }
    delete widget;
    return result;
}

void Widgets::setEnableState(WidgetHandle id, bool enabled) {
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

bool Widgets::isEnabled(WidgetHandle id) {
    auto &ctx = TinyUi::getContext();
    const Widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        return widget->isEnabled();
    }

    return false;
}

ret_code Widgets::setFocus(WidgetHandle id)  {
    auto &ctx = TinyUi::getContext();
    Widget *widget = findWidget(id, ctx.mRoot);
    if (widget == nullptr) {
        return ErrorCode;
    }

    ctx.mFocus = widget;

    return ResultOk;
}

Widget *Widgets::getWidgetById(WidgetHandle id) {
    auto &ctx = TinyUi::getContext();
    return findWidget(id, ctx.mRoot);
}

bool Widgets::beginChild() {
    auto &ctx = TinyUi::getContext();
    return true;
}

bool Widgets::endChild() {
    auto &ctx = TinyUi::getContext();
    return true;
}

static constexpr size_t BufferSize = 1024;

ret_code Widgets::getOpenFileDialog(const char *title, const char *extensions, std::string &filename) {
    filename.clear();
#ifdef TINYUI_WINDOWS
    // Init data
    char szFile[BufferSize] = { '\0' };
    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;

    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrTitle = title;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = extensions;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.
    if (::GetOpenFileName(&ofn) == TRUE) {
        filename = ofn.lpstrFile;
    } else {
        return OpCancelled;
    }
#else
    char buffer[BufferSize] = { '\0' };
    FILE *f = popen("zenity --file-selection", "r");
    if (f == nullptr) {
        return OpCancelled;
    }
    fgets(buffer, BufferSize, f);
    const int retCode = pclose(f);
    if (retCode != 0) {
        return ErrorCode;
    }
    filename = buffer;
#endif // TINYUI_WINDOWS

    return ResultOk;
}

ret_code Widgets::getSaveFileDialog(const char *title, const char *extensions, std::string &filename) {
    filename.clear();

#ifdef TINYUI_WINDOWS
    char szFile[BufferSize] = { '\0' };
    // Initialize OPENFILENAME
    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;

    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrTitle = title;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = extensions;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST;

    // Display the Open dialog box.
    if (TRUE == GetSaveFileName(&ofn)) {
        filename = ofn.lpstrFile;
    } else {
        return OpCancelled;
    }
#else
    FILE *f = popen("zenity --file-selection", "w");
    if (f == nullptr) {
        return OpCancelled;
    }
    char buffer[BufferSize] = { '\0' };
    fgets(buffer, BufferSize, f);
    const int retCode = pclose(f);
    if (retCode != 0) {
        return ErrorCode;
    }

    filename = buffer;
#endif // TINYUI_WINDOWS
    
    return ResultOk;
}

} // namespace tinyui
