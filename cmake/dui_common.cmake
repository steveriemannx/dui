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
    include_directories(/usr/local/include)
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

# Switch for the skia lib subdirectory name (by default Windows assembles the path by rules; other platforms can pin a fixed directory, e.g. the llvm build)
option(DUI_SKIA_LIB_SUBPATH "Skia lib sub path" OFF)

# Wayland support (optional on Linux; replaces SDL)
if(DUI_OS_LINUX)
    option(DUI_ENABLE_WAYLAND "Enable Wayland (alternative to SDL)" OFF)
endif()

# SDL support: off by default on Windows, on by default on other platforms
# If Wayland is enabled, SDL is disabled by default
if(DUI_OS_WINDOWS)
    option(DUI_ENABLE_SDL "Enable SDL" OFF)
else()
    if(DUI_ENABLE_WAYLAND)
        set(DUI_ENABLE_SDL_DEFAULT OFF)
    else()
        set(DUI_ENABLE_SDL_DEFAULT ON)
    endif()
    option(DUI_ENABLE_SDL "Enable SDL" ${DUI_ENABLE_SDL_DEFAULT})
endif()

# CEF support: off by default, only enabled by specific projects
option(DUI_ENABLE_CEF "Enable CEF" OFF)

# Whether to enable CEF 109 (off by default; CEF 109 supports Windows 7, while other CEF versions only run on Windows 10 and later)
option(DUI_CEF_109 "Enable CEF 109" OFF)

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
set(DUI_LIB_PATH "${DUI_ROOT}/lib")
set(DUI_BIN_PATH "${DUI_ROOT}/bin")
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
if(DUI_SKIA_LIB_SUBPATH STREQUAL "" OR DUI_SKIA_LIB_SUBPATH STREQUAL "OFF")
    if(DUI_MULTI_CONFIG)
        # Multi-config generator (VS / Xcode): build both debug and release Skia.
        # Generator expressions in the platform files select the right path per configuration.
        set(DUI_SKIA_LIB_PATH_DEBUG   "${DUI_SKIA_SRC_ROOT_DIR}/out/${DUI_COMPILER_NAME}.${DUI_SYSTEM_PROCESSOR}.debug")
        set(DUI_SKIA_LIB_PATH_RELEASE "${DUI_SKIA_SRC_ROOT_DIR}/out/${DUI_COMPILER_NAME}.${DUI_SYSTEM_PROCESSOR}.release")
        # Default for global link_directories / add_custom_target dependencies
        set(DUI_SKIA_LIB_PATH "${DUI_SKIA_LIB_PATH_RELEASE}")
    else()
        # Single-config generator: path reflects the build type
        set(DUI_SKIA_LIB_PATH "${DUI_SKIA_SRC_ROOT_DIR}/out/${DUI_COMPILER_NAME}.${DUI_SYSTEM_PROCESSOR}.${DUI_BUILD_TYPE}")
    endif()
else()
    # Externally specified subdirectory name (user manages build variants themselves)
    set(DUI_SKIA_LIB_PATH "${DUI_SKIA_SRC_ROOT_DIR}/out/${DUI_SKIA_LIB_SUBPATH}")
endif()
set(DUI_SKIA_LIBS svg skshaper skottie sksg jsonreader skia)

# Build Skia from the zip-downloaded source at make time (see cmake/dui_deps.cmake: dui_skia target).
# ON: auto-build with gn + ninja; OFF: use a prebuilt Skia you provide yourself.
option(DUI_BUILD_SKIA_FROM_SOURCE "Build Skia from the downloaded source (gn + ninja)" ON)

# Build SDL3 from the zip-downloaded source at make time (see cmake/dui_deps.cmake: dui_sdl target).
# ON: auto-build with cmake; OFF: use a prebuilt SDL3 you provide yourself.
option(DUI_BUILD_SDL_FROM_SOURCE "Build SDL3 from the downloaded source" ON)

