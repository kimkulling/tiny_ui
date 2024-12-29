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

// Forward declarations
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
    StatusBar,          ///< A status bar widget
    Count               ///< The number of widgets
};

/// @brief This enum is used to descripe the alignment of a widget.
enum class LayoutPolicy {
    Invalid = -1,       ///< Not initialized
    Absolute,           ///< Absolute layout
    Relative,           ///< Relative layout
    Count               ///< The number of layouts
};

/// @brief This enum is used to descripe the alignment of a widget.
using Id = uint64_t;

/// @brief This enum is used to descripe the alignment of a widget.
enum class WidgetStyle : uint32_t {
    tui_border_style = 1 ///< The widget has a border
};

/// @brief This enum is used to descripe the alignment of a widget.
using WidgetArray = std::vector<Widget*>;

/// @brief This struct contains all the data which is needed to describe a widget.
struct Widget {
    Id          mId;            ///< The unique id of the widget
    WidgetType  mType;          ///< The type of the widget
    Widget     *mParent;        ///< The parent widget
    bool        mEnabled;       ///< The enabled state of the widget 
    Rect        mRect;          ///< The rectangle of the widget
    bool        mFilledRect;    ///< The filled rectangle state
    uint32_t    mStyles;        ///< The style of the widget
    Alignment   mAlignment;     ///< The alignment of the widget
    std::string mText;          ///< The label text  
    Image      *mImage;         ///< The image of the widget
    WidgetArray mChildren;      ///< The children of the widget
    CallbackI  *mCallback;      ///< The callback of the widget

    /// @brief The default class constructor.
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

    /// @brief The class destructor.
    ~Widget() = default;

    /// @brief Check if the widget has a specific style.
    /// @param style The style to check.
    /// @return true if the widget has the style, false if not.
    bool hasStyle(WidgetStyle style) const {
        return mStyles & static_cast<uint32_t>(style);
    }

    /// @brief Set the style of the widget.
    /// @param style The style to set.
    void setStyle(WidgetStyle style) {
        mStyles = mStyles | static_cast<uint32_t>(style);
    }

    /// @brief Enables the widget
    void enable() {
        mEnabled = true;
    }

    /// @brief Disables the widget.
    void disable()  {
        mEnabled = false;
    }

    /// @brief Check if the widget is enabled.
    /// @return true if the widget is enabled, false if not.
    bool isEnabled() const {
        return mEnabled;
    }

    Widget(const Widget &) = delete;
    Widget &operator=(const Widget &) = delete;
};

/// @brief The widgets access wrapper class.
/// This class is used to create and manage widgets.
struct Widgets {
    /// @brief The root item id.
    static const Id RootItem = 0;

    /// @brief The default class constructor.
    Widgets() = default;

    /// @brief The class destructor.
    ~Widgets() = default;

    /// @brief Create a new widget from the type container.
    /// @param ctx      The context to create the widget in.
    /// @param id       The unique id of the widget.
    /// @param parentId The parent id of the widget.
    /// @param text     The text of the widget.
    /// @param x        The x-coordinate of the widget.
    /// @param y        The y-coordinate of the widget.
    /// @param w        The width of the widget.
    /// @param h        The height of the widget.
    /// @return ResultOk if the widget was created, ErrorCode if not.
    static ret_code container(Context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h);

    /// @brief Create a new widget from the type box.
    /// @param ctx      The context to create the widget in.
    /// @param id       The unique id of the widget.
    /// @param parentId The parent id of the widget.
    /// @param x        The x-coordinate of the widget.
    /// @param y        The y-coordinate of the widget.
    /// @param w        The width of the widget.
    /// @param h        The height of the widget.
    /// @param bg       The background color of the widget.
    /// @param filled   The filled state of the widget.
    /// @return ResultOk if the widget was created, ErrorCode if not.
    static ret_code box(Context &ctx, Id id, Id parentId, int x, int y, int w, int h, const Color4 &bg, bool filled);

    /// @brief Will look for a widget by its id.
    /// @param id   The id of the widget to look for.
    /// @param root The root widget to start the search.
    /// @return The found widget or nullptr if not found.
    static Widget *findWidget(Id id, Widget *root);

    /// @brief Will look for a widget by its id.
    /// @param x      The x-coordinate of the point.
    /// @param y      The y-coordinate of the point.
    /// @param root   The root widget to start the search.
    /// @param found  The found widget.
    static void findSelectedWidget(int x, int y, Widget *currentChild, Widget **found);

