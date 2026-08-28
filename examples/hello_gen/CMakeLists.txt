cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(hello_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert hello.xml to pure C++ code at build time.
# The XML is the design-time format: write/debug the UI in XML, generate code at
# build time, zero layout XML parsing at runtime.
# The XML source lives in the hello example's folder (../hello/hello.xml),
# kept as the single source of truth for the hello example's three modes.
set(GEN_XML_FILES ../hello/hello.xml)
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed bin/resources into the executable as a custom binary archive (Qt qrc style, no zip)
set(EMBED_RES_DIR "${DUI_ROOT}/bin/resources")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
