# AGENTS.md - Agentic Coding Guidelines for tiny_ui

## Project Overview

tiny_ui is a C++17 library for creating simple UIs on Windows and Linux, using SDL2 as the renderer. The project is built with CMake and vcpkg for dependency management.

---

## Build, Lint, and Test Commands

### Building the Project

```bash
# Configure with CMake (uses vcpkg toolchain)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the library and samples
cmake --build build --config Release
```

On Windows with Visual Studio, you can also open `tiny_ui.slnx` or use the `.vcxproj` files.

### Dependencies

The project requires:
- SDL2
- SDL2_image
- SDL2_ttf
- glm

These are managed via vcpkg (see `vcpkg.json`).

### Running the Sample Application

```bash
# After building, run the sample
./bin/release/tiny_ui_sample.exe   # Windows
./bin/tiny_ui_sample              # Linux
```

### Linting and Formatting

The project uses `.clang-format` for code formatting:

```bash
# Format a single file
clang-format -i src/tinyui.cpp

# Format all source files
find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

### Testing

**Note**: This project currently has no automated tests. The test step in CI (`.github/workflows/cmake.yml`) is commented out. If adding tests:

```bash
# Run tests via CTest (when implemented)
cd build
ctest -C Release
```

To run a single test (when tests exist), use CTest's filtering:
```bash
ctest -R test_name_pattern
```

---

## Code Style Guidelines

### General Style

- **C++ Standard**: C++17
- **Formatting**: Uses `.clang-format` (LLVM-based, see `.clang-format`)
- **Indentation**: 4 spaces, no tabs
- **Column Limit**: 0 (unlimited)

### File Organization

- Header files (`.h`) go in `src/`
- Implementation files (`.cpp`) go in `src/`
- Backend-specific code goes in `src/backends/`
- Samples go in `samples/`
- All source files must include the MIT License header

### Imports and Includes

```cpp
// Order: 1. Project headers, 2. STL/Standard headers, 3. External libraries
#include "tinyui.h"
#include "widgets.h"
#include "backends/sdl2_renderer.h"

#include <iostream>
#include <cstring>

#include "stb_image.h"
```

Include order (per `.clang-format`):
1. Local project headers (`"*.h"`)
2. System headers (`<*.h>`)
3. Other system headers (`<*>`)

### Naming Conventions

- **Types/Structs/Enums**: PascalCase (e.g., `struct Widget`, `enum class Alignment`)
- **Functions**: camelCase (e.g., `createContext()`, `initScreen()`)
- **Member Variables**: `m` prefix (e.g., `mId`, `mType`, `mParent`)
- **Constants**: PascalCase or `k` prefix (e.g., `InvalidHandle`, `kDefaultSize`)
- **Namespaces**: lowercase (e.g., `namespace tinyui`)

### Type Conventions

- Use `int32_t` for explicit-width integers
- Use `ret_code` (defined as `int32_t`) for return codes
- Use `nullptr` instead of `NULL`
- Use `const` extensively for pointer parameters and member functions
- Use `constexpr` for compile-time constants
- Use `static constexpr` for class-level constants

### Return Codes

The library uses a consistent return code pattern:
```cpp
static constexpr ret_code InvalidRenderHandle = -3;
static constexpr ret_code InvalidHandle = -2;
static constexpr ret_code ErrorCode = -1;
static constexpr ret_code ResultOk = 0;
```

### Error Handling

- Return error codes for recoverable errors
- Use `assert()` for programming errors/invariants (development only)
- Validate input parameters at the start of functions

### Enums

Use `enum class` for type-safe enums:
```cpp
enum class Alignment : int32_t {
    Invalid = -1,
    Left = 0,
    Center,
    Right,
    Count  // Use Count for array sizing
};
```

### Structs and Classes

- Use structs for POD (Plain Old Data) types
- Use `= default` for trivial constructors/destructors
- Initialize member variables with default values in class definition
- Use Doxygen-style comments for documentation

### Code Example

```cpp
/// @brief The tiny ui context.
struct Context {
    bool               mCreated{false};
    bool               mRequestShutdown{false};
    const char        *mAppTitle{nullptr};
    Widget            *mRoot{nullptr};

    /// @brief Will create a new tiny ui context.
    /// @param title The title of the context.
    /// @return The created context.
    static Context *create(const char *title);
    
    /// @brief Will destroy a valid tinyui context.
    /// @param ctx The context to destroy.
    static void destroy(Context *ctx);
};
```

### Common Patterns

```cpp
// Function returning error code
ret_code TinyUi::initScreen(int32_t x, int32_t y, int32_t w, int32_t h) {
    if (w <= 0 || h <= 0) {
        return ErrorCode;
    }
    // ... implementation
    return ResultOk;
}

// Callback interface
struct CallbackI {
    typedef int (*funcCallback) (uint32_t id, void *data);
    funcCallback mfuncCallback[Events::NumEvents];
    void *mInstance{nullptr};
};
```

---

## CI/CD

The project uses GitHub Actions (see `.github/workflows/cmake.yml`):
- Builds on Ubuntu latest
- Installs SDL2, SDL2-image, SDL2-ttf via apt
- Runs CMake configure and build

---

## Key Files

- `CMakeLists.txt` - Main build configuration
- `vcpkg.json` - Dependency manifest
- `.clang-format` - Code formatting rules
- `.github/workflows/cmake.yml` - CI pipeline
- `src/tinyui.h` - Main header with core types
- `src/widgets.h` - Widget definitions
- `src/backends/` - Platform-specific rendering backends
