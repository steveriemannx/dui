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

if ! is_windows; then
    echo "Please run this script on Windows/MSYS2 system."
    exit 1
fi

if [ "$1" == "-sdl" ]; then    
    SDL_PARAM="-DDUI_ENABLE_SDL=ON"
else
    SDL_PARAM=""
fi

echo "SDL_PARAM: ${SDL_PARAM}"

DUI_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
SKIA_SRC_ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../../skia" && pwd)
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
    DUI_COMPILER_ID=msys2-llvm
else
    DUI_CC=gcc
    DUI_CXX=g++
    DUI_COMPILER_ID=msys2-gcc
fi

DUI_CMAKE="cmake --fresh -DCMAKE_C_COMPILER=$DUI_CC -DCMAKE_CXX_COMPILER=$DUI_CXX"
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
if [ "$has_clang" -eq 1 ]; then
    DUI_SKIA_LIB_SUBPATH=llvm.${CPU_ARCH}.release
else
    DUI_SKIA_LIB_SUBPATH=gcc.${CPU_ARCH}.release
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

# Build third-party libraries
DUI_THIRD_PARTY_LIBS=("zlib" "libpng" "cximage" "libwebp")
for third_party_lib in "${DUI_THIRD_PARTY_LIBS[@]}"; do
    $DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/dui/third_party/$third_party_lib" -B "$DUI_BUILD_DIR/$third_party_lib" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE}
    $DUI_MAKE "$DUI_BUILD_DIR/$third_party_lib" $DUI_MAKE_THREADS
done

# Build dui
$DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/dui" -B "$DUI_BUILD_DIR/dui" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE} ${SDL_PARAM}
$DUI_MAKE "$DUI_BUILD_DIR/dui" $DUI_MAKE_THREADS

# Build each program under examples
DUI_PROGRAMS=("basic" "controls" "ColorPicker" "DpiAware" "chat" "layout" "ListBox" "ListCtrl" "MoveControl" "MultiLang" "render" "RichEdit" "VirtualListBox" "threads" "TreeView" "WebView2" "WebView2Browser" "ChildWindow" "XmlPreview")
for dui_bin in "${DUI_PROGRAMS[@]}"; do
    $DUI_CMAKE -S "$DUI_SRC_ROOT_DIR/examples/$dui_bin" -B "$DUI_BUILD_DIR/$dui_bin" -DCMAKE_BUILD_TYPE=${DUI_BUILD_TYPE} -DDUI_SKIA_LIB_SUBPATH="$DUI_SKIA_LIB_SUBPATH" ${SDL_PARAM}
    $DUI_MAKE "$DUI_BUILD_DIR/$dui_bin" $DUI_MAKE_THREADS
done
