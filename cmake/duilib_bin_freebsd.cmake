# DUILIB_OS_FREEBSD
if(NOT DUILIB_OS_FREEBSD)
    message(FATAL_ERROR "NOT DUILIB_OS_FREEBSD!")
endif()

# Remove *.mm
list(REMOVE_ITEM SRC_FILES ${DUILIB_PROJECT_SRC_DIR}/main_macos.mm)

# Set the sources the executable depends on
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Embedded resources dependency
if(DEFINED DUILIB_EMBED_RES_SRC AND TARGET "${PROJECT_NAME}_embed_res")
    add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_embed_res")
endif()

# Platform standard libraries
set(DUILIB_FREEBSD_LIBS pthread dl)

find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(X11 REQUIRED)

# Libraries required on FreeBSD
target_link_libraries(${PROJECT_NAME} ${DUILIB_LIBS} ${DUILIB_SDL_LIBS} ${DUILIB_SKIA_LIBS}  ${DUILIB_FREEBSD_LIBS} ${X11_LIBRARIES} Freetype::Freetype Fontconfig::Fontconfig)


