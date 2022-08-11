#include "sdl2_renderer.h"

#include <cassert>
#include <iostream>

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

void logError(const char *message) {
    assert(message != nullptr);

    std::cerr << message <<", error " << SDL_GetError() << "\n";
}

int TinyUi::initRenderer(Context &ctx) {
    if ((SDL_Init(SDL_INIT_VIDEO ) == -1)) {
        logError("Error while SDL_Init.");
        ctx.mCreated = false;
        return -1;
    }

    ctx.mCreated = true;

    return 0;
}

int TinyUi::create_button(Context &ctx, unsigned int id, unsigned int parentId,int x, int y, int w, int h, CallbackI *callback) {
    if (ctx.mRenderer == nullptr) {
        return -1;
    }

    Widget *child = new Widget;
    child->mId = id;
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

int TinyUi::releaseRenderer(Context &ctx) {
    if (!ctx.mCreated) {
        std::cerr << "Not initialized.\n";
        return -1;
    }
    
    SDL_DestroyRenderer(ctx.mRenderer);
    ctx.mRenderer = nullptr;
    SDL_DestroyWindow(ctx.mWindow);
    ctx.mWindow = nullptr;
    SDL_Quit();

    return 0;
}

int TinyUi::initScreen(Context &ctx, int x, int y, int w, int h) {
    SDL_Surface *screen = nullptr;
    const char *title = ctx.title;
    if (ctx.title == nullptr) {
        title = "untitled";
    }

    ctx.mWindow = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (ctx.mWindow == nullptr) {
        logError("Error while SDL_CreateWindow.");
        return -1;
    }
    
    ctx.mRenderer = SDL_CreateRenderer(ctx.mWindow, -1, SDL_RENDERER_ACCELERATED);
    if (nullptr == ctx.mRenderer) {
        logError("Eror while SDL_CreateRenderer.");
        return -1;
    }

    return 0;
}

int TinyUi::beginRender(Context &ctx, color4 bg) {
    SDL_SetRenderDrawColor(ctx.mRenderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(ctx.mRenderer);        

    return 0;
}

int TinyUi::draw_rect(Context &ctx, int x, int y, int w, int h, bool filled, color4 fg){
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    SDL_SetRenderDrawColor(ctx.mRenderer, fg.r, fg.g, fg.b, fg.a);
    if (filled) {
        SDL_RenderFillRect(ctx.mRenderer, &r);
    } else {
        SDL_RenderDrawRect(ctx.mRenderer, &r);
    }

    return 0;
}

int TinyUi::closeScreen(Context &ctx) {
    if (ctx.mWindow == nullptr) {
        return -1;
    }

    SDL_DestroyWindow(ctx.mWindow);
    ctx.mWindow = nullptr;

    return 0;
}

int TinyUi::endRender(Context &ctx) {
    SDL_RenderPresent(ctx.mRenderer);

    return 0;
}

bool TinyUi::run(Context &ctx) {
    if (!ctx.mCreated) {
        return false;
    }

    bool running = true;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
                TinyUi::onMousePress(event.button);
                break;
        }
    }

    return running;
}

void TinyUi::onMousePress(SDL_MouseButtonEvent& b) {
    printf("pressed at %d|%d\n", b.x, b.y);
}