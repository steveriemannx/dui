# DUI_OS_LINUX
if(NOT DUI_OS_LINUX)
    message(FATAL_ERROR "NOT DUI_OS_LINUX!")
endif()

if(DUI_ENABLE_CEF)
    # Using the CEF module: add the CEF source root to the include path
    include_directories(${DUI_CEF_SRC_ROOT_DIR})

    if(NOT DUI_MULTI_CONFIG)
        # Path to the cef library (.a/.so shared library path)
        link_directories("${DUI_CEF_LIB_PATH}")
    endif()

    # Stage the CEF runtime (libcef.so, icudtl.dat, locales, ...) next to the
    # executable, mirroring the Windows layout: "<exe_dir>/cef_binary".
    # CefManager looks there at runtime, so the examples work regardless of the
    # build directory name (no manual copy).
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                               "${DUI_CEF_SRC_ROOT_DIR}/Release"
                               "$<TARGET_FILE_DIR:${PROJECT_NAME}>/cef_binary"
                       COMMENT "Copying CEF runtime to the output directory (cef_binary/)")
endif()

# Remove *.mm
list(REMOVE_ITEM SRC_FILES ${DUI_PROJECT_SRC_DIR}/main_macos.mm)

# Set the sources the executable depends on
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Embedded resources dependency
if(DEFINED DUI_EMBED_RES_SRC AND TARGET "${PROJECT_NAME}_embed_res")
    add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_embed_res")
endif()

# Per-config Skia link directories (multi-config generators: VS, Xcode)
dui_target_skia_link_dirs(${PROJECT_NAME})

# Generate C++ code from XML dependency
if(TARGET "${PROJECT_NAME}_gen_xml_code")
    add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_gen_xml_code")
endif()

# Platform standard libraries
set(DUI_LINUX_LIBS X11 freetype fontconfig pthread dl)

# Wayland support
if(DUI_ENABLE_WAYLAND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WAYLAND=1)
    target_include_directories(${PROJECT_NAME} PRIVATE ${DUI_WAYLAND_INCLUDE_DIRS})
    set(DUI_WAYLAND_NOX11_LIBS freetype fontconfig pthread dl)
    target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_WAYLAND_LIBS} ${DUI_SKIA_LIBS} ${DUI_WAYLAND_NOX11_LIBS})
else()
    # Libraries required on Linux (SDL3)
    target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SDL_LIBS} ${DUI_SKIA_LIBS} ${DUI_CEF_LIBS} ${DUI_LINUX_LIBS})
endif()

