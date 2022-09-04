#pragma once

#include <vector>
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
    std::vector<Widget*> mChildren;
    CallbackI mCallback;
};

struct Widgets {
    static int create_button(Context &ctx, unsigned int id, unsigned int parentId,int x, int y, int w, int h, CallbackI *callback);
    static void render_widgets(Context &ctx);
};

} // namespace TinyUi
