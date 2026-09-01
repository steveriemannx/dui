cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define project name and language
project(hello2 CXX)

# dui source root
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Project source directory
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Include the common implementation
include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# Self-contained example: copy the flat resources directory next to the
# executable. main.cpp loads from that local folder, so hello2 does not depend
# on the global dui resources tree at runtime.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${DUI_PROJECT_SRC_DIR}/resources"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/hello2_resources"
    COMMENT "Copying self-contained hello2 resources next to the executable")

if(DUI_OS_MACOS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${DUI_PROJECT_SRC_DIR}/resources"
                "${DUI_BIN_PATH}/${PROJECT_NAME}.app/Contents/MacOS/hello2_resources"
        COMMENT "Copying self-contained hello2 resources into the app bundle")
endif()
