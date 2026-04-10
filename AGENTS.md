# AGENTS.md - Agentic Coding Guidelines for tiny_ui

## Project Overview

tiny_ui is a C++17 library for creating simple UIs on Windows and Linux, using SDL2 as the renderer. Built with CMake and vcpkg.

---

## Build Commands

### Building

```bash
# Configure with CMake (uses vcpkg toolchain)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build library and samples
cmake --build build --config Release
```

Or use CMake Presets: `cmake --preset default`

On Windows: open `tiny_ui.slnx` or use `.vcxproj` files.

### Dependencies (via vcpkg)
- SDL2, SDL2_image, SDL2_ttf, glm

### Run Sample

```bash
./bin/release/tiny_ui_sample.exe   # Windows
./bin/tiny_ui_sample               # Linux
```

---

## Linting and Testing

### Formatting

```bash
clang-format -i src/tinyui.cpp              # single file
find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i  # all
```

### Testing

No automated tests exist yet (CI test step is commented out). When added:

```bash
cd build && ctest -C Release              # all tests
ctest -R test_name_pattern                 # single test
```

---

## Code Style

### General
- **C++ Standard**: C++17
- **Formatting**: `.clang-format` (LLVM-based)
- **Indentation**: 4 spaces, no tabs
- **Column Limit**: 0 (unlimited)

### File Organization
- Headers (`.h`) and sources (`.cpp`) in `src/`
- Backend code in `src/backends/`
- Samples in `samples/`
- All files require MIT License header

### Include Order (per clang-format)
1. Local project (`"*.h"`)
2. System headers (`<*.h>`)
3. Other headers (`<*>`)

```cpp
#include "tinyui.h"
#include "backends/sdl2_renderer.h"

#include <iostream>

#include "stb_image.h"
```

### Naming
- **Types/Enums**: PascalCase (`Widget`, `Alignment`)
- **Functions**: camelCase (`createContext`)
- **Members**: `m` prefix (`mId`, `mRoot`)
- **Constants**: PascalCase (`ResultOk`) or `k` prefix
- **Namespaces**: lowercase (`tinyui`)

### Types
- Use `int32_t` for explicit-width integers
- Use `ret_code` (`int32_t`) for return values
- Use `nullptr` not `NULL`
- Use `constexpr` for compile-time constants
- Use `static constexpr` for class constants

### Return Codes
```cpp
static constexpr ret_code InvalidRenderHandle = -3;
static constexpr ret_code InvalidHandle = -2;
static constexpr ret_code ErrorCode = -1;
static constexpr ret_code ResultOk = 0;
```

### Error Handling
- Return error codes for recoverable errors
- Use `assert()` for invariants
- Validate inputs at function start

### Enums
```cpp
enum class Alignment : int32_t {
    Invalid = -1,
    Left = 0, Center, Right,
    Count  // for array sizing
};
```

### Structs/Classes
- Use structs for POD types
- Use `= default` for trivial constructors/destructors
- Initialize members with default values
- Use Doxygen-style `/// @brief` comments

---

## CI/CD

GitHub Actions (`.github/workflows/cmake.yml`): builds on Ubuntu, installs SDL2 libs, runs CMake configure + build.

---

## Key Files
- `CMakeLists.txt` - build config
- `vcpkg.json` - dependencies
- `.clang-format` - formatting rules
- `src/tinyui.h` - core types
- `src/widgets.h` - widget definitions
- `src/backends/` - platform backends
