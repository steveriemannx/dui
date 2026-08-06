# 第三方库统一纳入顶层构建（参照 develop2 分支的做法，按 dui 适配）
# 说明：
#   - skia/SDL 不在此内建：保持兄弟目录外部预编译（gn/ninja 构建 skia，cmake 构建 SDL3）
#   - 各库 target 名与 cmake/dui_common.cmake 的 DUI_LIBS 一致
#     （dui dui-cximage dui-webp dui-png dui-zlib）

add_subdirectory(zlib)      # target: dui-zlib

# libpng 通过 find_package(ZLIB) 依赖 zlib：
# 顶层构建时提供 ZLIB::ZLIB 导入目标（独立构建 libpng 时仍走 find_package，见 libpng/CMakeLists.txt）
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB INTERFACE IMPORTED)
    set_target_properties(ZLIB::ZLIB PROPERTIES
        INTERFACE_LINK_LIBRARIES dui-zlib
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/zlib")
    set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/zlib")
endif()

add_subdirectory(cximage)   # target: dui-cximage
add_subdirectory(libpng)    # target: png_static（OUTPUT_NAME dui-png）
add_subdirectory(libwebp)   # target: dui-webp

# libcef 内建：统一从下载的 cef_binary 发行包构建 wrapper（Windows/Mac/Linux 一致；
# FreeBSD 不编译 CEF；MinGW 不支持 CEF，见构建脚本；CEF 109 分支保留 libcef_win_109）
if(WIN32 AND DUI_CEF_109)
    add_subdirectory(libcef/libcef_win_109)
elseif((WIN32 AND NOT MINGW) OR APPLE OR DUI_OS_LINUX)
    # 发行包缺失时自动下载（幂等，缓存于 third_party/downloads/）；bat/sh 流程也会
    # 提前抓取到同一个目录。
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libcef/cef_binary/CMakeLists.txt")
        dui_deps_download_cef()
    endif()
    set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libcef/cef_binary")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
    find_package(CEF REQUIRED)
    # 只内建 libcef_dll 的 libcef_dll_wrapper 目标（发行包自带的 CMakeLists 引导
    # 还会内建 tests/，不需要）；wrapper 归档输出到 lib/ 供示例按名链接。
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${DUI_ROOT}/lib/")
    add_subdirectory("${CEF_ROOT}/libcef_dll" libcef_dll_wrapper)
endif()
