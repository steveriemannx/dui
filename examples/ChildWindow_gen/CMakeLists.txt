cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ChildWindow_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert child_window.xml to pure C++ code at build time.
# The XML source follows the platform-default theme used by the runtime resources.
if(APPLE)
    set(CHILD_WINDOW_THEME macos26)
elseif(WIN32)
    set(CHILD_WINDOW_THEME windows11)
else()
    set(CHILD_WINDOW_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${CHILD_WINDOW_THEME}/child_window/child_window.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed the common resources and this example's child window skin.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${CHILD_WINDOW_THEME}/global.xml"
    "themes/${CHILD_WINDOW_THEME}/public"
    "themes/${CHILD_WINDOW_THEME}/child_window"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# Path to the SDL library headers (ChildWindowPaint_SDL.cpp uses SDL headers directly)
if(DUI_ENABLE_SDL)
    include_directories("${DUI_SDL_SRC_ROOT_DIR}/include")
endif()
