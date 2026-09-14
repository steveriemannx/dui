cmake_minimum_required(VERSION 4.0)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(WebView2 CXX)

# Define an executable that uses WebView2 (read by the Windows platform half of dui_app.cmake)
set(DUI_WEBVIEW2_EXE ON)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(WebView2
    MainForm.cpp
    main.cpp
)

target_link_libraries(WebView2 PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(WebView2)
