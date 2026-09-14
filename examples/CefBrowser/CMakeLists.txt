cmake_minimum_required(VERSION 4.0)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(CefBrowser CXX)

# dui source root directory
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Use the CEF module. Both have to be set before the include below, because they decide
# which packaging path this example takes.
set(DUI_ENABLE_CEF ON)

# Name of the resource subdirectory under the active platform theme (macOS: macos26)
set(DUI_THEME_DIR_NAME "cef_browser")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# The sources the previous layer globbed out of DUI_SRC_SUB_DIRS, listed. That variable is
# gone with the layer that consumed it.
set(_cef_browser_sources
    browser/BrowserBox.cpp
    browser/BrowserForm.cpp
    browser/BrowserManager.cpp
    browser/DragDropManager.cpp
    browser/DragForm.cpp
    main.cpp
)
if(APPLE)
    list(APPEND _cef_browser_sources main_macos.mm)
endif()
if(DUI_OS_WINDOWS)
    list(APPEND _cef_browser_sources
        Windows/BrowserBox_Windows.cpp
        Windows/BrowserForm_Windows.cpp
        Windows/taskbar/DwmUtil.cpp
        Windows/taskbar/TaskbarManager.cpp
    )
endif()

if(DUI_OS_MACOS)
    # On this one path the application does not create its target; see the note in
    # examples/cef/CMakeLists.txt and cmake/dui_cef_macos.cmake.
    set(SRC_FILES ${_cef_browser_sources})
    dui_finalize_app()
else()
    add_executable(CefBrowser ${_cef_browser_sources})
    target_link_libraries(CefBrowser PRIVATE dui::app)
    dui_finalize_app(CefBrowser)
endif()
