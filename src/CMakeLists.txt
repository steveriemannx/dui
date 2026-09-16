cmake_minimum_required(VERSION 4.0)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source code and run \"cmake -S ${CMAKE_SOURCE_DIR} -B .\" from there")
endif()

# project() must run before dui_common.cmake: it populates CMAKE_SYSTEM_NAME,
# which dui_common.cmake uses to pick the platform and native window backend
# switch). Without it, a standalone "cmake -S src" configure saw an empty
# CMAKE_SYSTEM_NAME and wrongly evaluated the non-Windows branch.
project(dui CXX C)

# dui
# MSVC runtime library flags are selected by an abstraction.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
if("${DUI_MD}" STREQUAL "ON")
    # MD/MDd
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
else()
    # MT/MTd
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
# dui end

# Make this directory self-contained so it can also be configured standalone
# (scripts/*_build.sh --standalone). In the top-level build dui_common.cmake is
# already included; its execution blocks are GLOBAL-property guarded and idempotent.
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/dui_common.cmake")

if(MSVC)
    # Get the number of logical CPU cores
    cmake_host_system_information(RESULT CPU_CORES QUERY NUMBER_OF_LOGICAL_CORES)
endif()

set(CMAKE_CXX_STANDARD 20) # C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # C++20

# Set the libcef directory path
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")  # macOS
    set(CEF_LIB_DIR "cef_binary")
    set(DUI_SRC_MACOS 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")  # Linux
    set(CEF_LIB_DIR "cef_binary")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows
    # 头文件目录与 Mac/Linux 一致取自 cef_binary（下载的发行包，含 include/）；
    # CEF 109 分支继续使用随仓库内建的 libcef_win_109（libcef_win 已删除）
    if(DUI_CEF_109)
        set(CEF_LIB_DIR "libcef_win_109")
    else()
        set(CEF_LIB_DIR "cef_binary")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")  # FreeBSD
    message(WARNING "CEF is not supported on FreeBSD - building without CEF support")
    set(CEF_LIB_DIR "")  # Empty or set to a dummy value
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Set source/dependency paths
get_filename_component(DUI_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
get_filename_component(SKIA_SRC_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/skia/" ABSOLUTE)

# The Wayland libraries themselves are detected in cmake/dui_common.cmake, included
# above, which sets DUI_WAYLAND_INCLUDE_DIRS and DUI_WAYLAND_LIBS. This file used to
# repeat that whole pkg_check_modules block -- and the copy here won, because it sits
# in this directory scope and so overrode the shared definition for everything the dui
# target links. Two copies meant the shared one could be corrected without changing
# what the library actually linked, which is what happened when the pkg-config imports
# were moved to IMPORTED_TARGET so the -L search path could travel with the library
# name: the Wayland/GL stack is linked by bare -l name, and on FreeBSD, where it lives
# in /usr/local/lib rather than a default ld directory, the link then failed with
# "unable to find library -lwayland-client".
if(DUI_ENABLE_WAYLAND)
    # Generate Wayland protocol headers
    find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)
    pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
    
    set(WAYLAND_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/wayland-generated-protocols")
    file(MAKE_DIRECTORY ${WAYLAND_GEN_DIR})
    
    # xdg-shell protocol
    set(XDG_SHELL_XML "${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml")
    add_custom_command(
        OUTPUT "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.h"
        COMMAND ${WAYLAND_SCANNER} client-header ${XDG_SHELL_XML} "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.h"
        DEPENDS ${XDG_SHELL_XML}
        COMMENT "Generating xdg-shell-client-protocol.h"
    )
    add_custom_command(
        OUTPUT "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.c"
        COMMAND ${WAYLAND_SCANNER} private-code ${XDG_SHELL_XML} "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.c"
        DEPENDS ${XDG_SHELL_XML}
        COMMENT "Generating xdg-shell-client-protocol.c"
    )
    add_custom_target(generate_wayland_protocols DEPENDS
        "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.h"
        "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.c"
    )

    
    list(APPEND DUI_WAYLAND_INCLUDE_DIRS ${WAYLAND_GEN_DIR})
endif()

# Conditionally include CEF directory
if(CEF_LIB_DIR AND NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW)
    get_filename_component(CEF_INCLUDE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/libcef/${CEF_LIB_DIR}/" ABSOLUTE)
else()
    message(STATUS "CEF includes disabled - not supported on this platform")
endif()

# Source files, listed rather than globbed. file(GLOB ... CONFIGURE_DEPENDS) is not
# reliable on the multi-config generators this project targets (Visual Studio, Xcode),
# and its failure mode is a source file quietly not being compiled. Adding a file to the
# library means adding a line to one of these lists.
#
# Platform selection is expressed as exclusive lists instead of as a filter applied
# afterwards, because two of its rules are load-bearing rather than cosmetic:
#
#   - DUI_SRC_BACKEND_X11 and DUI_SRC_BACKEND_WAYLAND are mutually exclusive and have to
#     stay that way. ScreenCapture_X11.cpp and ScreenCapture_Wayland.cpp both guard on
#     `DUI_BUILD_FOR_LINUX || DUI_BUILD_FOR_FREEBSD` -- neither names its own backend --
#     so the file name was the only thing keeping them out of the same library. Pulling
#     in both is a duplicate-symbol link error on Linux, not a harmless empty object.
#   - DUI_SRC_MACOS_OBJCXX exists because Objective-C++ cannot be compiled by MSVC or by
#     GCC/Clang on Linux at all.
#
# What deliberately stays in the common lists: the *_MacOS.cpp files (each is wrapped in
# `#if defined (DUI_BUILD_FOR_MACOS)`, so off macOS they compile to an empty translation
# unit -- the object count is higher than the number of files contributing code, and that
# is the behaviour this refactor preserves), and Core/X11Platform.cpp, whose name does not
# match `_X11.` and which likewise compiles to an empty TU in a Wayland build.

set(DUI_SRC_COMMON
    "${CMAKE_CURRENT_LIST_DIR}/dui.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Animation/AnimationManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Animation/AnimationPlayer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Animation/EasingFunctions.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Box/ListBox.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Box/ListBoxHelper.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Box/ScrollBox.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Box/TabBox.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Box/VirtualListBox.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Box/XmlBox.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/GridLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/HFlowLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/HLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/HTileLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/Layout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/VFlowLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/VLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/VTileLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/VirtualHLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/VirtualHTileLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/VirtualVLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Layout/VirtualVTileLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/AddressBar.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/BitmapControl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/CheckCombo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ChildWindow.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ChildWindowImpl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/CircleProgress.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ColorControl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ColorConvert.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ColorPicker.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ColorPickerCustom.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ColorPickerRegular.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ColorPickerStatard.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ColorPickerStatardGray.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ColorSlider.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/Combo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ComboButton.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/DateTime.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/DateTimeWnd_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/DateTimeWnd_Native.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/DateTimeWnd_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/DirectoryTree.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/DirectoryTreeImpl_Linux.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/DirectoryTreeImpl_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/DirectoryTreeImpl_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/FilterCombo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/HotKey.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/IPAddress.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/IconControl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/LabelImpl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/Line.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlData.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlHeader.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlHeaderItem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlIcon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlIconView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlItem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlLabel.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlListView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlReportView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlSubItem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/ListCtrlView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/MacTrafficLights.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/Menu.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/MenuBar.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/Progress.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/PropertyGrid.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/RichEditData.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/RichEditHost_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/RichEdit_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/RichEdit_Native.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/RichEdit_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/RichTextImpl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/Slider.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/TabCtrl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/TextDrawer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Control/TreeView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Box.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/BoxShadow.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ClickThrough_Linux.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ClickThrough_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ColorManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Control.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ControlDropTargetImpl_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ControlDropTargetImpl_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ControlDropTargetUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ControlFinder.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ControlLoading.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/CursorManager_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/DpiAwareness_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/DpiAwareness_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/DpiManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/DragWindow.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/DragWindowFilter_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/EventArgs.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/FontManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/FrameworkThread.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/FullscreenBox.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/GlobalManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/IconManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ImageList.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ImageManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Keyboard_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Keycode_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/LangManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/MemoryResourceManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/MessageLoop_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/NativeWindow_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/PlaceHolder.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ScrollBar.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Shadow.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/StateColorMap.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/StateColorMap2.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ThreadManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ThreadMessage_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ThreadMessage_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/TimerManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ToolTip_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ToolTip_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/UiColors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Window.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/WindowBase.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/WindowBuilder.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/WindowCreateParam.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/WindowDropTarget_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/WindowDropTarget_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/WindowManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/X11Platform.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/APngDecoder.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/FrameSequence_gif.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageAttribute.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoderFactory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoderUtil.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_Common.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_GIF.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_ICO.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_Icon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_JPEG.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_LOTTIE.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_PAG.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_PNG.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_SVG.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageDecoder_WEBP.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageInfo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageLoadParam.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImagePlayer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/ImageUtil.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_Animation.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_Bitmap.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_GIF.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_ICO.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_JPEG.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_LOTTIE.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_PAG.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_PNG.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_Svg.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/Image_WEBP.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/StateImage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Image/StateImageMap.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Render/AutoClip.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Render/BitmapAlpha.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Bitmap_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Brush_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/DrawRichText.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/DrawSkiaImage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/FontMgr_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Font_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/HorizontalDrawText.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Matrix_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Path_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Pen_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/RenderFactory_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Render_Skia.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Render_Skia_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkGLWindowContext_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkRasterWindowContext_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkTextBox.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/VerticalDrawText.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/WindowRgn_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ApiWrapper_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/AttributeUtil.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/BitmapHelper_Native.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/BitmapHepler_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/BitmapHepler_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/Clipboard_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/DiskUtils_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/FileDialog_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/FilePath.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/FilePathUtil.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/FileTime.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/FileUtil.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/InlineHook_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/LogUtil.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/MonitorUtil_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/PerformanceUtil.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ProcessSingleton.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ScreenCapture_Linux.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ScreenCapture_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ShadowWnd_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ShadowWnd_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/StringCharset.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/StringConvert.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/StringUtil.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/SystemUtil_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/TrayIcon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/TrayIcon_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/TrayIcon_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/WinImplBase.cpp"
)

# X11 native backend. Appended when DUI_ENABLE_WAYLAND is off -- which includes
# macOS and Windows, where the file compiles to an empty TU. That is what happened
# before this list existed too.
set(DUI_SRC_BACKEND_X11
    "${CMAKE_CURRENT_LIST_DIR}/Core/MessageLoop_X11.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/NativeWindow_X11.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Render_Skia_X11.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ScreenCapture_X11.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/TrayIcon_X11.cpp"
)

# Wayland native backend, appended only on a Wayland build.
set(DUI_SRC_BACKEND_WAYLAND
    "${CMAKE_CURRENT_LIST_DIR}/Core/ControlDropTargetImpl_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/CursorManager_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/DpiAwareness_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Input_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Keyboard_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/MessageLoop_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/NativeWindow_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ThreadMessage_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/ToolTip_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Core/WindowDropTarget_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Render_Skia_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkRasterWindowContext_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/Clipboard_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/FileDialog_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ScreenCapture_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/SystemUtil_Wayland.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/TrayIcon_Wayland.cpp"
)

# Objective-C++, appended only on Darwin.
set(DUI_SRC_MACOS_OBJCXX
    "${CMAKE_CURRENT_LIST_DIR}/Core/ClickThrough_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Core/CursorManager_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Core/Keyboard_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Core/MessageLoop_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Core/NativeWindow_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/Render_Skia_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkGLWindowContext_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkMetalWindowContext_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/RenderSkia/SkRasterWindowContext_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/Clipboard_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/FileDialog_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/MonitorUtil_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/ScreenCapture_MacOS.mm"
    "${CMAKE_CURRENT_LIST_DIR}/Utils/SystemUtil_MacOS.mm"
)

# CEF controls: compiled only when CEF is enabled (see below).
set(DUI_SRC_CEF
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefControl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefControlNative.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefControlOffScreen.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefManager_Linux.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefManager_MacOS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefManager_Windows.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefWindowUtils_Linux.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefWindowUtils_Windows.cpp"
)

# Separate from DUI_SRC_CEF because it was previously filtered out on non-macOS
# by list(REMOVE_ITEM); folding it in would compile it on every platform CEF is
# enabled on.
set(DUI_SRC_CEF_MACOS
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/CefWindowUtils_MacOS.mm"
)

set(DUI_SRC_CEF_INTERNAL
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/CefBrowserHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/CefClientApp.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/CefClientAppBrowser.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/CefClientAppRender.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/CefJSBridge.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/CefJsHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/CefMemoryBlock.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/CefRegisteredFunctions.cpp"
)

# Windows-only CEF internals.
set(DUI_SRC_CEF_INTERNAL_WINDOWS
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/Windows/CefOsrDropTarget.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/Windows/bytes_write_handler.cc"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/Windows/osr_dragdrop_win.cc"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/Windows/osr_ime_handler_win.cc"
    "${CMAKE_CURRENT_LIST_DIR}/CEFControl/internal/Windows/util_win.cc"
)

# Vendored third-party sources that really are single files. The three globs this
# replaces (third_party/apng, stb_image, svg) resolved to nothing at all: apng/ does not
# exist, and the other two are header-only, already reachable through DUI_SRC_ROOT_DIR on
# the include path. A glob over third_party/xml also matched pugixml.LICENSE.md, which is
# why this list is worth writing out.
set(UTF_SRC "${DUI_SRC_ROOT_DIR}/third_party/convert_utf/ConvertUTF.cpp")
set(XML_SRC "${DUI_SRC_ROOT_DIR}/third_party/xml/pugixml.cpp")

set(GIFLIB_HDRS
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_hash.h
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_lib.h
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_lib_private.h
)

set(GIFLIB_SRCS
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/dgif_lib.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/egif_lib.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_err.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_font.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gif_hash.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/gifalloc.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/openbsd-reallocarray.c
    ${DUI_SRC_ROOT_DIR}/third_party/giflib/quantize.c
)

set(SKIA_HDRS
    ${SKIA_SRC_ROOT_DIR}/tools/window/DisplayParams.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/RasterWindowContext.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/WindowContext.h
    ${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.h
)

set(SKIA_SRCS
    ${SKIA_SRC_ROOT_DIR}/tools/window/WindowContext.cpp
    ${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp
)

# Skia's unix raster window context is not compilable off unix -- it includes
# XlibWindowInfo.h, which includes <X11/X.h>. dui compiles these Skia sources into the
# library, and this one used to be listed unconditionally, so a Windows build tried to
# compile X11 code. dui's own RenderSkia/SkRasterWindowContext_Windows.cpp is what the
# Windows build uses; Skia's Windows variant is not referenced at all.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND SKIA_SRCS
        ${SKIA_SRC_ROOT_DIR}/tools/window/unix/RasterWindowContext_unix.cpp
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS: the Metal window-context base (requires the Metal-enabled libskia.a)
    list(APPEND SKIA_HDRS ${SKIA_SRC_ROOT_DIR}/tools/window/MetalWindowContext.h)
    list(APPEND SKIA_SRCS ${SKIA_SRC_ROOT_DIR}/tools/window/MetalWindowContext.mm)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows platform: several skia source files depended on separately
    set(SKIA_WGL_SRCS
        ${SKIA_SRC_ROOT_DIR}/tools/ganesh/gl/win/SkWGL.h
        ${SKIA_SRC_ROOT_DIR}/tools/ganesh/gl/win/SkWGL_win.cpp
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows platform: source files depended on separately
    set(UDIS_HDRS 
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/decode.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/extern.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/itab.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/syn.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/types.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/udint.h
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/udis86.h
    )
    set(UDIS_SRCS 
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/udis86.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/syn-att.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/syn-intel.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/syn.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/itab.c
        ${DUI_SRC_ROOT_DIR}/third_party/libudis86/decode.c
    )
endif()

# Native backend, chosen once. The two backends are mutually exclusive; see the note on
# the source lists above for why that is not interchangeable with "compile both".
set(DUI_SRC_PLATFORM ${DUI_SRC_COMMON})
if(DUI_ENABLE_WAYLAND)
    list(APPEND DUI_SRC_PLATFORM ${DUI_SRC_BACKEND_WAYLAND})
else()
    list(APPEND DUI_SRC_PLATFORM ${DUI_SRC_BACKEND_X11})
endif()
# Objective-C++ only exists on Darwin (the .mm files were previously removed by an
# explicit list(REMOVE_ITEM) block on every other platform).
if(DUI_SRC_MACOS)
    list(APPEND DUI_SRC_PLATFORM ${DUI_SRC_MACOS_OBJCXX})
endif()

# Define the base sources (always included)
set(LIBRARY_SOURCES
    ${DUI_SRC_PLATFORM}
    ${UTF_SRC}
    ${XML_SRC}
    ${GIFLIB_HDRS}
    ${GIFLIB_SRCS}
    ${SKIA_HDRS}
    ${SKIA_SRCS}
    ${SKIA_WGL_SRCS}
    ${UDIS_HDRS}
    ${UDIS_SRCS}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows, WebView2
    list(APPEND LIBRARY_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/WebView2/WebView2Control.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WebView2/WebView2ControlImpl.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WebView2/WebView2EnvironmentOptions.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WebView2/WebView2Manager.cpp"
    )
endif()

# Optional MVVM data-binding module. Additive: new files only, no existing source
# changed, so nothing here affects builds that do not call into it.
# Gated on the OR of both options for the same reason as CEF above -- the library is
# built once at root scope, so examples cannot enable the module for the library.
if(DUI_ENABLE_MVVM OR DUI_BUILD_MVVM_EXAMPLES)
    list(APPEND LIBRARY_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Binding/BindingGroup.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Binding/ObservableObject.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Binding/PropertyRegistry.cpp"
    )
    message(STATUS "Binding module enabled")
else()
    message(STATUS "Binding module disabled")
endif()

# Only include CEF sources on supported platforms (Windows, Linux, macOS)
# Wayland build excludes CEF; MinGW does not support CEF (see the build scripts)
if(DUI_ENABLE_WAYLAND)
    message(STATUS "CEF support disabled (Wayland build)")
elseif(NOT (DUI_ENABLE_CEF OR DUI_BUILD_CEF_EXAMPLES))
    message(STATUS "CEF support disabled")
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND NOT MINGW)
    list(APPEND LIBRARY_SOURCES
        ${DUI_SRC_CEF}
        ${DUI_SRC_CEF_INTERNAL}
    )
    if(DUI_SRC_MACOS)
        list(APPEND LIBRARY_SOURCES ${DUI_SRC_CEF_MACOS})
    endif()
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND NOT MINGW)
        list(APPEND LIBRARY_SOURCES ${DUI_SRC_CEF_INTERNAL_WINDOWS})
    endif()
    # dui.cpp includes dui_cef.h on this same condition. It has to be a definition
    # rather than a plain platform check: the CEF include root exists only after the
    # distribution has been downloaded, and that happens only when CEF is enabled --
    # so on a clean checkout the default (-DDUI_ENABLE_CEF=OFF) compile of dui.cpp
    # failed with "include/cef_app.h file not found".
    set(DUI_CEF_COMPILED ON)
    message(STATUS "CEF support enabled (${CMAKE_SYSTEM_NAME})")
else()
    message(STATUS "CEF support disabled (FreeBSD is not supported)")
endif()

if(DUI_ENABLE_WAYLAND)
    list(APPEND LIBRARY_SOURCES "${WAYLAND_GEN_DIR}/xdg-shell-client-protocol.c")
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
    add_dependencies(${PROJECT_NAME} generate_wayland_protocols)
    # System libraries are PRIVATE rather than unqualified (which is PUBLIC):
    # PUBLIC also hands every consumer their usage requirements -- include
    # directories, compile definitions, flags -- and puts them in the exported
    # interface. The link line is not what changes: CMake propagates the private
    # dependencies of a static library to consumers as $<LINK_ONLY:...>, because
    # the symbols have to resolve somewhere.
    target_link_libraries(${PROJECT_NAME} PRIVATE ${DUI_WAYLAND_LIBS})
    find_package(Freetype REQUIRED)
    find_package(Fontconfig REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype Fontconfig::Fontconfig)
    # Threads and the dynamic loader are this library's dependencies, not the
    # application's: Core/ThreadManager.cpp and Core/FrameworkThread.cpp are compiled
    # into the archive, and so are the Skia archives a consumer pulls in that reference
    # pthread_create from SkExecutor.cpp. Naming them only on dui::app left every target
    # that links dui directly -- tests/ -- with "undefined symbol: pthread_create".
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread dl)
    if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        find_package(X11 REQUIRED)
        target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
        # Core/MessageLoop_Wayland.cpp is compiled into this archive against the
        # epoll-shim headers, so the shim is the library's dependency too. dui::app
        # carries it as well, for standalone application builds that link dui by bare
        # name; see cmake/dui_common.cmake for where it is found.
        target_include_directories(${PROJECT_NAME} PRIVATE ${EPOLL_SHIM_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${EPOLL_SHIM_LIBRARY})
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_package(Freetype REQUIRED)
    find_package(Fontconfig REQUIRED)
    find_package(X11 REQUIRED)
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
    # ${X11_LIBRARIES} belongs here, next to the include path that was already here:
    # the X11 backend sources (MessageLoop_X11, NativeWindow_X11, Render_Skia_X11,
    # ScreenCapture_X11, TrayIcon_X11, X11Platform) are compiled into this archive,
    # so libdui.a is what references XCreateGC and friends. Without the libraries the
    # symbols were resolved only by the applications, whose dui::app carries them --
    # so an in-tree consumer that links dui directly (tests/) failed to link with
    # "undefined symbol: XCreateGC" while every example built.
    #
    # pthread and dl are here for the same reason and by the same route: the archive
    # references pthread_create itself, and named them nowhere.
    target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES} pthread dl Freetype::Freetype Fontconfig::Fontconfig)
    target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows takes none of what the branch below looks for. The libraries its binaries
    # link -- user32, gdi32, shell32, dwmapi and the rest -- are attached in
    # cmake/dui_install.cmake, and the CEF and WebView2 integration is attached to the
    # application target by cmake/dui_app_windows.cmake.
    #
    # This branch did not exist until the first Windows configure of this branch failed
    # on it: Windows fell into the else() below, which is the X11 one, and
    # find_package(Freetype REQUIRED) stopped the configure. The Linux and FreeBSD work
    # that added those lookups had no way to notice -- on macOS the same branch finds
    # them (a Mac with Homebrew installed has all three), so the only platform that could
    # have caught it was the one nobody built on.
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
else()
    # Create the library with the filtered sources
    add_library(${PROJECT_NAME} ${LIBRARY_SOURCES})
    find_package(X11 REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
    find_package(Freetype REQUIRED)
    find_package(Fontconfig REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES} Freetype::Freetype Fontconfig::Fontconfig)
endif()

# Two settings that used to be directory-scoped in dui_common.cmake, where they reached
# every target in the scope including ones that did not want them. Both are Linux/FreeBSD
# only, and both are now attached to a target:
#
#   -Wl,--no-as-needed
#     The library pulls in image and codec archives whose symbols are only referenced
#     from inside the library, and a linker running with --as-needed drops those. INTERFACE
#     so it still reaches everything that links dui -- applications and tests alike.
#
#   /usr/local/include
#     FreeBSD installs the X11/fontconfig/freetype headers there. PUBLIC because the
#     library's own translation units need it, and consumers of the installed package need
#     it for the same reason. It is deliberately not directory-scoped any more: that also
#     reached the third_party subprojects, where the system expat.h shadows Skia's bundled
#     copy. Applications get the same path from cmake/dui_app_freebsd.cmake.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    target_link_options(${PROJECT_NAME} INTERFACE -Wl,--no-as-needed)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND EXISTS "/usr/local/include")
    target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include)
endif()

# In-tree alias matching the installed name (install(EXPORT ... NAMESPACE dui::)),
# so "target_link_libraries(app PRIVATE dui::dui)" — the documented form — also
# resolves when building against the source tree instead of an install.
add_library(dui::dui ALIAS ${PROJECT_NAME})

# The public headers require C++20. CMAKE_CXX_STANDARD above is a directory
# variable, so it does not travel with the installed package; without this,
# consumers are never told what the headers need.
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if(DUI_MULTI_CONFIG)
    foreach(_config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${_config}" _config_upper)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_${_config_upper} "${DUI_LIB_PATH}/${_config}")
    endforeach()
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${DUI_LIB_PATH}")
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${DUI_SRC_ROOT_DIR}
        ${DUI_SRC_ROOT_DIR}/include
        ${SKIA_SRC_ROOT_DIR}
        ${DUI_WAYLAND_INCLUDE_DIRS}
        ${CEF_INCLUDE_ROOT_DIR}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE SK_GANESH SK_GL SK_RELEASE)

# Tells dui.cpp whether to pull in dui_cef.h. Set above, next to the CEF sources it
# has to agree with: the two are one condition, and the include root does not exist
# when CEF is off.
if(DUI_CEF_COMPILED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_CEF_COMPILED=1)
endif()
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        "/utf-8"
        # /W4 without /WX: the warning level is raised, warnings still do not
        # fail the build, so this is safe to add unverified. Expect noise from
        # the vendored sources compiled into this target (giflib, libudis86,
        # Skia's tools/window) -- that is what -Wno-* is for if it becomes
        # annoying, not a reason to drop the level.
        "/W4"
        $<$<COMPILE_LANGUAGE:C>:/MP${CPU_CORES}>
        $<$<COMPILE_LANGUAGE:CXX>:/MP${CPU_CORES}>
    )
    # /permissive- is deliberately NOT passed. MSVC already enables conformance
    # mode implicitly for /std:c++20 (VS 2019 16.11+; /std:c++latest since 16.8),
    # and CMAKE_CXX_STANDARD 20 above selects exactly that, so on the toolchains
    # in use the flag is redundant. On an older toolchain it would instead be the
    # first thing to turn latent Microsoft-isms in this codebase and in the
    # vendored sources into hard errors -- and no Windows host is available to
    # this change, so that cannot be verified. Decision: leave it off; if a
    # Windows build ever wants it, add it here and fix the fallout then.
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()
if(DUI_ENABLE_WAYLAND)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WAYLAND=1)
elseif(DUI_OS_LINUX OR DUI_OS_FREEBSD)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_X11=1)
endif()

