cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(MultiLang_code CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(MULTILANG_EMBED_THEME "${DUI_EXAMPLE_THEME}")
if(MULTILANG_EMBED_THEME STREQUAL "")
    set(MULTILANG_EMBED_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${MULTILANG_EMBED_THEME}/global.xml"
    "themes/${MULTILANG_EMBED_THEME}/public"
    "themes/${MULTILANG_EMBED_THEME}/MultiLang"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(MultiLang_code
    MainForm.cpp
    main.cpp
)

target_link_libraries(MultiLang_code PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(MultiLang_code)
