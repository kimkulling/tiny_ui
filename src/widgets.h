#pragma once

#include <vector>
#include <string>
#include "tinyui.h"

namespace tinyui {

struct tui_context;

enum class WidgetType {
    UnknownType = -1,
    ButtonType,
    PanelType,

    NumTypes
};

struct tui_widget {
    unsigned int mId;
    WidgetType mType;
    tui_widget *mParent;
    tui_rect mRect;
    std::string mText;
    std::vector<tui_widget*> mChildren;
    tui_callbackI mCallback;

    tui_widget() :
            mId(0), mType(WidgetType::UnknownType), mParent(nullptr), mRect(), mText(), mChildren() {}
    
    ~tui_widget() = default;
    
    void mergeWithRect( const tui_rect &r ) {
        if (r.x1 < mRect.x1 || mRect.x1 == -1)
            mRect.x1 = r.x1;
        if (r.y1 < mRect.y1 || mRect.y1 == -1)
            mRect.y1 = r.y1;
        if (mRect.width > r.width || mRect.width == -1)
            mRect.width = r.width;
        if (mRect.height > r.height || mRect.height == -1)
            mRect.height = r.height;
        mRect.set(mRect.x1, mRect.y1, mRect.width, mRect.height);
    }
};

struct Widgets {
    static int create_button(tui_context &ctx, unsigned int id, unsigned int parentId, const char *text, int x, int y, int w, int h, tui_callbackI *callback);
    static int create_panel(tui_context &ctx, unsigned int id, unsigned int parentId, int x, int y, int w, int h, tui_callbackI *callback);
    static void render_widgets(tui_context &ctx);
    static void onMouseButton(int x, int y , tui_mouseState state, tui_context &ctx);
};

} // namespace tinyui
