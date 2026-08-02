#!/bin/bash
set -e

DUILIB_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
SKIA_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../skia" && pwd)
echo "DUILIB_SRC_ROOT_DIR: $DUILIB_SRC_ROOT_DIR"
echo "SKIA_SRC_ROOT_DIR: $SKIA_SRC_ROOT_DIR"

# Use clang if available, otherwise gcc
if command -v clang &> /dev/null && command -v clang++ &> /dev/null; then
    DUILIB_CC=clang
    DUILIB_CXX=clang++
    DUILIB_COMPILER_ID=llvm
else
    DUILIB_CC=gcc
    DUILIB_CXX=g++
    DUILIB_COMPILER_ID=gcc
fi

DUILIB_CMAKE="cmake -DCMAKE_C_COMPILER=$DUILIB_CC -DCMAKE_CXX_COMPILER=$DUILIB_CXX"
DUILIB_MAKE="cmake --build"
DUILIB_MAKE_THREADS="-j 6"
DUILIB_BUILD_TYPE=Release
DUILIB_BUILD_DIR="$DUILIB_SRC_ROOT_DIR/scripts/build_temp/${DUILIB_COMPILER_ID}_wayland_build"
DUILIB_WAYLAND_FLAG="-DDUILIB_ENABLE_WAYLAND=ON"

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
    DUILIB_SKIA_LIB_SUBPATH=llvm.${CPU_ARCH}.release
elif [[ -d "${SKIA_SRC_ROOT_DIR}/out/gcc.${CPU_ARCH}.release" ]]; then
    DUILIB_SKIA_LIB_SUBPATH=gcc.${CPU_ARCH}.release
else
    DUILIB_SKIA_LIB_SUBPATH=llvm.${CPU_ARCH}.release
fi

echo "DUILIB_SKIA_LIB_SUBPATH:${DUILIB_SKIA_LIB_SUBPATH}"

target_dir="$DUILIB_BUILD_DIR"
if [[ ! -d "$target_dir" ]]; then
    mkdir -p "$target_dir"
fi

# ============================================================
# Top-level CMake build (default; follows the develop2 branch, Wayland mode):
#   Configure the whole repository at once (duilib + third-party libraries + all examples),
#   Build everything at once; use --target <example name> to build a single target
# ============================================================
STANDALONE=false
if [[ "$*" == *"--standalone"* ]]; then
    STANDALONE=true
fi

if [ "$STANDALONE" = false ]; then
    DUILIB_TOP_BUILD_DIR="$DUILIB_BUILD_DIR/top"
    mkdir -p "$DUILIB_TOP_BUILD_DIR"

    $DUILIB_CMAKE -S "$DUILIB_SRC_ROOT_DIR" -B "$DUILIB_TOP_BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=${DUILIB_BUILD_TYPE} \
        -DDUILIB_SKIA_LIB_SUBPATH="$DUILIB_SKIA_LIB_SUBPATH" \
        $DUILIB_WAYLAND_FLAG
    if [ $? -ne 0 ]; then
        echo "Top-level cmake configure failed."
        exit 1
    fi

    $DUILIB_MAKE "$DUILIB_TOP_BUILD_DIR" $DUILIB_MAKE_THREADS
    exit $?
fi

# ============================================================
# --standalone: build each example as an independent CMake project (legacy mode)
# ============================================================
# Build third party libs (CEF excluded for Wayland)
DUILIB_THIRD_PARTY_LIBS=("zlib" "libpng" "cximage" "libwebp")
for third_party_lib in "${DUILIB_THIRD_PARTY_LIBS[@]}"; do
    $DUILIB_CMAKE -S "$DUILIB_SRC_ROOT_DIR/duilib/third_party/$third_party_lib" -B "$DUILIB_BUILD_DIR/$third_party_lib" -DCMAKE_BUILD_TYPE=${DUILIB_BUILD_TYPE}
    $DUILIB_MAKE "$DUILIB_BUILD_DIR/$third_party_lib" $DUILIB_MAKE_THREADS
done

# Build duilib with Wayland
$DUILIB_CMAKE -S "$DUILIB_SRC_ROOT_DIR/duilib" -B "$DUILIB_BUILD_DIR/duilib" -DCMAKE_BUILD_TYPE=${DUILIB_BUILD_TYPE} ${DUILIB_WAYLAND_FLAG}
$DUILIB_MAKE "$DUILIB_BUILD_DIR/duilib" $DUILIB_MAKE_THREADS

# Build all examples except CEF-related ones

# Build all examples except CEF and Windows-specific ones
# Three modes: XML mode + mode 2 code generation (*_gen) + mode 3 pure code (*_code)
DUILIB_PROGRAMS=("basic" "controls" "ColorPicker" "DpiAware" "chat" "layout" "ListBox" "ListCtrl" "MoveControl" "MultiLang" "render" "RichEdit" "VirtualListBox" "threads" "TreeView" "ChildWindow" "XmlPreview" "codeui" "embedxml" "genui" "genlist" "basic_gen" "controls_gen" "ColorPicker_gen" "DpiAware_gen" "chat_gen" "layout_gen" "ListBox_gen" "ListCtrl_gen" "MoveControl_gen" "MultiLang_gen" "render_gen" "RichEdit_gen" "VirtualListBox_gen" "threads_gen" "TreeView_gen" "ChildWindow_gen" "XmlPreview_gen" "basic_code" "controls_code" "ColorPicker_code" "DpiAware_code" "chat_code" "layout_code" "ListBox_code" "ListCtrl_code" "MoveControl_code" "MultiLang_code" "render_code" "RichEdit_code" "VirtualListBox_code" "threads_code" "TreeView_code" "ChildWindow_code" "XmlPreview_code")
for duilib_bin in "${DUILIB_PROGRAMS[@]}"; do
    echo "Building example: $duilib_bin"
    $DUILIB_CMAKE -S "$DUILIB_SRC_ROOT_DIR/examples/$duilib_bin" -B "$DUILIB_BUILD_DIR/$duilib_bin" -DCMAKE_BUILD_TYPE=${DUILIB_BUILD_TYPE} -DDUILIB_SKIA_LIB_SUBPATH="$DUILIB_SKIA_LIB_SUBPATH" ${DUILIB_WAYLAND_FLAG}
    $DUILIB_MAKE "$DUILIB_BUILD_DIR/$duilib_bin" $DUILIB_MAKE_THREADS
done

echo ""
echo "======================================"
echo "Wayland build completed successfully!"
echo "Library: $DUILIB_SRC_ROOT_DIR/lib/libduilib.a"
echo "Binaries: $DUILIB_SRC_ROOT_DIR/bin/"
echo "======================================"

