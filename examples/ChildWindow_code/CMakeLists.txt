cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ChildWindow_code CXX)

get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUI_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUI_SRC_ROOT_DIR}/cmake/dui_common.cmake")

include("${DUI_SRC_ROOT_DIR}/cmake/dui_bin.cmake")

# Path to the SDL library headers (ChildWindowPaint_SDL.cpp uses SDL headers directly)
if(DUI_ENABLE_SDL)
    include_directories("${DUI_SDL_SRC_ROOT_DIR}/include")
endif()
