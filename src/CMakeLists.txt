cmake_minimum_required(VERSION 3.18)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source code and run \"cmake -S ${CMAKE_SOURCE_DIR} -B .\" from there")
endif()

# project() must run before dui_common.cmake: it populates CMAKE_SYSTEM_NAME,
# which dui_common.cmake uses to pick the platform and native window backend
# switch). Without it, a standalone "cmake -S src" configure saw an empty
# CMAKE_SYSTEM_NAME and wrongly evaluated the non-Windows branch.
project(dui CXX C)

# dui
# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
if("${DUI_MD}" STREQUAL "ON")
    # MD/MDd
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
else()
    # MT/MTd
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
# dui end

# Make this directory self-contained so it can also be configured standalone
# (scripts/*_build.sh --standalone). In the top-level build dui_common.cmake is
# already included; its execution blocks are GLOBAL-property guarded and idempotent.
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/dui_common.cmake")

if(MSVC)
    # Get the number of logical CPU cores
    cmake_host_system_information(RESULT CPU_CORES QUERY NUMBER_OF_LOGICAL_CORES)
endif()

set(CMAKE_CXX_STANDARD 20) # C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # C++20

# Set the libcef directory path
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")  # macOS
    set(CEF_LIB_DIR "cef_binary")
    set(DUI_SRC_MACOS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")  # Linux
    set(CEF_LIB_DIR "cef_binary")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows
    # 头文件目录与 Mac/Linux 一致取自 cef_binary（下载的发行包，含 include/）；
    # CEF 109 分支继续使用随仓库内建的 libcef_win_109（libcef_win 已删除）
    if(DUI_CEF_109)
        set(CEF_LIB_DIR "libcef_win_109")
    else()
        set(CEF_LIB_DIR "cef_binary")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")  # FreeBSD
    message(WARNING "CEF is not supported on FreeBSD - building without CEF support")
    set(CEF_LIB_DIR "")  # Empty or set to a dummy value
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Set source/dependency paths
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
get_filename_component(SKIA_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/skia/" ABSOLUTE)

# Wayland library detection
if(DUI_ENABLE_WAYLAND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
    pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
    pkg_check_modules(WAYLAND_CURSOR REQUIRED wayland-cursor)
    pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
    pkg_check_modules(EGL REQUIRED egl)
    pkg_check_modules(GLESV2 REQUIRED glesv2)
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
    set(DUI_WAYLAND_INCLUDE_DIRS
        ${WAYLAND_CLIENT_INCLUDE_DIRS}
        ${WAYLAND_EGL_INCLUDE_DIRS}
        ${WAYLAND_CURSOR_INCLUDE_DIRS}
        ${XKBCOMMON_INCLUDE_DIRS}
        ${EGL_INCLUDE_DIRS}
        ${GLESV2_INCLUDE_DIRS}
        ${WAYLAND_PROTOCOLS_INCLUDE_DIRS}
    )
    set(DUI_WAYLAND_LIBS
        ${WAYLAND_CLIENT_LIBRARIES}
        ${WAYLAND_EGL_LIBRARIES}
        ${WAYLAND_CURSOR_LIBRARIES}
        ${XKBCOMMON_LIBRARIES}
        ${EGL_LIBRARIES}
        ${GLESV2_LIBRARIES}
    )
    
    # Generate Wayland protocol headers
    find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)
    pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
    
    set(WAYLAND_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/wayland-generated-protocols")
    file(MAKE_DIRECTORY ${WAYLAND_GEN_DIR})
    
    # xdg-shell protocol
    set(XDG_SHELL_XML "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml")
    add_custom_command(
        OUTPUT "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.h"
        COMMAND ${WAYLAND_SCANNER} client-header ${XDG_SHELL_XML} "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.h"
        DEPENDS ${XDG_SHELL_XML}
        COMMENT "Generating xdg-shell-client-protocol.h"
    )
    add_custom_command(
        OUTPUT "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.c"
        COMMAND ${WAYLAND_SCANNER} private-code ${XDG_SHELL_XML} "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.c"
        DEPENDS ${XDG_SHELL_XML}
        COMMENT "Generating xdg-shell-client-protocol.c"
    )
    add_custom_target(generate_wayland_protocols DEPENDS
        "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.h"
        "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.c"
    )

    
    list(APPEND DUI_WAYLAND_INCLUDE_DIRS ${WAYLAND_GEN_DIR})
endif()

# Conditionally include CEF directory
if(CEF_LIB_DIR AND NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW)
    get_filename_component(CEF_INCLUDE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/libcef/${CEF_LIB_DIR}/" ABSOLUTE)
else()
    message(STATUS "CEF includes disabled - not supported on this platform")
endif()

# Add source code files. CONFIGURE_DEPENDS keeps new source files visible on the
# next configure without relying on directory-wide include/link settings.
file(GLOB DIR_LIB_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/*.cc"
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/*.mm"
)
file(GLOB ANIMATION_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Animation/*.[cm]*")
file(GLOB BOX_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Box/*.[cm]*")
file(GLOB LAYOUT_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Layout/*.[cm]*")
file(GLOB CONTROL_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Control/*.[cm]*")
file(GLOB CORE_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Core/*.[cm]*")
file(GLOB IMAGE_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Image/*.[cm]*")
file(GLOB RENDER_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Render/*.[cm]*")
file(GLOB RENDER_SKIA_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/*.[cm]*")
file(GLOB UTILS_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/Utils/*.[cm]*")

# Select the native backend source set. Platform files remain in the glob so
# Windows/macOS builds keep their native implementations, but Linux/FreeBSD
# must never pull Wayland-only sources into the X11 build (or vice versa).
if(DUI_ENABLE_WAYLAND)
    foreach(_native_src_list DIR_LIB_SRC ANIMATION_SRC BOX_SRC LAYOUT_SRC CONTROL_SRC CORE_SRC IMAGE_SRC RENDER_SRC RENDER_SKIA_SRC UTILS_SRC)
        list(FILTER ${_native_src_list} EXCLUDE REGEX ".*_X11\\..*")
    endforeach()
else()
    foreach(_native_src_list DIR_LIB_SRC ANIMATION_SRC BOX_SRC LAYOUT_SRC CONTROL_SRC CORE_SRC IMAGE_SRC RENDER_SRC RENDER_SKIA_SRC UTILS_SRC)
        list(FILTER ${_native_src_list} EXCLUDE REGEX ".*_Wayland\\..*")
    endforeach()
endif()

# Only include CEF sources on supported platforms (Windows, Linux, macOS)
# Wayland build excludes CEF; MinGW does not support CEF (see the build scripts)
if(DUI_ENABLE_WAYLAND OR NOT (DUI_ENABLE_CEF OR DUI_BUILD_CEF_EXAMPLES))
    message(STATUS "CEF support disabled")
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW)
    file(GLOB CEF_CONTROL_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/CEFControl/*.[cm]*")
    file(GLOB CEF_CONTROL_INTERNAL_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/*.[cm]*")
    list(APPEND PROJECT_SOURCES ${CEF_CONTROL_SRC} ${CEF_CONTROL_INTERNAL_SRC})
    message(STATUS "CEF support enabled (${CMAKE_SYSTEM_NAME})")
else()
    message(STATUS "CEF support disabled (FreeBSD/MinGW is not supported)")
endif()

# On non-macOS platforms, remove macOS-only source files to avoid compile errors.
if(NOT DUI_SRC_MACOS)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/ClickThrough_MacOS.mm)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/NativeWindow_MacOS.mm)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/MessageLoop_MacOS.mm)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/CursorManager_MacOS.mm)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/Keyboard_MacOS.mm)
    list(REMOVE_ITEM CEF_CONTROL_SRC ${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefWindowUtils_MacOS.mm)
    list(REMOVE_ITEM UTILS_SRC ${CMAKE_CURRENT_LIST_DIR}/Utils/ScreenCapture_MacOS.mm)
    list(REMOVE_ITEM UTILS_SRC ${CMAKE_CURRENT_LIST_DIR}/Utils/Clipboard_MacOS.mm)
    list(REMOVE_ITEM UTILS_SRC ${CMAKE_CURRENT_LIST_DIR}/Utils/FileDialog_MacOS.mm)
    list(REMOVE_ITEM UTILS_SRC ${CMAKE_CURRENT_LIST_DIR}/Utils/MonitorUtil_MacOS.mm)
    list(REMOVE_ITEM UTILS_SRC ${CMAKE_CURRENT_LIST_DIR}/Utils/SystemUtil_MacOS.mm)
    list(REMOVE_ITEM RENDER_SKIA_SRC ${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Render_Skia_MacOS.mm)
    list(REMOVE_ITEM RENDER_SKIA_SRC ${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkGLWindowContext_MacOS.mm)
    list(REMOVE_ITEM RENDER_SKIA_SRC ${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkMetalWindowContext_MacOS.mm)
    list(REMOVE_ITEM RENDER_SKIA_SRC ${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkRasterWindowContext_MacOS.mm)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND NOT MINGW)  # Windows CEF internals (not on MinGW)
    file(GLOB CEF_CONTROL_INTERNAL_WINDOWS_SRC CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/Windows/*.[cm]*")
endif()

file(GLOB APNG_SRC CONFIGURE_DEPENDS "${DUI_SRC_ROOT_DIR}/third_party/apng/*.[cm]*")
file(GLOB STB_SRC CONFIGURE_DEPENDS "${DUI_SRC_ROOT_DIR}/third_party/stb_image/*.[cm]*")
file(GLOB SVG_SRC CONFIGURE_DEPENDS "${DUI_SRC_ROOT_DIR}/third_party/svg/*.[cm]*")
file(GLOB UTF_SRC CONFIGURE_DEPENDS "${DUI_SRC_ROOT_DIR}/third_party/convert_utf/*.[cm]*")
file(GLOB XML_SRC CONFIGURE_DEPENDS "${DUI_SRC_ROOT_DIR}/third_party/xml/*.[cm]*")

set(GIFLIB_HDRS
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_hash.h
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_lib.h
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_lib_private.h
)

set(GIFLIB_SRCS
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/dgif_lib.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/egif_lib.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_err.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_font.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_hash.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gifalloc.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/openbsd-reallocarray.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/quantize.c
)

set(SKIA_HDRS
    ${SKIA_SRC_ROOT_DIR}/tools/window/DisplayParams.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/RasterWindowContext.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/WindowContext.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.h
)

set(SKIA_SRCS
    ${SKIA_SRC_ROOT_DIR}/tools/window/WindowContext.cpp
    ${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp
    ${SKIA_SRC_ROOT_DIR}/tools/window/unix/RasterWindowContext_unix.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS: the Metal window-context base (requires the Metal-enabled libskia.a)
    list(APPEND SKIA_HDRS ${SKIA_SRC_ROOT_DIR}/tools/window/MetalWindowContext.h)
    list(APPEND SKIA_SRCS ${SKIA_SRC_ROOT_DIR}/tools/window/MetalWindowContext.mm)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows platform: several skia source files depended on separately
    set(SKIA_WGL_SRCS
        ${SKIA_SRC_ROOT_DIR}/tools/ganesh/gl/win/SkWGL.h
        ${SKIA_SRC_ROOT_DIR}/tools/ganesh/gl/win/SkWGL_win.cpp
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows platform: source files depended on separately
    set(UDIS_HDRS 
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/decode.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/extern.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/itab.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/syn.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/types.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/udint.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/udis86.h
    )
    set(UDIS_SRCS 
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/udis86.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/syn-att.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/syn-intel.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/syn.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/itab.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/decode.c
    )
endif()

# Define the base sources (always included)
set(LIBRARY_SOURCES
    ${DIR_LIB_SRC}
    ${ANIMATION_SRC}
    ${BOX_SRC}
    ${LAYOUT_SRC}    
    ${CONTROL_SRC}
    ${CORE_SRC}
    ${IMAGE_SRC}
    ${RENDER_SRC}
    ${RENDER_SKIA_SRC}
    ${UTILS_SRC}
    ${APNG_SRC}
    ${STB_SRC}
    ${SVG_SRC}
    ${UTF_SRC}
    ${XML_SRC}
    ${GIFLIB_HDRS}
    ${GIFLIB_SRCS}
    ${SKIA_HDRS}
    ${SKIA_SRCS}
    ${SKIA_WGL_SRCS}
    ${UDIS_HDRS}
    ${UDIS_SRCS}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows, WebView2
    aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/WebView2 WEBVIEW2_CONTROL_SRC)
    list(APPEND LIBRARY_SOURCES
        ${WEBVIEW2_CONTROL_SRC}
    )
endif()

# Only include CEF sources on supported platforms (Windows, Linux, macOS)
# Wayland build excludes CEF; MinGW does not support CEF (see the build scripts)
if(DUI_ENABLE_WAYLAND)
    message(STATUS "CEF support disabled (Wayland build)")
elseif(NOT (DUI_ENABLE_CEF OR DUI_BUILD_CEF_EXAMPLES))
    message(STATUS "CEF support disabled")
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW)
    list(APPEND LIBRARY_SOURCES
        ${CEF_CONTROL_SRC}
        ${CEF_CONTROL_INTERNAL_SRC}
        ${CEF_CONTROL_INTERNAL_WINDOWS_SRC}
    )
    message(STATUS "CEF support enabled (${CMAKE_SYSTEM_NAME})")
else()
    message(STATUS "CEF support disabled (FreeBSD is not supported)")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_package(Freetype REQUIRED)
    find_package(Fontconfig REQUIRED)
    find_package(X11 REQUIRED)
    # Add generated protocol source before creating library
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
    target_link_libraries(${PROJECT_NAME} Freetype::Freetype Fontconfig::Fontconfig)
    target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
elseif(DUI_ENABLE_WAYLAND)
    list(APPEND LIBRARY_SOURCES "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.c")
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
    add_dependencies(${PROJECT_NAME} generate_wayland_protocols)
    target_link_libraries(${PROJECT_NAME} ${DUI_WAYLAND_LIBS})
else()
    # Create the library with the filtered sources
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
    find_package(X11 REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
    find_package(Freetype REQUIRED)
    find_package(Fontconfig REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} Freetype::Freetype Fontconfig::Fontconfig)
endif()

if(DUI_MULTI_CONFIG)
    foreach(_config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${_config}" _config_upper)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_${_config_upper} "${DUI_LIB_PATH}/${_config}")
    endforeach()
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${DUI_LIB_PATH}")
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${DUI_SRC_ROOT_DIR}
        ${DUI_SRC_ROOT_DIR}/include
        ${SKIA_SRC_ROOT_DIR}
        ${DUI_WAYLAND_INCLUDE_DIRS}
        ${CEF_INCLUDE_ROOT_DIR}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE SK_GANESH SK_GL SK_RELEASE)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        "/utf-8"
        $<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>
        $<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>
    )
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()
if(DUI_ENABLE_WAYLAND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WAYLAND=1)
elseif(DUI_OS_LINUX OR DUI_OS_FREEBSD)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_X11=1)
endif()

# Optional Qt-like platform entry shim.
# Applications that write a normal C++ main() on Windows can link dui_entry;
# it provides WinMain/wWinMain and forwards to main().
add_library(dui_entry STATIC "${CMAKE_CURRENT_SOURCE_DIR}/Entry/EntryPoint_main.cpp")

# Build-order wiring: the Skia archive built by cmake/dui_deps.cmake at make time
# must exist before this library links (dui_bin*.cmake links them into the executables).
if(TARGET dui_skia)
    add_dependencies(${PROJECT_NAME} dui_skia)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows, WebView2
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WEBVIEW2=1)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_USE_WEBVIEW2_LOADER_DLL=1)
endif()

# MinGW doesn't support CEF. Disable it for the library explicitly: without
# DUI_CEF=0, dui_config.h defaults CEF to enabled on Windows (undefined = ON),
# which breaks MinGW builds where the CEF headers are unavailable.
# MSVC / Linux / macOS always compile the CEF code in the library.
if(DUI_MINGW)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_CEF=0)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Skia automatically enables compile options such as AVX/AVX2; dui references some skia sources directly and must keep the compile options consistent
    include(${DUI_SRC_ROOT_DIR}/cmake/dui_compiles.cmake)

    # Compiler arguments
    set(DUI_COMPILER_FLAGS
        -fno-strict-aliasing            # Avoid assumptions regarding non-aliasing of objects of different types
        -fstack-protector               # Protect some vulnerable functions from stack-smashing (security feature)
        -funwind-tables                 # Support stack unwinding for backtrace()
        -fvisibility=hidden             # Give hidden visibility to declarations that are not explicitly marked as visible
        -Wall                           # Enable all warnings
#        -Werror                         # Treat warnings as errors
        -Wno-unknown-pragmas
        -Wextra                         # Enable additional warnings
        -Wendif-labels                  # Warn whenever an #else or an #endif is followed by text
        -Wno-missing-field-initializers # Don't warn about missing field initializers
        -Wno-unused-parameter           # Don't warn about unused parameters
    )
    set(DUI_CXX_COMPILER_FLAGS
        -fno-threadsafe-statics         # Don't generate thread-safe statics
        -fvisibility-inlines-hidden     # Give hidden visibility to inlined class member functions
        -frtti
        -Wno-narrowing                  # Don't warn about type narrowing
        -Wsign-compare                  # Warn about mixed signed/unsigned type comparisons
        -Wno-unused-variable
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        list(APPEND DUI_CXX_COMPILER_FLAGS
            -Wno-reorder
            -Wno-cast-function-type
          )
          set_source_files_properties("${DUI_SRC_ROOT_DIR}/third_party/libudis86/decode.c" PROPERTIES COMPILE_OPTIONS "-Wno-implicit-fallthrough")
          if(DUI_HAVE_AVX2)
            set_source_files_properties("${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp" PROPERTIES COMPILE_OPTIONS "-mavx2")
          elseif(DUI_HAVE_AVX)
            set_source_files_properties("${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp" PROPERTIES COMPILE_OPTIONS "-mavx")
          endif()
    endif()
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND DUI_COMPILER_FLAGS
            -Wnewline-eof                   # Warn about no newline at end of file
          )
        list(APPEND DUI_CXX_COMPILER_FLAGS
            -fobjc-call-cxx-cdtors          # Call the constructor/destructor of C++ instance variables in ObjC objects
            -Wno-reorder-ctor
            -Wno-unused-lambda-capture
            -Wno-unused-private-field
          )
    endif()
    
    target_compile_options(${PROJECT_NAME} PRIVATE ${DUI_COMPILER_FLAGS})
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: ${DUI_CXX_COMPILER_FLAGS}>)
endif()

# Third-party libraries are included in the build (following the develop2 branch: third_party/CMakeLists.txt)
# skia/native backend are not built here; keep them prebuilt in sibling directories
# The project owns the public install layout below; suppress vendored projects'
# independent install rules (libpng otherwise assumes its original output name).
set(SKIP_INSTALL_ALL ON)
add_subdirectory(../third_party third_party)

# Ensure third-party libraries build before dui (dui sources reference some third-party headers)
add_dependencies(${PROJECT_NAME} dui-cximage png_static dui-zlib dui-webp)
