#pragma once

#include <vector>
#include <string>
#include "tinyui.h"

namespace TinyUi {

struct Context;

enum class WidgetType {
    UnknownType = -1,
    ButtonType,
    PanelType,

    NumTypes
};

struct Widget {
    unsigned int mId;
    WidgetType mType;
    Widget *mParent;
    rect mRect;
    std::string mText;
    std::vector<Widget*> mChildren;
    CallbackI mCallback;

    Widget() : mId(0), mType(WidgetType::UnknownType), mParent(nullptr), mRect(), mText(), mChildren() {}
    
    ~Widget() = default;
    
    void mergeWithRect( const rect &r ) {
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
    static int create_button(Context &ctx, unsigned int id, unsigned int parentId, const char *text, int x, int y, int w, int h, CallbackI *callback);
    static int create_panel(Context &ctx, unsigned int id, unsigned int parentId, int x, int y, int w, int h, CallbackI *callback);
    static void render_widgets(Context &ctx);
    static void onMouseButton(int x, int y , MouseState state, Context &ctx);
};

} // namespace TinyUi
