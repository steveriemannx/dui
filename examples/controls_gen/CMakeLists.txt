cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(controls_gen CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Mode 2 (XML-to-code generation): convert controls.xml to pure C++ code at build time.
# The layouts of AboutForm/TestForm/AnimationForm are small and are now hand-built in code;
# The two menus (settings_menu.xml/tray_menu.xml) are now pure-code menus.
set(GEN_XML_FILES controls.xml)
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
