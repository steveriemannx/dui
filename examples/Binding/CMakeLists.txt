cmake_minimum_required(VERSION 3.18)

# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

# Define project name and language
project(Binding CXX)

# dui source root
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

# Project source directory
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

# Include the common implementation
include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# The window UI is loaded from the shared theme resources
# (resources/themes/<platform-default>/binding/binding.xml, copied next to the
# executable by the global dui resource copy in dui_bin.cmake).

if(NOT DUI_MVVM_AVAILABLE)
    message(FATAL_ERROR
        "The Binding example needs the binding module. Configure with -DDUI_ENABLE_MVVM=ON "
        "(or leave DUI_BUILD_MVVM_EXAMPLES=ON, which compiles the module into the library).")
endif()
