cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define project name and language
project(hello3 CXX)

# dui source root
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Project source directory
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Include the common implementation
include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# Copy only the flat resource files next to the executable.
file(GLOB HELLO3_RESOURCE_FILES CONFIGURE_DEPENDS
    "${DUI_PROJECT_SRC_DIR}/*.xml"
    "${DUI_PROJECT_SRC_DIR}/*.ttf"
    "${DUI_PROJECT_SRC_DIR}/*.svg"
    "${DUI_PROJECT_SRC_DIR}/*.png"
    "${DUI_PROJECT_SRC_DIR}/*.json"
    "${DUI_PROJECT_SRC_DIR}/*.txt"
    "${DUI_PROJECT_SRC_DIR}/*.ico"
    "${DUI_PROJECT_SRC_DIR}/*.cur")
foreach(RESOURCE_FILE IN LISTS HELLO3_RESOURCE_FILES)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${RESOURCE_FILE}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endforeach()

if(DUI_OS_MACOS)
    foreach(RESOURCE_FILE IN LISTS HELLO3_RESOURCE_FILES)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${RESOURCE_FILE}"
                    "${DUI_BIN_PATH}/${PROJECT_NAME}.app/Contents/MacOS")
    endforeach()
endif()
