cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(VirtualListBox_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

if(APPLE)
    set(VLB_EMBED_THEME macos26)
elseif(WIN32)
    set(VLB_EMBED_THEME windows11)
else()
    set(VLB_EMBED_THEME default)
endif()
# Mode 2 (XML-to-code generation): convert the active platform theme's XML to
# pure C++ code at build time. item.xml remains a code-built item template.
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${VLB_EMBED_THEME}/virtual_list_box/main.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${VLB_EMBED_THEME}/global.xml"
    "themes/${VLB_EMBED_THEME}/public"
    "themes/${VLB_EMBED_THEME}/virtual_list_box"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(VirtualListBox_gen
    DataProvider.cpp
    Item.cpp
    MainForm.cpp
    main.cpp
)

target_link_libraries(VirtualListBox_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(VirtualListBox_gen)
