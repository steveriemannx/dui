# dui application build entry point.
#
# An application includes this file, owns its own target, links dui::app, and calls
# dui_finalize_app():
#
#     cmake_minimum_required(VERSION 4.0)
#     project(my_app CXX)
#     include("<dui>/cmake/dui_app.cmake")
#     add_executable(my_app main.cpp MainForm.cpp)
#     target_link_libraries(my_app PRIVATE dui::app)
#     dui_finalize_app(my_app)
#
# This replaces the previous contract, in which cmake/dui_bin.cmake created the
# executable itself and no application ever called add_executable. The split is forced by
# what CMake can and cannot express:
#
#   dui::app          an INTERFACE target, carrying everything that is a *usage
#                     requirement*: include directories, the C++ standard, compile
#                     definitions and options, link options, link directories, and the
#                     libraries. An application gets all of it by linking one target.
#   dui_finalize_app  a function, for the work that cannot travel through a target:
#                     RUNTIME_OUTPUT_DIRECTORY is not an interface property, a POST_BUILD
#                     step has no interface form at all, and add_dependencies has to name
#                     a concrete target. Doing it in a function also means it runs after
#                     the application has created its target, which is when these have to
#                     be set.
#
# The platform files are included below and provide the two halves for their OS.

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source code and run \"cmake -S ${CMAKE_SOURCE_DIR} -B .\" from there")
endif()

# Mandatory, and the reason this file exists as the single entry point: the documented
# snippet in cmake/README.md omitted it, which made the documented build impossible.
include("${CMAKE_CURRENT_LIST_DIR}/dui_common.cmake")

# dui::app carries cxx_std_20 for every application that links it. This directory-level
# setting is for the one target that does not: on the macOS CEF path the target is created
# by cmake/dui_cef_macos.cmake, which is CEF's own template. It is what the previous layer
# did for every application, so keeping it changes nothing for the others.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# cmake/dui_cef_macos.cmake reads this for its bundle resources (seven places), and it is
# CEF's own cefclient template, so the variable stays part of the interface. Applications
# do not set it any more -- it used to be one of the two variables every example had to
# compute by hand, and getting it wrong only showed up inside that one packaging path.
set(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

if(DUI_LOG)
    message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
    message(STATUS "C compiler  : ${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
    message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# Platform file: sets dui::app's usage requirements and defines
# dui_finalize_app_platform(). Included once -- the target below is created once, in the
# first scope that includes this file, and CMake targets and functions are global after
# that.
if(NOT TARGET dui_app)
    add_library(dui_app INTERFACE)
    add_library(dui::app ALIAS dui_app)

    # The public headers require C++20. dui carries this too, but an application
    # configured standalone links `dui` as a bare library name with no imported target,
    # so this is what guarantees the standard there.
    target_compile_features(dui_app INTERFACE cxx_std_20)

    target_include_directories(dui_app INTERFACE
        "$<BUILD_INTERFACE:${DUI_ROOT}>"
        "$<BUILD_INTERFACE:${DUI_ROOT}/include>"
    )

    if(DUI_OS_WINDOWS)
        include("${CMAKE_CURRENT_LIST_DIR}/dui_app_windows.cmake")
    elseif(DUI_OS_LINUX)
        include("${CMAKE_CURRENT_LIST_DIR}/dui_app_linux.cmake")
    elseif(DUI_OS_MACOS)
        include("${CMAKE_CURRENT_LIST_DIR}/dui_app_macos.cmake")
    elseif(DUI_OS_FREEBSD)
        include("${CMAKE_CURRENT_LIST_DIR}/dui_app_freebsd.cmake")
    else()
        message(FATAL_ERROR "Unknown OS!")
    endif()
endif()

# Per-target finishing work. Call it once, after the target exists and after any
# dui_add_gen_code() / dui_add_embed_res() call.
#
# On the macOS CEF path the target is created by cmake/dui_cef_macos.cmake, which is a
# retargeted copy of CEF's own cefclient template and is not ours to restructure -- call
# this with no argument there, and do not call add_executable.
function(dui_finalize_app)
    # Defaults to the project's own target, which is what every application uses.
    set(_dui_target "${PROJECT_NAME}")
    if(ARGN)
        set(_dui_target "${ARGN}")
    endif()

    # Everything below is per-target work that only the platform layer can do, including
    # whether the target exists at all: on the macOS CEF path the platform layer creates
    # it (see the note above), so the existence check lives there rather than here.
    dui_finalize_app_platform("${_dui_target}")
endfunction()
