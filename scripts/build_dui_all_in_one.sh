#!/usr/bin/env bash

# Linux / MacOS / FreeBSD / MSYS2(Windows)

# Force enable SDL (MSYS2-Windows only)
ENABLE_SDL=0
if [ "$1" == "-sdl" ]; then
    ENABLE_SDL=1
fi

# Force enable Wayland (Linux only)
ENABLE_WAYLAND=0
if [ "$1" == "-wayland" ]; then
    ENABLE_WAYLAND=1
fi

CURRENT_DIR=$(pwd)
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

CPU_ARCH_STR=$(uname -m)
if [ "$CPU_ARCH_STR" = "x86_64" ] || [ "$CPU_ARCH_STR" = "amd64" ]; then
    CPU_ARCH=x64
elif [ "$CPU_ARCH_STR" = "aarch64" ] || [ "$CPU_ARCH_STR" = "arm64" ]; then
    CPU_ARCH=arm64
elif [ "$CPU_ARCH_STR" = "armv7l" ]; then
    CPU_ARCH=arm
elif [ "$CPU_ARCH_STR" = "i386" ] || [ "$CPU_ARCH_STR" = "i686" ]; then
    CPU_ARCH=x86
else
    if [ "$(uname -s)" == "Darwin" ]; then
        CPU_ARCH=arm64
    else
        CPU_ARCH=x64
    fi
fi

# Checking the necessary software
if ! command -v git &> /dev/null
then
    echo "- git not found!"
    exit 1
else
    echo "git found at:"
    which git
fi

if ! command -v python3 &> /dev/null
then
    echo "- python3 not found!"
    exit 1
else
    echo "python3 found at:"
    which python3
fi

if ! command -v ninja &> /dev/null
then
    echo "- ninja not found!"
    exit 1
else
    echo "ninja found at:"
    which ninja
fi

if ! command -v unzip &> /dev/null
then
    echo "- unzip not found!"
    exit 1
else
    echo "unzip found at:"
    which unzip
fi

if ! command -v cmake &> /dev/null
then
    echo "- cmake not found!"
    exit 1
else
    echo "cmake found at:"
    which cmake
fi

# flag
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

cd "$SCRIPT_DIR"
pwd
# zip source archives have no .git dir, so locate the repo root by CMakeLists.txt
if [ ! -f "./dui/CMakeLists.txt" ]; then
    if [ -f "../../dui/CMakeLists.txt" ]; then
        cd ../../
    fi
fi
pwd

start_time=$(date +%s)
retry_delay=10

# Download a zip archive (curl preferred, wget fallback) with retry
# $1: URL, $2: output file
download_zip() {
    local url="$1"
    local output="$2"
    if ! command -v curl &> /dev/null && ! command -v wget &> /dev/null; then
        echo "- curl/wget not found!"
        exit 1
    fi
    echo "=== Downloading: $url ==="
    if command -v curl &> /dev/null; then
        curl -L -f -C - -o "$output" --retry 3 --connect-timeout 15 --retry-delay 10 --speed-limit 100 --speed-time 120 "$url"
    else
        wget -O "$output" -t 3 --timeout 15 -c "$url"
    fi
    if [ $? -ne 0 ]; then
        sleep $retry_delay
        download_zip "$url" "$output"
    fi
}

# Extract a zip archive into $2, stripping the single top-level folder
# $1: zip path, $2: dest dir
extract_zip() {
    local zip_path="$1"
    local dest="$2"
    local tmp_dir="$dest.tmp"
    local top_dir
    rm -rf "$tmp_dir" "$dest"
    unzip -q -o "$zip_path" -d "$tmp_dir"
    if [ $? -ne 0 ]; then
        echo "unzip failed: $zip_path"
        exit 1
    fi
    top_dir=$(find "$tmp_dir" -mindepth 1 -maxdepth 1 -type d | head -n 1)
    if [ -z "$top_dir" ]; then
        echo "unexpected zip layout: $zip_path"
        exit 1
    fi
    mkdir -p "$dest"
    cp -R "$top_dir"/. "$dest"/
    rm -rf "$tmp_dir" "$zip_path"
}

