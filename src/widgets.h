#pragma once

#include "tinyui.h"

namespace tinyui {

struct tui_context;

/// @brief
enum class WidgetType {
    Invalid = -1,
    ContainerType = 0,
    ButtonType,
    LabelType,
    PanelType,
    Count
};

/// @brief
enum class LayoutType {
    Invalid = -1,
    HorizontalLayout,
    VerticalLayout,
    Count
};

/// @brief
enum class LayoutPolicy {
    Invalid,
    AbsolutePolicy,
    RelativePolicy,
    Count
};

/// @brief
using Id = uint32_t;

/// @brief
struct tui_widget {
    Id mId;
    WidgetType mType;
    tui_widget *mParent;
    bool mEnabled;
    tui_rect mRect;
    std::string mText;
    tui_image *mImage;
    std::vector<tui_widget*> mChildren;
    tui_callbackI *mCallback;

    tui_widget() :
            mId(0),
            mType(WidgetType::Invalid), 
            mParent(nullptr), 
            mEnabled(true), 
            mRect(), mText(), 
            mImage(nullptr), 
            mChildren(), 
            mCallback(nullptr) {
        // empty
    }
    
    ~tui_widget() = default;
};
 
/// @brief
struct Widgets {
    /// @brief
    static tui_ret_code createContainer(tui_context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h);

    /// @brief
    static tui_widget *findWidget(Id id, tui_widget *root);

    /// @brief
    static void findSelectedWidget(int x, int y, tui_widget *currentChild, tui_widget **found);

    /// @brief
    static tui_ret_code createLabel(tui_context &ctx, Id id, Id parentId, const char *text, int x, int y, int w, int h);
    
    /// @brief
    static tui_ret_code createButton(tui_context &ctx, Id id, Id parentId, const char *text, tui_image *image, int x, int y, 
        int w, int h, tui_callbackI *callback);
    
    /// @brief
    static tui_ret_code createPanel(tui_context &ctx, Id id, Id parentId, int x, int y, int w, int h, tui_callbackI *callback);
    
    /// @brief
    static void renderWidgets(tui_context &ctx);
    
    /// @brief
    static void onMouseButton(int x, int y, int eventType, tui_mouseState state, tui_context &ctx);
    
    /// @brief
    static void onMouseMove(int x, int y, int eventType, tui_mouseState state, tui_context &ctx);
    
    /// @brief
    static void onKey(const char *key, bool isDown, tui_context &ctx);
    
    /// @brief
    static void clear(tui_context &ctx);
    
    /// @brief
    static void setEnableState(tui_context &ctx, Id id, bool enabled);
    
    /// @brief
    static bool isEnabled(tui_context &ctx, Id id);
};

} // namespace tinyui
