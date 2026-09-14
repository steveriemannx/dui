cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(controls_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Mode 2 (XML-to-code generation): convert the window layouts to pure C++ code at build time.
# The two menus (settings_menu.xml/tray_menu.xml) remain runtime menu resources.
if(APPLE)
    set(CONTROLS_THEME macos26)
elseif(WIN32)
    set(CONTROLS_THEME windows11)
else()
    set(CONTROLS_THEME "${DUI_EXAMPLE_THEME}")
    if(CONTROLS_THEME STREQUAL "")
        set(CONTROLS_THEME default)
    endif()
endif()
set(GEN_XML_FILES
    "${DUI_ROOT}/resources/themes/${CONTROLS_THEME}/controls/controls.xml"
    "${DUI_ROOT}/resources/themes/${CONTROLS_THEME}/controls/about.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
# Only embed the common resources (active theme's global.xml + public assets, plus fonts and
# lang) and this example's own skin (themes/<platform-theme>/controls), not the whole tree.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${CONTROLS_THEME}/global.xml"
    "themes/${CONTROLS_THEME}/public"
    "themes/${CONTROLS_THEME}/controls"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(controls_gen
    AboutForm.cpp
    AnimationForm.cpp
    ControlForm.cpp
    TestForm.cpp
    main.cpp
)

target_link_libraries(controls_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(controls_gen)
