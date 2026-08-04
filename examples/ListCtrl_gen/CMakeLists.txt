cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ListCtrl_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert list_ctrl.xml to pure C++ code at build time.
# Note: loading*.xml is the ListCtrl control's internal skin (documented exception) and continues to be read from disk.
set(GEN_XML_FILES list_ctrl.xml)
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
