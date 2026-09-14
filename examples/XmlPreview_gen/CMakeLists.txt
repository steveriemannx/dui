cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(XmlPreview_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Mode 2 (XML-to-code generation): convert xml_preview.xml to pure C++ code at build time.
# Note: XmlBox loading a user-selected XML at runtime is the example's feature itself (not a layout).
if(APPLE)
    set(XP_THEME macos26)
elseif(WIN32)
    set(XP_THEME windows11)
else()
    set(XP_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${XP_THEME}/xml_preview/xml_preview.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${XP_THEME}/global.xml"
    "themes/${XP_THEME}/public"
    "themes/${XP_THEME}/xml_preview"
    "themes/${XP_THEME}/controls"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(XmlPreview_gen
    MainForm.cpp
    main.cpp
)

target_link_libraries(XmlPreview_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(XmlPreview_gen)
