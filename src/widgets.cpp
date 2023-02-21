
#include "widgets.h"
#include "sdl2_renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cassert>
#include <map>

namespace tinyui {

using ImageCache = std::map<const char*, tui_image*>;

static ImageCache sImageCache;

static tui_image *find_image(const char *filename) {
    assert(filename != nullptr);

    ImageCache::iterator it = sImageCache.find(filename);
    if (it == sImageCache.end()) {
        return nullptr;
    }

    return it->second;
}

static tui_image *load_into_cache(const char *filename) {
    if (filename == nullptr) {
        return nullptr;
    }

    tui_image *image = find_image(filename);
    if (image != nullptr) {
        return image;
    }

    image = new tui_image;
    if (image == nullptr) {
        return nullptr;
    }

    int x, y, comp;
    image->mImage = stbi_load(filename, &x, &y, &comp, 0);
    if (image == nullptr) {
        return nullptr;
    }
    
    sImageCache[filename] = image;

    return image;
}

static tui_widget *findWidget(unsigned int id, tui_widget *root) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->mId == id) {
        return root;
    }

    for (size_t i=0; i<root->mChildren.size(); ++i) {
        tui_widget *child = root->mChildren[i];
        if (child == nullptr) {
            continue;
        }
        if (child->mId == id) {
            return child;
        }
        tui_widget *found = findWidget(id, child);
        if(found != nullptr) {
            return found;
        }
    }

    return nullptr;
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
        return ErrorCode;
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

int Widgets::create_label(tui_context &ctx, unsigned int id, unsigned int parentId, const char *text, int x, int y, int w, int h) {
    if (ctx.mRoot == nullptr) {
        return ErrorCode;
    }

    tui_widget *widget = create_widget(ctx, id);
    widget->mRect.set(x, y, w, h);
    widget->mType = WidgetType::LabelType;
    if (text != nullptr) {
        widget->mText.assign(text);
    }
    widget->mParent = set_parent(ctx, widget, parentId);

    return 0;
}

int Widgets::create_button(tui_context &ctx, unsigned int id, unsigned int parentId, const char *text, tui_image *image, 
        int x, int y, int w, int h, tui_callbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return ErrorCode;
    }

    tui_widget *child = create_widget(ctx, id);
    child->mType = WidgetType::ButtonType;
    child->mRect.set(x, y, w, h);
    child->mCallback = callback;
    if (text != nullptr) {
        child->mText.assign(text);
    }
    if (image != nullptr) {
        child->mImage = image;
    }

    child->mParent = set_parent(ctx, child, parentId);

    return 0;
}

int Widgets::create_panel(tui_context &ctx, unsigned int id, unsigned int parentId, int x, int y, int w, int h, tui_callbackI *callback) {
    if (ctx.mSDLContext.mRenderer == nullptr) {
        return ErrorCode;
    }
    tui_widget *child = create_widget(ctx, id);
    child->mType = WidgetType::PanelType;
    child->mRect.set(x, y, w, h);

    child->mParent = set_parent(ctx, child, parentId);

    return 0;
}

static void render(tui_context &ctx, tui_widget *currentWidget) {
    if (!currentWidget->mEnabled) {
        return;
    }

    const tui_rect &r = currentWidget->mRect;
    switch( currentWidget->mType) {
        case WidgetType::ButtonType:
            {
                Renderer::draw_rect(ctx, r.x1, r.y1, r.width, r.height, true, ctx.mStyle.mFg);
                if (!currentWidget->mText.empty()) {
                    SDL_Color fg = { 0x00, 0x00, 0xff }, bg = { 0xff, 0xff, 0xff };
                    Renderer::drawText(ctx, currentWidget->mText.c_str(), currentWidget->mRect.height - 2, currentWidget->mRect, fg, bg);                
                }
            }
            break;

        case WidgetType::LabelType: 
            {
                if (!currentWidget->mText.empty()) {
                    SDL_Color fg = { 0x00,0x00,0xff }, bg = {0xff,0xff,0xff};   
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

    for (auto &child : currentWidget->mChildren) {
        render(ctx, child);
    }
}

void Widgets::render_widgets(tui_context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    render(ctx, ctx.mRoot);
}

void Widgets::onMouseButton(int x, int y, int eventType, tui_mouseState state, tui_context &ctx) {
    assert(eventType >= 0);
    assert(eventType < tui_events::NumEvents);

    if (ctx.mRoot == nullptr) {
        return;
    }

    tui_widget *found = nullptr;
    findSelectedWidget(x, y, ctx.mRoot, &found);
    if (found != nullptr) {
        printf("Clicked %d\n", found->mId);
        if (found->mCallback->mfuncCallback[eventType] != nullptr) {
            found->mCallback->mfuncCallback[eventType](found->mId, found->mCallback->mInstance);
        }
    } else {
        printf("Clicked, but not found\n");
    }
}

void recursive_clear(tui_widget *current) {
    if (current == nullptr) {
        return;
    }
    if (current->mChildren.empty()) {
        return;
    }

    for (size_t i = 0; i < current->mChildren.size(); ++i) {
        recursive_clear(current->mChildren[i]);
        delete current;
    }
}

void Widgets::clear(tui_context &ctx) {
    if (ctx.mRoot == nullptr) {
        return;
    }

    tui_widget *current = ctx.mRoot;
    recursive_clear(current);
}


void Widgets::setEnableState(tui_context &ctx, unsigned int id, bool enabled) {
    tui_widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        widget->mEnabled = enabled;
    }
}

bool Widgets::isEnabled(tui_context &ctx, unsigned int id) {
    tui_widget *widget = findWidget(id, ctx.mRoot);
    if (widget != nullptr) {
        return widget->mEnabled;
    }

    return false;
}

} // namespace tinyui
