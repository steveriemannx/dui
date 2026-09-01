cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(XmlPreview_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert xml_preview.xml to pure C++ code at build time.
# Note: XmlBox loading a user-selected XML at runtime is the example's feature itself (not a layout).
if(APPLE)
    set(XP_THEME macos26)
elseif(WIN32)
    set(XP_THEME windows11)
else()
    set(XP_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${XP_THEME}/xml_preview/xml_preview.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${XP_THEME}/global.xml"
    "themes/${XP_THEME}/public"
    "themes/${XP_THEME}/xml_preview"
    "themes/${XP_THEME}/controls"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
