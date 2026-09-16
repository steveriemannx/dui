cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ListBox_code CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
# Only embed the common resources (active theme's global.xml + public assets, plus fonts and
# lang) and this example's own skin (themes/<platform-theme>/list_box), not the whole tree.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(LISTBOX_EMBED_THEME "${DUI_EXAMPLE_THEME}")
if(LISTBOX_EMBED_THEME STREQUAL "")
    set(LISTBOX_EMBED_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${LISTBOX_EMBED_THEME}/global.xml"
    "themes/${LISTBOX_EMBED_THEME}/public"
    "themes/${LISTBOX_EMBED_THEME}/list_box"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(ListBox_code
    Item.cpp
    ListBoxForm.cpp
    main.cpp
)

target_link_libraries(ListBox_code PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(ListBox_code)
