cmake_minimum_required(VERSION 4.0)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define project name and language
project(Binding CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(Binding
    MainForm.cpp
    main.cpp
)

target_link_libraries(Binding PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(Binding)

# The window UI is loaded from the shared theme resources
# (resources/themes/<platform-default>/binding/binding.xml, copied next to the
# executable by the resource sync in dui_common.cmake).

if(NOT DUI_MVVM_AVAILABLE)
    message(FATAL_ERROR
        "The Binding example needs the binding module. Configure with -DDUI_ENABLE_MVVM=ON "
        "(or leave DUI_BUILD_MVVM_EXAMPLES=ON, which compiles the module into the library).")
endif()
