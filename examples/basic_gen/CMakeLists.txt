cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(basic_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert basic.xml to pure C++ code at build time (xml_to_code tool)
# XML is the design-time format: write/debug the UI in XML, generate code at build time, zero layout XML parsing at runtime.
# Note: keep GEN_AUTO_EMBED OFF (images are read from the resources directory on disk; embedding uses memfd and is Linux-only)
set(GEN_XML_FILES basic.xml)
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
