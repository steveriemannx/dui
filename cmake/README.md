# dui CMake Project Configuration

This document describes in detail the purpose and usage of the CMake configuration files (`*.cmake`) in the `dui\cmake` directory, for maintainers' reference.

## Directory Overview

```
cmake/
├── dui_common.cmake        # Common configuration (OS/compiler/CPU detection, path setup)
├── dui_app.cmake           # Application entry point: dui::app and dui_finalize_app()
├── dui_app_windows.cmake   # Windows platform-specific configuration
├── dui_app_linux.cmake     # Linux platform-specific configuration
├── dui_app_macos.cmake     # macOS platform-specific configuration
├── dui_app_freebsd.cmake   # FreeBSD platform-specific configuration
├── dui_app_install.cmake.in # The same layer, packaged: the consumer-side dui::app
├── dui_compiles.cmake      # Compile-option detection (AVX/AVX2)
├── dui_cef_macos.cmake     # macOS CEF packaging (CEF's own cefclient template)
├── dui_deps.cmake          # External deps: Skia zip + patch, CEF download, gn
├── dui_gen_code.cmake      # XML → C++ code generation target
├── dui_embed_res.cmake     # Resources embedded into the executable
└── dui_install.cmake       # install() rules and the exported package
```

`dui_bin.cmake` and its four `dui_bin_<platform>.cmake` files were the previous
application layer — the helper created the executable itself. They have been replaced by
`dui_app*.cmake`; see "Basic Usage" below for the contract that replaces them.

---

## Configuration Files in Detail

### 1. dui_common.cmake (Common Configuration)

**Purpose:** Defines cross-platform common configuration variables and switches; it is the base configuration file of every CMake project.

**Main detection items:**

#### 1.1 OS detection
```cmake
DUI_OS_WINDOWS  # Windows system
DUI_OS_LINUX    # Linux system
DUI_OS_MACOS    # macOS system
DUI_OS_FREEBSD  # FreeBSD system
```

#### 1.2 Compiler detection
```cmake
DUI_COMPILER_MSVC    # MSVC compiler
DUI_COMPILER_GCC     # GCC compiler
DUI_COMPILER_LLVM    # Clang/LLVM compiler
DUI_COMPILER_NAME    # Compiler name (e.g. "msvc", "mingw64-gcc", "mingw64-llvm")
```

#### 1.3 CPU architecture detection
```cmake
DUI_SYSTEM_PROCESSOR  # Processor type: x86, x64, arm32, arm64
DUI_BITS_64           # Whether this is a 64-bit system
```

#### 1.4 Build type detection
```cmake
DUI_BUILD_TYPE        # "debug" or "release"
```

**Main switches:**

| Option | Default | Description |
|----------|--------|------|
| `DUI_LOG` | OFF | Print dui debug logs |
| `DUI_SKIA_LIB_SUBPATH` | OFF | Skia library subdirectory (OFF = auto-composed) |
| `DUI_ENABLE_CEF` | OFF | Enable CEF browser support |
| `DUI_CEF_109` | OFF | Use CEF 109 (supports Win7) |
| `DUI_ENABLE_SANITIZERS` | OFF | Build dui with AddressSanitizer + UndefinedBehaviorSanitizer (development only; applied to the `dui` target as PUBLIC compile/link options, see src/CMakeLists.txt) |
| `DUI_ENABLE_MVVM` | OFF | Compile the optional data-binding module into the library |
| `DUI_WEBVIEW2_EXE` | OFF | WebView2 executable (Windows only) |
| `DUI_MINGW_STATIC` | ON | MinGW static linking |

**Main path variables:**

| Variable | Description |
|--------|------|
| `DUI_ROOT` | dui source root directory |
| `DUI_LIB_PATH` | dui library output directory |
| `DUI_BIN_PATH` | dui executable output directory |
| `DUI_SKIA_SRC_ROOT_DIR` | Skia source directory |
| `DUI_SKIA_LIB_PATH` | Skia library directory |
| `DUI_CEF_SRC_ROOT_DIR` | libCEF source directory |
| `DUI_CEF_LIB_PATH` | libCEF library directory |

