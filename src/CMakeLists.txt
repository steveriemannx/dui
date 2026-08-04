cmake_minimum_required(VERSION 3.18)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source code and run \"cmake -S ${CMAKE_SOURCE_DIR} -B .\" from there")
endif()

# Make this directory self-contained so it can also be configured standalone
# (scripts/*_build.sh --standalone). In the top-level build dui_common.cmake is
# already included; its execution blocks are GLOBAL-property guarded and idempotent.
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/dui_common.cmake")

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

project(dui CXX C)

if(MSVC)
    # Source files use UTF-8 encoding
    add_compile_options("/utf-8")
    
    # Get the number of logical CPU cores
    cmake_host_system_information(RESULT CPU_CORES QUERY NUMBER_OF_LOGICAL_CORES)
    # Set C/C++ compile options (enable multi-threaded compilation)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>)
endif()

set(CMAKE_CXX_STANDARD 20) # C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # C++20

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Skia build options
add_definitions(-DSK_GANESH -DSK_GL -DSK_RELEASE)

# Set the libcef directory path
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")  # macOS
    set(CEF_LIB_DIR "libcef_macos")
    set(DUI_SRC_MACOS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")  # Linux
    set(CEF_LIB_DIR "libcef_linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows
    set(CEF_LIB_DIR "libcef_win")
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")  # FreeBSD
    message(WARNING "CEF is not supported on FreeBSD - building without CEF support")
    set(CEF_LIB_DIR "")  # Empty or set to a dummy value
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Set include directories
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
get_filename_component(SKIA_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/skia/" ABSOLUTE)
get_filename_component(SDL_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/SDL3/" ABSOLUTE)

# Wayland option (Linux only, alternative to SDL)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    option(DUI_ENABLE_WAYLAND "Enable Wayland (alternative to SDL)" OFF)
endif()

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

    
    include_directories(${WAYLAND_GEN_DIR})
    list(APPEND DUI_WAYLAND_INCLUDE_DIRS ${WAYLAND_GEN_DIR})
endif()

# Always include these directories
include_directories(${DUI_SRC_ROOT_DIR})
include_directories(${DUI_SRC_ROOT_DIR}/include)
include_directories(${SKIA_SRC_ROOT_DIR})
if(DUI_ENABLE_WAYLAND)
    include_directories(${DUI_WAYLAND_INCLUDE_DIRS})
else()
    include_directories("${SDL_SRC_ROOT_DIR}/include")
endif()

# Conditionally include CEF directory
if(CEF_LIB_DIR AND NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    get_filename_component(CEF_INCLUDE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/libcef/${CEF_LIB_DIR}/" ABSOLUTE)
    include_directories(${CEF_INCLUDE_ROOT_DIR})
else()
    message(STATUS "CEF includes disabled - not supported on this platform")
endif()

# Set the library output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${DUI_SRC_ROOT_DIR}/lib/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${DUI_SRC_ROOT_DIR}/lib/")

# Add source code files
aux_source_directory(${CMAKE_CURRENT_LIST_DIR} DIR_LIB_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Animation ANIMATION_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Box BOX_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Layout LAYOUT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Control CONTROL_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Core CORE_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Image IMAGE_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Render RENDER_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/RenderSkia RENDER_SKIA_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Utils UTILS_SRC)

# Only include CEF sources on supported platforms (Windows, Linux, macOS)
# Wayland build excludes CEF
if(DUI_ENABLE_WAYLAND)
    message(STATUS "CEF support disabled (Wayland build)")
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/CEFControl CEF_CONTROL_SRC)
    aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal CEF_CONTROL_INTERNAL_SRC)
    list(APPEND PROJECT_SOURCES ${CEF_CONTROL_SRC} ${CEF_CONTROL_INTERNAL_SRC})
    message(STATUS "CEF support enabled (${CMAKE_SYSTEM_NAME})")
else()
    message(STATUS "CEF support disabled (FreeBSD is not supported)")
endif()

# On non-macOS platforms, remove *.mm files to avoid compile errors
if(NOT DUI_SRC_MACOS)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/SDL_MacOS.mm)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/SDL_MacOS.h)    
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/ClickThrough_MacOS.mm)    
    list(REMOVE_ITEM CEF_CONTROL_SRC ${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefWindowUtils_MacOS.mm)    
    list(REMOVE_ITEM UTILS_SRC ${CMAKE_CURRENT_LIST_DIR}/Utils/ScreenCapture_MacOS.mm)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows
    aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/Windows CEF_CONTROL_INTERNAL_WINDOWS_SRC)
endif()

aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/apng APNG_SRC)
aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/stb_image STB_SRC)
aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/svg SVG_SRC)
aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/convert_utf UTF_SRC)
aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/xml XML_SRC)

set(UNZIP_HDRS
    ${DUI_SRC_ROOT_DIR}/third_party/zlib/contrib/minizip/ioapi.h
    ${DUI_SRC_ROOT_DIR}/third_party/zlib/contrib/minizip/unzip.h
)

set(UNZIP_SRCS
    ${DUI_SRC_ROOT_DIR}/third_party/zlib/contrib/minizip/ioapi.c
    ${DUI_SRC_ROOT_DIR}/third_party/zlib/contrib/minizip/unzip.c
)

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
)

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
    ${UNZIP_HDRS}
    ${UNZIP_SRCS}
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
# Wayland build excludes CEF
if(DUI_ENABLE_WAYLAND)
    message(STATUS "CEF support disabled (Wayland build)")
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
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
endif()

# Build-order wiring: the SDL3/Skia archives (built by cmake/dui_deps.cmake at make time)
# must exist before this library links (dui_bin*.cmake links them into the executables).
# SDL3 is wired first so it builds before Skia (faster feedback on SDL issues).
if(TARGET dui_sdl)
    add_dependencies(${PROJECT_NAME} dui_sdl)
endif()
if(TARGET dui_skia)
    add_dependencies(${PROJECT_NAME} dui_skia)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(DUI_ENABLE_WAYLAND)
        # Enable Wayland support for Linux (alternative to SDL)
        target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WAYLAND=1)
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(DUI_ENABLE_SDL)
        # Enable SDL support for windows
        target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_SDL=1)
    endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows, WebView2
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WEBVIEW2=1)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_USE_WEBVIEW2_LOADER_DLL=1)
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
# skia/SDL are not built here; keep them prebuilt in sibling directories
add_subdirectory(../third_party third_party)

# Ensure third-party libraries build before dui (dui sources reference some third-party headers)
add_dependencies(${PROJECT_NAME} dui-cximage png_static dui-zlib dui-webp)

