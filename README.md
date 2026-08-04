<p align="center">
  <img src="https://github.com/kimkulling/tiny_ui/blob/main/assets/images/tinyui_logo.png" />
</p>

# tiny_ui

A lightweight C++17 UI library for Windows and Linux, using SDL2 as the renderer.

## Features

- **Widgets**:
  - Panel
  - Button
    - Text
    - Images   
  - Label
  - Progress Bar
  - Imageview
  - TreeView (experimental)
  - Input-Field (experimental)
  - Status Bar
  - Standard-Dialogs
    - Open-File-Dialog
    - Save-File-Dialog
- **Platforms**:
  - Windows
  - Linux

## Build

```bash
# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
## Samples

You can check out samples at [Samples-Section](samples/README.md)
## Quick Start

```cpp
#include <iostream>
#include "widgets.h"

using namespace tinyui;

static int onQuit(unsigned int id, void *data) {
    auto *ctx = static_cast<Context*>(data);
    ctx->mRequestShutdown = true;
    return ResultOk;
}

int main() {
    Context *ctx = Context::create("Sample", TinyUi::getDefaultStyle());
    if (ctx == nullptr) {
        return -1;
    }
    TinyUi::initScreen(20, 20, 1024, 768);

    // This is my parent panel
    const auto parentPanel = Widgets::panel(ctx, WidgetHandle::getRootHandle(), "Dialog", 90, 5, 120, 300, nullptr);

    // And these are my two children wisgets
    Widgets::label(ctx, parentPanel, "Title", 100, 10, 100, 20, Alignment::Center);
    Widgets::button(ctx, parentPanel, "Quit", nullptr, 100, 50, 100, 40, &onQuit);

    // My render loop
    while (TinyUi::run()) {
        TinyUi::render();
    }

    TinyUi::release();
    return 0;
}
```

## Running the Sample

```bash
./bin/release/tiny_ui_sample.exe   # Windows
./bin/tiny_ui_sample               # Linux
```

![Sample](assets/images/sample1.png)

## Planned

- Checkboxes
- Dategrid
- Selection-Boxes
- Tabs
- Togglebuttons
- Layouter
- Engine Integration
- Tutorials
- Website

---

[![CMake](https://github.com/kimkulling/tiny_ui/actions/workflows/cmake.yml/badge.svg)](https://github.com/kimkulling/tiny_ui/actions/workflows/cmake.yml)
[![Quality Gate](https://sonarcloud.io/api/project_badges/measure?project=kimkulling_tiny_ui&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=kimkulling_tiny_ui)