# SDL source root and library directories (optional on Windows, required on other platforms)
if(DUI_ENABLE_SDL)
    get_filename_component(DUI_SDL_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/SDL3/" ABSOLUTE)
    if(EXISTS "${DUI_SDL_SRC_ROOT_DIR}/lib64/")
        set(DUI_SDL_LIB_PATH "${DUI_SDL_SRC_ROOT_DIR}/lib64")
    elseif(EXISTS "${DUI_SDL_SRC_ROOT_DIR}/lib/")
        set(DUI_SDL_LIB_PATH "${DUI_SDL_SRC_ROOT_DIR}/lib")
    elseif(DUI_BUILD_SDL_FROM_SOURCE)
        # SDL3 built from the fetched source at make time; installed into the build directory
        set(DUI_SDL_LIB_PATH "${CMAKE_BINARY_DIR}/sdl3-install/lib")
    else()
        set(DUI_SDL_LIB_PATH "${DUI_SDL_SRC_ROOT_DIR}/lib")
    endif()
    if(DUI_OS_WINDOWS AND NOT MINGW)
        set(DUI_SDL_LIBS SDL3-static.lib)
    else()
        set(DUI_SDL_LIBS SDL3)
    endif()
endif()

# Wayland library (optional on Linux; replaces SDL)
if(DUI_ENABLE_WAYLAND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
    pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
    pkg_check_modules(WAYLAND_CURSOR REQUIRED wayland-cursor)
    pkg_check_modules(WLROOTS REQUIRED wlroots-0.18)
    pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
    pkg_check_modules(EGL REQUIRED egl)
    pkg_check_modules(GLESV2 REQUIRED glesv2)
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
    
    set(DUI_WAYLAND_LIBS
        ${WAYLAND_CLIENT_LIBRARIES}
        ${WAYLAND_EGL_LIBRARIES}
        ${WAYLAND_CURSOR_LIBRARIES}
        ${WLROOTS_LIBRARIES}
        ${XKBCOMMON_LIBRARIES}
        ${EGL_LIBRARIES}
        ${GLESV2_LIBRARIES}
    )
    set(DUI_WAYLAND_INCLUDE_DIRS
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
    
    message(STATUS "DUI_ENABLE_SDL: ${DUI_ENABLE_SDL}")
    if(DUI_ENABLE_SDL)        
        message(STATUS "DUI_SDL_SRC_ROOT_DIR: ${DUI_SDL_SRC_ROOT_DIR}")
        message(STATUS "DUI_SDL_LIB_PATH: ${DUI_SDL_LIB_PATH}")
        message(STATUS "DUI_SDL_LIBS: ${DUI_SDL_LIBS}")
    endif()
    message(STATUS "DUI_ENABLE_WAYLAND: ${DUI_ENABLE_WAYLAND}")
    if(DUI_ENABLE_WAYLAND)
        message(STATUS "DUI_WAYLAND_LIBS: ${DUI_WAYLAND_LIBS}")
        message(STATUS "DUI_WAYLAND_INCLUDE_DIRS: ${DUI_WAYLAND_INCLUDE_DIRS}")
    endif()
    message(STATUS "")
endif()

# ---- Resource sync: keep the runtime resource tree in bin/ in sync with the repo-root
# resources/ directory (fonts/lang/themes) and generate the resources.zip archive.
# bin/ is build output and may be deleted at any time; configure re-creates it.
# Idempotent per configure run (GLOBAL-property guarded); safe to call per scope.
function(dui_sync_resources)
    get_property(_dui_res_synced GLOBAL PROPERTY DUI_RESOURCES_SYNCED)
    if(_dui_res_synced)
        return()
    endif()
    set_property(GLOBAL PROPERTY DUI_RESOURCES_SYNCED TRUE)

    set(_res_src "${DUI_ROOT}/resources")
    if(NOT EXISTS "${_res_src}/themes")
        return()  # resources/ not present (e.g. library-only build)
    endif()

    # 1. Generate resources.zip next to resources/ (zip with a "resources/" top-level folder,
    #    matching the runtime path convention used by ZipManager / the embedded-zip flow)
    set(_res_zip "${_res_src}/resources.zip")
    if(WIN32)
        # Use relative paths: libarchive's bsdtar may parse Windows
        # drive-letter paths (D:/...) as remote URLs, causing
        # "Cannot connect to D: resolve failed".
        execute_process(
            COMMAND tar -a -cf resources/resources.zip resources
            WORKING_DIRECTORY "${DUI_ROOT}"
            RESULT_VARIABLE _zip_result
        )
    else()
        execute_process(
            COMMAND zip -q -r "${_res_zip}" resources
            WORKING_DIRECTORY "${DUI_ROOT}"
            RESULT_VARIABLE _zip_result
        )
    endif()
    if(_zip_result EQUAL 0 AND EXISTS "${_res_zip}")
        message(STATUS "resources.zip: ${_res_zip}")
    else()
        message(WARNING "resources.zip creation failed; the zip resource mode will be unavailable")
    endif()

    # 2. Copy resources/ + resources.zip into bin/ (create bin/ if missing - it is build
    #    output and may have been deleted; configure must re-create the resource tree)
    file(MAKE_DIRECTORY "${DUI_BIN_PATH}/resources")
    file(COPY "${_res_src}/fonts" "${_res_src}/lang" "${_res_src}/themes"
         DESTINATION "${DUI_BIN_PATH}/resources")
    if(EXISTS "${_res_zip}")
        file(COPY "${_res_zip}" DESTINATION "${DUI_BIN_PATH}")
    endif()
    message(STATUS "Resources synced to ${DUI_BIN_PATH}")
endfunction()

# Dependency management: Skia/SDL3 sources are downloaded/extracted from zips at
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
        "\"${CMAKE_CXX_COMPILER}\" /nologo /std:c++17 /O2 /EHsc ${_inc_str} ${_srcs_str} /Fe:\"${_exe}\"\r\n")
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

