cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(controls_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert the window layouts to pure C++ code at build time.
# The two menus (settings_menu.xml/tray_menu.xml) remain runtime menu resources.
if(APPLE)
    set(CONTROLS_THEME macos26)
elseif(WIN32)
    set(CONTROLS_THEME windows11)
else()
    set(CONTROLS_THEME default)
endif()
set(GEN_XML_FILES
    "${DUI_ROOT}/resources/themes/${CONTROLS_THEME}/controls/controls.xml"
    "${DUI_ROOT}/resources/themes/${CONTROLS_THEME}/controls/about.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
# Only embed the common resources (active theme's global.xml + public assets, plus fonts and
# lang) and this example's own skin (themes/<platform-theme>/controls), not the whole tree.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${CONTROLS_THEME}/global.xml"
    "themes/${CONTROLS_THEME}/public"
    "themes/${CONTROLS_THEME}/controls"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
