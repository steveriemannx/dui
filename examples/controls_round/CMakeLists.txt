cmake_minimum_required(VERSION 4.0)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define the project name and language
project(controls_round CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(controls_round
    AboutForm.cpp
    AnimationForm.cpp
    ControlForm.cpp
    TestForm.cpp
    main.cpp
)

# NativeMenuHelper.mm is the macOS native menu helper. See the note in
# examples/controls/CMakeLists.txt: Objective-C++ needs a frontend g++ lacks by default on
# Linux, and the previous layer did not compile this file there either.
if(NOT DUI_OS_LINUX AND NOT DUI_OS_FREEBSD)
    target_sources(controls_round PRIVATE NativeMenuHelper.mm)
endif()

target_link_libraries(controls_round PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(controls_round)
