cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(genlist CXX)

get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")

# Generate C++ UI code from main.xml at build time with auto image embedding
# main.xml -> InitMain() (window layout + embedded images from global.xml classes)
set(GEN_XML_FILES virtual_list_box/main.xml)
set(GEN_FUNC_NAME Init)
set(GEN_AUTO_EMBED ON)
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_gen_code.cmake")

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
