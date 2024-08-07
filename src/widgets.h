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

struct tui_context;

/// @brief
enum class WidgetType {
    Invalid = -1,
    Container = 0,
    Button,
    Label,
    Panel,
    Box,
    Count
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

struct tui_widget {
    Id mId;
    WidgetType mType;
    tui_widget *mParent;
    bool mEnabled;
    tui_rect mRect;
    bool mFilledRect;
    uint32_t mStyles;
    std::string mText;
    tui_image *mImage;
    std::vector<tui_widget*> mChildren;
    tui_callbackI *mCallback;

    tui_widget() :
            mId(0),
            mType(WidgetType::Invalid),
            mParent(nullptr),
            mEnabled(true),
            mRect(),
            mFilledRect(true),
            mStyles(0u),
            mText(),
            mImage(nullptr),
            mChildren(),
            mCallback(nullptr) {
        // empty
    }

    ~tui_widget() = default;

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

    tui_widget(const tui_style &) = delete;
    tui_widget& operator = (const tui_style &) = delete;
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
    static tui_ret_code Container(tui_context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h);

    /// @brief
    static tui_ret_code Box(tui_context &ctx, Id id, Id parentId, int x, int y, int w, int h, const tui_color4 &bg, bool filled);

    /// @brief
    static tui_widget *findWidget(Id id, tui_widget *root);

    /// @brief
    static void findSelectedWidget(int x, int y, tui_widget *currentChild, tui_widget **found);

    /// @brief
    static tui_ret_code Label(tui_context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h);

    /// @brief
    static tui_ret_code Button(tui_context &ctx, Id id, Id parentId, const char *text, tui_image *image, int x, int y, 
        int w, int h, tui_callbackI *callback);

    /// @brief
    static tui_ret_code Panel(tui_context &ctx, Id id, Id parentId, int x, int y, int w, int h, tui_callbackI *callback);

    /// @brief
    static void renderWidgets(tui_context &ctx);

    /// @brief
    static void onMouseButton(int x, int y, int eventType, tui_mouseState state, tui_context &ctx);

    /// @brief
    static void onMouseMove(int x, int y, int eventType, tui_mouseState state, tui_context &ctx);

    /// @brief
    static void onKey(const char *key, bool isDown, tui_context &ctx);

    /// @brief
    static void clear(tui_context &ctx);

    /// @brief
    static void setEnableState(tui_context &ctx, Id id, bool enabled);

    /// @brief
    static bool isEnabled(tui_context &ctx, Id id);

    /// @brief
    static tui_widget *getWidgetById(tui_context &ctx, Id id);
};

} // namespace tinyui
