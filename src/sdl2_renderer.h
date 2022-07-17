#pragma once

#include <SDL.h>

struct color4 {
    int r,g,b,a;
};

struct handle {
    int h_;
};

handle *initScreen(int x, int y, int w, int h);
int draw_rect(int x, int y, int w, int h, bool filled, color4 fg, color4 bg);
int closeScreen(handle *h);
