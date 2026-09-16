cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(layout_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Mode 2 (XML-to-code generation): convert the platform layout.xml to pure C++ code.
set(LAYOUT_THEME "${DUI_EXAMPLE_THEME}")
if(LAYOUT_THEME STREQUAL "")
    set(LAYOUT_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${LAYOUT_THEME}/layout/layout.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS "themes/${LAYOUT_THEME}/global.xml" "themes/${LAYOUT_THEME}/public" "themes/${LAYOUT_THEME}/layout" "fonts" "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(layout_gen
    MainForm.cpp
    main.cpp
)

target_link_libraries(layout_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(layout_gen)