**Main library list variables:**

| Variable | Description |
|--------|------|
| `DUI_LIBS` | dui base library list |
| `DUI_SKIA_LIBS` | Skia library list (svg, skshaper, skottie, sksg, jsonreader, skia) |
| `DUI_CEF_LIBS` | libCEF library list |

**Skia path composition rule:**
```cmake
# Skia libraries are output alongside the other dui libraries, with no
# extra prefix/variant directory. Multi-config generators use Debug/Release;
# single-config generators use the current CMAKE_BUILD_TYPE.
${DUI_LIB_PATH}/Debug
${DUI_LIB_PATH}/Release

# Examples:
# build/lib/Debug
# build/lib/Release
```

---

### 2. dui_app.cmake (Application Entry Point)

**Purpose:** the one file an application includes. It includes `dui_common.cmake`, creates
the `dui::app` interface target, and defines `dui_finalize_app()`.

#### 2.1 What an application writes

```cmake
include("${DUI_ROOT}/cmake/dui_app.cmake")
add_executable(my_app main.cpp MainForm.cpp)
target_link_libraries(my_app PRIVATE dui::app)
dui_finalize_app(my_app)
```

The target belongs to the application, and its sources are listed rather than globbed:
`file(GLOB ... CONFIGURE_DEPENDS)` is not reliable on the multi-config generators
(Visual Studio, Xcode) this project targets, and its failure mode is a source file
quietly not being compiled.

#### 2.2 `dui::app` — everything that is a usage requirement

| Property | Contents |
|---|---|
| `INTERFACE_INCLUDE_DIRECTORIES` | `DUI_ROOT`, `DUI_ROOT/include`, and the platform's CEF include root |
| `INTERFACE_COMPILE_FEATURES` | `cxx_std_20` |
| `INTERFACE_COMPILE_DEFINITIONS` | `UNICODE` / `_UNICODE` (Windows), `DUI_CEF=1\|0`, `DUI_WEBVIEW2=1`, `DUI_WAYLAND=1` |
| `INTERFACE_COMPILE_OPTIONS` | the platform's warning and ABI flags |
| `INTERFACE_LINK_OPTIONS` | `-Wl,--no-as-needed` (Linux/FreeBSD), `/DELAYLOAD:libcef.dll`, `-mwindows` (MinGW) |
| `INTERFACE_LINK_DIRECTORIES` | Skia's library directory on Linux/FreeBSD (which link Skia by bare name), the CEF and WebView2 directories |
| `INTERFACE_LINK_LIBRARIES` | `dui`, the vendored image libraries, `dui_skia_libs`, the platform libraries and frameworks |

#### 2.3 `dui_finalize_app(target)` — everything that cannot travel

A function rather than an interface property, because none of this has an interface form:
`RUNTIME_OUTPUT_DIRECTORY` is not an interface property, a `POST_BUILD` step has no
interface equivalent, and `add_dependencies` has to name a concrete target.

It sets the runtime output directory (and the per-config variants on MSVC) and the
`MSVC_RUNTIME_LIBRARY`, the Windows subsystem/entry point and `.rc`/manifest sources,
assembles and ad-hoc signs the macOS `.app` bundle, stages the CEF and WebView2 runtimes,
and wires the build-order edges onto Skia and onto the targets created by
`dui_gen_code.cmake` / `dui_embed_res.cmake`.

Per-application switches — CEF and WebView2 — are applied here rather than on the shared
`dui::app`, because `dui::app` is created once, by whichever application includes it first.
An application that sets `DUI_ENABLE_CEF` would otherwise either leak that setting into
every other application or not get it itself. The platform files handle this with a
per-application interface target that names `dui::app` first, which also keeps the CEF
libraries after dui's on the link line.

#### 2.4 Platform dispatch

