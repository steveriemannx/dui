cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ChildWindow_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Mode 2 (XML-to-code generation): convert child_window.xml to pure C++ code at build time.
# The XML source follows the platform-default theme used by the runtime resources.
if(APPLE)
    set(CHILD_WINDOW_THEME macos26)
elseif(WIN32)
    set(CHILD_WINDOW_THEME windows11)
else()
    set(CHILD_WINDOW_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${CHILD_WINDOW_THEME}/child_window/child_window.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed the common resources and this example's child window skin.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${CHILD_WINDOW_THEME}/global.xml"
    "themes/${CHILD_WINDOW_THEME}/public"
    "themes/${CHILD_WINDOW_THEME}/child_window"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
# The three ChildWindowPaint_* files are all listed: the previous layer globbed every
# *.cpp in the directory regardless of platform, and each one guards its body internally
# (DUI_BUILD_FOR_MACOS / DUI_BUILD_FOR_WIN / neither).
add_executable(ChildWindow_gen
    ChildWindowPaint_MacOS.cpp
    ChildWindowPaint_Native.cpp
    ChildWindowPaint_Win.cpp
    MainForm.cpp
    MyChildWindowEvents.cpp
    main.cpp
)

target_link_libraries(ChildWindow_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(ChildWindow_gen)
