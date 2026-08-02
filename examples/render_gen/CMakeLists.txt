cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(render_gen CXX)

get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")

# Mode 2 (XML-to-code generation): convert render.xml to pure C++ code at build time
# (with 13 <Include src="page_*.xml"/> expansions). The MenuBar menu is now a builder callback.
set(GEN_XML_FILES render.xml)
set(GEN_FUNC_NAME Init)
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_gen_code.cmake")

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
