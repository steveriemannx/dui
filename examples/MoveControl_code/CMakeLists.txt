cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(MoveControl_code CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Embed resources for code mode
set(EMBED_RES_DIR "${DUI_SRC_ROOT_DIR}/resources")
set(EMBED_RES_PATHS "${DUI_SRC_ROOT_DIR}/resources/themes")
set(EMBED_RES_THEME "default")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(MoveControl_code
    AppDb.cpp
    AppItemUi.cpp
    MoveControlForm.cpp
    main.cpp
)

target_link_libraries(MoveControl_code PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(MoveControl_code)