SKIA_ZIP_URL="https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.1.zip"
SDL_ZIP_URL="https://github.com/libsdl-org/SDL/releases/download/release-3.4.14/SDL3-3.4.14.zip"

# Fetch SDL into third_party/SDL3 first, then skia (same layout as the CMake build; idempotent)
if ! is_windows || [ "$ENABLE_SDL" == "1" ]; then
    echo "- Fetching SDL ..."
    if [ ! -f "./dui/third_party/SDL3/CMakeLists.txt" ]; then
        download_zip "$SDL_ZIP_URL" ./SDL.zip
        extract_zip ./SDL.zip ./dui/third_party/SDL3
    fi
    if [ ! -f "./dui/third_party/SDL3/CMakeLists.txt" ]; then
        echo "fetch SDL failed!"
        cd "$CURRENT_DIR"
        exit 1
    fi
fi

SKIA_VERSION="skia-dui-0.1.1"
# Fetch skia into third_party/skia (same layout as the CMake build; idempotent).
# The version marker (.dui_skia_version) triggers a re-fetch when the tag changes.
echo "- Fetching skia ..."
if [ -f "./dui/third_party/skia/BUILD.gn" ] && { [ ! -f "./dui/third_party/skia/.dui_skia_version" ] || [ "$(cat ./dui/third_party/skia/.dui_skia_version 2>/dev/null)" != "$SKIA_VERSION" ]; }; then
    echo "  Skia version mismatch; re-fetching ..."
    rm -rf ./dui/third_party/skia
fi
if [ ! -f "./dui/third_party/skia/BUILD.gn" ]; then
    download_zip "$SKIA_ZIP_URL" ./skia.zip
    extract_zip ./skia.zip ./dui/third_party/skia
    printf '%s' "$SKIA_VERSION" > ./dui/third_party/skia/.dui_skia_version
fi
if [ ! -f "./dui/third_party/skia/BUILD.gn" ]; then
    echo "fetch skia failed!"
    cd "$CURRENT_DIR"
    exit 1
fi

# Prefer a system gn; otherwise build gn from source (official instructions; idempotent).
# The clone tries the Google source 2 times, then falls back to a GitHub mirror.
# Slow/hung clones abort quickly: 10s connect timeout, and the transfer fails if it
# stays below 200 KB/s for 30s (git http.lowSpeed* options). Tune the numbers below
# if your network is slower than 200 KB/s but still usable.
gn_git_clone() {
    git -c http.connectTimeout=10 -c http.lowSpeedLimit=204800 -c http.lowSpeedTime=30 clone "$@"
}

# Manual-install hint shown when gn is unavailable (static list; the Debian/Ubuntu
# package is named "generate-ninja" because the name "gn" is taken there)
gn_install_hint() {
    echo ""
    echo "  gn is required to build skia but could not be downloaded. Please install it"
    echo "  manually and re-run this script. Known package names:"
    echo "    Debian/Ubuntu: sudo apt install generate-ninja   (NOT \"gn\")"
    echo "    Fedora/RHEL:   sudo dnf install gn"
    echo "    Arch Linux:    sudo pacman -S gn"
    echo "    FreeBSD:       pkg install gn"
    echo "    MSYS2:         pacman -S mingw-w64-x86_64-gn"
    echo "    macOS:         no package - build gn from source (see the gn README)"
    echo ""
    echo "  Or download a prebuilt gn binary from CIPD (a zip; version pinned in skia's"
    echo "  bin/fetch-gn) and unzip it to dui/third_party/skia/bin/gn (gn.exe on Windows):"
    echo "    https://chrome-infra-packages.appspot.com/dl/gn/gn/windows-amd64/+/git_revision:b2afae122eeb6ce09c52d63f67dc53fc517dbdc8"
    echo "  (use linux-amd64 / mac-amd64 / mac-arm64 / linux-arm64 instead of windows-amd64"
    echo "  for other platforms; the script uses the binary directly once it is in place)"
    echo ""
    echo "  Or clone the gn source into dui/third_party/gn and re-run - the script will"
    echo "  build it from source automatically (clone the full history, not --depth 1):"
    echo "    git clone https://gn.googlesource.com/gn dui/third_party/gn"
    echo "    # Google source unreachable? Use the GitHub mirror instead:"
    echo "    git clone https://github.com/ArthurSonzogni/gn dui/third_party/gn"
    echo ""
}
GN_BIN=""
# gn acquisition order: existing source build -> system gn -> skia's bin/fetch-gn
# (prebuilt CIPD binary) -> clone Google source (2 attempts) -> GitHub mirror
# -> build from source.
# 1) reuse an existing source-built gn
if [ -f "./dui/third_party/gn/out/gn" ]; then
    GN_BIN=$(cd ./dui/third_party/gn/out 2>/dev/null && pwd)/gn
