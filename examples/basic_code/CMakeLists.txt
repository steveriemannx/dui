cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(basic_code CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Embed only the platform theme (self-contained: macos26/public now carries all
# shared images, no default-theme fallback) plus this example's own skin
# (themes/<platform-theme>/basic). Always: fonts + lang.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(BASIC_EMBED_THEME "${DUI_EXAMPLE_THEME}")
if(BASIC_EMBED_THEME STREQUAL "")
    set(BASIC_EMBED_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${BASIC_EMBED_THEME}/global.xml"
    "themes/${BASIC_EMBED_THEME}/public"
    "themes/${BASIC_EMBED_THEME}/basic"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(basic_code
    MainForm.cpp
    main.cpp
)

target_link_libraries(basic_code PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(basic_code)
