cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(chat_code CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(CHAT_THEME "${DUI_EXAMPLE_THEME}")
if(CHAT_THEME STREQUAL "")
    set(CHAT_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${CHAT_THEME}/global.xml"
    "themes/${CHAT_THEME}/public"
    "themes/${CHAT_THEME}/chat"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(chat_code
    MainForm.cpp
    main.cpp
)

target_link_libraries(chat_code PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(chat_code)
