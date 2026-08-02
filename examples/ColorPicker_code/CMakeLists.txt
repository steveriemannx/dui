cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ColorPicker_code CXX)

get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")

# 
# This example has no layout XML of its own (MainThread directly creates the library control ColorPicker).
# The ColorPicker control's internal skin color_picker.xml is a documented exception (library-internal skin asset) and continues to be read from disk.
# Therefore this example needs no generation step; it builds the same way as pure-code mode.

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")