fi
# 2) system gn (zero cost - already installed, no download needed)
if [ -z "$GN_BIN" ] && command -v gn &> /dev/null; then
    echo "- Using system gn: $(command -v gn)"
    GN_BIN=$(command -v gn)
fi
# 3) try skia's own bin/fetch-gn (prebuilt CIPD binary into skia/bin)
if [ -z "$GN_BIN" ] && [ -f "./dui/third_party/skia/bin/fetch-gn" ]; then
    echo "- Trying skia's bin/fetch-gn ..."
    (cd ./dui/third_party/skia && python3 bin/fetch-gn)
    if [ -f "./dui/third_party/skia/bin/gn.exe" ]; then
        GN_BIN=$(cd ./dui/third_party/skia/bin 2>/dev/null && pwd)/gn.exe
    elif [ -x "./dui/third_party/skia/bin/gn" ]; then
        GN_BIN=$(cd ./dui/third_party/skia/bin 2>/dev/null && pwd)/gn
    fi
fi
# 4) build gn from source: clone Google source (2 attempts), fall back to a
#    GitHub mirror, then compile (official instructions; idempotent)
if [ -z "$GN_BIN" ]; then
    echo "- gn not found; building gn from source ..."
    if [ ! -d "./dui/third_party/gn/.git" ]; then
        # NOTE: full clone required - build/gen.py runs `git describe --match initial-commit`
        # to generate last_commit_position.h, which fails on a shallow clone (the tag only
        # exists in the full history). The repo is small (~40MB, ~30s to clone).
        GN_GOOGLE_URL="https://gn.googlesource.com/gn"
        GN_GITHUB_MIRROR_URL="https://github.com/ArthurSonzogni/gn"
        gn_cloned=0
        for attempt in 1 2; do
            echo "  Cloning gn from Google source (attempt ${attempt}/2): ${GN_GOOGLE_URL}"
            if gn_git_clone "${GN_GOOGLE_URL}" ./dui/third_party/gn; then
                gn_cloned=1
                break
            fi
            rm -rf ./dui/third_party/gn
            echo "  Clone failed (or too slow); retrying in 10s ..."
            sleep 10
        done
        if [ "$gn_cloned" -ne 1 ]; then
            echo "  Google source failed after 2 attempts; trying GitHub mirror: ${GN_GITHUB_MIRROR_URL}"
            if ! gn_git_clone "${GN_GITHUB_MIRROR_URL}" ./dui/third_party/gn; then
                echo "clone gn failed from both sources!"
                gn_install_hint
                cd "$CURRENT_DIR"
                exit 1
            fi
        fi
    fi
    if [ -d "./dui/third_party/gn/.git" ] && is_windows; then
        # Windows checkouts convert text files to CRLF (core.autocrlf), which breaks
        # gn's own unit tests (format_test_data comparison); normalize to LF once
        if [ "$(git -C ./dui/third_party/gn config --get core.autocrlf 2>/dev/null)" != "false" ]; then
            echo "  Normalizing gn checkout line endings to LF ..."
            git -C ./dui/third_party/gn config core.autocrlf false
            git -C ./dui/third_party/gn rm --cached -r . >/dev/null 2>&1
            git -C ./dui/third_party/gn reset --hard >/dev/null 2>&1
        fi
    fi
    (cd ./dui/third_party/gn && python3 build/gen.py && ninja -C out)
    GN_BIN=$(cd ./dui/third_party/gn/out 2>/dev/null && pwd)/gn
    if [ ! -x "$GN_BIN" ]; then
        echo "gn build failed! Install gn or check the build log above."
        gn_install_hint
        cd "$CURRENT_DIR"
        exit 1
    fi
