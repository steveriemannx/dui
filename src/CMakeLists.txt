cmake_minimum_required(VERSION 3.18)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source code and run \"cmake -S ${CMAKE_SOURCE_DIR} -B .\" from there")
endif()

# Make this directory self-contained so it can also be configured standalone
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/dui_common.cmake")

set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
if("${DUI_MD}" STREQUAL "ON")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

project(dui CXX C)

if(MSVC)
    add_compile_options("/utf-8")
    cmake_host_system_information(RESULT CPU_CORES QUERY NUMBER_OF_LOGICAL_CORES)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Skia build options
add_definitions(-DSK_GANESH -DSK_GL -DSK_RELEASE)

# CEF lib dir: unified (SDL-only, still optional per platform)
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    message(WARNING "CEF is not supported on FreeBSD - building without CEF support")
    set(CEF_LIB_DIR "")
else()
    set(CEF_LIB_DIR "cef_binary")
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(DUI_SRC_MACOS 1)
    endif()
    if(DUI_CEF_109)
        set(CEF_LIB_DIR "libcef_win_109")
    endif()
endif()

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
get_filename_component(SKIA_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/skia/" ABSOLUTE)
get_filename_component(SDL_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/SDL3/" ABSOLUTE)

# Include directories - SDL only
include_directories(${DUI_SRC_ROOT_DIR})
include_directories(${DUI_SRC_ROOT_DIR}/include)
include_directories(${SKIA_SRC_ROOT_DIR})
include_directories("${SDL_SRC_ROOT_DIR}/include")

if(CEF_LIB_DIR AND NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW)
    get_filename_component(CEF_INCLUDE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/libcef/${CEF_LIB_DIR}/" ABSOLUTE)
    include_directories(${CEF_INCLUDE_ROOT_DIR})
else()
    message(STATUS "CEF includes disabled")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${DUI_SRC_ROOT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${DUI_SRC_ROOT_DIR}/lib")

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} DIR_LIB_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Animation ANIMATION_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Box BOX_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Layout LAYOUT_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Control CONTROL_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Core CORE_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Image IMAGE_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Render RENDER_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/RenderSkia RENDER_SKIA_SRC)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/Utils UTILS_SRC)

# CEF sources (SDL-only still supports CEF on all desktop platforms except FreeBSD/MinGW)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW)
    aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/CEFControl CEF_CONTROL_SRC)
    aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal CEF_CONTROL_INTERNAL_SRC)
    list(APPEND PROJECT_SOURCES ${CEF_CONTROL_SRC} ${CEF_CONTROL_INTERNAL_SRC})
    message(STATUS "CEF support enabled (${CMAKE_SYSTEM_NAME})")
else()
    message(STATUS "CEF support disabled")
endif()

# Remove ObjC++ sources on non-macOS
if(NOT DUI_SRC_MACOS)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/SDL_MacOS.mm)
    list(REMOVE_ITEM CORE_SRC ${CMAKE_CURRENT_LIST_DIR}/Core/SDL_MacOS.h)
    list(REMOVE_ITEM CEF_CONTROL_SRC ${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefWindowUtils_MacOS.mm)
    list(REMOVE_ITEM UTILS_SRC ${CMAKE_CURRENT_LIST_DIR}/Utils/ScreenCapture_MacOS.mm)
endif()

aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/apng APNG_SRC)
aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/stb_image STB_SRC)
aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/svg SVG_SRC)
aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/convert_utf UTF_SRC)
aux_source_directory(${DUI_SRC_ROOT_DIR}/third_party/xml XML_SRC)
set(GIFLIB_HDRS
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_hash.h
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_lib.h
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_lib_private.h
)
set(GIFLIB_SRCS
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/dgif_lib.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/egif_lib.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_err.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_font.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_hash.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gifalloc.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/openbsd-reallocarray.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/quantize.c
)
set(SKIA_HDRS
    ${SKIA_SRC_ROOT_DIR}/tools/window/DisplayParams.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/RasterWindowContext.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/WindowContext.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.h
)
set(SKIA_SRCS
    ${SKIA_SRC_ROOT_DIR}/tools/window/WindowContext.cpp
    ${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp
)

set(LIBRARY_SOURCES
    ${DIR_LIB_SRC}
    ${ANIMATION_SRC}
    ${BOX_SRC}
    ${LAYOUT_SRC}
    ${CONTROL_SRC}
    ${CORE_SRC}
    ${IMAGE_SRC}
    ${RENDER_SRC}
    ${RENDER_SKIA_SRC}
    ${UTILS_SRC}
    ${APNG_SRC}
    ${STB_SRC}
    ${SVG_SRC}
    ${UTF_SRC}
    ${XML_SRC}
    ${GIFLIB_HDRS}
    ${GIFLIB_SRCS}
    ${SKIA_HDRS}
    ${SKIA_SRCS}
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW)
    list(APPEND LIBRARY_SOURCES
        ${CEF_CONTROL_SRC}
        ${CEF_CONTROL_INTERNAL_SRC}
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_package(Freetype REQUIRED)
    find_package(Fontconfig REQUIRED)
    find_package(X11 REQUIRED)
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
    target_link_libraries(${PROJECT_NAME} Freetype::Freetype Fontconfig::Fontconfig)
    target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
else()
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
endif()

if(TARGET dui_sdl)
    add_dependencies(${PROJECT_NAME} dui_sdl)
endif()
if(TARGET dui_skia)
    add_dependencies(${PROJECT_NAME} dui_skia)
endif()

# SDL-only: always enable SDL
target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_SDL=1)

if(DUI_MINGW)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_CEF=0)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    include(${DUI_SRC_ROOT_DIR}/cmake/dui_compiles.cmake)
    set(DUI_COMPILER_FLAGS
        -fno-strict-aliasing
        -fstack-protector
        -funwind-tables
        -fvisibility=hidden
        -Wall
        -Wno-unknown-pragmas
        -Wextra
        -Wendif-labels
        -Wno-missing-field-initializers
        -Wno-unused-parameter
    )
    set(DUI_CXX_COMPILER_FLAGS
        -fno-threadsafe-statics
        -fvisibility-inlines-hidden
        -frtti
        -Wno-narrowing
        -Wsign-compare
        -Wno-unused-variable
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        list(APPEND DUI_CXX_COMPILER_FLAGS -Wno-reorder -Wno-cast-function-type)
        set_source_files_properties("${DUI_SRC_ROOT_DIR}/third_party/libudis86/decode.c" PROPERTIES COMPILE_OPTIONS "-Wno-implicit-fallthrough")
        if(DUI_HAVE_AVX2)
          set_source_files_properties("${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp" PROPERTIES COMPILE_OPTIONS "-mavx2")
        elseif(DUI_HAVE_AVX)
          set_source_files_properties("${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp" PROPERTIES COMPILE_OPTIONS "-mavx")
        endif()
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND DUI_COMPILER_FLAGS -Wnewline-eof)
        list(APPEND DUI_CXX_COMPILER_FLAGS -fobjc-call-cxx-cdtors -Wno-reorder-ctor -Wno-unused-lambda-capture -Wno-unused-private-field)
    endif()
    target_compile_options(${PROJECT_NAME} PRIVATE ${DUI_COMPILER_FLAGS})
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: ${DUI_CXX_COMPILER_FLAGS}>)
endif()

add_subdirectory(../third_party third_party)
add_dependencies(${PROJECT_NAME} dui-cximage png_static dui-zlib dui-webp)
