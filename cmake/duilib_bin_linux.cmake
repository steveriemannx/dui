# DUILIB_OS_LINUX
if(NOT DUILIB_OS_LINUX)
    message(FATAL_ERROR "NOT DUILIB_OS_LINUX!")
endif()

if(DUILIB_ENABLE_CEF)
    # Using the CEF module: add the CEF source root to the include path
    include_directories(${DUILIB_CEF_SRC_ROOT_DIR})

    if(NOT DUILIB_MULTI_CONFIG)
        # Path to the cef library (.a/.so shared library path)
        link_directories("${DUILIB_CEF_LIB_PATH}")
    endif()
endif()

# Remove *.mm
list(REMOVE_ITEM SRC_FILES ${DUILIB_PROJECT_SRC_DIR}/main_macos.mm)

# Set the sources the executable depends on
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Embedded resources dependency
if(DEFINED DUILIB_EMBED_RES_SRC AND TARGET "${PROJECT_NAME}_embed_res")
    add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_embed_res")
endif()

# Per-config Skia link directories (multi-config generators: VS, Xcode)
duilib_target_skia_link_dirs(${PROJECT_NAME})

# Embed XML dependency
if(DEFINED DUILIB_EMBED_XML_HEADER)
    add_dependencies(${PROJECT_NAME} embed_xml)
endif()

# Generate C++ code from XML dependency
if(TARGET "${PROJECT_NAME}_gen_xml_code")
    add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_gen_xml_code")
endif()

# Platform standard libraries
set(DUILIB_LINUX_LIBS X11 freetype fontconfig pthread dl)

# Wayland support
if(DUILIB_ENABLE_WAYLAND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUILIB_WAYLAND=1)
    target_include_directories(${PROJECT_NAME} PRIVATE ${DUILIB_WAYLAND_INCLUDE_DIRS})
    set(DUILIB_WAYLAND_NOX11_LIBS freetype fontconfig pthread dl)
    target_link_libraries(${PROJECT_NAME} ${DUILIB_LIBS} ${DUILIB_WAYLAND_LIBS} ${DUILIB_SKIA_LIBS} ${DUILIB_WAYLAND_NOX11_LIBS})
else()
    # Libraries required on Linux (SDL3)
    target_link_libraries(${PROJECT_NAME} ${DUILIB_LIBS} ${DUILIB_SDL_LIBS} ${DUILIB_SKIA_LIBS} ${DUILIB_CEF_LIBS} ${DUILIB_LINUX_LIBS})
endif()

