# FreeBSD platform half of cmake/dui_app.cmake.
# Sets dui::app's usage requirements, then defines dui_finalize_app_platform().
if(NOT DUI_OS_FREEBSD)
    message(FATAL_ERROR "NOT DUI_OS_FREEBSD!")
endif()

# Nothing here is per-application: FreeBSD has no CEF (see src/CMakeLists.txt) and the
# native backend is a single global choice (DUI_ENABLE_WAYLAND), so every usage
# requirement can travel through the shared dui_app target. The finalize function is
# still where the target-only properties go, because it runs after the application has
# created its target.

# ---- Usage requirements: everything an application gets by linking dui::app ----------

# Per-config Skia link directories (multi-config generators: VS, Xcode).
# This is the dui_target_skia_link_dirs() macro from cmake/dui_bin.cmake, which the app
# entry point does not include. The link line further down names the Skia archives by
# bare name (${DUI_SKIA_LIBS}), so this search path is what resolves them.
if(DUI_MULTI_CONFIG)
    target_link_directories(dui_app INTERFACE
        "$<$<CONFIG:Debug>:${DUI_SKIA_LIB_PATH_DEBUG}>"
        "$<$<NOT:$<CONFIG:Debug>>:${DUI_SKIA_LIB_PATH_RELEASE}>"
    )
else()
    target_link_directories(dui_app INTERFACE "${DUI_SKIA_LIB_PATH}")
endif()

# FreeBSD keeps third-party libs in /usr/local/lib; the Wayland/GL stack is
# linked by bare -l name, so the target needs this search path explicitly.
# target_link_directories() acts on the target itself and is therefore not
# affected by directory-scope ordering the way link_directories() was.
if(EXISTS "/usr/local/lib")
    target_link_directories(dui_app INTERFACE /usr/local/lib)
endif()
# /usr/local/include stays on a *target* rather than a directory: a directory-scoped
# include here also reaches dui_skia, where the system expat.h shadows Skia's bundled
# copy. INTERFACE on dui_app reaches exactly the applications that link it, which is the
# same set the old PRIVATE include on the example target reached.
if(EXISTS "/usr/local/include")
    target_include_directories(dui_app INTERFACE /usr/local/include)
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
    target_compile_definitions(dui_app INTERFACE DUI_WAYLAND=1)
    target_include_directories(dui_app INTERFACE ${DUI_WAYLAND_INCLUDE_DIRS}
        ${EPOLL_SHIM_INCLUDE_DIR})
    # A standalone application build links dui by bare name, so the flag cannot come from
# the dui target's interface there; it is carried here as well. In-tree applications
# get it from both, which is harmless for a link option.
target_link_options(dui_app INTERFACE -Wl,--no-as-needed)

target_link_libraries(dui_app INTERFACE ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_WAYLAND_LIBS} ${EPOLL_SHIM_LIBRARY} ${DUI_FREEBSD_LIBS} Freetype::Freetype Fontconfig::Fontconfig)
else()
    find_package(X11 REQUIRED)
    target_link_libraries(dui_app INTERFACE ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_FREEBSD_LIBS} ${X11_LIBRARIES} Freetype::Freetype Fontconfig::Fontconfig)
endif()

# ---- Per-target work --------------------------------------------------------------

function(dui_finalize_app_platform _target)
    if(NOT TARGET "${_target}")
        message(FATAL_ERROR
            "dui_finalize_app: '${_target}' is not a target. Create it with "
            "add_executable() before calling this.")
    endif()

    # Example-only headers stay private to the target, and generated includes are
    # target-local so one application's generated files cannot leak into another.
    target_include_directories("${_target}" PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        ${DUI_GENERATED_INCLUDE_DIRS}
    )

    if(TARGET dui_skia)
        add_dependencies("${_target}" dui_skia)
    endif()
    if(TARGET "${_target}_gen_xml_code")
        add_dependencies("${_target}" "${_target}_gen_xml_code")
    endif()
    if(TARGET "${_target}_embed_res")
        add_dependencies("${_target}" "${_target}_embed_res")
    endif()

    # Set the executable output directory. CMAKE_RUNTIME_OUTPUT_DIRECTORY (the directory
    # variable the old layer used) does not reach a target that already exists, so it
    # becomes a target property here.
    set_target_properties("${_target}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${DUI_BIN_PATH}"
    )
endfunction()
