#include "sdl2_iodevice.h"

namespace tinyui {

bool IODevice::update(SDL_Event &event) {
     return SDL_PollEvent(&event);
}

} // namespace tinyui
