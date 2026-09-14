cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(TreeView_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

if(APPLE)
    set(TREEVIEW_EMBED_THEME macos26)
elseif(WIN32)
    set(TREEVIEW_EMBED_THEME windows11)
else()
    set(TREEVIEW_EMBED_THEME default)
endif()
# Mode 2 (XML-to-code generation): convert the active theme XML to C++.
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${TREEVIEW_EMBED_THEME}/tree_view/tree_view.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${TREEVIEW_EMBED_THEME}/global.xml"
    "themes/${TREEVIEW_EMBED_THEME}/public"
    "themes/${TREEVIEW_EMBED_THEME}/tree_view"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(TreeView_gen
    ComputerView.cpp
    ExplorerView.cpp
    MainForm.cpp
    SimpleFileView.cpp
    main.cpp
)

target_link_libraries(TreeView_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(TreeView_gen)