# Optional Qt-like platform entry shim.
# Applications that write a normal C++ main() on Windows can link dui_entry;
# it provides WinMain/wWinMain and forwards to main().
add_library(dui_entry STATIC "${CMAKE_CURRENT_SOURCE_DIR}/Entry/EntryPoint_main.cpp")

# Same in-tree/installed naming agreement as dui::dui above.
add_library(dui::dui_entry ALIAS dui_entry)
target_compile_features(dui_entry PUBLIC cxx_std_20)

# Build-order wiring: the Skia archive built by cmake/dui_deps.cmake at make time
# must exist before this library links (dui_app*.cmake links them into the executables).
if(TARGET dui_skia)
    add_dependencies(${PROJECT_NAME} dui_skia)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")  # Windows, WebView2
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_WEBVIEW2=1)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_USE_WEBVIEW2_LOADER_DLL=1)
endif()

# MinGW doesn't support CEF. Disable it for the library explicitly: without
# DUI_CEF=0, dui_config.h defaults CEF to enabled on Windows (undefined = ON),
# which breaks MinGW builds where the CEF headers are unavailable.
# MSVC / Linux / macOS always compile the CEF code in the library.
if(DUI_MINGW)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DUI_CEF=0)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Skia automatically enables compile options such as AVX/AVX2; dui references some skia sources directly and must keep the compile options consistent
    include(${DUI_SRC_ROOT_DIR}/cmake/dui_compiles.cmake)

    # Compiler arguments
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
    # Note: -fno-threadsafe-statics is deliberately NOT set. It disables the C++11
    # guarantee that a function-local static is initialised exactly once, safely,
    # under concurrency, and that guarantee is load-bearing here: function-local
    # static singletons (GlobalManager, PerformanceUtil, WebView2Manager), static
    # lookup tables read from several paths (EventArgs, EasingFunctions), and ~25
    # GetProcAddress caches in ApiWrapper_Windows.cpp all rely on it, in a design
    # with separate decode threads and 50+ mutexes.
    set(DUI_CXX_COMPILER_FLAGS
        -fvisibility-inlines-hidden     # Give hidden visibility to inlined class member functions
        -frtti
        -Wno-narrowing                  # Don't warn about type narrowing
        -Wsign-compare                  # Warn about mixed signed/unsigned type comparisons
        -Wno-unused-variable
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        list(APPEND DUI_CXX_COMPILER_FLAGS
            -Wno-reorder
            -Wno-cast-function-type
          )
          set_source_files_properties("${DUI_SRC_ROOT_DIR}/third_party/libudis86/decode.c" PROPERTIES COMPILE_OPTIONS "-Wno-implicit-fallthrough")
          if(DUI_HAVE_AVX2)
            set_source_files_properties("${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp" PROPERTIES COMPILE_OPTIONS "-mavx2")
          elseif(DUI_HAVE_AVX)
            set_source_files_properties("${SKIA_SRC_ROOT_DIR}/tools/window/GLWindowContext.cpp" PROPERTIES COMPILE_OPTIONS "-mavx")
          endif()
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
    
    target_compile_options(${PROJECT_NAME} PRIVATE ${DUI_COMPILER_FLAGS})
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: ${DUI_CXX_COMPILER_FLAGS}>)
endif()

