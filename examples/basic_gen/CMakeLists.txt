cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(basic_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Mode 2 (XML-to-code generation): convert basic.xml to pure C++ code at build time.
# The XML is the design-time format: write/debug the UI in XML, generate code at
# build time, zero layout XML parsing at runtime.
# The XML source follows the platform theme (cmake/dui_common.cmake sets
# DUI_EXAMPLE_THEME): Windows -> windows11, macOS -> macos26, Linux -> gnome,
# FreeBSD -> freebsd.
set(BASIC_THEME "${DUI_EXAMPLE_THEME}")
if(BASIC_THEME STREQUAL "")
    set(BASIC_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${BASIC_THEME}/basic/basic.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed only the platform theme (self-contained: macos26/public now carries all
# shared images, no default-theme fallback) plus this example's own skin
# (themes/<platform-theme>/basic). Always: fonts + lang.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${BASIC_THEME}/global.xml"
    "themes/${BASIC_THEME}/public"
    "themes/${BASIC_THEME}/basic"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(basic_gen
    MainForm.cpp
    main.cpp
)

target_link_libraries(basic_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(basic_gen)
