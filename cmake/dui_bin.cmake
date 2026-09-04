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

# Linux and FreeBSD still use this helper. Keep the link directories target
# local; native macOS/Windows paths link through dui_skia_libs directly.
macro(dui_target_skia_link_dirs _target)
    if(DUI_MULTI_CONFIG)
        target_link_directories(${_target} PRIVATE
            "$<$<CONFIG:Debug>:${DUI_SKIA_LIB_PATH_DEBUG}>"
            "$<$<NOT:$<CONFIG:Debug>>:${DUI_SKIA_LIB_PATH_RELEASE}>"
        )
    else()
        target_link_directories(${_target} PRIVATE "${DUI_SKIA_LIB_PATH}")
    endif()
endmacro()

# Set the executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${DUI_BIN_PATH}")

# Add sources and store them in the SRC_FILES variable. CONFIGURE_DEPENDS
# replaces the old aux_source_directory() scan while retaining automatic source
# discovery for standalone example projects.
file(GLOB SRC_FILES CONFIGURE_DEPENDS
    "${DUI_PROJECT_SRC_DIR}/*.c"
    "${DUI_PROJECT_SRC_DIR}/*.cc"
    "${DUI_PROJECT_SRC_DIR}/*.cpp"
    "${DUI_PROJECT_SRC_DIR}/*.mm"
)

# Add source files from subdirectories
if(DUI_SRC_SUB_DIRS)
    foreach(ITEM IN LISTS DUI_SRC_SUB_DIRS)
        file(GLOB SUB_DIR_SRC_FILES CONFIGURE_DEPENDS
            "${DUI_PROJECT_SRC_DIR}/${ITEM}/*.c"
            "${DUI_PROJECT_SRC_DIR}/${ITEM}/*.cc"
            "${DUI_PROJECT_SRC_DIR}/${ITEM}/*.cpp"
            "${DUI_PROJECT_SRC_DIR}/${ITEM}/*.mm"
        )
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

# Keep example-only headers private to this target. Generated includes are also
# target-local, preventing one example's generated files from leaking to others.
target_include_directories(${PROJECT_NAME} PRIVATE
    ${DUI_ROOT} ${DUI_ROOT}/include ${DUI_PROJECT_SRC_DIR}
    ${DUI_GENERATED_INCLUDE_DIRS}
)

if(DUI_OS_WINDOWS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

# Build-order wiring for the Skia archive built by cmake/dui_deps.cmake.
if(TARGET dui_skia)
    add_dependencies(${PROJECT_NAME} dui_skia)
endif()