The platform file sets `dui::app`'s usage requirements and defines
`dui_finalize_app_platform()`:
```cmake
if(DUI_OS_WINDOWS)
    include("${CMAKE_CURRENT_LIST_DIR}/dui_app_windows.cmake")
elseif(DUI_OS_LINUX)
    include("${CMAKE_CURRENT_LIST_DIR}/dui_app_linux.cmake")
elseif(DUI_OS_MACOS)
    include("${CMAKE_CURRENT_LIST_DIR}/dui_app_macos.cmake")
elseif(DUI_OS_FREEBSD)
    include("${CMAKE_CURRENT_LIST_DIR}/dui_app_freebsd.cmake")
endif()
```

---

### 3. dui_app_windows.cmake (Windows Platform Configuration)

Each of the platform files below has two halves, following §2.2 and §2.3: the
`dui::app` usage requirements (`target_*(dui_app INTERFACE ...)`), and a
`dui_finalize_app_platform(_target)` function for the per-target work. Code snippets
below that show `${PROJECT_NAME}` are from the previous layer and now appear as either
the interface target or `${_target}` -- read them for intent, not verbatim.

**Purpose:** Windows-specific compile and link configuration.

**Main configuration items:**

#### 3.1 MSVC runtime library
```cmake
if("${DUI_MD}" STREQUAL "ON")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")  # MD/MDd
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")     # MT/MTd
endif()
```

#### 3.2 MSVC compile options
```cmake
target_compile_options(${PROJECT_NAME} PRIVATE
    "/utf-8"
    $<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>
    $<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>
)
```

#### 3.3 Unicode encoding
```cmake
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
```

#### 3.4 MinGW-w64 special handling
```cmake
if(DUI_MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-mwindows ${CMAKE_EXE_LINKER_FLAGS}")    # Windows program
    if(DUI_MINGW_STATIC)
        set(CMAKE_EXE_LINKER_FLAGS "-static ${CMAKE_EXE_LINKER_FLAGS}")  # Static linking
    endif()
endif()
```

#### 3.5 Manifest file configuration
```cmake
if(DUI_BITS_64)
    set(DUI_WIN_MANIFEST "${DUI_ROOT}/cmake/manifest/dui.x64.manifest")
else()
    set(DUI_WIN_MANIFEST "${DUI_ROOT}/cmake/manifest/dui.x86.manifest")
endif()
```

#### 3.6 MSVC subsystem setting
```cmake
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup")
```

#### 3.7 CEF delay loading
```cmake
if(DUI_ENABLE_CEF)
    target_link_options(${PROJECT_NAME} PRIVATE "/DELAYLOAD:libcef.dll")
endif()
```

#### 3.8 WebView2 support
```cmake
if(DUI_WEBVIEW2_EXE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WEBVIEW2=1)
    # Automatically copies WebView2Loader.dll
endif()
```

#### 3.9 Windows system dependency libraries
```cmake
set(DUI_WINDOWS_LIBS Comctl32 Imm32 Opengl32 User32 shlwapi)
# Optional: Version.lib Winmm.lib Setupapi.lib (native backend dependencies)
```

---

### 4. dui_app_linux.cmake (Linux Platform Configuration)

Each of the platform files below has two halves, following §2.2 and §2.3: the
`dui::app` usage requirements (`target_*(dui_app INTERFACE ...)`), and a
`dui_finalize_app_platform(_target)` function for the per-target work. Code snippets
below that show `${PROJECT_NAME}` are from the previous layer and now appear as either
the interface target or `${_target}` -- read them for intent, not verbatim.

**Purpose:** Linux-specific compile and link configuration.

**Main configuration items:**

```cmake
# CEF support
if(DUI_ENABLE_CEF)
    include_directories(${DUI_CEF_SRC_ROOT_DIR})
    link_directories("${DUI_CEF_LIB_PATH}")
endif()

# Linux system dependency libraries
set(DUI_LINUX_LIBS X11 freetype fontconfig pthread dl)

# Link command
 target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_CEF_LIBS} ${DUI_LINUX_LIBS})
```

---

### 5. dui_app_macos.cmake (macOS Platform Configuration)

