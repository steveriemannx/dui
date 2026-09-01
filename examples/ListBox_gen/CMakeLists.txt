cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ListBox_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

if(APPLE)
    set(LISTBOX_EMBED_THEME macos26)
elseif(WIN32)
    set(LISTBOX_EMBED_THEME windows11)
else()
    set(LISTBOX_EMBED_THEME default)
endif()

# Mode 2 (XML-to-code generation): convert the active platform theme's XML to
# pure C++ code at build time. item.xml remains a code-built item template.
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${LISTBOX_EMBED_THEME}/list_box/list_box.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
# Only embed the common resources (active theme's global.xml + public assets, plus fonts and
# lang) and this example's own skin (themes/<platform-theme>/list_box), not the whole tree.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${LISTBOX_EMBED_THEME}/global.xml"
    "themes/${LISTBOX_EMBED_THEME}/public"
    "themes/${LISTBOX_EMBED_THEME}/list_box"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
