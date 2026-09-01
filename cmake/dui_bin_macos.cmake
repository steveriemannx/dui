# DUI_OS_MACOS
if(NOT DUI_OS_MACOS)
    message(FATAL_ERROR "NOT DUI_OS_MACOS!")
endif()

# macOS platform: find the required frameworks
find_library(ACCELERATE Accelerate)
find_library(COREFOUNDATION CoreFoundation)
find_library(CORETEXT CoreText)
find_library(COREGRAPHICS CoreGraphics)

# Platform standard libraries
set(DUI_MACOS_LIBS pthread dl)

# macOS uses the native (Cocoa/AppKit) backend only; SDL is not supported
# on this platform, so SDL3 is never linked here.

if(DUI_ENABLE_CEF)
    # The CEF implementation on macOS is complex; related code is kept in a separate file
    include("${CMAKE_CURRENT_LIST_DIR}/dui_cef_macos.cmake") 
else()
    # Keep the compiler arguments consistent with CEF's internal settings
    set(DUI_COMPILER_FLAGS
        -fno-strict-aliasing            # Avoid assumptions regarding non-aliasing of objects of different types
        -fstack-protector               # Protect some vulnerable functions from stack-smashing (security feature)
        -funwind-tables                 # Support stack unwinding for backtrace()
        -fvisibility=hidden             # Give hidden visibility to declarations that are not explicitly marked as visible
        -Wall                           # Enable all warnings
#        -Werror                         # Treat warnings as errors
        -Wno-unknown-pragmas
        -Wextra                         # Enable additional warnings
        -Wendif-labels                  # Warn whenever an #else or an #endif is followed by text
        -Wno-missing-field-initializers # Don't warn about missing field initializers
        -Wno-unused-parameter           # Don't warn about unused parameters
    )
    set(DUI_CXX_COMPILER_FLAGS
        -fno-threadsafe-statics         # Don't generate thread-safe statics
        -fvisibility-inlines-hidden     # Give hidden visibility to inlined class member functions
        -frtti
        -Wno-narrowing                  # Don't warn about type narrowing
        -Wsign-compare                  # Warn about mixed signed/unsigned type comparisons
        -Wno-unused-variable
    )
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        list(APPEND DUI_CXX_COMPILER_FLAGS
            -Wno-reorder
          )
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
    
    # Build a plain executable first, then assemble the .app bundle in a
    # POST_BUILD step. This way the .app only appears once the target is
    # actually built (no empty "shells" for not-yet-built targets cluttering
    # the bin folder, which MACOSX_BUNDLE would otherwise create eagerly).
    add_executable(${PROJECT_NAME} ${SRC_FILES})

    # Keep the plain executable in a hidden staging dir (not in the bin top
    # level) so the bin folder only shows the assembled .app bundles. We do
    # NOT delete it afterwards, otherwise CMake would relink the target on
    # every build (it would see its output file missing).
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${DUI_BIN_PATH}/.bin")

    # Generate the Info.plist into a per-target temp location (NOT into bin),
    # so no bundle skeleton is created at configure time.
    set(_dui_plist_in "${CMAKE_CURRENT_LIST_DIR}/MacOSXBundleInfo.plist.in")
    set(_dui_plist_out "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.Info.plist")
    set(BUNDLE_EXECUTABLE "${PROJECT_NAME}")
    set(BUNDLE_IDENTIFIER "com.dui.${PROJECT_NAME}")
    set(BUNDLE_NAME "${PROJECT_NAME}")
    set(BUNDLE_SHORT_VERSION "1.0")
    set(BUNDLE_VERSION "1.0")
    configure_file("${_dui_plist_in}" "${_dui_plist_out}" @ONLY)

    set(_dui_bundle_dir "${DUI_BIN_PATH}/${PROJECT_NAME}.app")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_dui_bundle_dir}/Contents/MacOS"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>"
                "${_dui_bundle_dir}/Contents/MacOS/${PROJECT_NAME}"
        COMMAND ${CMAKE_COMMAND} -E copy "${_dui_plist_out}"
                "${_dui_bundle_dir}/Contents/Info.plist"
        COMMAND ${CMAKE_COMMAND} -E rm -f "${_dui_bundle_dir}/Contents/MacOS/resources"
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${DUI_ROOT}/resources"
                "${_dui_bundle_dir}/Contents/MacOS/resources"
        # Ad-hoc code sign (Make/Ninja do not sign automatically, unlike Xcode);
        # otherwise Finder shows a "prohibited" icon even though it runs.
        COMMAND /usr/bin/codesign --force --deep --sign - "${_dui_bundle_dir}"
        COMMENT "Assembling ${PROJECT_NAME}.app")

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

    # Set the compiler arguments
    target_compile_options(${PROJECT_NAME} PRIVATE ${DUI_COMPILER_FLAGS} ${DUI_CXX_COMPILER_FLAGS})
    
    # macOS platform: set the link libraries (mind the order!)
    target_link_libraries(${PROJECT_NAME}
                            # Third-party libraries (in dependency order)
                            ${DUI_LIBS} ${DUI_SKIA_LIBS} ${DUI_CEF_LIBS}
                            # System libraries
                            ${ACCELERATE} ${COREFOUNDATION} ${CORETEXT} ${COREGRAPHICS} ${DUI_MACOS_LIBS}
                            # Explicit framework declarations (must come last)
                            "-framework AppKit" "-framework Foundation" "-framework Metal" "-framework Cocoa"
                            "-framework QuartzCore" "-framework OpenGL"
                          )
endif()
