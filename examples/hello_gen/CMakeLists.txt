cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(hello_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Mode 2 (XML-to-code generation): convert hello.xml to pure C++ code at build time.
# The XML is the design-time format: write/debug the UI in XML, generate code at
# build time, zero layout XML parsing at runtime.
# The XML source follows the platform-default theme (same as the hello XML example,
# see GetDefaultThemePath): Windows -> windows11, macOS -> macos26, others -> default.
if(APPLE)
    set(HELLO_THEME macos26)
elseif(WIN32)
    set(HELLO_THEME windows11)
else()
    set(HELLO_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${HELLO_THEME}/hello/hello.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
# Only embed the common resources (active theme's global.xml + public assets, plus fonts and
# lang) and this example's own skin (themes/<platform-theme>/hello), not the whole tree.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
# Embed only the platform theme (self-contained: macos26/public now carries all
# shared images, no default-theme fallback) plus this example's own skin
# (themes/<platform-theme>/hello). Always: fonts + lang.
set(EMBED_RES_PATHS
    "themes/${HELLO_THEME}/global.xml"
    "themes/${HELLO_THEME}/public"
    "themes/${HELLO_THEME}/hello"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(hello_gen
    MainForm.cpp
    main.cpp
)

target_link_libraries(hello_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(hello_gen)
