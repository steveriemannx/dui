#!/usr/bin/env bash
# Fetch the SDL3 source (zip archive) into third_party/SDL3
#
# Replaces the old git submodule / git clone acquisition for third_party/SDL3:
# downloads the official release source zip below and extracts it (stripping the
# top-level folder) so the existing CMake make-time build (duilib_sdl with cmake)
# keeps working unchanged.
#
# Usage:
#   ./scripts/fetch_sdl.sh            # fetch if not already present (idempotent)
#   ./scripts/fetch_sdl.sh -f         # force re-download and re-extract

set -e

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
THIRD_PARTY_DIR=$(cd "${SCRIPT_DIR}/../third_party" && pwd)
DOWNLOAD_DIR="${THIRD_PARTY_DIR}/.download"

# --- Source (keep in sync with third_party/README.md) ---
SDL_ZIP_URL="https://github.com/libsdl-org/SDL/releases/download/release-3.4.14/SDL3-3.4.14.zip"
SDL_DEST="${THIRD_PARTY_DIR}/SDL3"
SDL_MARKER="${SDL_DEST}/CMakeLists.txt"

# --- Flags ---
FORCE=0
if [ "$1" == "-f" ]; then
    FORCE=1
fi

# --- Tool checks ---
if ! command -v unzip &> /dev/null; then
    echo "- unzip not found!"
    exit 1
fi

has_curl=0
has_wget=0
if command -v curl &> /dev/null; then has_curl=1; fi
if command -v wget &> /dev/null; then has_wget=1; fi
if [ "$has_curl$has_wget" == "00" ]; then
    echo "- curl/wget not found!"
    exit 1
fi

# Download with retry: $1 = URL, $2 = save path
download_with_retry() {
    local url="$1"
    local save_path="$2"
    local max_retries=3
    if [ "$has_curl" == "1" ]; then
        echo "=== Downloading(curl): $url ==="
        curl -L -f -C - -o "$save_path" \
            --retry "$max_retries" \
            --connect-timeout 15 \
            --retry-delay 10 --speed-limit 100 --speed-time 120 \
            "$url"
    else
        echo "=== Downloading(wget): $url ==="
        wget -O "$save_path" -t "$max_retries" --timeout 15 -c "$url"
    fi
}

# Extract the zip into the destination, stripping the single top-level folder it
# contains.  Overlays the files onto the destination, so existing build output
# is preserved; use -f for a forced refresh.
extract_zip() {
    local zip_path="$1"
    local dest="$2"
    local tmp_dir
    tmp_dir=$(mktemp -d)
    unzip -q -o "$zip_path" -d "$tmp_dir"

    local top_dir
    top_dir=$(find "$tmp_dir" -mindepth 1 -maxdepth 1 -type d | head -n 1)
    if [ -z "$top_dir" ]; then
        echo "- unexpected zip layout: $zip_path"
        rm -rf "$tmp_dir"
        exit 1
    fi

    mkdir -p "$dest"
    cp -R "$top_dir"/. "$dest"/
    rm -rf "$tmp_dir"
}

if [ -f "$SDL_MARKER" ] && [ "$FORCE" != "1" ]; then
    echo "- SDL3 already present at $SDL_DEST (use -f to re-extract)"
    exit 0
fi

mkdir -p "$DOWNLOAD_DIR"
SDL_ZIP_PATH="$DOWNLOAD_DIR/$(basename "$SDL_ZIP_URL")"
if [ ! -f "$SDL_ZIP_PATH" ]; then
    download_with_retry "$SDL_ZIP_URL" "$SDL_ZIP_PATH" || { echo "- download failed: $SDL_ZIP_URL"; exit 1; }
else
    echo "- SDL3 archive already downloaded: $SDL_ZIP_PATH"
fi

# Drop leftover git metadata if the directory used to be a submodule checkout
rm -rf "$SDL_DEST/.git"
extract_zip "$SDL_ZIP_PATH" "$SDL_DEST"
if [ ! -f "$SDL_MARKER" ]; then
    echo "- extraction failed: $SDL_MARKER not found"
    exit 1
fi
echo "- SDL3 ready at $SDL_DEST"
