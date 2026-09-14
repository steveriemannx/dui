cmake_minimum_required(VERSION 4.0)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define project name and language
project(hello CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(hello
    MainForm.cpp
    main.cpp
)

target_link_libraries(hello PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(hello)

# The hello window UI is loaded from the shared theme resources
# (resources/themes/<platform-default>/hello/hello.xml, copied next to the
# executable by the resource sync in dui_common.cmake). No per-example XML copy
# is needed.
