# duilib_deps.cmake
# Dependency management for Skia / SDL3 / CEF:
#   - Skia and SDL3 sources are vendored as git submodules (third_party/skia, third_party/SDL3),
#     so they are present at configure time (src/CMakeLists.txt compiles skia's tools/window sources
#     directly into the duilib library).
#   - The actual BUILD happens at make time via add_custom_target (duilib_skia / duilib_sdl):
#     Skia is built with gn + ninja, SDL3 with cmake --build --target install into the build dir.
#   - CEF is a binary distribution (not source); when missing it is downloaded and extracted
#     automatically at configure time (URLs follow docs/CEF.md).
#
# Included from duilib_common.cmake (both the top-level build and standalone example builds).

# ---- Configure-time: source guards, skia args.gn, CEF download ----
function(duilib_deps_configure)
    # CEF download is idempotent (existence check) and must run per-scope:
    # DUILIB_ENABLE_CEF is turned ON by the cef/CefBrowser examples in their own scope.
    if(DUILIB_ENABLE_CEF)
        duilib_deps_download_cef()
    endif()

    get_property(_duilib_deps_done GLOBAL PROPERTY DUILIB_DEPS_CONFIGURED)
    if(_duilib_deps_done)
        return()
    endif()
    set_property(GLOBAL PROPERTY DUILIB_DEPS_CONFIGURED TRUE)

    # Source guards: the submodules must be initialized, otherwise give a clear error
    # instead of CMake's cryptic "Cannot find source file" from src/CMakeLists.txt.
    if(NOT EXISTS "${DUILIB_SKIA_SRC_ROOT_DIR}/BUILD.gn")
        message(FATAL_ERROR
            "Skia source not found at ${DUILIB_SKIA_SRC_ROOT_DIR}.\n"
            "Initialize the submodules first:\n"
            "    git submodule update --init --recursive\n"
            "(or set -DDUILIB_BUILD_SKIA_FROM_SOURCE=OFF and provide Skia yourself)")
    endif()
    if(DUILIB_ENABLE_SDL AND NOT EXISTS "${DUILIB_SDL_SRC_ROOT_DIR}/CMakeLists.txt")
        message(FATAL_ERROR
            "SDL3 source not found at ${DUILIB_SDL_SRC_ROOT_DIR}.\n"
            "Initialize the submodules first:\n"
            "    git submodule update --init --recursive\n"
            "(or set -DDUILIB_BUILD_SDL_FROM_SOURCE=OFF and provide SDL3 yourself)")
    endif()

    # Write Skia's args.gn now (gn gen reads it at make time; DEPENDS on it re-triggers gn gen
    # when the build type or compiler changes).
    if(DUILIB_BUILD_SKIA_FROM_SOURCE)
        set(_is_debug false)
        if(DUILIB_BUILD_TYPE STREQUAL "debug")
            set(_is_debug true)
        endif()

        # Standalone example builds declare project(... CXX) only, so CMAKE_C_COMPILER is empty there.
        set(_skia_cc "${CMAKE_C_COMPILER}")
        if(NOT _skia_cc)
            set(_skia_cc "${CMAKE_CXX_COMPILER}")
        endif()

        # FreeBSD extras (parity with scripts/build_skia.sh)
        set(_gn_extra "")
        if(DUILIB_OS_FREEBSD)
            set(_gn_extra "ar = \"llvm-ar\"
skia_enable_fontmgr_fontconfig = true
skia_use_freetype = true
extra_ldflags = [ \"-L/usr/local/lib\" ]
")
        endif()

        set(GN_ARGS_CONTENT "
target_cpu = \"${DUILIB_SYSTEM_PROCESSOR}\"
cc = \"${_skia_cc}\"
cxx = \"${CMAKE_CXX_COMPILER}\"
is_trivial_abi = false
is_official_build = true
skia_use_libwebp_encode = false
skia_use_libwebp_decode = false
skia_use_libpng_encode = false
skia_use_libpng_decode = false
skia_use_zlib = false
skia_use_libjpeg_turbo_encode = false
skia_use_libjpeg_turbo_decode = false
skia_enable_fontmgr_win_gdi = false
skia_use_icu = false
skia_use_expat = false
skia_use_xps = false
skia_enable_pdf = false
skia_use_wuffs = false
skia_enable_svg = true
skia_use_expat = true
skia_use_system_expat = false
is_debug = ${_is_debug}
${_gn_extra}extra_cflags = [ \"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\" ]
")
        file(MAKE_DIRECTORY "${DUILIB_SKIA_LIB_PATH}")
        file(WRITE "${DUILIB_SKIA_LIB_PATH}/args.gn" "${GN_ARGS_CONTENT}")
    endif()
endfunction()

# ---- Make-time build targets ----
function(duilib_deps_add_targets)
    get_property(_duilib_deps_targets_done GLOBAL PROPERTY DUILIB_DEPS_TARGETS_ADDED)
    if(_duilib_deps_targets_done)
        return()
    endif()
    set_property(GLOBAL PROPERTY DUILIB_DEPS_TARGETS_ADDED TRUE)

    # ---- Skia: gn gen + ninja (ninja is incremental: interrupted builds self-heal,
    # ---- args.gn changes re-trigger gn gen; no stamp file needed).
    if(DUILIB_BUILD_SKIA_FROM_SOURCE)
        if(NOT EXISTS "${DUILIB_SKIA_LIB_PATH}/libskia.a")
            find_program(GN_EXECUTABLE gn REQUIRED)
            find_program(NINJA_EXECUTABLE ninja REQUIRED)

            add_custom_command(
                OUTPUT "${DUILIB_SKIA_LIB_PATH}/libskia.a"
                COMMAND ${GN_EXECUTABLE} gen "${DUILIB_SKIA_LIB_PATH}"
                COMMAND ${NINJA_EXECUTABLE} -C "${DUILIB_SKIA_LIB_PATH}"
                WORKING_DIRECTORY "${DUILIB_SKIA_SRC_ROOT_DIR}"
                DEPENDS "${DUILIB_SKIA_LIB_PATH}/args.gn" "${DUILIB_SKIA_SRC_ROOT_DIR}/BUILD.gn"
                BYPRODUCTS "${DUILIB_SKIA_LIB_PATH}/libsvg.a"
                           "${DUILIB_SKIA_LIB_PATH}/libskshaper.a"
                           "${DUILIB_SKIA_LIB_PATH}/libskottie.a"
                           "${DUILIB_SKIA_LIB_PATH}/libsksg.a"
                           "${DUILIB_SKIA_LIB_PATH}/libjsonreader.a"
                COMMENT "Building Skia (gn gen + ninja)..."
                USES_TERMINAL VERBATIM
            )
            add_custom_target(duilib_skia DEPENDS "${DUILIB_SKIA_LIB_PATH}/libskia.a")
        else()
            message(STATUS "Using prebuilt Skia: ${DUILIB_SKIA_LIB_PATH}")
        endif()
    endif()

    # ---- SDL3: cmake configure + build + install into the build dir (keeps the submodule clean).
    if(DUILIB_ENABLE_SDL AND DUILIB_BUILD_SDL_FROM_SOURCE)
        if(NOT EXISTS "${DUILIB_SDL_SRC_ROOT_DIR}/lib" AND NOT EXISTS "${DUILIB_SDL_SRC_ROOT_DIR}/lib64")
            set(SDL_BUILD_DIR "${CMAKE_BINARY_DIR}/sdl3-build")
            set(SDL_INSTALL_DIR "${CMAKE_BINARY_DIR}/sdl3-install")
            set(SDL_STAMP "${SDL_INSTALL_DIR}/.duilib_built")

            set(_sdl_build_type "${CMAKE_BUILD_TYPE}")
            if(NOT _sdl_build_type)
                set(_sdl_build_type Release)
            endif()

            # On MSVC the linked library name is SDL3-static.lib -> build SDL statically there.
            # Note: SDL static builds default to /MD; with -DDUILIB_MD=OFF (/MT) there may be a
            # runtime library mismatch.
            set(_sdl_shared ON)
            set(_sdl_static OFF)
            if(DUILIB_OS_WINDOWS AND NOT MINGW)
                set(_sdl_shared OFF)
                set(_sdl_static ON)
            endif()

            add_custom_command(
                OUTPUT "${SDL_STAMP}"
                COMMAND ${CMAKE_COMMAND} -S "${DUILIB_SDL_SRC_ROOT_DIR}" -B "${SDL_BUILD_DIR}"
                    -DCMAKE_INSTALL_PREFIX=${SDL_INSTALL_DIR}
                    -DCMAKE_INSTALL_LIBDIR=lib
                    -DSDL_SHARED=${_sdl_shared} -DSDL_STATIC=${_sdl_static}
                    -DSDL_TEST_LIBRARY=OFF
                    -DCMAKE_BUILD_TYPE=${_sdl_build_type}
                COMMAND ${CMAKE_COMMAND} --build "${SDL_BUILD_DIR}" --target install
                COMMAND ${CMAKE_COMMAND} -E touch "${SDL_STAMP}"
                DEPENDS "${DUILIB_SDL_SRC_ROOT_DIR}/CMakeLists.txt"
                COMMENT "Building SDL3 (cmake --build --target install)..."
                USES_TERMINAL VERBATIM
            )
            add_custom_target(duilib_sdl DEPENDS "${SDL_STAMP}")
        else()
            message(STATUS "Using prebuilt SDL3: ${DUILIB_SDL_SRC_ROOT_DIR}")
        endif()
    endif()
endfunction()

# ---- CEF binary distribution download (only when missing) ----
function(duilib_deps_download_cef)
    if(DEFINED CEF_ROOT)
        return()
    endif()
    set(_cef_dest "${DUILIB_ROOT}/third_party/libcef/cef_binary")
    if(EXISTS "${_cef_dest}/CMakeLists.txt")
        return()
    endif()

    # Version + platform/arch mapping (URLs from docs/CEF.md)
    set(_cef_ver "142.0.10+g29548e2+chromium-142.0.7444.135")
    set(_cef_ver_enc "142.0.10%2Bg29548e2%2Bchromium-142.0.7444.135")
    if(DUILIB_CEF_109)
        set(_cef_ver "109.1.18+gf1c41e4+chromium-109.0.5414.120")
        set(_cef_ver_enc "109.1.18%2Bgf1c41e4%2Bchromium-109.0.5414.120")
    endif()

    if(DUILIB_OS_MACOS)
        if(DUILIB_SYSTEM_PROCESSOR STREQUAL "arm64")
            set(_cef_suffix "macosarm64")
        else()
            set(_cef_suffix "macosx64")
        endif()
    elseif(DUILIB_OS_LINUX)
        if(DUILIB_SYSTEM_PROCESSOR STREQUAL "arm64")
            set(_cef_suffix "linuxarm64")
        else()
            set(_cef_suffix "linux64")
        endif()
    elseif(DUILIB_OS_WINDOWS)
        if(DUILIB_BITS_64)
            set(_cef_suffix "windows64")
        else()
            set(_cef_suffix "windows32")
        endif()
    else()
        message(FATAL_ERROR "CEF auto-download is not supported on this platform; provide the CEF distribution manually.")
    endif()

    set(_cef_url "https://cef-builds.spotifycdn.com/cef_binary_${_cef_ver_enc}_${_cef_suffix}.tar.bz2")
    set(_cef_dl_dir "${DUILIB_ROOT}/third_party/libcef/.download")
    set(_cef_archive "${_cef_dl_dir}/cef_binary_${_cef_ver}_${_cef_suffix}.tar.bz2")
    set(_cef_extracted "${DUILIB_ROOT}/third_party/libcef/cef_binary_${_cef_ver}_${_cef_suffix}")

    file(MAKE_DIRECTORY "${_cef_dl_dir}")
    if(NOT EXISTS "${_cef_archive}")
        message(STATUS "Downloading CEF binary distribution: ${_cef_url}")
        file(DOWNLOAD "${_cef_url}" "${_cef_archive}" STATUS _cef_status)
    else()
        message(STATUS "CEF archive already downloaded: ${_cef_archive}")
        set(_cef_status 0)
    endif()
    list(GET _cef_status 0 _cef_code)
    if(NOT _cef_code EQUAL 0)
        file(REMOVE_RECURSE "${_cef_dl_dir}")
        message(FATAL_ERROR "CEF download failed (HTTP ${_cef_code}): ${_cef_url}")
    endif()

    message(STATUS "Extracting CEF binary distribution...")
    file(ARCHIVE_EXTRACT INPUT "${_cef_archive}" DESTINATION "${DUILIB_ROOT}/third_party/libcef")
    if(NOT EXISTS "${_cef_extracted}")
        # Fallback: system tar (e.g. CMake built without bzip2 support)
        execute_process(
            COMMAND tar -xjf "${_cef_archive}"
            WORKING_DIRECTORY "${DUILIB_ROOT}/third_party/libcef"
            RESULT_VARIABLE _cef_tar_result
        )
        if(NOT _cef_tar_result EQUAL 0 OR NOT EXISTS "${_cef_extracted}")
            file(REMOVE_RECURSE "${_cef_dl_dir}")
            message(FATAL_ERROR "CEF archive extraction failed: ${_cef_archive}")
        endif()
    endif()

    if(EXISTS "${_cef_dest}")
        file(REMOVE_RECURSE "${_cef_dest}")
    endif()
    file(RENAME "${_cef_extracted}" "${_cef_dest}")
    file(REMOVE_RECURSE "${_cef_dl_dir}")
    message(STATUS "CEF binary distribution ready: ${_cef_dest}")
endfunction()
