cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ChildWindow_code CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Embed the common resources and this example's child window skin.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
if(APPLE)
    set(CHILD_WINDOW_EMBED_THEME macos26)
elseif(WIN32)
    set(CHILD_WINDOW_EMBED_THEME windows11)
else()
    set(CHILD_WINDOW_EMBED_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${CHILD_WINDOW_EMBED_THEME}/global.xml"
    "themes/${CHILD_WINDOW_EMBED_THEME}/public"
    "themes/${CHILD_WINDOW_EMBED_THEME}/child_window"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# Path to the SDL library headers (ChildWindowPaint_SDL.cpp uses SDL headers directly)
if(DUI_ENABLE_SDL)
    include_directories("${DUI_SDL_SRC_ROOT_DIR}/include")
endif()
