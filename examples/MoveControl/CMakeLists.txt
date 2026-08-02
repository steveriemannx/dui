cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(MoveControl CXX)

# duilib source root directory
get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE) 

# Project source directory
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE) 

# Name of the resource (*.rc) file; the default is "<ProjectName>.rc" and its encoding must be UTF-8
# The resource file must be in the source directory; only effective on Windows, ignored on other platforms
set(DUILIB_WINRES_FILE_NAME "${PROJECT_NAME}.rc")

# Include the common implementation
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
