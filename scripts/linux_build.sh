#!/bin/bash

# check windows
is_windows() {
    case "$(uname -s)" in
        CYGWIN*|MINGW32*|MSYS*|MINGW*)
            return 0  # Windows
            ;;
        *)
            return 1  # Not Windows
            ;;
    esac
}

if [ "$(uname -s)" == "Darwin" ]; then
    echo "Please run this script on Linux system."
    exit 1
elif [ "$(uname -s)" == "FreeBSD" ]; then
    echo "Please run this script on Linux system."
    exit 1
elif is_windows; then
    echo "Please run this script on Linux system."
    exit 1
else
    echo "Linux"
fi

DUI_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
SKIA_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../third_party/skia" && pwd)
echo "DUI_SRC_ROOT_DIR: $DUI_SRC_ROOT_DIR"
echo "SKIA_SRC_ROOT_DIR: $SKIA_SRC_ROOT_DIR"

# compiler flag
has_gcc=0
has_clang=0

# gcc/g++
if command -v gcc &> /dev/null && command -v g++ &> /dev/null; then
    has_gcc=1
fi

# clang/clang++
if command -v clang &> /dev/null && command -v clang++ &> /dev/null; then
    has_clang=1
fi

if [ "$has_gcc$has_clang" == "00" ]; then
    echo "- GCC/G++ not found in PATH"
    echo "- Clang/Clang++ not found in PATH"
    exit 1
fi

# Set the compiler
if [ "$has_clang" -eq 1 ]; then
    DUI_CC=clang
    DUI_CXX=clang++
    DUI_COMPILER_ID=llvm
else
    DUI_CC=gcc
    DUI_CXX=g++
    DUI_COMPILER_ID=gcc
fi

# UOS always uses gcc: clang++ causes link errors
if grep -q "ID=uos" /etc/os-release; then
    echo "UOS"
    DUI_CC=gcc
    DUI_CXX=g++
    DUI_COMPILER_ID=gcc
fi

# --fresh only on request (like macos_build.sh): incremental configure by default
DUI_CMAKE_REFRESH=
if [[ "$*" == *"--fresh"* ]]; then
    cmake_version=$(cmake --version | grep -oE '[0-9]+\.[0-9]+')
    required_version=3.24
    if [ $(echo "$cmake_version >= $required_version" | bc) -eq 1 ]; then
        DUI_CMAKE_REFRESH=--fresh
    fi
fi

DUI_CMAKE="cmake ${DUI_CMAKE_REFRESH} -DCMAKE_C_COMPILER=$DUI_CC -DCMAKE_CXX_COMPILER=$DUI_CXX"
DUI_MAKE="cmake --build"
DUI_MAKE_THREADS="-j 6"

# Build type: Debug or Release
DUI_BUILD_TYPE=Release

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
    CPU_ARCH=x64
fi

# Check available skia libs
if [[ -d "${SKIA_SRC_ROOT_DIR}/out/llvm.${CPU_ARCH}.release" ]]; then
    DUI_SKIA_LIB_SUBPATH=llvm.${CPU_ARCH}.release
elif [[ -d "${SKIA_SRC_ROOT_DIR}/out/gcc.${CPU_ARCH}.release" ]]; then
    DUI_SKIA_LIB_SUBPATH=gcc.${CPU_ARCH}.release
else
    DUI_SKIA_LIB_SUBPATH=llvm.${CPU_ARCH}.release
fi

if [[ ! -d "${SKIA_SRC_ROOT_DIR}/out/${DUI_SKIA_LIB_SUBPATH}" ]]; then
    echo "Please compile the skia first or run build_dui_all_in_one.sh."
    exit 1
fi

echo "DUI_SKIA_LIB_SUBPATH:${DUI_SKIA_LIB_SUBPATH}"

# Build temporary directory
DUI_BUILD_DIR="$DUI_SRC_ROOT_DIR/scripts/build_temp/${DUI_COMPILER_ID}_build"

target_dir="$DUI_BUILD_DIR"
if [[ ! -d "$target_dir" ]]; then
    mkdir -p "$target_dir"
fi

# ============================================================
# Top-level CMake build (default; follows the develop2 branch):
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
        -DDUI_SKIA_LIB_SUBPATH="$DUI_SKIA_LIB_SUBPATH"
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
# Build third-party libraries
DUI_THIRD_PARTY_LIBS=("zlib" "libpng" "cximage" "libwebp" "libcef/libcef_linux")
for third_party_lib in "${DUI_THIRD_PARTY_LIBS[@]}"; do
    $DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/dui/third_party/$third_party_lib" -B "$DUI_BUILD_DIR/$third_party_lib" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE}
    $DUI_MAKE "$DUI_BUILD_DIR/$third_party_lib" $DUI_MAKE_THREADS
done

# Build dui
$DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/dui" -B "$DUI_BUILD_DIR/dui" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE}
$DUI_MAKE "$DUI_BUILD_DIR/dui" $DUI_MAKE_THREADS

# Build each program under examples
# Three modes: XML mode (the original 21) + mode 2 code generation (*_gen) + mode 3 pure code (*_code)
DUI_PROGRAMS=("basic" "controls" "ColorPicker" "DpiAware" "chat" "layout" "ListBox" "ListCtrl" "MoveControl" "MultiLang" "render" "RichEdit" "VirtualListBox" "threads" "TreeView" "cef" "CefBrowser" "ChildWindow" "XmlPreview" "codeui" "basic_gen" "controls_gen" "ColorPicker_gen" "DpiAware_gen" "chat_gen" "layout_gen" "ListBox_gen" "ListCtrl_gen" "MoveControl_gen" "MultiLang_gen" "render_gen" "RichEdit_gen" "VirtualListBox_gen" "threads_gen" "TreeView_gen" "ChildWindow_gen" "XmlPreview_gen" "basic_code" "controls_code" "ColorPicker_code" "DpiAware_code" "chat_code" "layout_code" "ListBox_code" "ListCtrl_code" "MoveControl_code" "MultiLang_code" "render_code" "RichEdit_code" "VirtualListBox_code" "threads_code" "TreeView_code" "ChildWindow_code" "XmlPreview_code")
for dui_bin in "${DUI_PROGRAMS[@]}"; do
    $DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/examples/$dui_bin" -B "$DUI_BUILD_DIR/$dui_bin" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE} -DDUI_SKIA_LIB_SUBPATH="$DUI_SKIA_LIB_SUBPATH"
    $DUI_MAKE "$DUI_BUILD_DIR/$dui_bin" $DUI_MAKE_THREADS
done
