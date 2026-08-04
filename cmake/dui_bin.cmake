if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source code and run \"cmake -S ${CMAKE_SOURCE_DIR} -B .\" from there")
endif()

# Enable C++20
set(CMAKE_CXX_STANDARD 20) # C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # C++20

# Print basic information (only when debugging is enabled; DUI_LOG is defined in dui_common.cmake, default OFF)
if(DUI_LOG)
    message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
    message(STATUS "C compiler  : ${CMAKE_C_COMPILER} ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
    message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# Set the project include path (dui)
include_directories(${DUI_ROOT})
include_directories(${DUI_ROOT}/include)

# Add this program's root directory to the include path
include_directories(${DUI_PROJECT_SRC_DIR})

# Set the project link directories
link_directories("${DUI_LIB_PATH}")          # path to the dui library

if(DUI_MULTI_CONFIG)
    # Per-config Skia paths are added via dui_target_skia_link_dirs() in the platform
    # files (generator expressions can't be used with global link_directories).
else()
    link_directories("${DUI_SKIA_LIB_PATH}") # path to the skia library (single-config)
endif()

# ---- Helper: add per-config Skia link directories to a target (multi-config generators) ----
macro(dui_target_skia_link_dirs _target)
    if(DUI_MULTI_CONFIG)
        target_link_directories(${_target} PRIVATE
            "$<$<CONFIG:Debug>:${DUI_SKIA_LIB_PATH_DEBUG}>"
            "$<$<NOT:$<CONFIG:Debug>>:${DUI_SKIA_LIB_PATH_RELEASE}>"
        )
    endif()
endmacro()

# ---- Helper: add per-config CEF link directories (Windows multi-config only) ----
macro(dui_target_cef_link_dirs _target)
    if(DUI_MULTI_CONFIG AND DUI_ENABLE_CEF AND DUI_OS_WINDOWS)
        target_link_directories(${_target} PRIVATE
            "$<$<CONFIG:Debug>:${DUI_CEF_LIB_PATH_DEBUG}>"
            "$<$<NOT:$<CONFIG:Debug>>:${DUI_CEF_LIB_PATH_RELEASE}>"
        )
    endif()
endmacro()

if(DUI_ENABLE_SDL)
    link_directories("${DUI_SDL_LIB_PATH}")  # path to the SDL library
endif()

# Set the executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DUI_BIN_PATH}")

# Add sources and store them in the SRC_FILES variable
aux_source_directory(${DUI_PROJECT_SRC_DIR} SRC_FILES)

# Add source files from subdirectories
if(DUI_SRC_SUB_DIRS)
    foreach(ITEM IN LISTS DUI_SRC_SUB_DIRS)
        # Add them one subdirectory at a time
        aux_source_directory("${DUI_PROJECT_SRC_DIR}/${ITEM}" SUB_DIR_SRC_FILES)
        list(APPEND SRC_FILES ${SUB_DIR_SRC_FILES})  # merge the list
    endforeach()
endif()

# Implemented separately per platform
if(DUI_OS_WINDOWS)
    # Windows platform
    include("${CMAKE_CURRENT_LIST_DIR}/dui_bin_windows.cmake") 
elseif(DUI_OS_LINUX)
    # Linux platform
    include("${CMAKE_CURRENT_LIST_DIR}/dui_bin_linux.cmake") 
elseif(DUI_OS_MACOS)
    # macOS platform
    include("${CMAKE_CURRENT_LIST_DIR}/dui_bin_macos.cmake") 
elseif(DUI_OS_FREEBSD)
    # FreeBSD platform
    include("${CMAKE_CURRENT_LIST_DIR}/dui_bin_freebsd.cmake") 
else()
    message(FATAL_ERROR "Unknown OS!")
endif()

# Build-order wiring: the executable links ${DUI_SKIA_LIBS}/${DUI_SDL_LIBS} by name, so the
# archives built by cmake/dui_deps.cmake must exist before linking (parallel make safety).
if(TARGET dui_skia)
    add_dependencies(${PROJECT_NAME} dui_skia)
endif()
if(TARGET dui_sdl)
    add_dependencies(${PROJECT_NAME} dui_sdl)
endif()
