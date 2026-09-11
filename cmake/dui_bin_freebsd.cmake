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

# Generate C++ code from XML dependency
if(TARGET "${PROJECT_NAME}_gen_xml_code")
    add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_gen_xml_code")
endif()

# Per-config Skia link directories (multi-config generators: VS, Xcode)
dui_target_skia_link_dirs(${PROJECT_NAME})

# FreeBSD keeps third-party libs in /usr/local/lib; the Wayland/GL stack is
# linked by bare -l name, so the target needs this search path explicitly.
# target_link_directories() acts on the target itself and is therefore not
# affected by directory-scope ordering the way link_directories() was.
if(EXISTS "/usr/local/lib")
    target_link_directories(${PROJECT_NAME} PRIVATE /usr/local/lib)
endif()
# /usr/local/include at TARGET scope only: a directory-scoped include here
# also reaches dui_skia, where the system expat.h shadows Skia's bundled copy.
if(EXISTS "/usr/local/include")
    target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/include)
endif()


# epoll-shim: the Wayland message loop is written against the Linux epoll/eventfd
# API. FreeBSD has eventfd in base, but epoll only via libepoll-shim (the same
# library the linuxulator uses). Prefer the shim over a kqueue rewrite so the
# event-loop code stays identical across platforms.
find_path(EPOLL_SHIM_INCLUDE_DIR
    NAMES sys/epoll.h
    HINTS /usr/local/include/libepoll-shim
    REQUIRED)
find_library(EPOLL_SHIM_LIBRARY
    NAMES epoll-shim
    HINTS /usr/local/lib
    REQUIRED)

# Platform standard libraries
set(DUI_FREEBSD_LIBS pthread dl)

find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)

# Libraries required on FreeBSD
if(DUI_ENABLE_WAYLAND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WAYLAND=1)
    target_include_directories(${PROJECT_NAME} PRIVATE ${DUI_WAYLAND_INCLUDE_DIRS}
        ${EPOLL_SHIM_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_WAYLAND_LIBS} ${EPOLL_SHIM_LIBRARY} ${DUI_FREEBSD_LIBS} Freetype::Freetype Fontconfig::Fontconfig)
else()
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_FREEBSD_LIBS} ${X11_LIBRARIES} Freetype::Freetype Fontconfig::Fontconfig)
endif()