Each of the platform files below has two halves, following §2.2 and §2.3: the
`dui::app` usage requirements (`target_*(dui_app INTERFACE ...)`), and a
`dui_finalize_app_platform(_target)` function for the per-target work. Code snippets
below that show `${PROJECT_NAME}` are from the previous layer and now appear as either
the interface target or `${_target}` -- read them for intent, not verbatim.

**Purpose:** macOS-specific compile and link configuration.

**Main configuration items:**

#### 5.1 System framework lookup
```cmake
find_library(ACCELERATE Accelerate)
find_library(COREFOUNDATION CoreFoundation)
find_library(CORETEXT CoreText)
find_library(COREGRAPHICS CoreGraphics)
```

#### 5.2 Compiler flags
```cmake
set(DUI_COMPILER_FLAGS
    -fno-strict-aliasing
    -fstack-protector
    -funwind-tables
    -fvisibility=hidden
    -Wall
    -Wextra
    # ... more flags
)

set(DUI_CXX_COMPILER_FLAGS
    -fno-threadsafe-statics
    -fvisibility-inlines-hidden
    -frtti
    # ... more flags
)
```

#### 5.3 Link command
```cmake
target_link_libraries(${PROJECT_NAME}
    ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_CEF_LIBS}
    ${ACCELERATE} ${COREFOUNDATION} ${CORETEXT} ${COREGRAPHICS} ${DUI_MACOS_LIBS}
    "-framework AppKit" "-framework Foundation" "-framework Metal" "-framework Cocoa"
)
```

---

### 6. dui_app_freebsd.cmake (FreeBSD Platform Configuration)

Each of the platform files below has two halves, following §2.2 and §2.3: the
`dui::app` usage requirements (`target_*(dui_app INTERFACE ...)`), and a
`dui_finalize_app_platform(_target)` function for the per-target work. Code snippets
below that show `${PROJECT_NAME}` are from the previous layer and now appear as either
the interface target or `${_target}` -- read them for intent, not verbatim.

**Purpose:** FreeBSD-specific compile and link configuration.

**Main configuration items:**

```cmake
# FreeBSD system dependency libraries
set(DUI_FREEBSD_LIBS pthread dl)

find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(X11 REQUIRED)

 target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_FREEBSD_LIBS} ${X11_LIBRARIES} Freetype::Freetype Fontconfig::Fontconfig)
```

---

### 7. dui_compiles.cmake (Compile-Option Detection)

**Purpose:** Detects CPU feature support of the target platform (e.g. AVX/AVX2 instruction sets).

**Detection items:**

```cmake
# AVX support detection
check_cxx_source_compiles("
    #include <immintrin.h>
    int main() {
        __m256 a = _mm256_set1_ps(0.0f);
        return 0;
    }
" DUI_HAVE_AVX)

# AVX2 support detection
check_cxx_source_compiles("
    #include <immintrin.h>
    int main() {
        __m256i a = _mm256_set1_epi32(0);
        return 0;
    }
" DUI_HAVE_AVX2)
```

---

### 8. dui_cef_macos.cmake (macOS CEF Special Configuration)

**Purpose:** Special configuration for CEF browser support on macOS.

**Main configuration items:**

- CEF_ROOT path setting
- CEF Framework configuration
- Helper app configuration (multi-process support)
- Resource file copying (themes, language packs, fonts, etc.)
- App Bundle configuration

---

## CMake Usage Examples

### Basic Usage

#### 1. Create CMakeLists.txt

There are two entry points, and they lead to the same three lines. **Inside dui's source
tree** (an example, or an application built alongside the library) you locate the module
by path:

```cmake
cmake_minimum_required(VERSION 4.0)
project(my_dui_app CXX)

get_filename_component(DUI_ROOT "<path to dui>" ABSOLUTE)
include("${DUI_ROOT}/cmake/dui_app.cmake")
```

**Against an installed dui**, `find_package` loads the same target and function from the
package, and the rest of the file is identical:

