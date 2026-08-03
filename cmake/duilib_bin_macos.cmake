# DUILIB_OS_MACOS
if(NOT DUILIB_OS_MACOS)
    message(FATAL_ERROR "NOT DUILIB_OS_MACOS!")
endif()

# macOS platform: find the required frameworks
find_library(ACCELERATE Accelerate)
find_library(COREFOUNDATION CoreFoundation)
find_library(CORETEXT CoreText)
find_library(COREGRAPHICS CoreGraphics)

# Platform standard libraries
set(DUILIB_MACOS_LIBS pthread dl)

if(DUILIB_ENABLE_CEF)
    # The CEF implementation on macOS is complex; related code is kept in a separate file
    include("${CMAKE_CURRENT_LIST_DIR}/duilib_cef_macos.cmake") 
else()
    # Keep the compiler arguments consistent with CEF's internal settings
    set(DUILIB_COMPILER_FLAGS
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
    set(DUILIB_CXX_COMPILER_FLAGS
        -fno-threadsafe-statics         # Don't generate thread-safe statics
        -fvisibility-inlines-hidden     # Give hidden visibility to inlined class member functions
        -frtti
        -Wno-narrowing                  # Don't warn about type narrowing
        -Wsign-compare                  # Warn about mixed signed/unsigned type comparisons
        -Wno-unused-variable
    )
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        list(APPEND DUILIB_CXX_COMPILER_FLAGS
            -Wno-reorder
          )
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        list(APPEND DUILIB_COMPILER_FLAGS
            -Wnewline-eof                   # Warn about no newline at end of file
          )
        list(APPEND DUILIB_CXX_COMPILER_FLAGS
            -fobjc-call-cxx-cdtors          # Call the constructor/destructor of C++ instance variables in ObjC objects
            -Wno-reorder-ctor
            -Wno-unused-lambda-capture
            -Wno-unused-private-field
          )
    endif()
    
    # Set the sources the executable depends on
    add_executable(${PROJECT_NAME} ${SRC_FILES})

    # Embedded resources dependency
    if(DEFINED DUILIB_EMBED_RES_SRC AND TARGET "${PROJECT_NAME}_embed_res")
        add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_embed_res")
    endif()

    # Generate C++ code from XML dependency
    if(TARGET "${PROJECT_NAME}_gen_xml_code")
        add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_gen_xml_code")
    endif()

    # Per-config Skia link directories (multi-config generators: VS, Xcode)
    duilib_target_skia_link_dirs(${PROJECT_NAME})

    # Set the compiler arguments
    target_compile_options(${PROJECT_NAME} PRIVATE ${DUILIB_COMPILER_FLAGS} ${DUILIB_CXX_COMPILER_FLAGS})
    
    # macOS platform: set the link libraries (mind the order!)
    target_link_libraries(${PROJECT_NAME}
                            # Third-party libraries (in dependency order)
                            ${DUILIB_LIBS} ${DUILIB_SDL_LIBS} ${DUILIB_SKIA_LIBS} ${DUILIB_CEF_LIBS}
                            # System libraries
                            ${ACCELERATE} ${COREFOUNDATION} ${CORETEXT} ${COREGRAPHICS} ${DUILIB_MACOS_LIBS}
                            # Explicit framework declarations (must come last)
                            "-framework AppKit" "-framework Foundation" "-framework Metal" "-framework Cocoa"
                          )
endif()
