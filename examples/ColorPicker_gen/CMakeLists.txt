cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ColorPicker_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# The ColorPicker window UI is loaded from embedded resources;
# the library-owned ColorPicker class manages its own XML skin internally.
# No standalone XML-to-code generation is needed (GEN_XML_FILES/GEN_FUNC_NAME
# are omitted) because the skin is private to the library.
include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Embed the platform theme resources used by the library-internal color picker skin.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
if(APPLE)
    set(COLORPICKER_THEME macos26)
elseif(WIN32)
    set(COLORPICKER_THEME windows11)
else()
    set(COLORPICKER_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${COLORPICKER_THEME}/global.xml"
    "themes/${COLORPICKER_THEME}/public"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