fi

if is_windows; then
    echo "ENABLE_SDL: $ENABLE_SDL"
fi

# download CEF
# Function: Download HTTPS files with curl (supports auto-retry on failure)
# Parameter 1: Target URL (required, HTTPS protocol)
# Parameter 2: Save path (optional, default: current directory, filename extracted from URL)
# Parameter 3: Max retry attempts (optional, default: 3 times)
# Parameter 4: Single request timeout (optional, default: 15 seconds)
curl_download_with_retry() {
    # Validate required parameter
    if [ -z "$1" ]; then
        echo "Error: Target URL (1st parameter) not specified"
        return 1
    fi

    # Initialize parameters with default values
    local url="$1"
    local save_path="${2:-$(basename "$url")}"  # Default: use filename from URL
    local max_retries="${3:-3}"
    local timeout="${4:-15}"
    local retry_count=0

    echo "=== Starting download(curl): $url ==="
    echo "Save path: $save_path"
    echo "Max retries: $max_retries, Timeout per attempt: $timeout seconds"

    # Loop for download with retries
    while [ $retry_count -le $max_retries ]; do
        # Core curl parameters explanation:
        # -L: Follow 301/302 redirects (common for HTTPS)
        # -f: Return non-zero exit code on failure (for error checking)
        # -o: Specify output file path
        # --retry: Number of retries (only for network errors)
        # --connect-timeout: Timeout for establishing connection
        curl -L -f -C - -o "$save_path" \
            --retry "$max_retries" \
            --connect-timeout "$timeout" \
            --retry-delay 30 --speed-limit 100 --speed-time 120 \
            "$url"

        # Check download result
        if [ $? -eq 0 ]; then
            echo "Download successful! File saved to: $save_path"
            return 0
        else
            retry_count=$((retry_count + 1))
            if [ $retry_count -le $max_retries ]; then
                echo "$retry_count-th download failed. $((max_retries - retry_count)) retries left. Retrying in 10 seconds..."
                sleep 10
            else
                echo "All retries failed (total $max_retries attempts). Please check URL validity or network connection."
                return 1
            fi
        fi
    done
}

# Function: Download HTTPS files with wget (supports auto-retry on failure)
# Parameter 1: Target URL (required, HTTPS protocol)
# Parameter 2: Save path (optional, default: current directory, filename extracted from URL)
# Parameter 3: Max retry attempts (optional, default: 3 times)
# Parameter 4: Single request timeout (optional, default: 15 seconds)
wget_download_with_retry() {
    # Validate required parameter
    if [ -z "$1" ]; then
        echo "Error: Target URL (1st parameter) not specified"
        return 1
    fi

    # Initialize parameters with default values
    local url="$1"
    local save_path="${2:-$(basename "$url")}"  # Default: use filename from URL
    local max_retries="${3:-3}"
    local timeout="${4:-15}"

    echo "=== Starting download(wget): $url ==="
    echo "Save path: $save_path"
    echo "Max retries: $max_retries, Timeout per attempt: $timeout seconds"

    # Core wget parameters explanation:
    # -O: Specify output file path (uppercase O, distinguish from lowercase -o for logs)
    # -t: Number of retries (0 = unlimited, here use max_retries)
    # --timeout: Timeout for connection and data transfer (in seconds)
    # -c: Resume broken download (supports resuming if download is interrupted)
    wget -O "$save_path" \
         -t "$max_retries" \
         --timeout "$timeout" \
         -c \
         "$url"

    # Check download result
    if [ $? -eq 0 ]; then
        echo "Download successful! File saved to: $save_path"
        return 0
    else
        echo "All retries failed (total $max_retries attempts). Please check URL validity or network connection."
        return 1
    fi
}

