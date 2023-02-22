#pragma once

#include "tinyui.h"

namespace tinyui {

struct tui_context;

enum class WidgetType {
    UnknownType = -1,
    ContainerType = 0,
    ButtonType,
    LabelType,
    PanelType,

    NumTypes
};

struct tui_widget {
    unsigned int mId;
    WidgetType mType;
    tui_widget *mParent;
    bool mEnabled;
    tui_rect mRect;
    std::string mText;
    tui_image *mImage;
    std::vector<tui_widget*> mChildren;
    tui_callbackI *mCallback;

    tui_widget() :
            mId(0), mType(WidgetType::UnknownType), mParent(nullptr), mEnabled(true), mRect(), mText(), mImage(nullptr), mChildren() {}
    
    ~tui_widget() = default;
};

struct Widgets {
    static tui_ret_code create_container(tui_context &ctx, unsigned int id, unsigned int parentId, const char *text, int x, int y, int w, int h);
    static tui_ret_code create_label(tui_context &ctx, unsigned int id, unsigned int parentId, const char *text, int x, int y, int w, int h);
    static tui_ret_code create_button(tui_context &ctx, unsigned int id, unsigned int parentId, const char *text, tui_image *image, int x, int y, 
        int w, int h, tui_callbackI *callback);
    static tui_ret_code create_panel(tui_context &ctx, unsigned int id, unsigned int parentId, int x, int y, int w, int h, tui_callbackI *callback);
    static void render_widgets(tui_context &ctx);
    static void onMouseButton(int x, int y, int eventType, tui_mouseState state, tui_context &ctx);
    static void onMouseMove(int x, int y, int eventType, tui_mouseState state, tui_context &ctx);
    static void clear(tui_context &ctx);
    static void setEnableState(tui_context &ctx, unsigned int id, bool enabled);
    static bool isEnabled(tui_context &ctx, unsigned int id);
};

} // namespace tinyui
