#include "tinyui.h"
#include "widgets.h"

namespace tinyui {

static const tui_style DefaultStyle = {
    tui_color4{ 255, 255, 255, 0 }, 
    tui_color4{ 0, 0, 0, 0 }, 
    tui_color4{ 0, 0, 255, 0 }, 
    tui_color4{ 200, 200, 200, 0 }, 
    2
};

const tui_style &get_default_style() {
    return DefaultStyle;
}

int tui_init(tui_context &ctx) {
    return 0;
}

int tui_release(tui_context &ctx) {
    return 0;
}

} // namespace tinyui
