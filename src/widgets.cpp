#include "widgets.h"
#include "sdl2_renderer.h"

namespace TinyUi {

static Widget *findWidget(unsigned int id, Widget *root) {
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

int Widgets::create_button(Context &ctx, unsigned int id, unsigned int parentId, const char *text, int x, int y, int w, int h, CallbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return -1;
    }

    Widget *child = new Widget;
    child->mId = id;
    child->mType = WidgetType::ButtonType;
    child->mRect.set(x, y, w, h);
    if (text != nullptr) {
        child->mText.assign(text);
    }

    Widget *parent = nullptr;
    if (parentId == 0) {
        if (ctx.mRoot == nullptr) {
            ctx.mRoot = new Widget;
            ctx.mRoot->mType = WidgetType::PanelType;
        }
        parent = ctx.mRoot;
    } else {
        parent = findWidget(parentId, ctx.mRoot);
    }
    parent->mChildren.emplace_back(child);
    parent->mergeWithRect(child->mRect);
    child->mParent = parent;

    return 0;
}

static void render(Context &ctx, Widget *currentWidget){
    const rect &r = currentWidget->mRect;
    switch( currentWidget->mType) {
        case WidgetType::ButtonType:
            {
                Renderer::draw_rect(ctx, r.x1, r.y1, r.width, r.height, true, ctx.mStyle.mFg);
                if (!currentWidget->mText.empty()) {
                    SDL_Color fg = { 0xff,0xff,0xff }, bg = {0x00,0x00,0x00};
                    
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), currentWidget->mRect.height-2, currentWidget->mRect, fg, bg);                
                }
            }
            break;

        case WidgetType::PanelType:
            break;

        default:
            break;
    }
    for (auto child : currentWidget->mChildren) {
        render(ctx, child);
    }
}

void Widgets::render_widgets(Context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    render(ctx, ctx.mRoot);
}

void findSelectedWidget(int x, int y , Widget *currentChild, Widget **found) {
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

void Widgets::onMouseButton(int x, int y , MouseState state, Context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    unsigned int id = 0;
    Widget *root = ctx.mRoot;
    Widget *found = nullptr;
    findSelectedWidget(x, y, root, &found);
    if (found != nullptr) {
        printf("Clicked %d\n", found->mId);
    } else {
        printf("Clicked, but not found\n");
    }
}

} // namespace TinyUi
