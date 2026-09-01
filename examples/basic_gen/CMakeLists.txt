cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(basic_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert basic.xml to pure C++ code at build time.
# The XML is the design-time format: write/debug the UI in XML, generate code at
# build time, zero layout XML parsing at runtime.
# The XML source follows the platform-default theme (same as the basic XML example,
# see GetDefaultThemePath): Windows -> windows11, macOS -> macos26, others -> default.
if(APPLE)
    set(BASIC_THEME macos26)
elseif(WIN32)
    set(BASIC_THEME windows11)
else()
    set(BASIC_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${BASIC_THEME}/basic/basic.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed only the platform theme (self-contained: macos26/public now carries all
# shared images, no default-theme fallback) plus this example's own skin
# (themes/<platform-theme>/basic). Always: fonts + lang.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${BASIC_THEME}/global.xml"
    "themes/${BASIC_THEME}/public"
    "themes/${BASIC_THEME}/basic"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
