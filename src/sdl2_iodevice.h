#pragma once

#include <SDL.h>

namespace tinyui {

struct IODevice {
    ~IODevice() = default;
    static bool update(SDL_Event &event);
};

} //  namespace tinyui
