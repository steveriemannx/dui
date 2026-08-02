cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(basic_code CXX)

get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
