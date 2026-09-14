# Windows platform half of cmake/dui_app.cmake.
# Sets dui::app's usage requirements, then defines dui_finalize_app_platform().
if(NOT DUI_OS_WINDOWS)
    message(FATAL_ERROR "NOT DUI_OS_WINDOWS!")
endif()

# How the two halves divide, because it is not only "can this travel through a target?":
#
#   dui_app is created once -- by the first application that includes cmake/dui_app.cmake,
#   in that application's directory scope (see the guard in that file). Everything set on
#   it below therefore has to be true for *every* application of the configuration.
#
#   dui_finalize_app_platform() below runs once per application, in that application's own
#   scope, so anything an application can decide for itself belongs there. DUI_ENABLE_CEF
#   and DUI_WEBVIEW2_EXE are exactly that: the cef/CefBrowser and WebView2 examples turn
#   them on in their own scope (cmake/dui_deps.cmake relies on the same per-scope
#   evaluation), so putting them on the shared dui_app would apply them to every other
#   application and *not* to the one that asked for them.

# ---- Usage requirements: everything an application gets by linking dui::app ----------

if(MSVC)
    # Get the number of logical CPU cores (/MP below is a usage requirement: it is part
    # of each application's own compile line).
    cmake_host_system_information(RESULT CPU_CORES QUERY NUMBER_OF_LOGICAL_CORES)

    target_compile_options(dui_app INTERFACE
        "/utf-8"
        $<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>
        $<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>
    )
endif()

# Settings for MinGW-w64 builds.
# The old layer prepended these to CMAKE_EXE_LINKER_FLAGS, a directory-scope variable;
# a link option on the one target every application links is the same thing here.
if(DUI_MINGW)
    # Generate a Windows GUI program instead of a console program
    set(_dui_mingw_link_options -mwindows)

    if(DUI_MINGW_STATIC)
        # Use static linking (prepended, as in the old flag string, so -static stays first)
        list(PREPEND _dui_mingw_link_options -static)
    endif()

    target_link_options(dui_app INTERFACE ${_dui_mingw_link_options})
endif()

# The switch for the UTF-16 ("wide") Windows APIs. Windows-only, and the same for every
# application, so it travels with dui_app. (It lived in the shared cmake/dui_bin.cmake,
# which this layer replaces; the dui library sets it for itself in src/CMakeLists.txt and
# an application's headers have to agree with it.)
target_compile_definitions(dui_app INTERFACE UNICODE _UNICODE)

# Libraries required on Windows
# dwmapi is needed for the OS-provided shadows (NativeWindow_Windows); MinGW/clang
# compilers ignore the #pragma comment(lib, "dwmapi.lib") directive, so link it here.
set(DUI_WINDOWS_LIBS Comctl32 Imm32 Opengl32 User32 shlwapi dwmapi)

if(DUI_USE_MAIN_ENTRY AND TARGET dui_entry)
    # A normal C++ main() is used; the dui_entry shim supplies the platform
    # WinMain/wWinMain and forwards to main() (Qt-like).
    list(APPEND DUI_WINDOWS_LIBS dui_entry)
endif()

