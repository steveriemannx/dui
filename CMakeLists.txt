cmake_minimum_required(VERSION 3.18)

# Top-level CMake management (following the develop2 branch pattern):
#   cmake -S . -B build      # configure the whole repository at once (duilib + third-party libraries + all examples)
#   cmake --build build      # build everything at once (--target <name> builds only one example)
# Standalone per-example builds are also kept (each example directory is an independent CMake project; see scripts/*.sh --standalone)
project(nim_duilib)

# Default to Release when no build type is explicitly specified (e.g. mkdir build && cd build && cmake .. && make)
# Note: with an empty build type there is no optimization, and some inline symbols won't be inlined, causing link failures
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug/Release)" FORCE)
endif()

# Dependency management (Skia/SDL3 submodule build targets + CEF download) must be in place
# BEFORE src/CMakeLists.txt, which compiles Skia's tools/window sources into the duilib library.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/duilib_common.cmake")

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
add_subdirectory(examples)

force_set_folder_recursive("${CMAKE_CURRENT_SOURCE_DIR}/examples" "examples")
