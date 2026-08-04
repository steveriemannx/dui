# dui CMake Project Configuration

This document describes in detail the purpose and usage of the CMake configuration files (`*.cmake`) in the `dui\cmake` directory, for maintainers' reference.

## Directory Overview

```
cmake/
├── dui_common.cmake        # Common configuration (OS/compiler/CPU detection, path setup)
├── dui_bin.cmake           # Executable base configuration (C++ standard, path setup)
├── dui_bin_windows.cmake   # Windows platform-specific configuration
├── dui_bin_linux.cmake     # Linux platform-specific configuration
├── dui_bin_macos.cmake     # macOS platform-specific configuration
├── dui_bin_freebsd.cmake   # FreeBSD platform-specific configuration
├── dui_compiles.cmake      # Compile-option detection (AVX/AVX2)
└── dui_cef_macos.cmake     # macOS CEF special configuration
```

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
| `DUI_ENABLE_SDL` | Windows=OFF, others=ON | Enable SDL input support |
| `DUI_ENABLE_CEF` | OFF | Enable CEF browser support |
| `DUI_CEF_109` | OFF | Use CEF 109 (supports Win7) |
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
| `DUI_SDL_SRC_ROOT_DIR` | SDL3 source directory |
| `DUI_SDL_LIB_PATH` | SDL3 library directory |
| `DUI_CEF_SRC_ROOT_DIR` | libCEF source directory |
| `DUI_CEF_LIB_PATH` | libCEF library directory |

**Main library list variables:**

| Variable | Description |
|--------|------|
| `DUI_LIBS` | dui base library list |
| `DUI_SKIA_LIBS` | Skia library list (svg, skshaper, skottie, sksg, jsonreader, skia) |
| `DUI_SDL_LIBS` | SDL3 library list |
| `DUI_CEF_LIBS` | libCEF library list |

**Skia path composition rule:**
```cmake
# When DUI_SKIA_LIB_SUBPATH is OFF, the path is auto-composed as:
${DUI_SKIA_SRC_ROOT_DIR}/out/${DUI_COMPILER_NAME}.${DUI_SYSTEM_PROCESSOR}.${DUI_BUILD_TYPE}

# Examples:
# skia/out/msvc.x64.release
# skia/out/mingw64-gcc.x64.release
# skia/out/llvm.x86.debug
```

---

### 2. dui_bin.cmake (Executable Base Configuration)

**Purpose:** Common CMake configuration for executables, applicable to all platforms.

**Main configuration items:**

#### 2.1 C++ standard
```cmake
set(CMAKE_CXX_STANDARD 20)             # C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)    # C++20 required
```

#### 2.2 Path configuration
```cmake
include_directories(${DUI_ROOT})              # dui root directory
include_directories(${DUI_PROJECT_SRC_DIR})   # project source directory
link_directories("${DUI_LIB_PATH}")           # dui library directory
link_directories("${DUI_SKIA_LIB_PATH}")      # Skia library directory
link_directories("${DUI_SDL_LIB_PATH}")       # SDL library directory (if enabled)
```

#### 2.3 Output directory
```cmake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DUI_BIN_PATH}")
```

#### 2.4 Source collection
```cmake
aux_source_directory(${DUI_PROJECT_SRC_DIR} SRC_FILES)
# Subdirectories are supported via the DUI_SRC_SUB_DIRS variable
```

#### 2.5 Platform dispatch
The matching platform configuration file is included automatically based on the OS:
```cmake
if(DUI_OS_WINDOWS)
    include("${CMAKE_CURRENT_LIST_DIR}/dui_bin_windows.cmake")
elseif(DUI_OS_LINUX)
    include("${CMAKE_CURRENT_LIST_DIR}/dui_bin_linux.cmake")
elseif(DUI_OS_MACOS)
    include("${CMAKE_CURRENT_LIST_DIR}/dui_bin_macos.cmake")
elseif(DUI_OS_FREEBSD)
    include("${CMAKE_CURRENT_LIST_DIR}/dui_bin_freebsd.cmake")
endif()
```