    /// @brief Creates a new widget from the type label.
    /// @param ctx          The context to create the widget in.
    /// @param id           The unique id of the widget.
    /// @param parentId     The parent id of the widget.
    /// @param text         The text of the widget.
    /// @param x            The x-coordinate of the widget.
    /// @param y            The y-coordinate of the widget.
    /// @param w            The width of the widget.
    /// @param h            The height of the widget.
    /// @param alignment    The alignment of the widget.
    /// @return ResultOk if the widget was created, ErrorCode if not.
    static ret_code label(Context &ctx, Id id, Id parentId, const char *text, int x, int y, 
        int w, int h, Alignment alignment);

    /// @brief Create a new widget from the type button.
    /// @param ctx      The context to create the widget in.
    /// @param id       The unique id of the widget.
    /// @param parentId The parent id of the widget.
    /// @param text     The text of the widget.
    /// @param image    The image of the widget.
    /// @param x        The x-coordinate of the widget.
    /// @param y        The y-coordinate of the widget.
    /// @param w        The width of the widget.
    /// @param h        The height of the widget.
    /// @param callback The callback of the widget.
    /// @return ResultOk if the widget was created, ErrorCode if not.
    static ret_code button(Context &ctx, Id id, Id parentId, const char *text, const char *image, int x, int y, 
        int w, int h, CallbackI *callback);

    /// @brief Creates a new widget from the type panel.
    /// @param ctx      The context to create the widget in.
    /// @param id       The unique id of the widget.
    /// @param parentId The parent id of the widget.
    /// @param title    The title of the widget.
    /// @param x        The x-coordinate of the widget.
    /// @param y        The y-coordinate of the widget.
    /// @param w        The width of the widget.
    /// @param h        The height of the widget.
    /// @param callback The callback of the widget.
    /// @return ResultOk if the widget was created, ErrorCode if not.
    static ret_code panel(Context &ctx, Id id, Id parentId, const char *title, int x, int y,
        int w, int h, CallbackI *callback);

    /// @brief Creates a new widget from the type treeview.
    /// @param ctx      The context to create the widget in.
    /// @param id       The unique id of the widget.
    /// @param parentId The parent id of the widget.
    /// @param title    The title of the widget.
    /// @param x        The x-coordinate of the widget.
    /// @param y        The y-coordinate of the widget.
    /// @param w        The width of the widget.
    /// @param h        The height of the widget.
    /// @return ResultOk if the widget was created, ErrorCode if not.
    static ret_code treeView(Context &ctx, Id id, Id parentId, const char *title, int x, int y, int w, int h);

    /// @brief Creates a new widget from the type status bar.
    /// @param ctx      The context to create the widget in.
    /// @param id       The unique id of the widget.
    /// @param parentId The parent id of the widget.
    /// @param title    The title of the widget.
    /// @param x        The x-coordinate of the widget.
    /// @param y        The y-coordinate of the widget.
    /// @param w        The width of the widget.
    /// @param h        The height of the widget.
    /// @return ResultOk if the widget was created, ErrorCode if not.
    static ret_code statusBar(Context &ctx, Id id, Id parentId, int x, int y, int w, int h);

    /// @brief Will render all widgets.
    /// @param ctx The context to render the widgets in.
    static void renderWidgets(Context &ctx);

    /// @brief The on-mouse-button-down event handler.
    /// @param ctx       The context to handle the event in.
    /// @param x         The x-coordinate of the mouse.
    /// @param y         The y-coordinate of the mouse.
    /// @param eventType The event type.
    /// @param state     The mouse state.
    static void onMouseButton(Context &ctx, int x, int y, int eventType, MouseState state);

    /// @brief The on-mouse-move event handler.
    /// @param ctx       The context to handle the event in.
    /// @param x         The x-coordinate of the mouse.
    /// @param y         The y-coordinate of the mouse.
    /// @param eventType The event type.
    /// @param state     The mouse state.
    static void onMouseMove(Context &ctx, int x, int y, int eventType, MouseState state);

    /// @brief The on-key event handler.
    /// @param ctx    The context to handle the event in.
    /// @param key    The key to handle.
    /// @param isDown The key state.
    static void onKey(Context &ctx, const char *key, bool isDown);

    /// @brief The on-char event handler.
    /// @param ctx The context to handle the event in.
    static void clear(Context &ctx);

    /// @brief The widget enabler
    /// @param ctx     The context to enable the widget in.
    /// @param id      The id of the widget to enable.
    /// @param enabled The enabled state of the widget.
    static void setEnableState(Context &ctx, Id id, bool enabled);

    /// @brief Will return true if the widget is enabled.
    /// @param ctx The context to check the widget in.
    /// @param id  The id of the widget to check.
    /// @return true if the widget is enabled, false if not.
    static bool isEnabled(Context &ctx, Id id);

    /// @brief Will return the widget by its id.
    /// @param ctx The context to get the widget from.
    /// @param id  The id of the widget to get.
    /// @return The widget or nullptr if not found.
    static Widget *getWidgetById(Context &ctx, Id id);
};

} // namespace tinyui
