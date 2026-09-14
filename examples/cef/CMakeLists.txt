cmake_minimum_required(VERSION 4.0)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(cef CXX)

# dui source root directory
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Use the CEF module. Both have to be set before the include below, because they decide
# which packaging path this example takes.
set(DUI_ENABLE_CEF ON)

# Name of the resource subdirectory under the active platform theme (macOS: macos26)
set(DUI_THEME_DIR_NAME "cef")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

set(_cef_sources
    CefForm.cpp
    main.cpp
)
if(APPLE)
    # macOS uses its own main (the CEF helper-process setup lives there); the previous
    # layer removed this file from its glob on every other platform.
    list(APPEND _cef_sources main_macos.mm)
endif()

if(DUI_OS_MACOS)
    # On this one path the application does not create its target. macOS CEF packaging
    # goes through cmake/dui_cef_macos.cmake, which is a retargeted copy of CEF's own
    # cefclient template: it creates the target, adds the helper app bundles and signs the
    # result, and it takes the application's sources from SRC_FILES. Call
    # dui_finalize_app() with no target there, and do not call add_executable.
    set(SRC_FILES ${_cef_sources})
    dui_finalize_app()
else()
    add_executable(cef ${_cef_sources})
    target_link_libraries(cef PRIVATE dui::app)
    dui_finalize_app(cef)
endif()
