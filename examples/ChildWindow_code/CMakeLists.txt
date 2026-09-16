cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ChildWindow_code CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Embed the common resources and this example's child window skin.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(CHILD_WINDOW_EMBED_THEME "${DUI_EXAMPLE_THEME}")
if(CHILD_WINDOW_EMBED_THEME STREQUAL "")
    set(CHILD_WINDOW_EMBED_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${CHILD_WINDOW_EMBED_THEME}/global.xml"
    "themes/${CHILD_WINDOW_EMBED_THEME}/public"
    "themes/${CHILD_WINDOW_EMBED_THEME}/child_window"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
# The three ChildWindowPaint_* files are all listed: the previous layer globbed every
# *.cpp in the directory regardless of platform, and each one guards its body internally
# (DUI_BUILD_FOR_MACOS / DUI_BUILD_FOR_WIN / neither).
add_executable(ChildWindow_code
    ChildWindowPaint_MacOS.cpp
    ChildWindowPaint_Native.cpp
    ChildWindowPaint_Win.cpp
    MainForm.cpp
    MyChildWindowEvents.cpp
    main.cpp
)

target_link_libraries(ChildWindow_code PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(ChildWindow_code)
