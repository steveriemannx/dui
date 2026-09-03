# dui Development Progress

## Current Scope

- SDL implementation and SDL-specific logic are intentionally not being fixed.
- macOS is the current verification platform.
- Windows code paths were reviewed but have not been built on a Windows host.
- No git commit has been created.

## Completed

### Core API and Lifetime

- Replaced `volatile` thread state with atomic state where needed.
- Fixed `FrameworkThread` stop and destruction ordering.
- Prevented a worker thread from detaching and outliving its owner.
- Fixed timer ID allocation synchronization.
- Fixed weak callback initialization synchronization.
- Fixed `SharePtr` self-reset and self-move cases.
- Added safe failure behavior for invalid `ControlPtrT` dereference.
- Made `ThreadManager::Clear()` stop registered worker threads.
- Fixed `ImageDecoderFactory::AddImageDecoder()` success return value.
- Fixed DPI scale comparison when pixel density is enabled.
- Fixed SVG constrained-size scale calculation.
- Added `GlobalManager::Startup()` failure rollback.
- Allowed macOS and Windows `ThreadMessage` objects to be initialized again after cleanup.

### CMake

- Converted the main native paths from directory-wide include/definition/link configuration to target-level configuration.
- Added a target-based Skia link target for native builds.
- Fixed macOS CEF bundle selection so ordinary examples do not enter the CEF bundle path.
- Added include paths to macOS CEF main and Helper targets.
- Fixed duplicate zlib linkage in native executable link lists.
- Moved generated XML code into the build tree.
- Added non-SDL multi-config output handling.
- Disabled vendored third-party default install rules that conflicted with renamed dui archives.
- Fixed zlib build/install interface include paths.
- Replaced remaining non-SDL `aux_source_directory()` source discovery with `file(GLOB CONFIGURE_DEPENDS)`.

### Tests

Added `tests/core_tests.cpp` and `tests/CMakeLists.txt`.

The current default tests cover:

- `SharePtr`
- `WeakCallback`
- `ImageDecoderFactory`
- `StringUtil`
- `StringConvert`
- `FilePath`
- `AttributeUtil`
- `UiRect`, `UiSize`, and `UiMargin`
- DPI scaling and `MulDiv`
- `FrameworkThread` task submission, cancellation, and restart behavior
- `TimerManager` registration, cancellation, and cleanup
- XML attribute parsing
- SVG decoding and invalid image data
- `GlobalManager` startup, shutdown, and failure rollback

CTest currently registers:

- `dui_core_tests`
- `dui_global_startup`
- `dui_global_failure_rollback`

### Install Package

Added:

- `cmake/dui_install.cmake`
- `cmake/duiConfig.cmake.in`

The install package currently installs:

- `libdui.a`
- `libdui_entry.a`
- dui public headers
- zlib public headers
- dui resources
- `duiTargets.cmake`
- `duiConfig.cmake`
- `duiConfigVersion.cmake`

Consumers can use:

```cmake
find_package(dui CONFIG REQUIRED)
target_link_libraries(app PRIVATE dui::dui)
```

## macOS Verification

### Native Build

Configuration directory: `build-macos-verify`

Verified successfully:

```text
dui target
hello.app
```

### CEF Build

Configuration directory: `build-macos-cef`

Verified successfully:

```text
dui target with CEF sources
cef.app
CefBrowser.app
all macOS CEF Helper targets
```

### Tests and Install

Verified successfully:

```text
3/3 CTest tests passed
cmake --install completed successfully
```

Example commands:

```bash
cmake -S . -B build-macos-cef \
  -DCMAKE_BUILD_TYPE=Release \
  -DDUI_ENABLE_CEF=ON \
  -DDUI_BUILD_CEF_EXAMPLES=ON \
  -DDUI_EXAMPLES_MODE=XML \
  -DDUI_BUILD_SDL_FROM_SOURCE=OFF \
  -DDUI_BUILD_TESTS=ON

cmake --build build-macos-cef --target cef CefBrowser dui_core_tests --parallel 8
ctest --test-dir build-macos-cef --output-on-failure
cmake --build build-macos-cef --target dui_entry
cmake --install build-macos-cef --prefix build-macos-cef/install
```

## Remaining Work

- Add actual PNG, JPEG, and WebP pixel decode tests in a render-aware test process.
- Add more resource loading and Box layout tests.
- Add real timer callback execution tests without relying on a running GUI loop.
- Add optional macOS GUI lifecycle tests, disabled by default in headless CI.
- Add an external consumer configure/link test for the installed package.
- Decide whether CEF framework/runtime files should be included in the install package.
- Build and test the native Windows configuration on the remote Windows host.
- Remove remaining macOS deprecation warnings.

## Remote Host

The remote host is expected at:

```text
steve@192.168.137.1
D:\projects-main\dui
```

The first SSH attempt was blocked by host-key verification. Remote testing has
not started yet.
