cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define project
project(codeui CXX)

# duilib source root
get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Project source directory
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Include common build rules
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")

# Include binary build rules
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
