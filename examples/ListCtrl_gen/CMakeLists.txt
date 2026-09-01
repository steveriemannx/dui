cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ListCtrl_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

if(APPLE)
    set(LISTCTRL_THEME macos26)
elseif(WIN32)
    set(LISTCTRL_THEME windows11)
else()
    set(LISTCTRL_THEME default)
endif()

# Mode 2 (XML-to-code generation): convert the active theme XML to C++.
# loading*.xml remains the ListCtrl control's internal runtime skin.
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${LISTCTRL_THEME}/list_ctrl/list_ctrl.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${LISTCTRL_THEME}/global.xml"
    "themes/${LISTCTRL_THEME}/public"
    "themes/${LISTCTRL_THEME}/list_ctrl"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
