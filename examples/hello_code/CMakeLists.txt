cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(hello_code CXX)

# Mode 3 (pure code): the UI is built entirely in C++ code (MainForm::OnInitWindow),
# no layout XML and no build-time generator.

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
# Only embed the common resources (active theme's global.xml + public assets, plus fonts and
# lang) and this example's own skin (themes/<platform-theme>/hello), not the whole tree.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
if(APPLE)
    set(HELLO_EMBED_THEME macos26)
elseif(WIN32)
    set(HELLO_EMBED_THEME windows11)
else()
    set(HELLO_EMBED_THEME default)
endif()
# Embed only the platform theme (self-contained: macos26/public now carries all
# shared images, no default-theme fallback) plus this example's own skin
# (themes/<platform-theme>/hello). Always: fonts + lang.
set(EMBED_RES_PATHS
    "themes/${HELLO_EMBED_THEME}/global.xml"
    "themes/${HELLO_EMBED_THEME}/public"
    "themes/${HELLO_EMBED_THEME}/hello"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(hello_code
    MainForm.cpp
    main.cpp
)

target_link_libraries(hello_code PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(hello_code)
