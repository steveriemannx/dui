cmake_minimum_required(VERSION 3.18)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ChildWindow_gen CXX)

get_filename_component(DUILIB_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
get_filename_component(DUILIB_PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_common.cmake")

# Mode 2 (XML-to-code generation): convert child_window.xml to pure C++ code at build time
# (with <Include src="child_window_item.xml" count="9"/> expansion)
set(GEN_XML_FILES child_window.xml)
set(GEN_FUNC_NAME Init)
include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_gen_code.cmake")

include("${DUILIB_SRC_ROOT_DIR}/cmake/duilib_bin.cmake")

# Path to the SDL library headers (ChildWindowPaint_SDL.cpp uses SDL headers directly)
if(DUILIB_ENABLE_SDL)
    include_directories("${DUILIB_SDL_SRC_ROOT_DIR}/include")
endif()
