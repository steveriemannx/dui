#!/bin/bash
if [ "$(uname -s)" != "Darwin" ]; then
    echo "Please run this script on macOS system."
    exit 1
fi

DUILIB_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
SKIA_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../skia" && pwd)
echo "DUILIB_SRC_ROOT_DIR: $DUILIB_SRC_ROOT_DIR"
echo "SKIA_SRC_ROOT_DIR: $SKIA_SRC_ROOT_DIR"

# Arguments:
#   --fresh       clean and re-configure (incremental by default)
#   --standalone  build each example as an independent CMake project (legacy mode); default uses top-level CMake management
DUILIB_CMAKE_REFRESH=
if [[ "$*" == *"--fresh"* ]]; then
    cmake_version=$(cmake --version | grep -oE '[0-9]+\.[0-9]+')
    required_version=3.24
    if [ $(echo "$cmake_version >= $required_version" | bc) -eq 1 ]; then
        DUILIB_CMAKE_REFRESH=--fresh
    fi
fi
STANDALONE=false
if [[ "$*" == *"--standalone"* ]]; then
    STANDALONE=true
fi

# Set the compiler
DUILIB_CC=clang
DUILIB_CXX=clang++
DUILIB_COMPILER_ID=llvm

DUILIB_CMAKE="cmake ${DUILIB_CMAKE_REFRESH} -DCMAKE_C_COMPILER=$DUILIB_CC -DCMAKE_CXX_COMPILER=$DUILIB_CXX"
DUILIB_MAKE="cmake --build"
DUILIB_MAKE_THREADS="-j 6"

# Build type: Debug or Release
DUILIB_BUILD_TYPE=Release

# Skia library subdirectory, fixed to the LLVM build; comment out this block to use the default rules
# Get the CPU architecture
CPU_ARCH_STR=$(uname -m)

# Convert to the standard architecture name
if [ "$CPU_ARCH_STR" = "x86_64" ] || [ "$CPU_ARCH_STR" = "amd64" ]; then
    CPU_ARCH=x64
elif [ "$CPU_ARCH_STR" = "aarch64" ] || [ "$CPU_ARCH_STR" = "arm64" ]; then
    CPU_ARCH=arm64
elif [ "$CPU_ARCH_STR" = "armv7l" ]; then
    CPU_ARCH=arm
elif [ "$CPU_ARCH_STR" = "i386" ] || [ "$CPU_ARCH_STR" = "i686" ]; then
    CPU_ARCH=x86
else
    CPU_ARCH=arm64
fi

DUILIB_SKIA_LIB_SUBPATH=llvm.${CPU_ARCH}.release
echo "DUILIB_SKIA_LIB_SUBPATH:${DUILIB_SKIA_LIB_SUBPATH}"
if [[ ! -d "${SKIA_SRC_ROOT_DIR}/out/${DUILIB_SKIA_LIB_SUBPATH}" ]]; then
    echo "Please compile the skia first or run build_duilib_all_in_one.sh."
    exit 1
fi

# Build temporary directory
DUILIB_BUILD_DIR="$DUILIB_SRC_ROOT_DIR/scripts/build_temp/${DUILIB_COMPILER_ID}_build"

# ============================================================
# Top-level CMake build (default; follows the develop2 branch):
#   Configure the whole repository at once (duilib + third-party libraries + all examples),
#   Build everything at once; use --target <example name> to build a single target
# ============================================================
if [ "$STANDALONE" = false ]; then
    DUILIB_TOP_BUILD_DIR="$DUILIB_BUILD_DIR/top"
    mkdir -p "$DUILIB_TOP_BUILD_DIR"

    $DUILIB_CMAKE -S "$DUILIB_SRC_ROOT_DIR" -B "$DUILIB_TOP_BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=${DUILIB_BUILD_TYPE} \
        -DDUILIB_SKIA_LIB_SUBPATH="$DUILIB_SKIA_LIB_SUBPATH"
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
target_dir="$DUILIB_BUILD_DIR"
if [[ ! -d "$target_dir" ]]; then
    mkdir -p "$target_dir"
fi

# Build third-party libraries
DUILIB_THIRD_PARTY_LIBS=("zlib" "libpng" "cximage" "libwebp" "libcef/libcef_macos")
for third_party_lib in "${DUILIB_THIRD_PARTY_LIBS[@]}"; do
    $DUILIB_CMAKE -S "$DUILIB_SRC_ROOT_DIR/duilib/third_party/$third_party_lib" -B "$DUILIB_BUILD_DIR/$third_party_lib" -DCMAKE_BUILD_TYPE=${DUILIB_BUILD_TYPE}
    $DUILIB_MAKE "$DUILIB_BUILD_DIR/$third_party_lib" $DUILIB_MAKE_THREADS
done

# Build duilib
$DUILIB_CMAKE -S "$DUILIB_SRC_ROOT_DIR/duilib" -B "$DUILIB_BUILD_DIR/duilib" -DCMAKE_BUILD_TYPE=${DUILIB_BUILD_TYPE}
$DUILIB_MAKE "$DUILIB_BUILD_DIR/duilib" $DUILIB_MAKE_THREADS

# Build each program under examples
# Three modes: XML mode (the original 21) + mode 2 code generation (*_gen) + mode 3 pure code (*_code)
DUILIB_PROGRAMS=("basic" "controls" "ColorPicker" "DpiAware" "chat" "layout" "ListBox" "ListCtrl" "MoveControl" "MultiLang" "render" "RichEdit" "VirtualListBox" "threads" "TreeView" "cef" "CefBrowser" "ChildWindow" "XmlPreview"
                 "basic_gen" "controls_gen" "ColorPicker_gen" "DpiAware_gen" "chat_gen" "layout_gen" "ListBox_gen" "ListCtrl_gen" "MoveControl_gen" "MultiLang_gen" "render_gen" "RichEdit_gen" "VirtualListBox_gen" "threads_gen" "TreeView_gen" "ChildWindow_gen" "XmlPreview_gen"
                 "basic_code" "controls_code" "ColorPicker_code" "DpiAware_code" "chat_code" "layout_code" "ListBox_code" "ListCtrl_code" "MoveControl_code" "MultiLang_code" "render_code" "RichEdit_code" "VirtualListBox_code" "threads_code" "TreeView_code" "ChildWindow_code" "XmlPreview_code")
for duilib_bin in "${DUILIB_PROGRAMS[@]}"; do
    $DUILIB_CMAKE -S "$DUILIB_SRC_ROOT_DIR/examples/$duilib_bin" -B "$DUILIB_BUILD_DIR/$duilib_bin" -DCMAKE_BUILD_TYPE=${DUILIB_BUILD_TYPE} -DDUILIB_SKIA_LIB_SUBPATH="$DUILIB_SKIA_LIB_SUBPATH"
    $DUILIB_MAKE "$DUILIB_BUILD_DIR/$duilib_bin" $DUILIB_MAKE_THREADS
done
