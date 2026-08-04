cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(CefBrowser CXX)

# dui source root directory
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE) 

# Project source directory
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE) 

# Name of the resource (*.rc) file; the default is "<ProjectName>.rc" and its encoding must be UTF-8
# The resource file must be in the source directory; only effective on Windows, ignored on other platforms
set(DUI_WINRES_FILE_NAME "${PROJECT_NAME}.rc")

# Use the CEF module
set(DUI_ENABLE_CEF ON)

# Include the common implementation
include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

# Define the subdirectories whose source files are enumerated
if(DUI_OS_WINDOWS)
    # Windows platform
    set(DUI_SRC_SUB_DIRS "browser" "Windows" "Windows/taskbar")
else()
    # Other platforms
    set(DUI_SRC_SUB_DIRS "browser")
endif()

# Name of the resource subdirectory (a subdirectory under resources/themes/default; used when macOS copies resources)
set(DUI_THEME_DIR_NAME "cef_browser")

# Include the common implementation
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")
