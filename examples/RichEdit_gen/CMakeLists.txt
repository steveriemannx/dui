cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(RichEdit_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

set(EMBED_THEME "${DUI_EXAMPLE_THEME}")
if(EMBED_THEME STREQUAL "")
    set(EMBED_THEME default)
endif()
# Mode 2 (XML-to-code generation): convert the active theme's XML layouts to C++.
# color_combox.xml remains a code-built internal template.
set(GEN_XML_FILES
    "${DUI_ROOT}/resources/themes/${EMBED_THEME}/rich_edit/rich_edit.xml"
    "${DUI_ROOT}/resources/themes/${EMBED_THEME}/rich_edit/find.xml"
    "${DUI_ROOT}/resources/themes/${EMBED_THEME}/rich_edit/replace.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${EMBED_THEME}/global.xml"
    "themes/${EMBED_THEME}/public"
    "themes/${EMBED_THEME}/rich_edit"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(RichEdit_gen
    FindForm.cpp
    MainForm.cpp
    ReplaceForm.cpp
    RichEditFindReplace.cpp
    main.cpp
)

target_link_libraries(RichEdit_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(RichEdit_gen)
