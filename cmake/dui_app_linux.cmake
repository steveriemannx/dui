# Linux platform half of cmake/dui_app.cmake.
# Sets dui::app's usage requirements, then defines dui_finalize_app_platform().
if(NOT DUI_OS_LINUX)
    message(FATAL_ERROR "NOT DUI_OS_LINUX!")
endif()

# How the two halves divide, because it is not only "can this travel through a target?":
#
#   dui_app is created once -- by the first application that includes cmake/dui_app.cmake,
#   in that application's directory scope (see the guard in that file). Everything set on
#   it below therefore has to be true for *every* application of the configuration.
#
#   dui_finalize_app_platform() below runs once per application, in that application's own
#   scope, so anything an application can decide for itself belongs there. DUI_ENABLE_CEF
#   is exactly that: the cef/CefBrowser examples turn it on in their own scope
#   (cmake/dui_deps.cmake relies on the same per-scope evaluation), so putting the CEF
#   include path, link directory and libraries on the shared dui_app would apply them to
#   every other application and *not* to the one that asked for them.

# ---- Usage requirements: everything an application gets by linking dui::app ----------

find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)
if(NOT DUI_ENABLE_WAYLAND)
    find_package(X11 REQUIRED)
endif()

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

# Platform standard libraries
set(DUI_LINUX_LIBS X11 freetype fontconfig pthread dl freetype fontconfig)

# Wayland support
if(DUI_ENABLE_WAYLAND)
    target_compile_definitions(dui_app INTERFACE DUI_WAYLAND=1)
    target_include_directories(dui_app INTERFACE ${DUI_WAYLAND_INCLUDE_DIRS})
    # A standalone application build links dui by bare name, so the flag cannot come from
# the dui target's interface there; it is carried here as well. In-tree applications
# get it from both, which is harmless for a link option.
target_link_options(dui_app INTERFACE -Wl,--no-as-needed)

target_link_libraries(dui_app INTERFACE ${DUI_LIBS} ${DUI_WAYLAND_LIBS} ${DUI_SKIA_LIBS} freetype fontconfig pthread dl)
else()
    # ${DUI_CEF_LIBS} is added per application in dui_finalize_app_platform(): it contains
    # a bare "libcef.so", which only resolves against the matching link directory, so the
    # two are set together there (see the CEF block below).
    target_link_libraries(dui_app INTERFACE ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_LINUX_LIBS})
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

    # ---- CEF ----------------------------------------------------------------------
    # Per application: the cef/CefBrowser examples set DUI_ENABLE_CEF in their own scope.
    if(DUI_ENABLE_CEF AND NOT DUI_ENABLE_WAYLAND)
        # One per-application INTERFACE target carries the CEF usage requirements,
        # because the position of these libraries on the link line matters and a library
        # linked straight to the application is emitted in front of the ones dui::app
        # carries: the wrapper is a static archive (third_party/CMakeLists.txt builds
        # CEF's libcef_dll_wrapper target, and with BUILD_SHARED_LIBS unset that
        # add_library() call produces a static archive) and it is libdui.a that
        # references it, while GNU ld resolves an archive only against symbols that are
        # still undefined when it reaches it. So the wrapper has to come after dui --
        # the position the old single link line gave it. Libraries that arrive through an
        # INTERFACE target are emitted after dui::app's, and naming dui::app in this
        # target's interface keeps that true even if an application calls
        # dui_finalize_app() before its own target_link_libraries() line.
        #
        # "libcef.so" is in ${DUI_CEF_LIBS} as a bare name and the link directory below
        # is what resolves it -- they are a matched pair and stay together here.
        add_library("${_target}_cef" INTERFACE)
        target_link_libraries("${_target}_cef" INTERFACE dui::app ${DUI_CEF_LIBS})
        # Using the CEF module: add the CEF source root to the include path
        target_include_directories("${_target}_cef" INTERFACE ${DUI_CEF_SRC_ROOT_DIR})
        if(NOT DUI_MULTI_CONFIG)
            # Path to the cef library (.a/.so shared library path)
            target_link_directories("${_target}_cef" INTERFACE "${DUI_CEF_LIB_PATH}")
        endif()
        target_link_libraries("${_target}" PRIVATE "${_target}_cef")

        # Stage the CEF runtime (libcef.so, icudtl.dat, locales, ...) next to the
        # executable, mirroring the Windows layout: "<exe_dir>/cef_binary".
        # CefManager looks there at runtime, so the examples work regardless of the
        # build directory name (no manual copy).
        set_target_properties("${_target}" PROPERTIES
                              BUILD_RPATH "\$ORIGIN/cef_binary")

        add_custom_command(TARGET "${_target}" POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_directory
                                   "${DUI_CEF_SRC_ROOT_DIR}/Release"
                                   "$<TARGET_FILE_DIR:${_target}>/cef_binary"
                           COMMAND ${CMAKE_COMMAND} -E copy_directory
                                   "${DUI_CEF_SRC_ROOT_DIR}/Resources"
                                   "$<TARGET_FILE_DIR:${_target}>/cef_binary"
                           COMMENT "Copying CEF runtime to the executable directory")
    endif()
endfunction()
