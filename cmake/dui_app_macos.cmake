# macOS platform half of cmake/dui_app.cmake.
# Sets dui::app's usage requirements, then defines dui_finalize_app_platform().
if(NOT DUI_OS_MACOS)
    message(FATAL_ERROR "NOT DUI_OS_MACOS!")
endif()

find_library(ACCELERATE Accelerate)
find_library(COREFOUNDATION CoreFoundation)
find_library(CORETEXT CoreText)
find_library(COREGRAPHICS CoreGraphics)

# Platform standard libraries
set(DUI_MACOS_LIBS pthread dl)

# ---- Usage requirements: everything an application gets by linking dui::app ----------

# Kept consistent with CEF's internal settings (this used to live in dui_bin_macos.cmake,
# in the branch that is not the CEF packaging path).
set(DUI_COMPILER_FLAGS
    -fno-strict-aliasing            # Avoid assumptions regarding non-aliasing of objects of different types
    -fstack-protector               # Protect some vulnerable functions from stack-smashing (security feature)
    -funwind-tables                 # Support stack unwinding for backtrace()
    -fvisibility=hidden             # Give hidden visibility to declarations that are not explicitly marked as visible
    -Wall                           # Enable all warnings
#   -Werror                         # Treat warnings as errors
    -Wno-unknown-pragmas
    -Wextra                         # Enable additional warnings
    -Wendif-labels                  # Warn whenever an #else or an #endif is followed by text
    -Wno-missing-field-initializers # Don't warn about missing field initializers
    -Wno-unused-parameter           # Don't warn about unused parameters
)
set(DUI_CXX_COMPILER_FLAGS
    -fvisibility-inlines-hidden     # Give hidden visibility to inlined class member functions
    -frtti
    -Wno-narrowing                  # Don't warn about type narrowing
    -Wsign-compare                  # Warn about mixed signed/unsigned type comparisons
    -Wno-unused-variable
)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND DUI_CXX_COMPILER_FLAGS -Wno-reorder)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND DUI_COMPILER_FLAGS
        -Wnewline-eof                   # Warn about no newline at end of file
    )
    list(APPEND DUI_CXX_COMPILER_FLAGS
        -fobjc-call-cxx-cdtors          # Call the constructor/destructor of C++ instance variables in ObjC objects
        -Wno-reorder-ctor
        -Wno-unused-lambda-capture
        -Wno-unused-private-field
    )
endif()

target_compile_options(dui_app INTERFACE
    ${DUI_COMPILER_FLAGS}
    "$<$<COMPILE_LANGUAGE:CXX>:${DUI_CXX_COMPILER_FLAGS}>"
)

# Mind the order: third-party libraries in dependency order, then the system libraries,
# then the explicit framework declarations last.
target_link_libraries(dui_app INTERFACE
    # Third-party libraries (in dependency order)
    dui dui-cximage dui-webp dui-png
    dui_skia_libs
    # System libraries
    ${ACCELERATE} ${COREFOUNDATION} ${CORETEXT} ${COREGRAPHICS} ${DUI_MACOS_LIBS}
    # Explicit framework declarations (must come last)
    "-framework AppKit" "-framework Foundation" "-framework Metal" "-framework Cocoa"
    "-framework QuartzCore" "-framework OpenGL"
)

# ---- Per-target work --------------------------------------------------------------

function(dui_finalize_app_platform _target)
    # The CEF packaging path on macOS is a retargeted copy of CEF's own cefclient
    # template: it creates the target itself, assembles the helper app bundles and signs
    # the result. It is not ours to restructure, so it is invoked here -- at the same
    # point the old layer invoked it -- and it owns everything from here on. Call
    # dui_finalize_app() with no target in that case, and do not call add_executable.
    if(DUI_ENABLE_CEF AND DEFINED DUI_THEME_DIR_NAME)
        # CMAKE_CURRENT_FUNCTION_LIST_DIR, not CMAKE_CURRENT_LIST_DIR: inside a function
        # the latter is the caller's directory (the example's), not this file's.
        include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/dui_cef_macos.cmake")
        return()
    endif()

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

    if(DUI_MULTI_CONFIG)
        set(_dui_config_dir "$<CONFIG>/")
    else()
        set(_dui_config_dir "")
    endif()

    # Keep the plain executable in a hidden staging dir (not in the bin top level) so the
    # bin folder only shows the assembled .app bundles. We do NOT delete it afterwards,
    # otherwise CMake would relink the target on every build (it would see its output
    # file missing).
    set_target_properties("${_target}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${DUI_BIN_PATH}/${_dui_config_dir}.bin"
    )

    # Build a plain executable first, then assemble the .app bundle in a POST_BUILD step.
    # This way the .app only appears once the target is actually built (no empty "shells"
    # for not-yet-built targets cluttering the bin folder, which MACOSX_BUNDLE would
    # otherwise create eagerly).
    #
    # Generate the Info.plist into a per-target temp location (NOT into bin), so no
    # bundle skeleton is created at configure time.
    # CMAKE_CURRENT_FUNCTION_LIST_DIR, not CMAKE_CURRENT_LIST_DIR: inside a function the
    # latter is the *caller's* directory, and this template lives next to this file.
    # (CMAKE_CURRENT_BINARY_DIR below is deliberately the caller's, so each application
    # gets its own generated plist.)
    set(_dui_plist_in "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/MacOSXBundleInfo.plist.in")
    set(_dui_plist_out "${CMAKE_CURRENT_BINARY_DIR}/${_target}.Info.plist")
    set(BUNDLE_EXECUTABLE "${_target}")
    set(BUNDLE_IDENTIFIER "com.dui.${_target}")
    set(BUNDLE_NAME "${_target}")
    set(BUNDLE_SHORT_VERSION "1.0")
    set(BUNDLE_VERSION "1.0")
    configure_file("${_dui_plist_in}" "${_dui_plist_out}" @ONLY)

    set(_dui_bundle_dir "${DUI_BIN_PATH}/${_dui_config_dir}${_target}.app")
    add_custom_command(TARGET "${_target}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_dui_bundle_dir}/Contents/MacOS"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${_target}>"
                "${_dui_bundle_dir}/Contents/MacOS/${_target}"
        COMMAND ${CMAKE_COMMAND} -E copy "${_dui_plist_out}"
                "${_dui_bundle_dir}/Contents/Info.plist"
        COMMAND ${CMAKE_COMMAND} -E rm -f "${_dui_bundle_dir}/Contents/MacOS/resources"
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${DUI_ROOT}/resources"
                "${_dui_bundle_dir}/Contents/MacOS/resources"
        # Ad-hoc code sign (Make/Ninja do not sign automatically, unlike Xcode);
        # otherwise Finder shows a "prohibited" icon even though it runs.
        COMMAND /usr/bin/codesign --force --deep --sign - "${_dui_bundle_dir}"
        COMMENT "Assembling ${_target}.app")
endfunction()
