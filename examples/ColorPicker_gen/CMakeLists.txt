cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ColorPicker_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# The ColorPicker window UI is loaded from embedded resources;
# the library-owned ColorPicker class manages its own XML skin internally.
# No standalone XML-to-code generation is needed (GEN_XML_FILES/GEN_FUNC_NAME
# are omitted) because the skin is private to the library.

# Embed the platform theme resources used by the library-internal color picker skin.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
if(APPLE)
    set(COLORPICKER_THEME macos26)
elseif(WIN32)
    set(COLORPICKER_THEME windows11)
else()
    set(COLORPICKER_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${COLORPICKER_THEME}/global.xml"
    "themes/${COLORPICKER_THEME}/public"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(ColorPicker_gen
    MainForm.cpp
    main.cpp
)

target_link_libraries(ColorPicker_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(ColorPicker_gen)