# flag
has_curl=0
has_wget=0
has_linux=0
has_macos=0

# curl
if command -v curl &> /dev/null; then
    has_curl=1
fi

# wget
if command -v wget &> /dev/null; then
    has_wget=1
fi

if [ "$(uname -s)" == "Darwin" ]; then
    has_macos=1
elif [ "$(uname -s)" == "FreeBSD" ]; then
    has_linux=0
elif is_windows; then
    has_linux=0
else
    has_linux=1
fi

if [ "$has_curl$has_wget" != "00" ] && [ "$has_linux$has_macos" != "00" ] && [ "$ENABLE_WAYLAND" != "1" ]; then
    # download CEF on Linux and MacOS (skipped for Wayland builds).
    # Shares the CMake flow's cache (third_party/downloads/) and install target
    # (third_party/libcef/cef_binary): no download when the archive is already cached.
    # Unified runtime directory (same name on Windows/Linux/macOS app bundle)
    libcef_runtime_dest_dir=./dui/bin/cef_binary
    libcef_cef_binary_dir=./dui/third_party/libcef/cef_binary
    libcef_cache_dir=./dui/third_party/downloads
    if [ -f "$libcef_cef_binary_dir/CMakeLists.txt" ]; then
        echo "- CEF already present: $libcef_cef_binary_dir"
    else
    # libcef file name prefix (same full archive as the CMake flow)
    libcef_file_name_prefix=cef_binary_142.0.10+g29548e2+chromium-142.0.7444.135
    if [ "$has_linux" == "1" ]; then
        # Linux
        if [ "$CPU_ARCH" == "arm64" ]; then
            libcef_file_name="${libcef_file_name_prefix}_linuxarm64"
        else
            libcef_file_name="${libcef_file_name_prefix}_linux64"
        fi
    else
        # MacOS
        if [ "$CPU_ARCH" == "arm64" ]; then
            libcef_file_name="${libcef_file_name_prefix}_macosarm64"
        else
            libcef_file_name="${libcef_file_name_prefix}_macosx64"
        fi
    fi

    if [ "$libcef_file_name" != "" ]; then
        # Use the shared cache; download into it only when missing
        libcef_local_file=${libcef_file_name}.tar.bz2
        libcef_cached_file="$libcef_cache_dir/$libcef_local_file"
        if [ -f "$libcef_cached_file" ]; then
            echo "- CEF archive cached: $libcef_cached_file"
        else
            if [ "$has_curl" == "1" ]; then
                curl_download_with_retry "https://cef-builds.spotifycdn.com/${libcef_file_name}.tar.bz2" "$libcef_cached_file" 100 30
            else
                wget_download_with_retry "https://cef-builds.spotifycdn.com/${libcef_file_name}.tar.bz2" "$libcef_cached_file" 100 30
            fi
        fi

        if [ -f "$libcef_cached_file" ]; then
            # Linux and MacOS: extract .tar.bz2 into third_party/libcef/
            echo "Extracting: ${libcef_cached_file} ..."
            mkdir -p ./dui/third_party/libcef
            tar -xjf "$libcef_cached_file" -C ./dui/third_party/libcef/
            if [ -d "./dui/third_party/libcef/$libcef_file_name" ]; then
                rm -rf "$libcef_cef_binary_dir"
                mv "./dui/third_party/libcef/$libcef_file_name" "$libcef_cef_binary_dir"
                echo "Extracted: ${libcef_cached_file}."

                if [ "$has_linux" == "1" ]; then
                    # Linux
                    mkdir -p $libcef_runtime_dest_dir
                    cp -rf $libcef_cef_binary_dir/Release/* $libcef_runtime_dest_dir
                    cp -rf $libcef_cef_binary_dir/Resources/* $libcef_runtime_dest_dir
                fi
            fi
        fi
        if [ ! -f "$libcef_cached_file" ]; then
            echo ""
            echo "CEF download failed! Options:"
            echo "  - re-run this script later (it retries automatically), or"
            echo "  - download the archive manually from https://cef-builds.spotifycdn.com/ and"
            echo "    place the .tar.bz2 file into $libcef_cache_dir (keep the original"
            echo "    filename; the script skips the download when the archive is cached), or"
            echo "    extract it into $libcef_cef_binary_dir (the script skips CEF when it"
            echo "    is already present there)"
            echo ""
        fi
    fi
    fi
fi
# download CEF end

# Build SDL3 before skia (faster feedback on SDL issues; skia is the longest step)
cmake_version=$(cmake --version | grep -oE '[0-9]+\.[0-9]+')
required_version=3.24
if [ $(echo "$cmake_version >= $required_version" | bc) -eq 1 ]; then
    DUI_CMAKE_REFRESH=--fresh
else
    DUI_CMAKE_REFRESH=
fi

# Skip the SDL build when the library is installed AND the source is not newer than it
# (a refreshed source zip re-extracts CMakeLists.txt with a new timestamp -> rebuild).
sdl_needs_build() {
    local lib_file
    lib_file=$(ls -t ./dui/third_party/SDL3/lib/libSDL3* ./dui/third_party/SDL3/lib/SDL3* 2>/dev/null | head -1)
    if [ -z "$lib_file" ]; then
        return 0  # not installed -> build
    fi
    if [ "./dui/third_party/SDL3/CMakeLists.txt" -nt "$lib_file" ]; then
        return 0  # source newer than the installed lib -> rebuild
    fi
    echo "- SDL3 already installed: $lib_file"
    return 1  # skip
}

# Parallel build jobs: 3/4 of the CPU cores (leave the rest for the system)
if [ "$(uname -s)" = "Darwin" ] || [ "$(uname -s)" = "FreeBSD" ]; then
    DUI_JOBS=$(( $(sysctl -n hw.ncpu) * 3 / 4 ))
else
    DUI_JOBS=$(( $(nproc) * 3 / 4 ))
fi
[ "$DUI_JOBS" -lt 1 ] && DUI_JOBS=1

if ! is_windows; then
    # build SDL on Linux/MacOS (no --fresh: the incremental build directory in scripts/build_temp is kept)
    if sdl_needs_build; then
        echo "- Building SDL ..."
        cmake -S "./dui/third_party/SDL3/" -B "./dui/scripts/build_temp/sdl3-build" -DCMAKE_INSTALL_PREFIX="./dui/third_party/SDL3" -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST_LIBRARY=OFF -DSDL_X11_XSCRNSAVER=OFF -DSDL_X11_XTEST=OFF -DCMAKE_BUILD_TYPE=Release
        cmake --build ./dui/scripts/build_temp/sdl3-build
        cmake --install ./dui/scripts/build_temp/sdl3-build
    fi
elif [ "$ENABLE_SDL" == "1" ]; then
    # build SDL on Windows (same skip logic)
    if sdl_needs_build; then
        echo "- Building SDL ..."
        if [ "$has_clang" -eq 1 ]; then
            DUI_SDL_DIR=sdl3-build.msys2.llvm
        else
            DUI_SDL_DIR=sdl3-build.msys2.gcc
        fi
        cmake -S "./dui/third_party/SDL3/" -B "./dui/scripts/build_temp/${DUI_SDL_DIR}" -DCMAKE_INSTALL_PREFIX="./dui/third_party/SDL3" -DSDL_SHARED=OFF -DSDL_STATIC=ON -DSDL_TEST_LIBRARY=OFF -DCMAKE_BUILD_TYPE=Release
        cmake --build ./dui/scripts/build_temp/${DUI_SDL_DIR} -j ${DUI_JOBS}
        cmake --install ./dui/scripts/build_temp/${DUI_SDL_DIR}
    fi
fi

# Verify gn is built before compiling skia
if [ ! -x "$GN_BIN" ]; then
    echo "gn not available: $GN_BIN (build it first or install gn)"
    cd "$CURRENT_DIR"
    exit 1
fi

echo "- Building skia ..."
# skia's find_headers.py action runs "gn" from PATH (non-Windows); make ours visible
if [ -n "$GN_BIN" ] && [ -x "$GN_BIN" ]; then
    GN_DIR=$(cd "$(dirname "$GN_BIN")" 2>/dev/null && pwd)
    case ":$PATH:" in
        *":$GN_DIR:"*) ;;
        *) export PATH="$GN_DIR:$PATH" ;;
    esac
fi
if [ "$has_clang" -eq 1 ]; then
    # clang/clang++
    echo "clang++ found at:"
    which clang++

    echo "clang found at:"
    which clang

    cd ./dui/third_party/skia
    # Check if the system is FreeBSD
    if [ "$(uname)" = "FreeBSD" ]; then
        echo "Building for FreeBSD"
        $GN_BIN gen out/llvm.${CPU_ARCH}.release --args="target_cpu=\"${CPU_ARCH}\" ar=\"llvm-ar\" skia_enable_fontmgr_fontconfig=true skia_use_freetype=true extra_ldflags = [ \"-L/usr/local/lib\" ] cc=\"clang\" cxx=\"clang++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\", \"-I/usr/local/include/freetype2\", \"-I/usr/local/include\"]"
    else
        $GN_BIN gen out/llvm.${CPU_ARCH}.release --args="target_cpu=\"${CPU_ARCH}\" cc=\"clang\" cxx=\"clang++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        
    fi
    ninja -C out/llvm.${CPU_ARCH}.release
    cd ../../../
else
    if [ "$has_gcc" -eq 1 ]; then
        # gcc/g++
        echo "g++ found at:"
        which g++

        echo "gcc found at:"
        which gcc

        cd ./dui/third_party/skia
        $GN_BIN gen out/gcc.${CPU_ARCH}.release --args="target_cpu=\"${CPU_ARCH}\" cc=\"gcc\" cxx=\"g++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        ninja -C out/gcc.${CPU_ARCH}.release
        cd ../../../
    fi
fi


# build dui
echo "- Building dui ..."
if [ "$(uname -s)" == "Darwin" ]; then
    echo "macOS"
    chmod +x ./dui/scripts/macos_build.sh
    ./dui/scripts/macos_build.sh
elif [ "$(uname -s)" == "FreeBSD" ]; then
    echo "FreeBSD"
    chmod +x ./dui/scripts/freebsd_build.sh
    ./dui/scripts/freebsd_build.sh
elif is_windows; then
    echo "Windows"
    ./dui/scripts/msys2_build.sh $1
else
    echo "Linux"
    if [ "$ENABLE_WAYLAND" == "1" ]; then
        echo "  using Wayland backend"
        chmod +x ./dui/scripts/linux_build_wayland.sh
        ./dui/scripts/linux_build_wayland.sh
    else
        chmod +x ./dui/scripts/linux_build.sh
        ./dui/scripts/linux_build.sh
    fi
fi

cd "$CURRENT_DIR"
echo

end_time=$(date +%s)
duration=$((end_time - start_time))
if (( duration >= 60 )); then
    minutes=$((duration / 60))
    echo "Execution completed in $minutes minute(s)"
else
    echo "Execution completed in $duration second(s)"
fi
