#!/bin/bash
set -e

DUI_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
SKIA_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../skia" && pwd)
echo "DUI_SRC_ROOT_DIR: $DUI_SRC_ROOT_DIR"
echo "SKIA_SRC_ROOT_DIR: $SKIA_SRC_ROOT_DIR"

# Use clang if available, otherwise gcc
if command -v clang &> /dev/null && command -v clang++ &> /dev/null; then
    DUI_CC=clang
    DUI_CXX=clang++
    DUI_COMPILER_ID=llvm
else
    DUI_CC=gcc
    DUI_CXX=g++
    DUI_COMPILER_ID=gcc
fi

DUI_CMAKE="cmake -DCMAKE_C_COMPILER=$DUI_CC -DCMAKE_CXX_COMPILER=$DUI_CXX"
DUI_MAKE="cmake --build"
# Parallel build jobs: 3/4 of the CPU cores (leave the rest for the system)
DUI_JOBS=$(( $(nproc) * 3 / 4 ))
[ "$DUI_JOBS" -lt 1 ] && DUI_JOBS=1
DUI_MAKE_THREADS="-j ${DUI_JOBS}"
DUI_BUILD_TYPE=Release
DUI_BUILD_DIR="$DUI_SRC_ROOT_DIR/scripts/build_temp/${DUI_COMPILER_ID}_wayland_build"
DUI_WAYLAND_FLAG="-DDUI_ENABLE_WAYLAND=ON"

# CPU architecture for Skia
CPU_ARCH_STR=$(uname -m)
if [ "$CPU_ARCH_STR" = "x86_64" ] || [ "$CPU_ARCH_STR" = "amd64" ]; then
    CPU_ARCH=x64
elif [ "$CPU_ARCH_STR" = "aarch64" ] || [ "$CPU_ARCH_STR" = "arm64" ]; then
    CPU_ARCH=arm64
else
    CPU_ARCH=x64
fi

# Detect skia lib
if [[ -d "${SKIA_SRC_ROOT_DIR}/out/llvm.${CPU_ARCH}.release" ]]; then
    DUI_SKIA_LIB_SUBPATH=llvm.${CPU_ARCH}.release
elif [[ -d "${SKIA_SRC_ROOT_DIR}/out/gcc.${CPU_ARCH}.release" ]]; then
    DUI_SKIA_LIB_SUBPATH=gcc.${CPU_ARCH}.release
else
    DUI_SKIA_LIB_SUBPATH=llvm.${CPU_ARCH}.release
fi

echo "DUI_SKIA_LIB_SUBPATH:${DUI_SKIA_LIB_SUBPATH}"

target_dir="$DUI_BUILD_DIR"
if [[ ! -d "$target_dir" ]]; then
    mkdir -p "$target_dir"
fi

# ============================================================
# Top-level CMake build (default; follows the develop2 branch, Wayland mode):
#   Configure the whole repository at once (dui + third-party libraries + all examples),
#   Build everything at once; use --target <example name> to build a single target
# ============================================================
STANDALONE=false
if [[ "$*" == *"--standalone"* ]]; then
    STANDALONE=true
fi

if [ "$STANDALONE" = false ]; then
    DUI_TOP_BUILD_DIR="$DUI_BUILD_DIR/top"
    mkdir -p "$DUI_TOP_BUILD_DIR"

    $DUI_CMAKE -S "$DUI_SRC_ROOT_DIR" -B "$DUI_TOP_BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE} \
        -DDUI_SKIA_LIB_SUBPATH="$DUI_SKIA_LIB_SUBPATH" \
        $DUI_WAYLAND_FLAG
    if [ $? -ne 0 ]; then
        echo "Top-level cmake configure failed."
        exit 1
    fi

    $DUI_MAKE "$DUI_TOP_BUILD_DIR" $DUI_MAKE_THREADS
    exit $?
fi

# ============================================================
# --standalone: build each example as an independent CMake project (legacy mode)
# ============================================================
# Build third party libs (CEF excluded for Wayland)
DUI_THIRD_PARTY_LIBS=("zlib" "libpng" "cximage" "libwebp")
for third_party_lib in "${DUI_THIRD_PARTY_LIBS[@]}"; do
    $DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/dui/third_party/$third_party_lib" -B "$DUI_BUILD_DIR/$third_party_lib" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE}
    $DUI_MAKE "$DUI_BUILD_DIR/$third_party_lib" $DUI_MAKE_THREADS
done

# Build dui with Wayland
$DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/dui" -B "$DUI_BUILD_DIR/dui" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE} ${DUI_WAYLAND_FLAG}
$DUI_MAKE "$DUI_BUILD_DIR/dui" $DUI_MAKE_THREADS

# Build all examples except CEF-related ones

# Build all examples except CEF and Windows-specific ones
# Three modes: XML mode + mode 2 code generation (*_gen) + mode 3 pure code (*_code)
DUI_PROGRAMS=("basic" "controls" "ColorPicker" "DpiAware" "chat" "layout" "ListBox" "ListCtrl" "MoveControl" "MultiLang" "render" "RichEdit" "VirtualListBox" "threads" "TreeView" "ChildWindow" "XmlPreview" "codeui" "embedxml" "genui" "genlist" "basic_gen" "controls_gen" "ColorPicker_gen" "DpiAware_gen" "chat_gen" "layout_gen" "ListBox_gen" "ListCtrl_gen" "MoveControl_gen" "MultiLang_gen" "render_gen" "RichEdit_gen" "VirtualListBox_gen" "threads_gen" "TreeView_gen" "ChildWindow_gen" "XmlPreview_gen" "basic_code" "controls_code" "ColorPicker_code" "DpiAware_code" "chat_code" "layout_code" "ListBox_code" "ListCtrl_code" "MoveControl_code" "MultiLang_code" "render_code" "RichEdit_code" "VirtualListBox_code" "threads_code" "TreeView_code" "ChildWindow_code" "XmlPreview_code")
for dui_bin in "${DUI_PROGRAMS[@]}"; do
    echo "Building example: $dui_bin"
    $DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/examples/$dui_bin" -B "$DUI_BUILD_DIR/$dui_bin" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE} -DDUI_SKIA_LIB_SUBPATH="$DUI_SKIA_LIB_SUBPATH" ${DUI_WAYLAND_FLAG}
    $DUI_MAKE "$DUI_BUILD_DIR/$dui_bin" $DUI_MAKE_THREADS
done

echo ""
echo "======================================"
echo "Wayland build completed successfully!"
echo "Library: $DUI_SRC_ROOT_DIR/lib/libdui.a"
echo "Binaries: $DUI_SRC_ROOT_DIR/bin/"
echo "======================================"

