cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(MoveControl_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

if(APPLE)
    set(MOVECONTROL_THEME macos26)
elseif(WIN32)
    set(MOVECONTROL_THEME windows11)
else()
    set(MOVECONTROL_THEME default)
endif()

# Mode 2 (XML-to-code generation): convert the active theme XML to C++.
# app_item.xml remains a code-built item template.
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${MOVECONTROL_THEME}/move_control/main.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources for gen mode.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${MOVECONTROL_THEME}/global.xml"
    "themes/${MOVECONTROL_THEME}/public"
    "themes/${MOVECONTROL_THEME}/move_control"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
