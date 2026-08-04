cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ColorPicker_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation) notes:
# This example has no layout XML of its own (MainThread directly creates the library control ColorPicker).
# The ColorPicker control's internal skin color_picker.xml is a documented exception (library-internal skin asset) and continues to be read from disk.
# Therefore this example needs no generation step; it builds the same way as pure-code mode.

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
