if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source code and run \"cmake -S ${CMAKE_SOURCE_DIR} -B .\" from there")
endif()

# Enable C++20
set(CMAKE_CXX_STANDARD 20) # C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # C++20

# Print basic information (only when debugging is enabled; DUILIB_LOG is defined in duilib_common.cmake, default OFF)
if(DUILIB_LOG)
    message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
    message(STATUS "C compiler  : ${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
    message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# Set the project include path (duilib)
include_directories(${DUILIB_ROOT})
include_directories(${DUILIB_ROOT}/include)

# Add this program's root directory to the include path
include_directories(${DUILIB_PROJECT_SRC_DIR})

# Set the project link directories
link_directories("${DUILIB_LIB_PATH}")          # path to the duilib library
link_directories("${DUILIB_SKIA_LIB_PATH}")     # path to the skia library

if(DUILIB_ENABLE_SDL)
    link_directories("${DUILIB_SDL_LIB_PATH}")  # path to the SDL library
endif()

# Set the executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DUILIB_BIN_PATH}")

# Add sources and store them in the SRC_FILES variable
aux_source_directory(${DUILIB_PROJECT_SRC_DIR} SRC_FILES)

# Add source files from subdirectories
if(DUILIB_SRC_SUB_DIRS)
    foreach(ITEM IN LISTS DUILIB_SRC_SUB_DIRS)
        # Add them one subdirectory at a time
        aux_source_directory("${DUILIB_PROJECT_SRC_DIR}/${ITEM}" SUB_DIR_SRC_FILES)
        list(APPEND SRC_FILES ${SUB_DIR_SRC_FILES})  # merge the list
    endforeach()
endif()

# Implemented separately per platform
if(DUILIB_OS_WINDOWS)
    # Windows platform
    include("${CMAKE_CURRENT_LIST_DIR}/duilib_bin_windows.cmake") 
elseif(DUILIB_OS_LINUX)
    # Linux platform
    include("${CMAKE_CURRENT_LIST_DIR}/duilib_bin_linux.cmake") 
elseif(DUILIB_OS_MACOS)
    # macOS platform
    include("${CMAKE_CURRENT_LIST_DIR}/duilib_bin_macos.cmake") 
elseif(DUILIB_OS_FREEBSD)
    # FreeBSD platform
    include("${CMAKE_CURRENT_LIST_DIR}/duilib_bin_freebsd.cmake") 
else()
    message(FATAL_ERROR "Unknown OS!")
endif()

# Build-order wiring: the executable links ${DUILIB_SKIA_LIBS}/${DUILIB_SDL_LIBS} by name, so the
# archives built by cmake/duilib_deps.cmake must exist before linking (parallel make safety).
if(TARGET duilib_skia)
    add_dependencies(${PROJECT_NAME} duilib_skia)
endif()
if(TARGET duilib_sdl)
    add_dependencies(${PROJECT_NAME} duilib_sdl)
endif()
