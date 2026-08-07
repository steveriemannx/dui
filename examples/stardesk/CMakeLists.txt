cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(stardesk CXX)

# C++20 (declared explicitly so the project stays self-contained when moved
# out of the dui tree; dui_common.cmake also sets 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Sources are collected per directory (aux_source_directory); platform-specific
# implementations live in src/platform/<os> and are added to the build only on
# the matching OS (.mm files must not be compiled outside macOS).
set(DUI_SRC_SUB_DIRS src src/app src/net src/session src/codec src/platform src/filetx src/ui)
if(APPLE)
    list(APPEND DUI_SRC_SUB_DIRS src/platform/apple)
elseif(WIN32)
    list(APPEND DUI_SRC_SUB_DIRS src/platform/win)
else()
    list(APPEND DUI_SRC_SUB_DIRS src/platform/x11)
endif()

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# ---- StarDesk-specific additions (after the target is created) ----

# libpng headers for screen-frame encoding/decoding (libdui-png is already
# in the default link list; only the headers need to be exposed).
target_include_directories(${PROJECT_NAME} PRIVATE "${DUI_SRC_ROOT_DIR}/third_party/libpng")

if(APPLE)
    # ScreenCaptureKit (macOS 14+) + CoreMedia (CMTime) for screen capture,
    # CoreVideo (CVPixelBuffer) + QuartzCore (CACurrentMediaTime) for the
    # SCStream frame delivery path. Plain signature: dui_bin_macos.cmake
    # already linked this target plain.
    target_link_libraries(${PROJECT_NAME}
        "-framework ScreenCaptureKit" "-framework CoreMedia"
        "-framework CoreVideo" "-framework QuartzCore")
elseif(WIN32)
    # Winsock2
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
    # UI strings contain CJK/UTF-8 literals (8 built-in languages)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
elseif(UNIX AND NOT APPLE)
    # X11 screen capture + XTest input injection (X11 session only; Wayland unsupported)
    find_package(X11 REQUIRED)
    find_library(XTST_LIB Xtst)
    if(X11_FOUND AND XTST_LIB)
        target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES} ${XTST_LIB})
    else()
        message(WARNING "X11/Xtst not found - screen capture and input injection will be unavailable on this system")
    endif()
endif()
