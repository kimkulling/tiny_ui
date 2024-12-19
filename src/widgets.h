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
#pragma once

#include "tinyui.h"

namespace tinyui {

struct Context;

/// @brief  This enum is used to descripe the widget type-
enum class WidgetType {
    Invalid = -1,       ///< Not initialized
    Container = 0,      ///< A container widget
    Button,             ///< A button widget
    Label,              ///< A label widget
    Panel,              ///< A panel widget
    Box,                ///< A box widget
    TreeView,           ///< A treeeview widget
    Count               ///< The number of widgets
};

/// @brief
enum class LayoutPolicy {
    Invalid = -1,
    Absolute,
    Relative,
    Count
};

/// @brief
using Id = uint64_t;

/// @brief
enum class WidgetStyle : uint32_t {
    tui_border_style = 1
};

using WidgetArray = std::vector<Widget*>;

struct Widget {
    Id          mId;
    WidgetType  mType;
    Widget     *mParent;
    bool        mEnabled;
    Rect        mRect;
    bool        mFilledRect;
    uint32_t    mStyles;
    Alignment   mAlignment;
    std::string mText;
    Image      *mImage;
    WidgetArray mChildren;
    CallbackI  *mCallback;

    Widget() :
            mId(0),
            mType(WidgetType::Invalid),
            mParent(nullptr),
            mEnabled(true),
            mRect(),
            mFilledRect(true),
            mStyles(0u),
            mAlignment(Alignment::Left),
            mText(),
            mImage(nullptr),
            mChildren(),
            mCallback(nullptr) {
        // empty
    }

    ~Widget() = default;

    bool hasStyle(WidgetStyle style) const {
        return mStyles & static_cast<uint32_t>(style);
    }

    void setStyle(WidgetStyle style) {
        mStyles = mStyles | static_cast<uint32_t>(style);
    }

    void enable() {
        mEnabled = true;
    }

    void disable()  {
        mEnabled = false;
    }

    bool isEnabled() const {
        return mEnabled;
    }

    Widget(const Widget &) = delete;
    Widget &operator=(const Widget &) = delete;
};

/// @brief
struct Widgets {
    /// @brief
    static const Id RootItem = 0;

    /// @brief
    Widgets() = default;

    /// @brief
    ~Widgets() = default;

    /// @brief
    static ret_code container(Context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h);

    /// @brief
    static ret_code box(Context &ctx, Id id, Id parentId, int x, int y, int w, int h, const Color4 &bg, bool filled);

    /// @brief
    static Widget *findWidget(Id id, Widget *root);

    /// @brief
    static void findSelectedWidget(int x, int y, Widget *currentChild, Widget **found);

    /// @brief
    static ret_code label(Context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h, Alignment alignment);

    /// @brief
    static ret_code button(Context &ctx, Id id, Id parentId, const char *text, const char *image, int x, int y, 
        int w, int h, CallbackI *callback);

    /// @brief
    static ret_code panel(Context &ctx, Id id, Id parentId, const char *title, int x, int y, int w, int h, CallbackI *callback);

    /// @brief
    static ret_code treeView(Context &ctx, Id id, Id parentId, const char *title, int x, int y, int w, int h);

    /// @brief
    static void renderWidgets(Context &ctx);

    /// @brief
    static void onMouseButton(int x, int y, int eventType, MouseState state, Context &ctx);

    /// @brief
    static void onMouseMove(int x, int y, int eventType, MouseState state, Context &ctx);

    /// @brief
    static void onKey(const char *key, bool isDown, Context &ctx);

    /// @brief
    static void clear(Context &ctx);

    /// @brief
    static void setEnableState(Context &ctx, Id id, bool enabled);

    /// @brief
    static bool isEnabled(Context &ctx, Id id);

    /// @brief
    static Widget *getWidgetById(Context &ctx, Id id);
};

} // namespace tinyui
