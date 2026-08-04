#!/usr/bin/env bash
# Build Skia from the forked repository (steveriemannx/skia, dui branch)
# Prerequisites: gn, ninja, clang/clang++ in PATH
#
# Usage:
#   ./scripts/build_skia.sh              # build for current platform (Release)
#   ./scripts/build_skia.sh Debug        # Debug build
#   ./scripts/build_skia.sh Release x64  # explicit arch
#
# The Skia source is expected at ../skia/ relative to the dui project root.
# Fetch it first if not already present (zip archive, no git clone needed):
#   curl -L -o skia.zip https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.0.zip
#   unzip skia.zip -d skia_tmp && mv skia_tmp/skia-skia-dui-0.1.0 ../skia

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SKIA_DIR="$(cd "${SCRIPT_DIR}/../../skia" && pwd)"

if [ ! -f "${SKIA_DIR}/BUILD.gn" ]; then
    echo "ERROR: Skia source not found at ${SKIA_DIR}"
    echo "Fetch the fork zip first (see the header comment of this script),"
    echo "or run the repository's fetcher:"
    echo "  scripts/fetch_skia.sh"
    exit 1
fi

BUILD_TYPE="${1:-Release}"
CPU_ARCH="${2:-}"
if [ -z "${CPU_ARCH}" ]; then
    case "$(uname -m)" in
        x86_64|amd64) CPU_ARCH=x64 ;;
        aarch64|arm64) CPU_ARCH=arm64 ;;
        *) CPU_ARCH=x64 ;;
    esac
fi

CC="${CC:-clang}"
CXX="${CXX:-clang++}"

# Lowercase build type for path
BUILD_TYPE_LOWER=$(echo "${BUILD_TYPE}" | tr '[:upper:]' '[:lower:]')
BUILD_DIR="${SKIA_DIR}/out/llvm.${CPU_ARCH}.${BUILD_TYPE_LOWER}"

echo "=== Building Skia ==="
echo "Source:   ${SKIA_DIR}"
echo "Build:    ${BUILD_DIR}"
echo "Arch:     ${CPU_ARCH}"
echo "Compiler: ${CC} / ${CXX}"
echo "Type:     ${BUILD_TYPE}"
echo ""

cd "${SKIA_DIR}"

if [ "$(uname)" = "FreeBSD" ]; then
    gn gen "${BUILD_DIR}" --args="target_cpu=\"${CPU_ARCH}\" ar=\"llvm-ar\" skia_enable_fontmgr_fontconfig=true skia_use_freetype=true skia_system_freetype2_include_path=\"/usr/local/include/freetype2\" extra_ldflags = [ \"-L/usr/local/lib\" ] cc=\"${CC}\" cxx=\"${CXX}\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=$([ "${BUILD_TYPE}" = "Debug" ] && echo true || echo false) extra_cflags=[\"-I/usr/local/include\", \"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
else
    gn gen "${BUILD_DIR}" --args="target_cpu=\"${CPU_ARCH}\" cc=\"${CC}\" cxx=\"${CXX}\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=$([ "${BUILD_TYPE}" = "Debug" ] && echo true || echo false) extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
fi

echo ""
echo "gn gen complete. Running ninja..."
ninja -C "${BUILD_DIR}"

echo ""
echo "=== Skia build complete ==="
echo "Library: ${BUILD_DIR}/libskia.a"
ls -lh "${BUILD_DIR}/libskia.a"
