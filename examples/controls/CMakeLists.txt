cmake_minimum_required(VERSION 4.0)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(controls CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(controls
    AboutForm.cpp
    AnimationForm.cpp
    ControlForm.cpp
    TestForm.cpp
    main.cpp
)

# NativeMenuHelper.mm is the macOS native menu helper. Objective-C++ needs a compiler
# frontend that g++ does not have by default on Linux (cc1objplus) -- and the previous
# layer never compiled this file there either: it globbed *.mm only when the platform was
# neither Linux nor FreeBSD. The body is #if defined(__APPLE__)-guarded, but that does not
# help: the file still has to be parsed as Objective-C++.
if(NOT DUI_OS_LINUX AND NOT DUI_OS_FREEBSD)
    target_sources(controls PRIVATE NativeMenuHelper.mm)
endif()

target_link_libraries(controls PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(controls)
