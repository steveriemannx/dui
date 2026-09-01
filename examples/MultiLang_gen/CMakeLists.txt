cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(MultiLang_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

if(APPLE)
    set(MULTILANG_THEME macos26)
elseif(WIN32)
    set(MULTILANG_THEME windows11)
else()
    set(MULTILANG_THEME default)
endif()
set(GEN_XML_FILES
    "${DUI_ROOT}/resources/themes/${MULTILANG_THEME}/MultiLang/MultiLang.xml"
    "${DUI_ROOT}/resources/themes/${MULTILANG_THEME}/MultiLang/lang_menu.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${MULTILANG_THEME}/global.xml"
    "themes/${MULTILANG_THEME}/public"
    "themes/${MULTILANG_THEME}/MultiLang"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
