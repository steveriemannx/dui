# DUI_OS_FREEBSD
if(NOT DUI_OS_FREEBSD)
    message(FATAL_ERROR "NOT DUI_OS_FREEBSD!")
endif()

# Remove *.mm
list(REMOVE_ITEM SRC_FILES ${DUI_PROJECT_SRC_DIR}/main_macos.mm)

# Set the sources the executable depends on
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Embedded resources dependency
if(DEFINED DUI_EMBED_RES_SRC AND TARGET "${PROJECT_NAME}_embed_res")
    add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_embed_res")
endif()

# Embed XML dependency
if(DEFINED DUI_EMBED_XML_HEADER)
    add_dependencies(${PROJECT_NAME} embed_xml)
endif()

# Generate C++ code from XML dependency
if(TARGET "${PROJECT_NAME}_gen_xml_code")
    add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_gen_xml_code")
endif()

# Per-config Skia link directories (multi-config generators: VS, Xcode)
dui_target_skia_link_dirs(${PROJECT_NAME})

# Platform standard libraries
set(DUI_FREEBSD_LIBS pthread dl)

find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(X11 REQUIRED)

# Libraries required on FreeBSD
target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SDL_LIBS} ${DUI_SKIA_LIBS}  ${DUI_FREEBSD_LIBS} ${X11_LIBRARIES} Freetype::Freetype Fontconfig::Fontconfig)


