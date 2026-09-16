# Register all examples in the top-level build (55 on Linux / 53 on Windows; varies with platform conditions)
# Three modes: XML mode (19 original examples) + mode 2 XML-to-code generation (*_gen) + mode 3 pure code (*_code)

# ---- Compile-time mode selection ----
# DUI_EXAMPLES_MODE selects which development-mode examples to build, avoiding a full build every time:
#   ALL  - build all three modes (default)
#   XML  - mode 1 only: XML layout (parsed at runtime)
#   GEN  - mode 2 only: generate C++ code from XML at build time (*_gen)
#   CODE - mode 3 only: pure code (*_code)
set(DUI_EXAMPLES_MODE "ALL" CACHE STRING "Which example mode to build: ALL/XML/GEN/CODE")
if(NOT DUI_EXAMPLES_MODE MATCHES "^(ALL|XML|GEN|CODE)$")
    message(FATAL_ERROR "DUI_EXAMPLES_MODE must be ALL/XML/GEN/CODE, got: ${DUI_EXAMPLES_MODE}")
endif()

# CEF and WebView2 are the only examples that download large binary distributions at
# configure time (CEF ≈ 200 MB; WebView2 Windows only). They are built by default in the
# ALL/XML modes; set these OFF to skip them entirely (no download, no CEF/WebView2 code).
option(DUI_BUILD_CEF_EXAMPLES "Build the cef/CefBrowser examples (downloads the CEF binary distribution)" ON)
option(DUI_BUILD_WEBVIEW2_EXAMPLES "Build the WebView2/WebView2Browser examples (Windows only)" ON)

# ---- XML mode (original examples) ----
if(DUI_EXAMPLES_MODE STREQUAL "ALL" OR DUI_EXAMPLES_MODE STREQUAL "XML")
add_subdirectory(basic)
# The binding module is optional; this example only exists when it is compiled in.
if(DUI_MVVM_AVAILABLE)
    add_subdirectory(Binding)
endif()
# CEF is not supported on FreeBSD (no official binary distribution; see src/CMakeLists.txt)
# nor on MinGW (the MinGW build scripts exclude it)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW AND NOT DUI_ENABLE_WAYLAND AND DUI_BUILD_CEF_EXAMPLES)
    # A space in a path is the one thing bmake cannot be told about. CEF's macOS
    # helper app bundles are named "<name> Helper" -- CEF's own build files write
    # the space in -- and CMake's makefile generator can only write such a path the
    # GNU make way. bmake does not read that, so the build dies with "don't know how
    # to make <dir>/bin_helper/cef", which names neither of the two real causes.
    # Say it here instead: this is the point the examples are added, and the
    # generator can still be changed. See cmake/dui_build_program.cmake.
    if(CMAKE_MAKE_PROGRAM MATCHES "bmake")
        message(WARNING
            "The CEF examples are enabled, but bmake is driving this build and cannot\n"
            "build them (a CEF helper app on macOS is named \"<name> Helper\", and a\n"
            "space in a path is written by CMake's makefile generator the GNU make\n"
            "way, which bmake does not read).\n"
            "Configure again with one of:\n"
            "  -G Ninja                              (recommended)\n"
            "  -DCMAKE_MAKE_PROGRAM=/usr/bin/make    (GNU make, ships with macOS)\n"
            "  -DDUI_BUILD_CEF_EXAMPLES=OFF          (leave the CEF examples out)")
    endif()
    add_subdirectory(cef)
    add_subdirectory(CefBrowser)
endif()
add_subdirectory(chat)
add_subdirectory(ChildWindow)
add_subdirectory(Combo)
add_subdirectory(ColorPicker)
add_subdirectory(controls)
add_subdirectory(controls_round)
add_subdirectory(DpiAware)
add_subdirectory(hello)
add_subdirectory(layout)
add_subdirectory(ListBox)
add_subdirectory(ListCtrl)
add_subdirectory(MoveControl)
add_subdirectory(MultiLang)
add_subdirectory(render)
add_subdirectory(RichEdit)
add_subdirectory(threads)
add_subdirectory(TreeView)
add_subdirectory(VirtualListBox)
# WebView2 is Windows-only (WebView2Control is compiled only with DUI_BUILD_FOR_WIN)
if(WIN32 AND DUI_BUILD_WEBVIEW2_EXAMPLES)
    add_subdirectory(WebView2)
    add_subdirectory(WebView2Browser)
endif()
add_subdirectory(XmlPreview)
endif()

# ---- Mode 2: XML-to-code generation (*_gen) ----
if(DUI_EXAMPLES_MODE STREQUAL "ALL" OR DUI_EXAMPLES_MODE STREQUAL "GEN")
endif()

# ---- Mode 3: pure code (*_code) ----
if(DUI_EXAMPLES_MODE STREQUAL "ALL" OR DUI_EXAMPLES_MODE STREQUAL "CODE")
endif()
