# 第三方库统一纳入顶层构建（参照 develop2 分支的做法，按 dui 适配）
# 说明：
#   - skia/SDL 不在此内建：保持兄弟目录外部预编译（gn/ninja 构建 skia，cmake 构建 SDL3）
#   - 各库 target 名与 cmake/duilib_common.cmake 的 DUILIB_LIBS 一致
#     （duilib duilib-cximage duilib-webp duilib-png duilib-zlib）

add_subdirectory(zlib)      # target: duilib-zlib

# libpng 通过 find_package(ZLIB) 依赖 zlib：
# 顶层构建时提供 ZLIB::ZLIB 导入目标（独立构建 libpng 时仍走 find_package，见 libpng/CMakeLists.txt）
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB INTERFACE IMPORTED)
    set_target_properties(ZLIB::ZLIB PROPERTIES
        INTERFACE_LINK_LIBRARIES duilib-zlib
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/zlib")
    set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/zlib")
endif()

add_subdirectory(cximage)   # target: duilib-cximage
add_subdirectory(libpng)    # target: png_static（OUTPUT_NAME duilib-png）
add_subdirectory(libwebp)   # target: duilib-webp

# libcef 按平台内建（FreeBSD 不编译 CEF）
if(WIN32)
    if(DUILIB_CEF_109)
        add_subdirectory(libcef/libcef_win_109)
    else()
        add_subdirectory(libcef/libcef_win)
    endif()
elseif(APPLE)
    add_subdirectory(libcef/libcef_macos)
elseif(UNIX)
    add_subdirectory(libcef/libcef_linux)
endif()
