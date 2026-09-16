cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(controls_code CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
# Only embed the common resources (active theme's global.xml + public assets, plus fonts and
# lang) and this example's own skin (themes/<platform-theme>/controls), not the whole tree.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(CONTROLS_EMBED_THEME "${DUI_EXAMPLE_THEME}")
if(CONTROLS_EMBED_THEME STREQUAL "")
    set(CONTROLS_EMBED_THEME default)
endif()
# Embed only the platform theme (self-contained: macos26/public now carries all
# shared images, no default-theme fallback) plus this example's own skin
# (themes/<platform-theme>/controls). Always: fonts + lang.
set(EMBED_RES_PATHS
    "themes/${CONTROLS_EMBED_THEME}/global.xml"
    "themes/${CONTROLS_EMBED_THEME}/public"
    "themes/${CONTROLS_EMBED_THEME}/controls"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(controls_code
    AboutForm.cpp
    AnimationForm.cpp
    ControlForm.cpp
    TestForm.cpp
    main.cpp
)

target_link_libraries(controls_code PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(controls_code)
