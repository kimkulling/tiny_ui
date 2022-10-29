#include "widgets.h"
#include "sdl2_renderer.h"

namespace tinyui {

static tui_widget *findWidget(unsigned int id, tui_widget *root) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->mId == id) {
        return root;
    }

    for (size_t i=0; i<root->mChildren.size(); ++i) {
        tui_widget *child = root->mChildren[i];
        if(child==nullptr) {
            continue;
        }
        if (child->mId==id) {
            return child;
        }
        tui_widget *found = findWidget(id, child);
        if(found != nullptr) {
            return found;
        }
    }

    return nullptr;
}

static tui_widget *create_widget(tui_context &ctx, unsigned int id) {
    tui_widget *widget = new tui_widget;
    widget->mId = id;

    return widget;
}

tui_widget *set_parent(tui_context &ctx, tui_widget *child, unsigned int parentId) {
    tui_widget *parent = nullptr;
    if (parentId == 0) {
        if (ctx.mRoot == nullptr) {
            ctx.mRoot = new tui_widget;
            ctx.mRoot->mType = WidgetType::ContainerType;
        }
        parent = ctx.mRoot;
    } else {
        parent = findWidget(parentId, ctx.mRoot);
    }

    parent->mChildren.emplace_back(child);
    parent->mRect.mergeWithRect(child->mRect);


    return parent;
}

int Widgets::create_container(tui_context &ctx, unsigned int id, unsigned int parentId, const char *text, 
        int x, int y, int w, int h) {
    if (ctx.mRoot != nullptr) {
        return -1;
    }

    tui_widget *widget = create_widget(ctx, id);
    ctx.mRoot = widget;
    widget->mRect.set(x, y, w, h);
    if (text != nullptr) {
        widget->mText.assign(text);
    }
    widget->mParent = set_parent(ctx, widget, parentId);

    return 0;
}

int Widgets::create_button(tui_context &ctx, unsigned int id, unsigned int parentId, const char *text, 
        int x, int y, int w, int h, tui_callbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return -1;
    }

    tui_widget *child = create_widget(ctx, id);
    child->mType = WidgetType::ButtonType;
    child->mRect.set(x, y, w, h);
    if (text != nullptr) {
        child->mText.assign(text);
    }

    child->mParent = set_parent(ctx, child, parentId);

    return 0;
}

int Widgets::create_panel(tui_context &ctx, unsigned int id, unsigned int parentId, int x, int y, int w, int h, tui_callbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return -1;
    }
    tui_widget *child = create_widget(ctx, id);
    child->mType = WidgetType::PanelType;
    child->mRect.set(x, y, w, h);

    child->mParent = set_parent(ctx, child, parentId);

    return 0;
}

static void render(tui_context &ctx, tui_widget *currentWidget) {
    const tui_rect &r = currentWidget->mRect;
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
            {
                Renderer::draw_rect(ctx, r.x1, r.y1, r.width, r.height, false, ctx.mStyle.mBorder);
            }
            break;

        default:
            break;
    }
    for (auto child : currentWidget->mChildren) {
        render(ctx, child);
    }
}

void Widgets::render_widgets(tui_context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    render(ctx, ctx.mRoot);
}

static void findSelectedWidget(int x, int y, tui_widget *currentChild, tui_widget **found) {
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

void Widgets::onMouseButton(int x, int y, tui_mouseState state, tui_context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    unsigned int id = 0;
    tui_widget *root = ctx.mRoot;
    tui_widget *found = nullptr;
    findSelectedWidget(x, y, root, &found);
    if (found != nullptr) {
        printf("Clicked %d\n", found->mId);
        if (found->mCallback.mfuncCallback != nullptr) {
            found->mCallback.mfuncCallback(found->mId, nullptr);
        }
    } else {
        printf("Clicked, but not found\n");
    }
}

} // namespace tinyui
