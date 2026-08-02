cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(cef CXX)

# duilib source root directory
get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE) 

# Project source directory
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE) 

# Name of the resource (*.rc) file; the default is "<ProjectName>.rc" and its encoding must be UTF-8
# The resource file must be in the source directory; only effective on Windows, ignored on other platforms
set(DUILIB_WINRES_FILE_NAME "${PROJECT_NAME}.rc")

# Use the CEF module
set(DUILIB_ENABLE_CEF ON)

# Name of the resource subdirectory (a subdirectory under resources/themes/default; used when macOS copies resources)
set(DUILIB_THEME_DIR_NAME "cef")

# Include the common implementation
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
