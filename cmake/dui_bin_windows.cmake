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
    
    # Source files use UTF-8 encoding
    add_compile_options("/utf-8")
    
    # Get the number of logical CPU cores
    cmake_host_system_information(RESULT CPU_CORES QUERY NUMBER_OF_LOGICAL_CORES)
    # Set C/C++ compile options (enable multi-threaded compilation)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>)
endif()

# Use Unicode encoding on Windows
add_definitions(-DUNICODE -D_UNICODE) 

if(MSVC)
    # MSVC needs Debug/Release set explicitly; otherwise it creates Debug/Release subdirectories automatically
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
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
    # Using the CEF module: add the CEF source root to the include path
    include_directories(${DUI_CEF_SRC_ROOT_DIR})

    if(DUI_MULTI_CONFIG)
        # Per-config CEF paths via dui_target_cef_link_dirs (called after target creation below)
    else()
        # Path to the cef library (.lib path)
        link_directories("${DUI_CEF_LIB_PATH}")
    endif()
endif()

if(DUI_WEBVIEW2_EXE)    
    # WebView2 library path (.lib path)
    link_directories("${DUI_ROOT}/third_party/Microsoft.Web.WebView2/build/native/${DUI_SYSTEM_PROCESSOR}") 
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

# Per-config Skia / CEF link directories (multi-config generators: VS, Xcode)
dui_target_skia_link_dirs(${PROJECT_NAME})
if(DUI_ENABLE_CEF)
    dui_target_cef_link_dirs(${PROJECT_NAME})
endif()

# The manifest file path must be embedded
if(DUI_BITS_64)
    set(DUI_WIN_MANIFEST "${DUI_ROOT}/msvc/manifest/dui.x64.manifest")
else()
    set(DUI_WIN_MANIFEST "${DUI_ROOT}/msvc/manifest/dui.x86.manifest")
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
set(DUI_WINDOWS_LIBS Comctl32 Imm32 Opengl32 User32 shlwapi)

if(DUI_ENABLE_SDL)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        # Enable SDL support for windows
        target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_SDL=1)
    endif()
    
    # Add the SDL dependency libs
    list(APPEND DUI_WINDOWS_LIBS Version.lib Winmm.lib Setupapi.lib)
endif()

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

target_link_libraries(${PROJECT_NAME} ${DUI_LIBS} ${DUI_SDL_LIBS} ${DUI_SKIA_LIBS} ${DUI_CEF_LIBS} ${DUI_WINDOWS_LIBS})
