cmake_minimum_required(VERSION 4.0)

# Before project(): on FreeBSD and macOS the make generators run on bmake rather
# than GNU make. See the file for why, and for -DCMAKE_MAKE_PROGRAM=<program>.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dui_build_program.cmake)

# Top-level CMake management (following the develop2 branch pattern):
#   cmake -S . -B build      # configure the whole repository at once (dui + third-party libraries + all examples)
#   cmake --build build      # build everything at once (--target <name> builds only one example)
# Standalone per-example builds are also kept (each example directory is an independent CMake project; see scripts/*.sh --standalone)
# The VERSION is the single source of truth for the packaged version: it becomes
# PROJECT_VERSION here and is what dui_install_package() feeds to
# write_basic_package_version_file(), so bumping it is the only edit a release needs.
project(dui VERSION 0.1.0)

# Default to Release when no build type is explicitly specified (e.g. mkdir build && cd build && cmake .. && make)
# Note: with an empty build type there is no optimization, and some inline symbols won't be inlined, causing link failures
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug/Release)" FORCE)
endif()

# Emit compile_commands.json for clang-tidy and other tooling. Single-config
# generators (Unix Makefiles / Ninja) honour this; multi-config generators
# (Visual Studio / Xcode) have no single command line per file and ignore it.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependency management (Skia build targets + CEF download) must be in place
# BEFORE src/CMakeLists.txt, which compiles Skia's tools/window sources into the dui library.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/dui_common.cmake")

# CEF examples enable CEF in their own scope, but the shared wrapper is built
# from the top-level third-party directory before the examples are configured.
option(DUI_BUILD_CEF_EXAMPLES "Build the cef/CefBrowser examples" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(POLICY CMP0177)
    cmake_policy(SET CMP0177 OLD)
endif()

# Recursively group targets into folders in the IDE (only effective with Visual Studio/Xcode etc. generators)
function(force_set_folder_recursive dir folder_name)
    get_property(targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target IN LISTS targets)
        set_target_properties(${target} PROPERTIES FOLDER "${folder_name}")
    endforeach()
    get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(subdir IN LISTS subdirs)
        force_set_folder_recursive("${subdir}" "${folder_name}")
    endforeach()
endfunction()

add_subdirectory(src)
option(DUI_BUILD_TESTS "Build dui unit tests" ON)
if(DUI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/dui_install.cmake")
# No VERSION argument: dui_install_package() defaults to PROJECT_VERSION, declared
# once in the project() call above.
dui_install_package()

# Consumers that only need the library -- the PolluxOS image build, say -- turn
# this off instead of editing examples/CMakeLists.txt by hand.  A full example
# build is 55 programs, and a local patch to that file gets in the way of
# switching branches in the submodule.
option(DUI_BUILD_EXAMPLES "Build the example programs" ON)
if(DUI_BUILD_EXAMPLES)
    add_subdirectory(examples)

    force_set_folder_recursive("${CMAKE_CURRENT_SOURCE_DIR}/examples" "examples")
endif()