```cmake
cmake_minimum_required(VERSION 4.0)
project(my_dui_app CXX)

find_package(dui CONFIG REQUIRED)      # provides dui::app and dui_finalize_app()
```

The consumer's `dui::app` is a thinner thing than the build tree's — it is created by
`dui_app.cmake` inside the package rather than by `dui_common.cmake`, so it does not
trigger dependency configuration, and it does not carry dui's own warning flags onto your
translation units. It also does not offer the CEF/WebView2 staging or the code-generation
targets, which are build-tree capabilities. The linking line and the finalize call are the
same either way:

```cmake
add_executable(my_dui_app
    MainForm.cpp
    main.cpp
)
target_link_libraries(my_dui_app PRIVATE dui::app)
dui_finalize_app(my_dui_app)
```

Four things to know about this shape:

- **You own the target.** `add_executable` is yours to write, and its sources are listed
  rather than globbed — a glob is not reliable on the multi-config generators
  (Visual Studio, Xcode) this project targets. `examples/hello/CMakeLists.txt` is the
  smallest complete example.
- **`dui::app`** is an INTERFACE target carrying the include directories, the C++ standard,
  the compile definitions and options, the link options and directories, and the libraries.
  Linking it is the whole of the configuration.
- **`dui_finalize_app(target)`** does what an INTERFACE target cannot: the runtime output
  directory, the Windows entry point, the macOS `.app` assembly and ad-hoc code signature,
  and — in the build tree — the manifest, the build-order edges onto Skia and any generated
  code, and the CEF/WebView2 runtime staging. Call it once, after the target exists.
- **In the build tree**, `dui_app.cmake` includes `dui_common.cmake` itself. You do not
  include it separately, and you do not set `DUI_PROJECT_SRC_DIR`. The packaged copy does
  not include it, deliberately — see above.

**Migrating from the previous contract** (`include(.../dui_bin.cmake)` with no
`add_executable`): that helper created the executable for you, and it has been replaced.
The conversion is mechanical — list the sources the helper used to glob, add the two calls
above, and delete the `DUI_PROJECT_SRC_DIR` line. The one case that is *not* mechanical is
the macOS CEF packaging path: `cmake/dui_cef_macos.cmake` creates and assembles the target
itself, so on that path do not call `add_executable` and call `dui_finalize_app()` with no
argument. `examples/cef/CMakeLists.txt` shows both branches.

#### 2. Build commands

```bash
# Create a build directory (in-source builds are forbidden)
mkdir build
cd build

# Configure the project
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release
```

---

### Advanced Examples

#### 1. Enable CEF support

```bash
cmake -S .. -B . -DDUI_ENABLE_CEF=ON -DCMAKE_BUILD_TYPE=Release
```

#### 2. Use CEF 109 (supports Win7)

```bash
cmake -S .. -B . -DDUI_ENABLE_CEF=ON -DDUI_CEF_109=ON -DCMAKE_BUILD_TYPE=Release
```

#### 4. Specify the Skia library path

```bash
cmake -S .. -B . -DDUI_SKIA_LIB_SUBPATH=llvm.x64.release -DCMAKE_BUILD_TYPE=Release
```

#### 5. Enable debug logs

```bash
cmake -S .. -B . -DDUI_LOG=ON -DCMAKE_BUILD_TYPE=Debug
```

#### 6. MSVC dynamic runtime

```bash
cmake -S .. -B . -DDUI_MD=ON -DCMAKE_BUILD_TYPE=Release
```

---

### MinGW-w64 Build Examples

#### 1. Use the GCC compiler

```bash
cmake -S .. -B ./build_gcc -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER=gcc ^
    -DCMAKE_CXX_COMPILER=g++ ^
    -DCMAKE_BUILD_TYPE=Release

cmake --build ./build_gcc
```

#### 2. Use the Clang compiler

```bash
cmake -S .. -B ./build_llvm -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER=clang ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_BUILD_TYPE=Release

cmake --build ./build_llvm
```

---

## Compile Switch Summary

