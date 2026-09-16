cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(threads_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Mode 2 (XML-to-code generation): convert threads.xml to pure C++ code at build time.
set(THREADS_THEME "${DUI_EXAMPLE_THEME}")
if(THREADS_THEME STREQUAL "")
    set(THREADS_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${THREADS_THEME}/threads/threads.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${THREADS_THEME}/global.xml"
    "themes/${THREADS_THEME}/public"
    "themes/${THREADS_THEME}/threads"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(threads_gen
    MainForm.cpp
    WorkerThread.cpp
    main.cpp
)

target_link_libraries(threads_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(threads_gen)
