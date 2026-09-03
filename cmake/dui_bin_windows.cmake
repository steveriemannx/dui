# DUI_OS_WINDOWS
if(NOT DUI_OS_WINDOWS)
    message(FATAL_ERROR "NOT DUI_OS_WINDOWS!")
endif()

if(MSVC)
    # MSVC compiler: set the C/C++ runtime library
    if("${DUI_MD}" STREQUAL "ON")
        # MD/MDd
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
    else()
        # MT/MTd
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
    
    # Get the number of logical CPU cores
    cmake_host_system_information(RESULT CPU_CORES QUERY NUMBER_OF_LOGICAL_CORES)
endif()

if(MSVC)
    # Keep every configuration in a predictable, matching output directory.
    foreach(_config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${_config}" _config_upper)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_config_upper} "${DUI_BIN_PATH}/${_config}")
    endforeach()
endif()

# Settings for MinGW-w64 builds
if(DUI_MINGW)
    # Generate a Windows GUI program instead of a console program
    set(CMAKE_EXE_LINKER_FLAGS "-mwindows ${CMAKE_EXE_LINKER_FLAGS}")
    
    if(DUI_MINGW_STATIC)
        # Use static linking
        set(CMAKE_EXE_LINKER_FLAGS "-static ${CMAKE_EXE_LINKER_FLAGS}")
    endif()
endif()

if(DUI_ENABLE_CEF)
    # CEF headers are private to this executable.
endif()

if(DUI_WEBVIEW2_EXE)    
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

if(DUI_ENABLE_CEF)
    target_include_directories(${PROJECT_NAME} PRIVATE ${DUI_CEF_SRC_ROOT_DIR})
    if(DUI_MULTI_CONFIG)
        target_link_directories(${PROJECT_NAME} PRIVATE
            "$<$<CONFIG:Debug>:${DUI_CEF_LIB_PATH_DEBUG}>"
            "$<$<NOT:$<CONFIG:Debug>>:${DUI_CEF_LIB_PATH_RELEASE}>"
        )
    else()
        target_link_directories(${PROJECT_NAME} PRIVATE "${DUI_CEF_LIB_PATH}")
    endif()
endif()
if(DUI_WEBVIEW2_EXE)
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${DUI_ROOT}/third_party/Microsoft.Web.WebView2/build/native/${DUI_SYSTEM_PROCESSOR}")
endif()
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        "/utf-8"
        $<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>
        $<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>
    )
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
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}.dir/${PROJECT_NAME}_manifest.rc" "1 24 \"${DUI_WIN_MANIFEST}\"")
    target_sources(${PROJECT_NAME} PRIVATE "${DUI_WINRES_FILE_NAME}" "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}.dir/${PROJECT_NAME}_manifest.rc")
elseif(DUI_WINRES_FILE_NAME)
    # Add the program's .rc file
    target_sources(${PROJECT_NAME} PRIVATE "${DUI_WINRES_FILE_NAME}")
endif()

if(MSVC)
    # Configure the manifest file
    target_sources(${PROJECT_NAME} PRIVATE 
        ${DUI_WIN_MANIFEST}
    )
    
    # When building with MSVC, set the subsystem property
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup"
    )
    
    if(DUI_ENABLE_CEF)
        # Set libcef.dll to load lazily (delay-load)
        target_link_options(${PROJECT_NAME} PRIVATE
            "/DELAYLOAD:libcef.dll"  # specify the delay-loaded DLL file name
        )
    endif()
endif()

# Libraries required on Windows
# dwmapi is needed for the OS-provided shadows (NativeWindow_Windows); MinGW/clang
# compilers ignore the #pragma comment(lib, "dwmapi.lib") directive, so link it here.
set(DUI_WINDOWS_LIBS Comctl32 Imm32 Opengl32 User32 shlwapi dwmapi)

if(DUI_WEBVIEW2_EXE)
    # Define the macro to enable WebView2
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WEBVIEW2=1)
    
    # Copy WebView2Loader.dll to the bin directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD  # executed after the target is built
                       COMMAND ${CMAKE_COMMAND} -E copy
                               "${DUI_ROOT}/third_party/Microsoft.Web.WebView2/build/native/${DUI_SYSTEM_PROCESSOR}/WebView2Loader.dll"
                               "$<TARGET_FILE_DIR:${PROJECT_NAME}>/WebView2Loader.dll"
                       COMMENT "Copying WebView2Loader.dll to runtime directory"
                      )
    
    # Add the WebView2 dependency libs
    list(APPEND DUI_WINDOWS_LIBS WinInet.lib WebView2Loader.dll.lib)
endif()

# Make the CEF feature switch explicit (matches the VS flow's DUI_CEF=$(LibCefEnabled)):
# without it, dui_config.h defaults DUI_BUILD_FOR_CEF to enabled on Windows.
# Note: the dui library itself always compiles the CEF code (the cef examples
# link against it), so this only affects the examples' own compilation.
if(DUI_ENABLE_CEF)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_CEF=1)

    # Stage the CEF runtime (libcef.dll, icudtl.dat, locales, ...) next to the
    # executable. CefManager_Windows looks for "<exe_dir>/cef_binary"; this makes
    # cef/CefBrowser examples run from any build directory name (no manual copy).
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                               "${DUI_CEF_SRC_ROOT_DIR}/Release"
                               "$<TARGET_FILE_DIR:${PROJECT_NAME}>/cef_binary"
                       COMMENT "Copying CEF runtime to the output directory (cef_binary/)")
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_CEF=0)
endif()

if(DUI_USE_MAIN_ENTRY AND TARGET dui_entry)
    # A normal C++ main() is used; the dui_entry shim supplies the platform
    # WinMain/wWinMain and forwards to main() (Qt-like).
    list(APPEND DUI_WINDOWS_LIBS dui_entry)
endif()

target_link_libraries(${PROJECT_NAME} dui dui-cximage dui-webp png_static
    dui_skia_libs ${DUI_CEF_LIBS} ${DUI_WINDOWS_LIBS})