# The libraries CEF and WebView2 need are per application and are linked to the
# application behind this list in dui_finalize_app_platform() (see the note at the top).
target_link_libraries(dui_app INTERFACE
    dui dui-cximage dui-webp dui-png
    dui_skia_libs ${DUI_WINDOWS_LIBS}
)

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
    # variable the old layer used) does not reach a target that already exists, so both it
    # and the per-configuration variants become target properties here.
    set_target_properties("${_target}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${DUI_BIN_PATH}"
    )

    if(MSVC)
        # Keep every configuration in a predictable, matching output directory.
        foreach(_dui_config ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER "${_dui_config}" _dui_config_upper)
            set_target_properties("${_target}" PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_${_dui_config_upper} "${DUI_BIN_PATH}/${_dui_config}"
            )
        endforeach()

        # MSVC compiler: set the C/C++ runtime library. Set as a target property rather
        # than as CMAKE_MSVC_RUNTIME_LIBRARY, which only initializes targets created
        # after it is set -- this one already exists.
        if("${DUI_MD}" STREQUAL "ON")
            # MD/MDd
            set_target_properties("${_target}" PROPERTIES
                MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
        else()
            # MT/MTd
            set_target_properties("${_target}" PROPERTIES
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()
    endif()

    # The manifest file path must be embedded
    if(DUI_BITS_64)
        set(DUI_WIN_MANIFEST "${DUI_ROOT}/cmake/manifest/dui.x64.manifest")
    else()
        set(DUI_WIN_MANIFEST "${DUI_ROOT}/cmake/manifest/dui.x86.manifest")
    endif()

    if(DUI_MINGW)
        # Settings for MinGW-w64 builds: add the *.rc file and configure the manifest file
        # Create the manifest.rc file
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${_target}.dir/${_target}_manifest.rc" "1 24 \"${DUI_WIN_MANIFEST}\"")
        target_sources("${_target}" PRIVATE "${DUI_WINRES_FILE_NAME}" "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${_target}.dir/${_target}_manifest.rc")
    elseif(DUI_WINRES_FILE_NAME)
        # Add the program's .rc file
        target_sources("${_target}" PRIVATE "${DUI_WINRES_FILE_NAME}")
    endif()

    if(MSVC)
        # Configure the manifest file
        target_sources("${_target}" PRIVATE
            ${DUI_WIN_MANIFEST}
        )

        # When building with MSVC, set the subsystem property
        set_target_properties("${_target}" PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup"
        )
    endif()

    # CEF and WebView2 are per-application switches -- the cef/CefBrowser and WebView2
    # examples set theirs in their own scope, which is also what makes cmake/dui_deps.cmake
    # download their SDKs -- so their usage requirements are carried by a per-application
    # INTERFACE target instead of by the shared dui_app. That also keeps their libraries
    # behind the ones dui::app carries, which is where the old single link line had them:
    # a library linked straight to the application is emitted in front of dui::app's, and
    # on MinGW (GNU ld) a library that arrives too early is skipped rather than resolving
    # the references the dui library makes to it.
    if(DUI_ENABLE_CEF OR DUI_WEBVIEW2_EXE)
        add_library("${_target}_dui_extra" INTERFACE)
        # Naming dui::app keeps the order even if an application calls
        # dui_finalize_app() before its own target_link_libraries() line.
        target_link_libraries("${_target}_dui_extra" INTERFACE dui::app)
    endif()

    # ---- CEF ----------------------------------------------------------------------
    if(DUI_ENABLE_CEF)
        # CEF headers are private to this executable.
        target_include_directories("${_target}_dui_extra" INTERFACE ${DUI_CEF_SRC_ROOT_DIR})

        if(DUI_MULTI_CONFIG)
            target_link_directories("${_target}_dui_extra" INTERFACE
                "$<$<CONFIG:Debug>:${DUI_CEF_LIB_PATH_DEBUG}>"
                "$<$<NOT:$<CONFIG:Debug>>:${DUI_CEF_LIB_PATH_RELEASE}>"
            )
        else()
            target_link_directories("${_target}_dui_extra" INTERFACE "${DUI_CEF_LIB_PATH}")
        endif()

        target_link_libraries("${_target}_dui_extra" INTERFACE ${DUI_CEF_LIBS})

        # Make the CEF feature switch explicit (matches the VS flow's DUI_CEF=$(LibCefEnabled)):
        # without it, dui_config.h defaults DUI_BUILD_FOR_CEF to enabled on Windows.
        # Note: the dui library itself always compiles the CEF code (the cef examples
        # link against it), so this only affects the examples' own compilation.
        target_compile_definitions("${_target}" PRIVATE DUI_CEF=1)

        if(MSVC)
            # Set libcef.dll to load lazily (delay-load)
            target_link_options("${_target}" PRIVATE
                "/DELAYLOAD:libcef.dll"  # specify the delay-loaded DLL file name
            )
        endif()

        # Stage the CEF runtime once during configure. CefManager_Windows looks for
        # "<exe_dir>/cef_binary"; all CEF examples share the same runtime directory.
        # GLOBAL property, not a per-target one: the staging is shared by every example.
        get_property(_dui_cef_runtime_staged GLOBAL PROPERTY DUI_CEF_RUNTIME_STAGED)
        if(NOT _dui_cef_runtime_staged)
            foreach(_dui_config Debug Release)
                set(_dui_cef_runtime_dir "${DUI_BIN_PATH}/${_dui_config}/cef_binary")
                file(MAKE_DIRECTORY "${_dui_cef_runtime_dir}")
                file(COPY "${DUI_CEF_SRC_ROOT_DIR}/Release/."
                     DESTINATION "${_dui_cef_runtime_dir}")
                file(COPY "${DUI_CEF_SRC_ROOT_DIR}/Resources/."
                     DESTINATION "${_dui_cef_runtime_dir}")
            endforeach()
            set_property(GLOBAL PROPERTY DUI_CEF_RUNTIME_STAGED TRUE)
            message(STATUS "Copied CEF runtime to Debug and Release output directories")
        endif()
    else()
        target_compile_definitions("${_target}" PRIVATE DUI_CEF=0)
    endif()

    # ---- WebView2 -----------------------------------------------------------------
    if(DUI_WEBVIEW2_EXE)
        target_link_directories("${_target}_dui_extra" INTERFACE
            "${DUI_ROOT}/third_party/Microsoft.Web.WebView2/build/native/${DUI_SYSTEM_PROCESSOR}")

        # Add the WebView2 dependency libs
        target_link_libraries("${_target}_dui_extra" INTERFACE WinInet.lib WebView2Loader.dll.lib)

        # Define the macro to enable WebView2
        target_compile_definitions("${_target}" PRIVATE DUI_WEBVIEW2=1)

        # Copy WebView2Loader.dll to the bin directory
        add_custom_command(TARGET "${_target}" POST_BUILD  # executed after the target is built
                           COMMAND ${CMAKE_COMMAND} -E copy
                                   "${DUI_ROOT}/third_party/Microsoft.Web.WebView2/build/native/${DUI_SYSTEM_PROCESSOR}/WebView2Loader.dll"
                                   "$<TARGET_FILE_DIR:${_target}>/WebView2Loader.dll"
                           COMMENT "Copying WebView2Loader.dll to runtime directory"
                          )
    endif()

    if(DUI_ENABLE_CEF OR DUI_WEBVIEW2_EXE)
        target_link_libraries("${_target}" PRIVATE "${_target}_dui_extra")
    endif()
endfunction()
