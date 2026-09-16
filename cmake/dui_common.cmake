# Operating system: Windows, Linux, macOS
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(DUI_OS_WINDOWS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DUI_OS_LINUX 1)
elseif(APPLE)
    set(DUI_OS_MACOS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(DUI_OS_FREEBSD 1)
    # FreeBSD pkg/ports install headers and libs under /usr/local. Neither the
    # compiler nor CMake's find_* modules reliably search that prefix by default
    # (e.g. FindX11 looks under /usr/X11R6/include and /usr/include), so add it
    # explicitly: this covers <X11/Xlib.h>, <fontconfig/fontconfig.h>, ... for the
    # library and every example, regardless of the compiler's default search path.
    set(CMAKE_INCLUDE_PATH "/usr/local/include" ${CMAKE_INCLUDE_PATH})
    # The include path itself is no longer directory-scoped here: it is set on the dui
    # target (src/CMakeLists.txt) and on dui::app (cmake/dui_app_freebsd.cmake). A
    # directory-scoped include reached the third_party subprojects too, where the system
    # expat.h shadows Skia's bundled copy.
    # dui_common.cmake is included once per example scope (each example configures
    # its own project()), so announce the platform only once instead of every time.
    get_property(_dui_os_announced GLOBAL PROPERTY DUI_OS_ANNOUNCED)
    if(NOT _dui_os_announced)
        set_property(GLOBAL PROPERTY DUI_OS_ANNOUNCED TRUE)
        message(STATUS "Building for FreeBSD")
    endif()
else()
    message(WARNING "Unknown OS: ${CMAKE_SYSTEM_NAME}")
endif()

# MinGW development environment
if(MINGW)
    set(DUI_MINGW 1)
    set(DUI_MINGW_NAME "mingw64-")
    
    # Use static linking for MinGW-w64 builds
    option(DUI_MINGW_STATIC "MINGW Static Link" ON)
    
endif()

# Define the switch variables (after changing them, clear the cmake build directory for them to take effect; otherwise the old cached values are used)
option(DUI_LOG "Print dui debug log" OFF)

# New examples use a normal C++ main() (Qt-like).  The dui_entry shim provides
# the Windows WinMain/wWinMain entry and forwards to main().  Old macro-based
# examples can still set this to OFF if they define their own entry.
if(NOT DEFINED DUI_USE_MAIN_ENTRY)
    set(DUI_USE_MAIN_ENTRY ON)
endif()

# Switch for the skia lib subdirectory name (by default Windows assembles the path by rules; other platforms can pin a fixed directory, e.g. the llvm build)
option(DUI_SKIA_LIB_SUBPATH "Skia lib sub path" OFF)

# Linux and FreeBSD select the native backend from the desktop session unless
# the caller explicitly provides DUI_ENABLE_WAYLAND. With no session to ask, each
# platform falls back to the desktop stack it actually ships -- Wayland on FreeBSD,
# X11 on Linux -- so a configure over ssh does not silently pick the other one.
if(DUI_OS_LINUX OR DUI_OS_FREEBSD)
    # -Wl,--no-as-needed is set on the targets that need it rather than directory-scoped:
    # on the dui target for in-tree consumers, and on dui::app for standalone application
    # builds that link dui by bare name.
    if(NOT DEFINED DUI_ENABLE_WAYLAND)
        # The session decides when it says anything. Wayland is tested first because a
        # Wayland session that runs XWayland exports DISPLAY as well, so a DISPLAY-only
        # test would misread it. With no session to ask -- ssh, CI, a bare tty -- the
        # platform's own desktop stack decides: FreeBSD's is Wayland, Linux's is still
        # usually X11. This is the case a plain `cmake -S . -B build` over ssh lands in,
        # which is why it cannot simply default to X11.
        if("$ENV{XDG_SESSION_TYPE}" STREQUAL "wayland" OR DEFINED ENV{WAYLAND_DISPLAY})
            set(DUI_ENABLE_WAYLAND_DEFAULT ON)
            set(_dui_backend_reason "the session is Wayland")
        elseif("$ENV{XDG_SESSION_TYPE}" STREQUAL "x11" OR DEFINED ENV{DISPLAY})
            set(DUI_ENABLE_WAYLAND_DEFAULT OFF)
            set(_dui_backend_reason "the session is X11")
        elseif(DUI_OS_FREEBSD)
            set(DUI_ENABLE_WAYLAND_DEFAULT ON)
            set(_dui_backend_reason "there is no session, and FreeBSD's desktop stack is Wayland")
        else()
            set(DUI_ENABLE_WAYLAND_DEFAULT OFF)
            set(_dui_backend_reason "there is no session, and Linux defaults to X11")
        endif()
        set(DUI_ENABLE_WAYLAND "${DUI_ENABLE_WAYLAND_DEFAULT}" CACHE BOOL
            "Enable the native Wayland backend (auto-detected from the desktop session)")
        if(DUI_ENABLE_WAYLAND)
            message(STATUS "Enabling the native Wayland backend: ${_dui_backend_reason}")
        else()
            message(STATUS "Enabling the native X11 backend: ${_dui_backend_reason}")
        endif()
    else()
        set(DUI_ENABLE_WAYLAND "${DUI_ENABLE_WAYLAND}" CACHE BOOL
            "Enable the native Wayland backend (explicit override)")
        message(STATUS "Native backend explicitly selected: ${DUI_ENABLE_WAYLAND}")
    endif()
endif()

# SDL3 window and input backend. It replaces the per-platform native backends
# rather than sitting beside them, so it is off unless asked for.
option(DUI_ENABLE_SDL "Use the SDL3 window and input backend" OFF)

# Which theme the SDL build ships as its default. The themes are complete
# trees, so switching this switches the whole look; the name is what
# GetDefaultThemePath() returns on this build.
set(DUI_SDL_THEME "default" CACHE STRING "Theme name the SDL build uses")


# This build ships one theme (themes/default); the platform themes the native
# backends were dressed by are not in the tree.
if(DUI_ENABLE_SDL)
    set(DUI_EXAMPLE_THEME default CACHE STRING "Theme used by SDL examples" FORCE)
elseif(DUI_OS_LINUX)
    set(DUI_EXAMPLE_THEME gnome CACHE STRING "Theme used by Linux examples" FORCE)
elseif(DUI_OS_FREEBSD)
    set(DUI_EXAMPLE_THEME freebsd CACHE STRING "Theme used by FreeBSD examples" FORCE)
elseif(DUI_OS_WINDOWS)
    set(DUI_EXAMPLE_THEME windows11 CACHE STRING "Theme used by Windows examples" FORCE)
elseif(DUI_OS_MACOS)
    set(DUI_EXAMPLE_THEME macos26 CACHE STRING "Theme used by macOS examples" FORCE)
else()
    set(DUI_EXAMPLE_THEME default CACHE STRING "Theme used by examples" FORCE)
endif()

# CEF support: off by default, only enabled by specific projects
option(DUI_ENABLE_CEF "Enable CEF" OFF)

# Whether to enable CEF 109 (off by default; CEF 109 supports Windows 7, while other CEF versions only run on Windows 10 and later)
option(DUI_CEF_109 "Enable CEF 109" OFF)

# MVVM data-binding module. Purely additive: it adds new files under src/Binding
# and include/dui/Binding and modifies no existing library source, so enabling it
# cannot change the behavior of code that does not call it.
# The options keep the "MVVM" name (the feature people ask for); the code --
# namespace ui::binding, src/Binding, include/dui/Binding -- is named after the
# mechanism, since the binding engine also serves a plain MVC-style model.
#   DUI_ENABLE_MVVM         - compile the module into the dui library
#   DUI_BUILD_MVVM_EXAMPLES - build the binding demo examples
# The library is compiled once, at root scope; an example cannot switch the module
# on for the library, so src/CMakeLists.txt builds it when EITHER option is on.
# This mirrors DUI_ENABLE_CEF / DUI_BUILD_CEF_EXAMPLES.
option(DUI_ENABLE_MVVM "Enable the MVVM data-binding module" OFF)
option(DUI_BUILD_MVVM_EXAMPLES "Build the Mvvm examples" ON)

# Derived: true when the MVVM module is actually compiled into the dui library.
# Consumers (examples, tests) should gate on this rather than on DUI_ENABLE_MVVM,
# so they stay in sync with what src/CMakeLists.txt actually builds.
if(DUI_ENABLE_MVVM OR DUI_BUILD_MVVM_EXAMPLES)
    set(DUI_MVVM_AVAILABLE ON)
else()
    set(DUI_MVVM_AVAILABLE OFF)
endif()

# WebView2 control binaries
if(DUI_OS_WINDOWS)
    option(DUI_WEBVIEW2_EXE "Is Windows WebView2 exe" OFF)
endif()

# Compiler type: msvc, gcc, llvm
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(DUI_COMPILER_MSVC 1)
    set(DUI_COMPILER_NAME "msvc")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(DUI_COMPILER_GCC 1)
    set(DUI_COMPILER_NAME "${DUI_MINGW_NAME}gcc")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(DUI_COMPILER_LLVM 1)
    set(DUI_COMPILER_NAME "${DUI_MINGW_NAME}llvm")
else()
    set(DUI_COMPILER_NAME "unknown")
    message(WARNING "Unknown CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
endif() 

# ISO C++20 instead of the compiler's default dialect (gnu++20 with GCC/Clang).
# Nothing in the project needs a GNU extension: every library and example
# translation unit the macOS configuration builds (240 + 182 files) compiles clean
# under -std=c++20, and the full library plus the examples were then built and
# tested with it. MSVC -- the other supported toolchain -- has no GNU extensions at
# all, so leaving the default on would only hide portability mistakes until someone
# tries a Windows build. Set here rather than next to CMAKE_CXX_STANDARD
# (src/CMakeLists.txt, cmake/dui_app.cmake) because dui_common.cmake is the one
# module the library, the tests and the examples all include, so one line covers all
# three. Pass -DCMAKE_CXX_EXTENSIONS=ON to get the GNU dialect back.
if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# AddressSanitizer + UndefinedBehaviorSanitizer (development builds only, off by default).
# The flags are applied to the dui target as PUBLIC compile/link options in
# src/CMakeLists.txt, so every consumer of the library (tests, examples) is
# instrumented along with it. Instrumenting the library alone would leave the
# calling code unchecked and would mix instrumented and uninstrumented
# allocations in one process, which ASan does not support.
option(DUI_ENABLE_SANITIZERS "Build dui with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

# CPU type
# Linux/macOS: only 64-bit is supported; no need for 32-bit
# Windows: must support both 64-bit and 32-bit, as some users still use 32-bit systems
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" lower_processor)
# Match order matters: arm64/aarch64 must come first, otherwise "arm64" is misjudged as arm32 by the "arm" prefix match
if(lower_processor MATCHES "aarch64|arm64")
    set(DUI_SYSTEM_PROCESSOR "arm64")
elseif(lower_processor MATCHES "armv7|arm")
    set(DUI_SYSTEM_PROCESSOR "arm32")
elseif(lower_processor MATCHES "i[3-6]86")
    set(DUI_SYSTEM_PROCESSOR "x86")
elseif(lower_processor MATCHES "x86_64|amd64")
    set(DUI_SYSTEM_PROCESSOR "x64")
else()
    set(DUI_SYSTEM_PROCESSOR "unknown")
    message(WARNING "Unknown CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

# 64-bit or 32-bit
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DUI_BITS_64 1)
endif()

# Build type: Debug or Release
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DUI_BUILD_TYPE "debug")
else()
    set(DUI_BUILD_TYPE "release")
endif()

# Multi-config generator detection (Visual Studio, Xcode).
# These generators do NOT set CMAKE_BUILD_TYPE at configure time — the user picks
# Debug/Release at build time. We must build both Skia variants so that the chosen
# configuration links against a CRT-compatible library.
if(CMAKE_CONFIGURATION_TYPES)
    set(DUI_MULTI_CONFIG TRUE)
else()
    set(DUI_MULTI_CONFIG FALSE)
endif()

# dui source root, library directory, and bin directory
get_filename_component(DUI_ROOT "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
set(DUI_LIB_PATH "${CMAKE_BINARY_DIR}/lib")
set(DUI_BIN_PATH "${CMAKE_BINARY_DIR}/bin")
set(DUI_LIBS dui dui-cximage dui-webp dui-png dui-zlib)

# CEF module source root (the CEF module is optional)
if(DUI_ENABLE_CEF)
    if(DUI_OS_WINDOWS)
        # Windows platform (same layout as Linux: the binary distribution lands in cef_binary/)
        set(DUI_CEF_SRC_ROOT_DIR "${DUI_ROOT}/third_party/libcef/cef_binary")
        set(DUI_CEF_LIB_PATH "${DUI_ROOT}/third_party/libcef/cef_binary/Release")
        if(DUI_MULTI_CONFIG)
            set(DUI_CEF_LIB_PATH_DEBUG   "${DUI_ROOT}/third_party/libcef/cef_binary/Debug")
            set(DUI_CEF_LIB_PATH_RELEASE "${DUI_ROOT}/third_party/libcef/cef_binary/Release")
        endif()
        if (DUI_CEF_109)
            set(DUI_CEF_WRAPPER_LIB_NAME libcef_dll_wrapper_109)
        else()
            set(DUI_CEF_WRAPPER_LIB_NAME libcef_dll_wrapper)
        endif()
    elseif(DUI_OS_LINUX)
        # Linux platform
        set(DUI_CEF_SRC_ROOT_DIR "${DUI_ROOT}/third_party/libcef/cef_binary")
        set(DUI_CEF_LIB_PATH "${DUI_ROOT}/third_party/libcef/cef_binary/Release")
        set(DUI_CEF_WRAPPER_LIB_NAME libcef_dll_wrapper)
    elseif(DUI_OS_MACOS)
        # macOS platform
        set(DUI_CEF_SRC_ROOT_DIR "${DUI_ROOT}/third_party/libcef/cef_binary")
        set(DUI_CEF_LIB_PATH "")
        # The CEF distribution's wrapper target is named libcef_dll_wrapper (libcef_dll/CMakeLists.txt)
        set(DUI_CEF_WRAPPER_LIB_NAME libcef_dll_wrapper)
    endif()
    if(DUI_OS_WINDOWS)
        set(DUI_CEF_LIBS libcef ${DUI_CEF_WRAPPER_LIB_NAME})
    elseif(DUI_OS_MACOS)
        set(DUI_CEF_LIBS ${DUI_CEF_WRAPPER_LIB_NAME})
    else()
        set(DUI_CEF_LIBS libcef.so ${DUI_CEF_WRAPPER_LIB_NAME} X11)
    endif()
endif()

# Skia source root and library directories (Skia is required)
get_filename_component(DUI_SKIA_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/skia/" ABSOLUTE)
if(DUI_MULTI_CONFIG)
    # Multi-config generator (VS / Xcode): Skia libs live in the same build/lib
    # per-config folders as the rest of the dui libraries, with no extra prefix.
    set(DUI_SKIA_LIB_PATH_DEBUG   "${DUI_LIB_PATH}/Debug")
    set(DUI_SKIA_LIB_PATH_RELEASE "${DUI_LIB_PATH}/Release")
    # Default for global link_directories / add_custom_target dependencies
    set(DUI_SKIA_LIB_PATH "${DUI_SKIA_LIB_PATH_RELEASE}")
else()
    # Single-config generator: path reflects the build type.
    # DUI_SKIA_LIB_SUBPATH is accepted for backward compatibility, but Skia output
    # no longer gets a separate variant prefix directory.
    set(DUI_SKIA_LIB_PATH "${DUI_LIB_PATH}/${DUI_BUILD_TYPE}")
endif()
# An instrumented dui must not link the uninstrumented Skia. SkASAN.h defines
# SK_SANITIZE_ADDRESS whenever a translation unit is compiled with -fsanitize=address,
# and SkTArray has a member under it, so the two disagree about the layout of every
# type containing a TArray. Measured: SkSVGSVG is 848 bytes in dui's translation units
# and 840 in Skia's, and reading a member of one makes ASan report heap-buffer-overflow
# inside an object Skia allocated -- an artifact of the mismatch, not a defect.
# A separate directory makes the two structurally unable to mix, and means switching
# the option on an existing tree builds Skia again instead of silently reusing what is
# already there. The cost is a full Skia build per sanitizer tree.
if(DUI_ENABLE_SANITIZERS)
    string(APPEND DUI_SKIA_LIB_PATH "-asan")
    if(DEFINED DUI_SKIA_LIB_PATH_DEBUG)
        string(APPEND DUI_SKIA_LIB_PATH_DEBUG "-asan")
    endif()
    if(DEFINED DUI_SKIA_LIB_PATH_RELEASE)
        string(APPEND DUI_SKIA_LIB_PATH_RELEASE "-asan")
    endif()
endif()

set(DUI_SKIA_LIBS svg skshaper skottie sksg jsonreader skia)

# Build Skia from the zip-downloaded source at make time (see cmake/dui_deps.cmake: dui_skia target).
# ON: auto-build with gn + ninja; OFF: use a prebuilt Skia you provide yourself.
option(DUI_BUILD_SKIA_FROM_SOURCE "Build Skia from the downloaded source (gn + ninja)" ON)

# Native Wayland libraries (optional on Linux and FreeBSD).
if(DUI_ENABLE_WAYLAND)
    find_package(PkgConfig REQUIRED)
    # IMPORTED_TARGET, not the bare form: __LIBRARIES holds bare -l names, and the -L
    # search path that makes those names resolvable lives in __LIBRARY_DIRS -- which
    # nothing links, so it never reaches the link line. That is invisible on Linux,
    # where the stack sits in a default ld search directory, and fatal on FreeBSD,
    # where it sits in /usr/local/lib and "ld: unable to find library -lwayland-client"
    # is the result. The imported target carries name and path together, so linking it
    # is enough: no consumer has to know where the distribution put its libraries.
    pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)
    pkg_check_modules(WAYLAND_EGL REQUIRED IMPORTED_TARGET wayland-egl)
    pkg_check_modules(WAYLAND_CURSOR REQUIRED IMPORTED_TARGET wayland-cursor)
    pkg_check_modules(XKBCOMMON REQUIRED IMPORTED_TARGET xkbcommon)
    pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)
    pkg_check_modules(GLESV2 REQUIRED IMPORTED_TARGET glesv2)
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED IMPORTED_TARGET wayland-protocols)

    set(DUI_WAYLAND_LIBS
        PkgConfig::WAYLAND_CLIENT
        PkgConfig::WAYLAND_EGL
        PkgConfig::WAYLAND_CURSOR
        PkgConfig::XKBCOMMON
        PkgConfig::EGL
        PkgConfig::GLESV2
    )
    set(DUI_WAYLAND_INCLUDE_DIRS
        ${WAYLAND_CLIENT_INCLUDE_DIRS}
        ${WAYLAND_EGL_INCLUDE_DIRS}
        ${WAYLAND_CURSOR_INCLUDE_DIRS}
        ${XKBCOMMON_INCLUDE_DIRS}
        ${EGL_INCLUDE_DIRS}
        ${GLESV2_INCLUDE_DIRS}
        ${WAYLAND_PROTOCOLS_INCLUDE_DIRS}
    )

    # epoll-shim: the Wayland message loop is written against the Linux epoll/eventfd
    # API. FreeBSD has eventfd in base, but epoll only via libepoll-shim (the same
    # library the linuxulator uses). Prefer the shim over a kqueue rewrite so the
    # event-loop code stays identical across platforms.
    #
    # Looked up here rather than in cmake/dui_app_freebsd.cmake because
    # Core/MessageLoop_Wayland.cpp is compiled into libdui.a: the shim is a link
    # dependency of the library, and applications are not the only targets that link
    # the library -- tests link it directly and never see dui::app, which exists only
    # once an example includes cmake/dui_app.cmake.
    if(DUI_OS_FREEBSD)
        find_path(EPOLL_SHIM_INCLUDE_DIR
            NAMES sys/epoll.h
            HINTS /usr/local/include/libepoll-shim
            REQUIRED)
        find_library(EPOLL_SHIM_LIBRARY
            NAMES epoll-shim
            HINTS /usr/local/lib
            REQUIRED)
    endif()
endif()

# Output logs: print variable data
if(DUI_LOG)
    message(STATUS "DUI_PROJECT_SRC_DIR: ${DUI_PROJECT_SRC_DIR}")
    message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
    if(DUI_OS_WINDOWS)
        message(STATUS "DUI_WINRES_FILE_NAME: ${DUI_WINRES_FILE_NAME}")
    endif()
    message(STATUS "") 

    if(DUI_OS_WINDOWS)
        message(STATUS "DUI_OS: Windows")
    endif()
    if(DUI_OS_LINUX)
        message(STATUS "DUI_OS: Linux")
    endif()
    if(DUI_OS_MACOS)
        message(STATUS "DUI_OS: MacOS")
    endif()
    if(DUI_OS_FREEBSD)
        message(STATUS "DUI_OS: FreeBSD")
    endif()

    message(STATUS "DUI_COMPILER_NAME: ${DUI_COMPILER_NAME}")
    message(STATUS "DUI_SYSTEM_PROCESSOR: ${DUI_SYSTEM_PROCESSOR}")

    if(DUI_MINGW)
        message(STATUS "DUI_MINGW: ON")
        message(STATUS "DUI_MINGW_STATIC: ${DUI_MINGW_STATIC}")
    else()
        message(STATUS "DUI_MINGW: OFF")
    endif()

    if(DUI_BITS_64)
        message(STATUS "DUI_BITS_64: ON")
    else()
        message(STATUS "DUI_BITS_64: OFF")
    endif()

    message(STATUS "DUI_BUILD_TYPE: ${DUI_BUILD_TYPE}")
    
    if(MSVC)
        # MSVC compiler: print the C/C++ runtime library
        message(STATUS "CMAKE_MSVC_RUNTIME_LIBRARY: ${CMAKE_MSVC_RUNTIME_LIBRARY}") 
    endif()
    
    message(STATUS "") 
    
    message(STATUS "DUI_ROOT: ${DUI_ROOT}")
    message(STATUS "DUI_BIN_PATH: ${DUI_BIN_PATH}")
    message(STATUS "DUI_LIB_PATH: ${DUI_LIB_PATH}")    
    message(STATUS "DUI_LIBS: ${DUI_LIBS}") 
    message(STATUS "") 
    
    message(STATUS "DUI_SKIA_SRC_ROOT_DIR: ${DUI_SKIA_SRC_ROOT_DIR}")
    message(STATUS "DUI_SKIA_LIB_SUBPATH: ${DUI_SKIA_LIB_SUBPATH}")
    message(STATUS "DUI_SKIA_LIB_PATH: ${DUI_SKIA_LIB_PATH}")
    message(STATUS "DUI_SKIA_LIBS: ${DUI_SKIA_LIBS}")
    message(STATUS "") 
    
    message(STATUS "DUI_ENABLE_CEF: ${DUI_ENABLE_CEF}") 
    if (DUI_ENABLE_CEF) 
        message(STATUS "DUI_CEF_SRC_ROOT_DIR: ${DUI_CEF_SRC_ROOT_DIR}") 
        message(STATUS "DUI_CEF_LIB_PATH: ${DUI_CEF_LIB_PATH}") 
        message(STATUS "DUI_CEF_109: ${DUI_CEF_109}") 
        message(STATUS "DUI_CEF_LIBS: ${DUI_CEF_LIBS}")
    endif()
    message(STATUS "") 
    
    if(DUI_OS_WINDOWS)
        message(STATUS "DUI_WEBVIEW2_EXE: ${DUI_WEBVIEW2_EXE}") 
        message(STATUS "") 
    endif()
    
    message(STATUS "DUI_ENABLE_WAYLAND: ${DUI_ENABLE_WAYLAND}")
    if(DUI_ENABLE_WAYLAND)
        message(STATUS "DUI_WAYLAND_LIBS: ${DUI_WAYLAND_LIBS}")
        message(STATUS "DUI_WAYLAND_INCLUDE_DIRS: ${DUI_WAYLAND_INCLUDE_DIRS}")
    endif()
    message(STATUS "")
endif()

# ---- Copy resources once per configured output configuration.
function(dui_sync_resources)
    if(NOT EXISTS "${DUI_ROOT}/resources")
        message(WARNING "Resource directory not found: ${DUI_ROOT}/resources")
        return()
    endif()

    if(DUI_OS_MACOS)
        message(STATUS "Resources are linked into macOS app bundles")
        return()
    endif()

    if(DUI_MULTI_CONFIG)
        foreach(_dui_config Debug Release)
            file(COPY "${DUI_ROOT}/resources"
                 DESTINATION "${DUI_BIN_PATH}/${_dui_config}")
        endforeach()
        message(STATUS "Copied resources to ${DUI_BIN_PATH}/Debug and ${DUI_BIN_PATH}/Release")
    else()
        file(COPY "${DUI_ROOT}/resources" DESTINATION "${DUI_BIN_PATH}")
        message(STATUS "Copied resources to ${DUI_BIN_PATH}")
    endif()
endfunction()

# Dependency management: Skia sources are downloaded/extracted from zips at
# configure time when missing (see dui_deps.cmake) and are built at make time;
# CEF is downloaded at configure time when missing.
# Both functions are idempotent (GLOBAL-property guarded).
# ---- Locate the VS installation's vcvarsall.bat and the matching arch ----
# Sets ${result_var} to the vcvarsall path and ${arch_var} to x86/x64/arm64;
# both are empty when the VS installation cannot be found.
function(dui_find_vcvarsall _result_var _arch_var)
    set(${_result_var} "" PARENT_SCOPE)
    set(${_arch_var} "" PARENT_SCOPE)
    set(_vswhere "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe")
    if(NOT EXISTS "${_vswhere}")
        return()
    endif()
    execute_process(
        COMMAND "${_vswhere}" -latest -property installationPath
        OUTPUT_VARIABLE _vs_install
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _vswhere_result
    )
    if(NOT _vswhere_result EQUAL 0 OR NOT _vs_install)
        return()
    endif()
    set(_vcvarsall "${_vs_install}/VC/Auxiliary/Build/vcvarsall.bat")
    if(NOT EXISTS "${_vcvarsall}")
        return()
    endif()
    if(CMAKE_VS_PLATFORM_NAME STREQUAL "Win32")
        set(_vc_arch x86)
    elseif(CMAKE_VS_PLATFORM_NAME STREQUAL "ARM64")
        set(_vc_arch arm64)
    else()
        set(_vc_arch x64)
    endif()
    set(${_result_var} "${_vcvarsall}" PARENT_SCOPE)
    set(${_arch_var} "${_vc_arch}" PARENT_SCOPE)
endfunction()

# ---- Rebuild a small MSVC command-line tool at configure time ----
# cl.exe needs the vcvarsall environment (INCLUDE/LIB) to find the standard library
# headers (iostream, stddef.h, ...), so locate the VS installation with vswhere and
# run the compile inside it. Sets ${result_var} to TRUE on success.
#   result_var    - output variable name (TRUE/FALSE) in the caller's scope
#   srcs          - source files to compile (list)
#   exe           - output executable path
#   include_dirs  - extra -I flags (list), may be empty
function(dui_build_msvc_tool _result_var _srcs _exe _include_dirs)
    set(${_result_var} FALSE PARENT_SCOPE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # clang-cl / clang++ / g++: the compiler finds its own standard headers
        execute_process(
            COMMAND "${CMAKE_CXX_COMPILER}" -std=c++17 -O2
                    ${_include_dirs} ${_srcs} -o "${_exe}"
            WORKING_DIRECTORY "${DUI_SRC_ROOT_DIR}"
            RESULT_VARIABLE _build_result
        )
        if(_build_result EQUAL 0)
            set(${_result_var} TRUE PARENT_SCOPE)
        endif()
        return()
    endif()
    # MSVC: cl needs the vcvarsall environment (INCLUDE/LIB) to find the standard
    # library headers, so locate the VS installation with vswhere and run the
    # compile through a small batch file - a cmd /c "call ... && cl ..." one-liner
    # breaks because CMake re-escapes the embedded quotes for CreateProcess and cmd
    # does not understand backslash-escaped quotes.
    dui_find_vcvarsall(_vcvarsall _vc_arch)
    if(NOT _vcvarsall)
        message(WARNING "vcvarsall not found - cannot rebuild ${_exe} automatically")
        return()
    endif()
    string(REPLACE ";" " " _srcs_str "${_srcs}")
    string(REPLACE ";" " " _inc_str "${_include_dirs}")
    set(_bat "${CMAKE_CURRENT_BINARY_DIR}/dui_rebuild_tool.bat")
    file(WRITE "${_bat}"
        "@call \"${_vcvarsall}\" ${_vc_arch} >nul\r\n"
        "\"${CMAKE_CXX_COMPILER}\" /nologo /std:c++17 /O2 /EHsc ${_inc_str} ${_srcs_str} /Fe:\"${_exe}\" /Fo\"${CMAKE_CURRENT_BINARY_DIR}/\"\r\n")
    execute_process(
        COMMAND cmd /c "${_bat}"
        WORKING_DIRECTORY "${DUI_SRC_ROOT_DIR}"
        RESULT_VARIABLE _build_result
        OUTPUT_VARIABLE _build_out
        ERROR_VARIABLE _build_err
    )
    if(_build_result EQUAL 0)
        set(${_result_var} TRUE PARENT_SCOPE)
    else()
        message(WARNING "Tool build failed (${_exe}):\n${_build_err}\n${_build_out}")
    endif()
endfunction()
include("${CMAKE_CURRENT_LIST_DIR}/dui_deps.cmake")
dui_deps_configure()
dui_deps_add_targets()
dui_sync_resources()
