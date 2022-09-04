#include "widgets.h"
#include "sdl2_renderer.h"

namespace TinyUi {

Widget *findWidget(unsigned int id, Widget *root) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->mId == id) {
        return root;
    }

    for (size_t i=0; i<root->mChildren.size(); ++i) {
        Widget *child = root->mChildren[i];
        if(child==nullptr) {
            continue;
        }
        if (child->mId==id) {
            return child;
        }
        Widget *found = findWidget(id, child);
        if(found != nullptr) {
            return found;
        }
    }

    return nullptr;
}

int Widgets::create_button(Context &ctx, unsigned int id, unsigned int parentId,int x, int y, int w, int h, CallbackI *callback) {
    if (ctx.mRenderer == nullptr) {
        return -1;
    }

    Widget *child = new Widget;
    child->mId = id;
    child->mRect.x = x;
    child->mRect.y = y;
    child->mRect.width = w;
    child->mRect.height  = h;
    if (parentId == 0) {
        ctx.mRoot = child;
    } else {
        Widget *parent = findWidget(parentId, ctx.mRoot);
        if (parent != nullptr) {
            parent->mChildren.emplace_back(child);
            child->mParent = parent;
        }
    }

    return 0;
}

static void render(Context &ctx, Widget *currentWidget){
    const rect &r = currentWidget->mRect;
    switch( currentWidget->mType) {
        case WidgetType::ButtonType:
            Renderer::draw_rect(ctx, r.x, r.y, r.width, r.height, true, ctx.mStyle.mFg);
            break;

        case WidgetType::PanelType:
            break;

        default:
            break;
    }
}

void Widgets::render_widgets(Context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    render(ctx, ctx.mRoot);
}

} // namespace TinyUi
