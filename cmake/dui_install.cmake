# Reusable install/package rules for the dui targets.
#
# Include this file after add_subdirectory(src), then call:
#   dui_install_package()
# It intentionally does not add targets or alter native backend configuration.

include_guard(GLOBAL)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

function(dui_install_package)
    set(options)
    set(oneValueArgs
        NAMESPACE
        VERSION
        RESOURCE_SOURCE_DIR
    )
    cmake_parse_arguments(DUI_INSTALL "${options}" "${oneValueArgs}" "" ${ARGN})

    get_filename_component(_dui_source_root
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)

    if(NOT DUI_INSTALL_NAMESPACE)
        set(DUI_INSTALL_NAMESPACE dui::)
    endif()
    if(NOT DUI_INSTALL_VERSION)
        # No hardcoded fallback: the version lives in exactly one place, the
        # top-level project(dui VERSION ...) call. A caller that declares no
        # version of its own gets that one; a caller that declares none at all
        # is a configuration mistake worth failing on, not worth guessing at.
        if(NOT PROJECT_VERSION)
            message(FATAL_ERROR
                "dui_install_package() needs a version: pass VERSION <x.y.z> "
                "or declare one in the top-level project(... VERSION ...)")
        endif()
        set(DUI_INSTALL_VERSION "${PROJECT_VERSION}")
    endif()
    if(NOT DUI_INSTALL_RESOURCE_SOURCE_DIR)
        set(DUI_INSTALL_RESOURCE_SOURCE_DIR "${_dui_source_root}/resources")
    endif()

    if(NOT TARGET dui)
        message(FATAL_ERROR "dui_install_package() requires the dui target")
    endif()
    if(NOT TARGET dui_entry)
        message(FATAL_ERROR "dui_install_package() requires the dui_entry target")
    endif()
    if(NOT IS_DIRECTORY "${DUI_INSTALL_RESOURCE_SOURCE_DIR}")
        message(FATAL_ERROR "dui resource directory does not exist: ${DUI_INSTALL_RESOURCE_SOURCE_DIR}")
    endif()

    # Public headers include each other as dui/...; make both build-tree and
    # installed-tree consumers resolve that same include spelling.
    target_include_directories(dui PUBLIC
        "$<BUILD_INTERFACE:${_dui_source_root}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
    target_include_directories(dui_entry PUBLIC
        "$<BUILD_INTERFACE:${_dui_source_root}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    # The vendored dependency libraries, under the dui-* names third_party/CMakeLists.txt
    # publishes. dui-png is an alias for libpng's png_static, which sets EXPORT_NAME
    # dui-png so the installed package exports dui::dui-png rather than the upstream
    # target name. This one list feeds both the linking and the install(EXPORT) set, so
    # the two cannot drift apart again.
    set(_dui_dependencies dui-cximage dui-webp dui-png dui-zlib)
    set(_dui_dependency_targets)
    foreach(_dui_dependency ${_dui_dependencies})
        if(NOT TARGET "${_dui_dependency}")
            continue()
        endif()
        # PRIVATE rather than unqualified (which is PUBLIC): this is an
        # implementation detail of the static library, not part of its interface.
        # Consumers keep the archives on their link line either way -- CMake
        # propagates a static library's private dependencies as $<LINK_ONLY:...>
        # -- but they no longer inherit the dependency's own usage requirements
        # (include directories, compile definitions, flags).
        target_link_libraries(dui PRIVATE "${_dui_dependency}")
        # install(TARGETS) rejects an ALIAS, so dereference to the target it points
        # at (dui-png -> png_static) when assembling the install set; the export puts
        # libpng back under its dui-png name via EXPORT_NAME.
        get_target_property(_dui_aliased_target "${_dui_dependency}" ALIASED_TARGET)
        if(_dui_aliased_target)
            list(APPEND _dui_dependency_targets "${_dui_aliased_target}")
        else()
            list(APPEND _dui_dependency_targets "${_dui_dependency}")
        endif()
    endforeach()

    # System libraries are PRIVATE for the same reason. Note that this is not about
    # the link line -- a static library's dependencies reach the consumer's link
    # line as $<LINK_ONLY:...> whether they are PRIVATE or PUBLIC, macOS
    # "-framework ..." strings included. What PRIVATE stops is the interface
    # spreading: with PUBLIC, every consumer also inherits the dependency's usage
    # requirements and the export file advertises it as part of dui's interface.
    if(WIN32)
        target_link_libraries(dui PRIVATE
            user32 gdi32 shell32 ole32 oleaut32 imm32 comctl32 dwmapi version winmm ws2_32
        )
    elseif(APPLE)
        target_link_libraries(dui PRIVATE
            "-framework AppKit"
            "-framework Foundation"
            "-framework Metal"
            "-framework Cocoa"
            "-framework QuartzCore"
            "-framework OpenGL"
        )
    endif()

    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/dui/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/dui"
    )
    install(DIRECTORY "${DUI_INSTALL_RESOURCE_SOURCE_DIR}/"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/dui/resources"
    )
    # Listed rather than globbed, like every other source/header list in this project:
    # a glob here decides what a consumer of the installed package can #include, and its
    # failure mode is a header silently missing from the install tree.
    install(FILES
        "${_dui_source_root}/third_party/zlib/crc32.h"
        "${_dui_source_root}/third_party/zlib/deflate.h"
        "${_dui_source_root}/third_party/zlib/gzguts.h"
        "${_dui_source_root}/third_party/zlib/inffast.h"
        "${_dui_source_root}/third_party/zlib/inffixed.h"
        "${_dui_source_root}/third_party/zlib/inflate.h"
        "${_dui_source_root}/third_party/zlib/inftrees.h"
        "${_dui_source_root}/third_party/zlib/trees.h"
        "${_dui_source_root}/third_party/zlib/zconf_msvc.h"
        "${_dui_source_root}/third_party/zlib/zlib.h"
        "${_dui_source_root}/third_party/zlib/zutil.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/dui/third_party/zlib"
    )

    # ---- Skia ----------------------------------------------------------------------
    # Skia is compiled by this project (gn + ninja, see cmake/dui_deps.cmake) rather than
    # taken from the system, so an installed package has to carry its archives too. dui is
    # a static library: it does not resolve their symbols, and nothing else in the package
    # provides them -- without these, find_package(dui) succeeds, dui::dui resolves, and
    # every program that actually calls into dui fails to link with sk_*/Sk* undefined.
    #
    # Only DUI_SKIA_LIBS is installed, which is exactly the set the in-tree applications
    # link (verified against a generated link.txt); the gn build produces four more
    # archives that nothing references.
    if(MSVC)
        set(_dui_skia_prefix "")
        set(_dui_skia_suffix ".lib")
    else()
        set(_dui_skia_prefix "lib")
        set(_dui_skia_suffix ".a")
    endif()

    # The paths are computed, not discovered. Skia is produced by the *build* (the
    # dui_skia target runs gn + ninja at make time), so on a fresh tree the archives do
    # not exist yet while this configure is running -- testing for them here would fail
    # every first configure. install(FILES) checks existence at install time instead,
    # which is exactly when they are supposed to be there, and names the missing archive
    # if the build was not run first.
    #
    # Multi-config generators build Skia per configuration into separate directories and
    # both would claim the same file name, so each configuration is installed into its
    # own subdirectory and the import below selects by $<CONFIG>, the same way
    # dui_skia_libs does in the build tree.
    set(DUI_SKIA_IMPORT_CODE "")
    if(DUI_MULTI_CONFIG)
        foreach(_cfg Debug Release)
            if(_cfg STREQUAL "Debug")
                set(_dui_skia_dir "${DUI_SKIA_LIB_PATH_DEBUG}")
            else()
                set(_dui_skia_dir "${DUI_SKIA_LIB_PATH_RELEASE}")
            endif()
            set(_dui_skia_files)
            foreach(_lib ${DUI_SKIA_LIBS})
                set(_f "${_dui_skia_dir}/${_dui_skia_prefix}${_lib}${_dui_skia_suffix}")
                list(APPEND _dui_skia_files "${_f}")
                get_filename_component(_name "${_f}" NAME)
                string(APPEND DUI_SKIA_IMPORT_CODE
                    "        \"$<$<CONFIG:${_cfg}>:\${PACKAGE_PREFIX_DIR}/${CMAKE_INSTALL_LIBDIR}/${_cfg}/${_name}>\"\n")
            endforeach()
            # CONFIGURATIONS keeps a single-configuration install from demanding the other
            # configuration's archives.
            install(FILES ${_dui_skia_files}
                    DESTINATION "${CMAKE_INSTALL_LIBDIR}/${_cfg}"
                    CONFIGURATIONS ${_cfg})
        endforeach()
        set(DUI_SKIA_IMPORT_CODE
            "        # Each line names one configuration; the generator expression picks the\n"
            "        # one being built, and only installed configurations exist.\n${DUI_SKIA_IMPORT_CODE}")
    else()
        set(_dui_skia_files)
        foreach(_lib ${DUI_SKIA_LIBS})
            set(_f "${DUI_SKIA_LIB_PATH}/${_dui_skia_prefix}${_lib}${_dui_skia_suffix}")
            list(APPEND _dui_skia_files "${_f}")
            get_filename_component(_name "${_f}" NAME)
            string(APPEND DUI_SKIA_IMPORT_CODE
                "        \"\${PACKAGE_PREFIX_DIR}/${CMAKE_INSTALL_LIBDIR}/${_name}\"\n")
        endforeach()
        install(FILES ${_dui_skia_files} DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    endif()

    if(NOT DUI_SKIA_LIBS)
        message(FATAL_ERROR "DUI_SKIA_LIBS is empty: nothing would be installed to link against")
    endif()

    set(_dui_install_targets dui dui_entry ${_dui_dependency_targets})
    install(TARGETS ${_dui_install_targets}
        EXPORT duiTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
    install(EXPORT duiTargets
        FILE duiTargets.cmake
        NAMESPACE "${DUI_INSTALL_NAMESPACE}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dui"
    )

    set(_dui_package_dir "${CMAKE_CURRENT_BINARY_DIR}/dui-package")
    file(MAKE_DIRECTORY "${_dui_package_dir}")
    configure_package_config_file(
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/duiConfig.cmake.in"
        "${_dui_package_dir}/duiConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dui"
    )
    # The application-facing half of the package: dui::app and dui_finalize_app(), so a
    # consumer writes what an in-tree example writes. It is a separate, deliberately
    # thinner file than the build tree's cmake/dui_app.cmake -- see its header for why it
    # must not include dui_common.cmake.
    configure_file(
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/dui_app_install.cmake.in"
        "${_dui_package_dir}/dui_app.cmake"
        @ONLY
    )
    write_basic_package_version_file(
        "${_dui_package_dir}/duiConfigVersion.cmake"
        VERSION "${DUI_INSTALL_VERSION}"
        COMPATIBILITY SameMajorVersion
    )
    install(FILES
        "${_dui_package_dir}/duiConfig.cmake"
        "${_dui_package_dir}/duiConfigVersion.cmake"
        "${_dui_package_dir}/dui_app.cmake"
        # The bundle template dui_finalize_app() uses on macOS.
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/MacOSXBundleInfo.plist.in"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dui"
    )
endfunction()
