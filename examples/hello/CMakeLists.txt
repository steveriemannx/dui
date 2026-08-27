cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define project name and language
project(hello CXX)

# dui source root
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Project source directory
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Name of the resource (*.rc) file; the default is "<ProjectName>.rc" and its encoding must be UTF-8
# The resource file must be in the source directory; only effective on Windows, ignored on other platforms
set(DUI_WINRES_FILE_NAME "${PROJECT_NAME}.rc")

# Include the common implementation
include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# hello.xml lives in this example folder, not in the shared resources tree.
# Copy it into the executable's resource directory after the global dui
# resource copy, so the skin loader can find themes/<SkinFolder>/<SkinFile>.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/themes/default/hello"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/hello.xml"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/themes/default/hello/hello.xml"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/themes/windows11/hello"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/hello.xml"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/themes/windows11/hello/hello.xml"
    COMMENT "Copying hello.xml next to the executable")