| Option | Type | Default | Description |
|----------|------|--------|------|
| `DUI_LOG` | BOOL | OFF | Print debug logs |
| `DUI_SKIA_LIB_SUBPATH` | STRING | OFF | Skia library subdirectory |
| `DUI_ENABLE_CEF` | BOOL | OFF | Enable CEF support |
| `DUI_CEF_109` | BOOL | OFF | CEF 109 version (Win7) |
| `DUI_WEBVIEW2_EXE` | BOOL | OFF | WebView2 executable |
| `DUI_MD` | BOOL | OFF | MSVC dynamic runtime (/MD) |
| `DUI_MINGW_STATIC` | BOOL | ON | MinGW static linking |

---

## Platform Differences

### Windows vs Linux vs macOS vs FreeBSD

| Item | Windows | Linux | macOS | FreeBSD |
|--------|---------|-------|-------|---------|
| C++ standard | C++20 | C++20 | C++20 | C++20 |
| Encoding | Unicode | UTF-8 | UTF-8 | UTF-8 |
| Graphics | Skia + GDI | Skia + X11 | Skia + Metal | Skia + X11 |
| Input support | Win32/native backend | X11/native backend | Cocoa/native backend | X11/native backend |
| Browser | CEF/WebView2 | CEF | CEF | ❌ |
| native backend default | OFF | ON | ON | ON |

### MSVC vs MinGW-w64

| Item | MSVC | MinGW-w64 |
|--------|------|-----------|
| Runtime | MT/MD | Static linking |
| Subsystem | WINDOWS | WINDOWS |
| Manifest | Embedded | RC file |
| CEF delay load | /DELAYLOAD | ❌ |

---

## Maintenance Notes

### 1. Clear the cache after changing switches

CMake caches option values; clear the cache after changing them:

```bash
rm -rf CMakeCache.txt CMakeFiles/
# Or delete the whole build directory and reconfigure
```

### 2. In-source builds are forbidden

The dui CMake configuration forcibly forbids in-source builds:

```cmake
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree build...")
endif()
```

### 3. Skia library path requirements

Make sure the Skia build output is in the correct directory:
```
build/lib/
├── Debug/
└── Release/
```

### 4. Third-party dependencies

Make sure the following dependencies are available before building:

| Platform | Required dependencies |
|------|----------|
| Windows | Skia |
| Linux | Skia, X11, Freetype, Fontconfig |
| macOS | Skia, Cocoa/Metal frameworks |
| FreeBSD | Skia, X11, Freetype, Fontconfig |

---

## Troubleshooting

### Q1: CMake reports "Unknown OS"

**Cause:** The operating system is not supported.

**Solution:** Check the OS detection logic in `dui_common.cmake`.

---

### Q2: Skia library not found

**Cause:** Skia is not built or the path is incorrect.

**Solution:**
1. Verify Skia is built: `ls build/lib/Debug` / `ls build/lib/Release`
2. Use `DUI_SKIA_LIB_SUBPATH` to specify the correct path

---

### Q3: MinGW build reports linker error

**Cause:** A dependency library may be missing or the link order is wrong.

**Solution:**
1. Make sure `DUI_MINGW_STATIC=ON`
2. Check `CMAKE_EXE_LINKER_FLAGS`

---

### Q4: CEF program fails to start

**Cause:** libcef.dll not found or version mismatch.

**Solution:**
1. Verify the CEF libraries were downloaded correctly
2. Check the `DUI_CEF_LIB_PATH` path
3. Copy libcef.dll to the executable directory

---

### Q5: WebView2 program cannot run

**Cause:** WebView2Loader.dll not found.

**Solution:**
1. Make sure `DUI_WEBVIEW2_EXE=ON`
2. Check whether WebView2Loader.dll is in the bin directory

---

### Q6: macOS build reports framework not found

**Cause:** Cocoa/AppKit frameworks are not linked correctly.

**Solution:**
1. Make sure Xcode (or a compiler with framework support) is used
2. Check the framework configuration in `dui_app_macos.cmake`