---

### 3. dui_bin_windows.cmake (Windows Platform Configuration)

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
add_compile_options("/utf-8")                      # UTF-8 source encoding
add_compile_options($<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>)   # Multi-core compile
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>)
```

#### 3.3 Unicode encoding
```cmake
add_definitions(-DUNICODE -D_UNICODE)
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
    set(DUI_WIN_MANIFEST "${DUI_ROOT}/msvc/manifest/dui.x64.manifest")
else()
    set(DUI_WIN_MANIFEST "${DUI_ROOT}/msvc/manifest/dui.x86.manifest")
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
# Optional: Version.lib Winmm.lib Setupapi.lib (SDL dependencies)
```

---

### 4. dui_bin_linux.cmake (Linux Platform Configuration)

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
target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SDL_LIBS} ${DUI_SKIA_LIBS} ${DUI_CEF_LIBS} ${DUI_LINUX_LIBS})
```

---

### 5. dui_bin_macos.cmake (macOS Platform Configuration)

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
    ${DUI_LIBS} ${DUI_SDL_LIBS} ${DUI_SKIA_LIBS} ${DUI_CEF_LIBS}
    ${ACCELERATE} ${COREFOUNDATION} ${CORETEXT} ${COREGRAPHICS} ${DUI_MACOS_LIBS}
    "-framework AppKit" "-framework Foundation" "-framework Metal" "-framework Cocoa"
)
```

---

### 6. dui_bin_freebsd.cmake (FreeBSD Platform Configuration)

**Purpose:** FreeBSD-specific compile and link configuration.

**Main configuration items:**

```cmake
# FreeBSD system dependency libraries
set(DUI_FREEBSD_LIBS pthread dl)

find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(X11 REQUIRED)

target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SDL_LIBS} ${DUI_SKIA_LIBS} ${DUI_FREEBSD_LIBS} ${X11_LIBRARIES} Freetype::Freetype Fontconfig::Fontconfig)
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

Create `CMakeLists.txt` in the project root:

```cmake
cmake_minimum_required(VERSION 3.21)
project(my_dui_app)

# Set the project source directory
set(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Include the dui CMake configuration
include(${DUI_ROOT}/cmake/dui_bin.cmake)
```

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

#### 1. Enable SDL support

```bash
cmake -S .. -B . -DDUI_ENABLE_SDL=ON -DCMAKE_BUILD_TYPE=Release
```

#### 2. Enable CEF support

```bash
cmake -S .. -B . -DDUI_ENABLE_CEF=ON -DCMAKE_BUILD_TYPE=Release
```

#### 3. Use CEF 109 (supports Win7)

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
    -DCMAKE_BUILD_TYPE=Release ^
    -DDUI_ENABLE_SDL=ON

cmake --build ./build_gcc
```

#### 2. Use the Clang compiler

```bash
cmake -S .. -B ./build_llvm -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER=clang ^
    -DCMAKE_CXX_COMPILER=clang++ ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DDUI_ENABLE_SDL=ON

cmake --build ./build_llvm
```

---

## Compile Switch Summary

| Option | Type | Default | Description |
|----------|------|--------|------|
| `DUI_LOG` | BOOL | OFF | Print debug logs |
| `DUI_SKIA_LIB_SUBPATH` | STRING | OFF | Skia library subdirectory |
| `DUI_ENABLE_SDL` | BOOL | Windows=OFF, others=ON | Enable SDL support |
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
| Input support | Win32/SDL | X11/SDL | Cocoa/SDL | X11/SDL |
| Browser | CEF/WebView2 | CEF | CEF | ❌ |
| SDL default | OFF | ON | ON | ON |

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
skia/out/
├── msvc.x64.release/
├── mingw64-gcc.x64.release/
├── mingw64-llvm.x64.release/
└── ...
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
1. Verify Skia is built: `ls skia/out/`
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
2. Check the framework configuration in `dui_bin_macos.cmake`
