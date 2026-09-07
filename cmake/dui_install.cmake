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
        set(DUI_INSTALL_VERSION 0.1.0)
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

    foreach(_dui_dependency dui-cximage dui-webp dui-png dui-zlib)
        if(TARGET "${_dui_dependency}")
            target_link_libraries(dui "${_dui_dependency}")
        endif()
    endforeach()

    if(WIN32)
        target_link_libraries(dui
            user32 gdi32 shell32 ole32 oleaut32 imm32 comctl32 dwmapi version winmm ws2_32
        )
    elseif(APPLE)
        target_link_libraries(dui
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
    file(GLOB _dui_zlib_headers
        "${_dui_source_root}/third_party/zlib/*.h"
    )
    install(FILES ${_dui_zlib_headers}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/dui/third_party/zlib"
    )

    set(_dui_install_targets dui dui_entry)
    foreach(_dui_dependency dui-cximage dui-webp png_static dui-zlib)
        if(TARGET "${_dui_dependency}")
            list(APPEND _dui_install_targets "${_dui_dependency}")
        endif()
    endforeach()
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
    write_basic_package_version_file(
        "${_dui_package_dir}/duiConfigVersion.cmake"
        VERSION "${DUI_INSTALL_VERSION}"
        COMPATIBILITY SameMajorVersion
    )
    install(FILES
        "${_dui_package_dir}/duiConfig.cmake"
        "${_dui_package_dir}/duiConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/dui"
    )
endfunction()
