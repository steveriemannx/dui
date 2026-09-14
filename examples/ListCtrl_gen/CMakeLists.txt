cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(ListCtrl_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

if(APPLE)
    set(LISTCTRL_THEME macos26)
elseif(WIN32)
    set(LISTCTRL_THEME windows11)
else()
    set(LISTCTRL_THEME default)
endif()

# Mode 2 (XML-to-code generation): convert the active theme XML to C++.
# loading*.xml remains the ListCtrl control's internal runtime skin.
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${LISTCTRL_THEME}/list_ctrl/list_ctrl.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${LISTCTRL_THEME}/global.xml"
    "themes/${LISTCTRL_THEME}/public"
    "themes/${LISTCTRL_THEME}/list_ctrl"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(ListCtrl_gen
    MainForm.cpp
    main.cpp
)

target_link_libraries(ListCtrl_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(ListCtrl_gen)