# ---- Sanitizers (DUI_ENABLE_SANITIZERS, off by default; see cmake/dui_common.cmake) ----
# PUBLIC on purpose: tests and examples link dui, and both halves of ASan --
# instrumentation and the runtime -- must cover the whole process, so the flags
# have to reach the consumers' compile and link lines. This also puts them into
# the installed INTERFACE_LINK_OPTIONS, which is the right outcome: a sanitized
# static library can only be consumed by a sanitized binary.
if(DUI_ENABLE_SANITIZERS)
    if(MSVC)
        # UNVERIFIED: no Windows host is available to this change.
        # MSVC implements only the address half (/fsanitize=address); there is no
        # /fsanitize=undefined and no UBSan runtime, so the undefined-behavior
        # half of this option is knowingly dropped on MSVC rather than silently
        # approximated. Building with UBSan on Windows needs clang-cl.
        target_compile_options(${PROJECT_NAME} PUBLIC /fsanitize=address)
        target_link_options(${PROJECT_NAME} PUBLIC /fsanitize=address)
    else()
        target_compile_options(${PROJECT_NAME} PUBLIC
            -fsanitize=address,undefined
            -fno-omit-frame-pointer     # keep usable stacks in the sanitizer reports
        )
        target_link_options(${PROJECT_NAME} PUBLIC
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
    endif()
endif()

# Third-party libraries are included in the build (following the develop2 branch: third_party/CMakeLists.txt)
# skia/native backend are not built here; keep them prebuilt in sibling directories
# The project owns the public install layout below; suppress vendored projects'
# independent install rules (libpng otherwise assumes its original output name).
set(SKIP_INSTALL_ALL ON)
add_subdirectory(../third_party third_party)

# Ensure third-party libraries build before dui (dui sources reference some third-party headers)
# dui-png is the alias third_party/CMakeLists.txt publishes for libpng's png_static
# target (whose archive is already named dui-png); use the dui-* name everywhere
# outside install(TARGETS), which is the only place that needs the real target.
add_dependencies(${PROJECT_NAME} dui-cximage dui-png dui-zlib dui-webp)
