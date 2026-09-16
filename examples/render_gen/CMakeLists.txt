cmake_minimum_required(VERSION 4.0)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(render_gen CXX)

# dui source root, and the build entry point (it includes dui_common.cmake itself).
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

# Mode 2 (XML-to-code generation): convert render.xml to pure C++ code at build time
set(RENDER_THEME "${DUI_EXAMPLE_THEME}")
if(RENDER_THEME STREQUAL "")
    set(RENDER_THEME default)
endif()
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${RENDER_THEME}/render/render.xml")
set(GEN_FUNC_NAME Init)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")

# Embed resources into the executable; embedded resources are accessed directly from memory.
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
set(EMBED_RES_PATHS
    "themes/${RENDER_THEME}/global.xml"
    "themes/${RENDER_THEME}/public"
    "themes/${RENDER_THEME}/render"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

# The application owns its target: sources are listed, not globbed.
add_executable(render_gen
    RenderForm.cpp
    RenderTest1.cpp
    RenderTest2.cpp
    main.cpp
)

target_link_libraries(render_gen PRIVATE dui::app)

# Output directory, Windows entry point, the macOS .app bundle and its code signature,
# and build ordering for the Skia archive and any generated code.
dui_finalize_app(render_gen)
