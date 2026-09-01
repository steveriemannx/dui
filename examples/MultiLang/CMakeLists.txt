cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(MultiLang CXX)

# dui source root directory
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE) 

# Project source directory
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE) 

# Include the common implementation
include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
