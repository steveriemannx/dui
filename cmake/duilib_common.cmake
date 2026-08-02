# Operating system: Windows, Linux, macOS
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(DUILIB_OS_WINDOWS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DUILIB_OS_LINUX 1)
elseif(APPLE)
    set(DUILIB_OS_MACOS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(DUILIB_OS_FREEBSD 1)
    message(STATUS "Building for FreeBSD")
else()
    message(WARNING "Unknown OS: ${CMAKE_SYSTEM_NAME}")
endif()

# MinGW development environment
if(MINGW)
    set(DUILIB_MINGW 1)
    set(DUILIB_MINGW_NAME "mingw64-")
    
    # Use static linking for MinGW-w64 builds
    option(DUILIB_MINGW_STATIC "MINGW Static Link" ON)
    
endif()

# Define the switch variables (after changing them, clear the cmake build directory for them to take effect; otherwise the old cached values are used)
option(DUILIB_LOG "Print duilib debug log" OFF)

# Switch for the skia lib subdirectory name (by default Windows assembles the path by rules; other platforms can pin a fixed directory, e.g. the llvm build)
option(DUILIB_SKIA_LIB_SUBPATH "Skia lib sub path" OFF)

# Wayland support (optional on Linux; replaces SDL)
if(DUILIB_OS_LINUX)
    option(DUILIB_ENABLE_WAYLAND "Enable Wayland (alternative to SDL)" OFF)
endif()

# SDL support: off by default on Windows, on by default on other platforms
# If Wayland is enabled, SDL is disabled by default
if(DUILIB_OS_WINDOWS)
    option(DUILIB_ENABLE_SDL "Enable SDL" OFF)
else()
    if(DUILIB_ENABLE_WAYLAND)
        set(DUILIB_ENABLE_SDL_DEFAULT OFF)
    else()
        set(DUILIB_ENABLE_SDL_DEFAULT ON)
    endif()
    option(DUILIB_ENABLE_SDL "Enable SDL" ${DUILIB_ENABLE_SDL_DEFAULT})
endif()

# CEF support: off by default, only enabled by specific projects
option(DUILIB_ENABLE_CEF "Enable CEF" OFF)

# Whether to enable CEF 109 (off by default; CEF 109 supports Windows 7, while other CEF versions only run on Windows 10 and later)
option(DUILIB_CEF_109 "Enable CEF 109" OFF)

# WebView2 control binaries
if(DUILIB_OS_WINDOWS)
    option(DUILIB_WEBVIEW2_EXE "Is Windows WebView2 exe" OFF)
endif()

# Compiler type: msvc, gcc, llvm
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(DUILIB_COMPILER_MSVC 1)
    set(DUILIB_COMPILER_NAME "msvc")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(DUILIB_COMPILER_GCC 1)
    set(DUILIB_COMPILER_NAME "${DUILIB_MINGW_NAME}gcc")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(DUILIB_COMPILER_LLVM 1)
    set(DUILIB_COMPILER_NAME "${DUILIB_MINGW_NAME}llvm")
else()
    set(DUILIB_COMPILER_NAME "unknown")
    message(WARNING "Unknown CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
endif() 

# CPU type
# Linux/macOS: only 64-bit is supported; no need for 32-bit
# Windows: must support both 64-bit and 32-bit, as some users still use 32-bit systems
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" lower_processor)
# Match order matters: arm64/aarch64 must come first, otherwise "arm64" is misjudged as arm32 by the "arm" prefix match
if(lower_processor MATCHES "aarch64|arm64")
    set(DUILIB_SYSTEM_PROCESSOR "arm64")
elseif(lower_processor MATCHES "armv7|arm")
    set(DUILIB_SYSTEM_PROCESSOR "arm32")
elseif(lower_processor MATCHES "i[3-6]86")
    set(DUILIB_SYSTEM_PROCESSOR "x86")
elseif(lower_processor MATCHES "x86_64|amd64")
    set(DUILIB_SYSTEM_PROCESSOR "x64")
else()
    set(DUILIB_SYSTEM_PROCESSOR "unknown")
    message(WARNING "Unknown CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

# 64-bit or 32-bit
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DUILIB_BITS_64 1)
endif()

# Build type: Debug or Release
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DUILIB_BUILD_TYPE "debug")
else()
    set(DUILIB_BUILD_TYPE "release")
endif()

# duilib source root, library directory, and bin directory
get_filename_component(DUILIB_ROOT "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
set(DUILIB_LIB_PATH "${DUILIB_ROOT}/lib")
set(DUILIB_BIN_PATH "${DUILIB_ROOT}/bin")
set(DUILIB_LIBS duilib duilib-cximage duilib-webp duilib-png duilib-zlib)

# CEF module source root (the CEF module is optional)
if(DUILIB_ENABLE_CEF)
    if(DUILIB_OS_WINDOWS)
        # Windows platform
        if (DUILIB_CEF_109)
            # Use CEF 109
            set(DUILIB_CEF_SRC_ROOT_DIR "${DUILIB_ROOT}/third_party/libcef/libcef_win_109")
            if(DUILIB_BITS_64)
                set(DUILIB_CEF_LIB_PATH "${DUILIB_ROOT}/third_party/libcef/libcef_win_109/lib/x64")
            else()
                set(DUILIB_CEF_LIB_PATH "${DUILIB_ROOT}/third_party/libcef/libcef_win_109/lib/Win32")
            endif()
            set(DUILIB_CEF_WRAPPER_LIB_NAME libcef_dll_wrapper_109)
        else()
            # Use the latest CEF version
            set(DUILIB_CEF_SRC_ROOT_DIR "${DUILIB_ROOT}/third_party/libcef/libcef_win")
            if(DUILIB_BITS_64)
                set(DUILIB_CEF_LIB_PATH "${DUILIB_ROOT}/third_party/libcef/libcef_win/lib/x64")
            else()
                set(DUILIB_CEF_LIB_PATH "${DUILIB_ROOT}/third_party/libcef/libcef_win/lib/Win32")
            endif()
            set(DUILIB_CEF_WRAPPER_LIB_NAME libcef_dll_wrapper)
        endif()
    elseif(DUILIB_OS_LINUX)
        # Linux platform
        set(DUILIB_CEF_SRC_ROOT_DIR "${DUILIB_ROOT}/third_party/libcef/libcef_linux")
        set(DUILIB_CEF_LIB_PATH "${DUILIB_ROOT}/bin/libcef_linux")
        set(DUILIB_CEF_WRAPPER_LIB_NAME cef_dll_wrapper)
    elseif(DUILIB_OS_MACOS)
        # macOS platform
        set(DUILIB_CEF_SRC_ROOT_DIR "${DUILIB_ROOT}/third_party/libcef/libcef_macos")
        set(DUILIB_CEF_LIB_PATH "")
        # The CEF distribution's wrapper target is named libcef_dll_wrapper (libcef_dll/CMakeLists.txt)
        set(DUILIB_CEF_WRAPPER_LIB_NAME libcef_dll_wrapper)
    endif()
    if(DUILIB_OS_WINDOWS)
        set(DUILIB_CEF_LIBS libcef ${DUILIB_CEF_WRAPPER_LIB_NAME})
    elseif(DUILIB_OS_MACOS)
        set(DUILIB_CEF_LIBS ${DUILIB_CEF_WRAPPER_LIB_NAME})
    else()
        set(DUILIB_CEF_LIBS libcef.so ${DUILIB_CEF_WRAPPER_LIB_NAME} X11)
    endif()
endif()

# Skia source root and library directories (Skia is required)
get_filename_component(DUILIB_SKIA_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/skia/" ABSOLUTE)
if(DUILIB_SKIA_LIB_SUBPATH STREQUAL "" OR DUILIB_SKIA_LIB_SUBPATH STREQUAL "OFF")
    # Assemble the path by rules
    set(DUILIB_SKIA_LIB_PATH "${DUILIB_SKIA_SRC_ROOT_DIR}/out/${DUILIB_COMPILER_NAME}.${DUILIB_SYSTEM_PROCESSOR}.${DUILIB_BUILD_TYPE}")
else()
    # Externally specified subdirectory name
    set(DUILIB_SKIA_LIB_PATH "${DUILIB_SKIA_SRC_ROOT_DIR}/out/${DUILIB_SKIA_LIB_SUBPATH}")
endif()
set(DUILIB_SKIA_LIBS svg skshaper skottie sksg jsonreader skia)

# Build Skia from the vendored submodule at make time (see cmake/duilib_deps.cmake: duilib_skia target).
# ON: auto-build with gn + ninja; OFF: use a prebuilt Skia you provide yourself.
option(DUILIB_BUILD_SKIA_FROM_SOURCE "Build Skia from the vendored submodule (gn + ninja)" ON)

# Build SDL3 from the vendored submodule at make time (see cmake/duilib_deps.cmake: duilib_sdl target).
# ON: auto-build with cmake; OFF: use a prebuilt SDL3 you provide yourself.
option(DUILIB_BUILD_SDL_FROM_SOURCE "Build SDL3 from the vendored submodule" ON)

# SDL source root and library directories (optional on Windows, required on other platforms)
if(DUILIB_ENABLE_SDL)
    get_filename_component(DUILIB_SDL_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/SDL3/" ABSOLUTE)
    if(EXISTS "${DUILIB_SDL_SRC_ROOT_DIR}/lib64/")
        set(DUILIB_SDL_LIB_PATH "${DUILIB_SDL_SRC_ROOT_DIR}/lib64")
    elseif(EXISTS "${DUILIB_SDL_SRC_ROOT_DIR}/lib/")
        set(DUILIB_SDL_LIB_PATH "${DUILIB_SDL_SRC_ROOT_DIR}/lib")
    elseif(DUILIB_BUILD_SDL_FROM_SOURCE)
        # SDL3 built from the submodule at make time; installed into the build directory
        set(DUILIB_SDL_LIB_PATH "${CMAKE_BINARY_DIR}/sdl3-install/lib")
    else()
        set(DUILIB_SDL_LIB_PATH "${DUILIB_SDL_SRC_ROOT_DIR}/lib")
    endif()
    if(DUILIB_OS_WINDOWS AND NOT MINGW)
        set(DUILIB_SDL_LIBS SDL3-static.lib)
    else()
        set(DUILIB_SDL_LIBS SDL3)
    endif()
endif()

# Wayland library (optional on Linux; replaces SDL)
if(DUILIB_ENABLE_WAYLAND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
    pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
    pkg_check_modules(WAYLAND_CURSOR REQUIRED wayland-cursor)
    pkg_check_modules(WLROOTS REQUIRED wlroots-0.18)
    pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
    pkg_check_modules(EGL REQUIRED egl)
    pkg_check_modules(GLESV2 REQUIRED glesv2)
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
    
    set(DUILIB_WAYLAND_LIBS
        ${WAYLAND_CLIENT_LIBRARIES}
        ${WAYLAND_EGL_LIBRARIES}
        ${WAYLAND_CURSOR_LIBRARIES}
        ${WLROOTS_LIBRARIES}
        ${XKBCOMMON_LIBRARIES}
        ${EGL_LIBRARIES}
        ${GLESV2_LIBRARIES}
    )
    set(DUILIB_WAYLAND_INCLUDE_DIRS
        ${WAYLAND_CLIENT_INCLUDE_DIRS}
        ${WAYLAND_EGL_INCLUDE_DIRS}
        ${WAYLAND_CURSOR_INCLUDE_DIRS}
        ${WLROOTS_INCLUDE_DIRS}
        ${XKBCOMMON_INCLUDE_DIRS}
        ${EGL_INCLUDE_DIRS}
        ${GLESV2_INCLUDE_DIRS}
        ${WAYLAND_PROTOCOLS_INCLUDE_DIRS}
    )
endif()

# Output logs: print variable data
if(DUILIB_LOG)
    message(STATUS "DUILIB_PROJECT_SRC_DIR: ${DUILIB_PROJECT_SRC_DIR}")
    message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
    if(DUILIB_OS_WINDOWS)
        message(STATUS "DUILIB_WINRES_FILE_NAME: ${DUILIB_WINRES_FILE_NAME}")
    endif()
    message(STATUS "") 

    if(DUILIB_OS_WINDOWS)
        message(STATUS "DUILIB_OS: Windows")
    endif()
    if(DUILIB_OS_LINUX)
        message(STATUS "DUILIB_OS: Linux")
    endif()
    if(DUILIB_OS_MACOS)
        message(STATUS "DUILIB_OS: MacOS")
    endif()
    if(DUILIB_OS_FREEBSD)
        message(STATUS "DUILIB_OS: FreeBSD")
    endif()

    message(STATUS "DUILIB_COMPILER_NAME: ${DUILIB_COMPILER_NAME}")
    message(STATUS "DUILIB_SYSTEM_PROCESSOR: ${DUILIB_SYSTEM_PROCESSOR}")

    if(DUILIB_MINGW)
        message(STATUS "DUILIB_MINGW: ON")
        message(STATUS "DUILIB_MINGW_STATIC: ${DUILIB_MINGW_STATIC}")
    else()
        message(STATUS "DUILIB_MINGW: OFF")
    endif()

    if(DUILIB_BITS_64)
        message(STATUS "DUILIB_BITS_64: ON")
    else()
        message(STATUS "DUILIB_BITS_64: OFF")
    endif()

    message(STATUS "DUILIB_BUILD_TYPE: ${DUILIB_BUILD_TYPE}")
    
    if(MSVC)
        # MSVC compiler: print the C/C++ runtime library
        message(STATUS "CMAKE_MSVC_RUNTIME_LIBRARY: ${CMAKE_MSVC_RUNTIME_LIBRARY}") 
    endif()
    
    message(STATUS "") 
    
    message(STATUS "DUILIB_ROOT: ${DUILIB_ROOT}")
    message(STATUS "DUILIB_BIN_PATH: ${DUILIB_BIN_PATH}")
    message(STATUS "DUILIB_LIB_PATH: ${DUILIB_LIB_PATH}")    
    message(STATUS "DUILIB_LIBS: ${DUILIB_LIBS}") 
    message(STATUS "") 
    
    message(STATUS "DUILIB_SKIA_SRC_ROOT_DIR: ${DUILIB_SKIA_SRC_ROOT_DIR}")
    message(STATUS "DUILIB_SKIA_LIB_SUBPATH: ${DUILIB_SKIA_LIB_SUBPATH}")
    message(STATUS "DUILIB_SKIA_LIB_PATH: ${DUILIB_SKIA_LIB_PATH}")
    message(STATUS "DUILIB_SKIA_LIBS: ${DUILIB_SKIA_LIBS}")
    message(STATUS "") 
    
    message(STATUS "DUILIB_ENABLE_CEF: ${DUILIB_ENABLE_CEF}") 
    if (DUILIB_ENABLE_CEF) 
        message(STATUS "DUILIB_CEF_SRC_ROOT_DIR: ${DUILIB_CEF_SRC_ROOT_DIR}") 
        message(STATUS "DUILIB_CEF_LIB_PATH: ${DUILIB_CEF_LIB_PATH}") 
        message(STATUS "DUILIB_CEF_109: ${DUILIB_CEF_109}") 
        message(STATUS "DUILIB_CEF_LIBS: ${DUILIB_CEF_LIBS}")
    endif()
    message(STATUS "") 
    
    if(DUILIB_OS_WINDOWS)
        message(STATUS "DUILIB_WEBVIEW2_EXE: ${DUILIB_WEBVIEW2_EXE}") 
        message(STATUS "") 
    endif()
    
    message(STATUS "DUILIB_ENABLE_SDL: ${DUILIB_ENABLE_SDL}")
    if(DUILIB_ENABLE_SDL)        
        message(STATUS "DUILIB_SDL_SRC_ROOT_DIR: ${DUILIB_SDL_SRC_ROOT_DIR}")
        message(STATUS "DUILIB_SDL_LIB_PATH: ${DUILIB_SDL_LIB_PATH}")
        message(STATUS "DUILIB_SDL_LIBS: ${DUILIB_SDL_LIBS}")
    endif()
    message(STATUS "DUILIB_ENABLE_WAYLAND: ${DUILIB_ENABLE_WAYLAND}")
    if(DUILIB_ENABLE_WAYLAND)
        message(STATUS "DUILIB_WAYLAND_LIBS: ${DUILIB_WAYLAND_LIBS}")
        message(STATUS "DUILIB_WAYLAND_INCLUDE_DIRS: ${DUILIB_WAYLAND_INCLUDE_DIRS}")
    endif()
    message(STATUS "")
endif()

# Dependency management: Skia/SDL3 sources come from the git submodules and are built at make time;
# CEF is downloaded at configure time when missing. Both functions are idempotent (GLOBAL-property guarded).
include("${CMAKE_CURRENT_LIST_DIR}/duilib_deps.cmake")
duilib_deps_configure()
duilib_deps_add_targets()
