cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define project name and language
project(ColorPicker CXX)

# dui source root
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Project source directory
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Include the common implementation
include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# The ColorPicker window UI is loaded from the shared theme resources
# (the library-owned ColorPicker class manages its own XML skin internally).
# No per-example XML copy is needed.
