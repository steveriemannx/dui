# dui_deps.cmake
# Dependency management for Skia / SDL3 / CEF:
#   - Skia and SDL3 sources are vendored as zip downloads (third_party/skia, third_party/SDL3);
#     when missing they are downloaded and extracted automatically at configure time
#     (see dui_deps_download_skia / dui_deps_download_sdl below), so they are present
#     at configure time (src/CMakeLists.txt compiles skia's tools/window sources directly
#     into the dui library). The zip archives are kept in third_party/downloads/
#     (gitignored) so that deleting the extracted source and reconfiguring re-extracts
#     from the cached archive without re-downloading.
#   - The actual BUILD happens at make time via add_custom_target (dui_skia / dui_sdl):
#     Skia is built with gn + ninja, SDL3 with cmake --build --target install into the build dir.
#   - CEF is a binary distribution (not source); when missing it is downloaded and extracted
#     automatically at configure time (URLs follow docs/CEF.md). Its archive is also cached
#     in third_party/downloads/.
#
# Included from dui_common.cmake (both the top-level build and standalone example builds).

# ---- Configure-time: source guards, skia args.gn, CEF download ----
function(dui_deps_configure)
    # SDL3/Skia source zips (idempotent; the fetch scripts are the manual/offline equivalent).
    # Download order: SDL3 first, then skia (mirrors the make-time build order).
    if(DUI_ENABLE_SDL)
        dui_deps_download_sdl()
    endif()
    dui_deps_download_skia()
    dui_deps_download_gn()

    # WebView2 SDK download (idempotent, Windows only)
    if(DUI_WEBVIEW2_EXE AND DUI_OS_WINDOWS)
        dui_deps_download_webview2()
    endif()

    # CEF download is idempotent (existence check) and must run per-scope:
    # DUI_ENABLE_CEF is turned ON by the cef/CefBrowser examples in their own scope.
    if(DUI_ENABLE_CEF)
        dui_deps_download_cef()
    endif()

    get_property(_dui_deps_done GLOBAL PROPERTY DUI_DEPS_CONFIGURED)
    if(_dui_deps_done)
        return()
    endif()
    set_property(GLOBAL PROPERTY DUI_DEPS_CONFIGURED TRUE)

    # Source guards: the zip-fetched sources must be present, otherwise give a clear error
    # instead of CMake's cryptic "Cannot find source file" from src/CMakeLists.txt.
    if(NOT EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/BUILD.gn")
        message(FATAL_ERROR
            "Skia source not found at ${DUI_SKIA_SRC_ROOT_DIR}.\n"
            "The automatic download failed; retry cmake configure, or download and extract\n"
            "https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.0.zip\n"
            "into ${DUI_SKIA_SRC_ROOT_DIR} manually.\n"
            "(or set -DDUI_BUILD_SKIA_FROM_SOURCE=OFF and provide Skia yourself)")
    endif()
    if(DUI_ENABLE_SDL AND NOT EXISTS "${DUI_SDL_SRC_ROOT_DIR}/CMakeLists.txt")
        message(FATAL_ERROR
            "SDL3 source not found at ${DUI_SDL_SRC_ROOT_DIR}.\n"
            "The automatic download failed; retry cmake configure, or download and extract\n"
            "https://github.com/libsdl-org/SDL/releases/download/release-3.4.14/SDL3-3.4.14.zip\n"
            "into ${DUI_SDL_SRC_ROOT_DIR} manually.\n"
            "(or set -DDUI_BUILD_SDL_FROM_SOURCE=OFF and provide SDL3 yourself)")
    endif()

    # Write Skia's args.gn now (gn gen reads it at make time; DEPENDS on it re-triggers gn gen
    # when the build type or compiler changes).
    if(DUI_BUILD_SKIA_FROM_SOURCE)
        # Standalone example builds declare project(... CXX) only, so CMAKE_C_COMPILER is empty there.
        set(_skia_cc "${CMAKE_C_COMPILER}")
        if(NOT _skia_cc)
            set(_skia_cc "${CMAKE_CXX_COMPILER}")
        endif()

        # FreeBSD extras (parity with scripts/build_skia.sh)
        set(_gn_extra "")
        if(DUI_OS_FREEBSD)
            set(_gn_extra "ar = \"llvm-ar\"
skia_enable_fontmgr_fontconfig = true
skia_use_freetype = true
skia_system_freetype2_include_path = \"/usr/local/include/freetype2\"
extra_ldflags = [ \"-L/usr/local/lib\" ]
extra_cflags = [ \"-I/usr/local/include\", \"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\" ]
")
        else()
            set(_gn_extra "extra_cflags = [ \"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\" ]
")
        endif()

        # Common gn args (everything except is_debug, which differs per config)
        set(GN_ARGS_COMMON "
target_cpu = \"${DUI_SYSTEM_PROCESSOR}\"
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
${_gn_extra}
")

        if(DUI_MULTI_CONFIG)
            # Debug: same release-grade build, but with debug CRT so it links against
            # Debug-configuration projects without _ITERATOR_DEBUG_LEVEL / RuntimeLibrary
            # mismatches. We keep is_debug=false + is_official_build=true to avoid pulling
            # in third_party/externals that the fetched zip archive doesn't have.
            file(MAKE_DIRECTORY "${DUI_SKIA_LIB_PATH_DEBUG}")
            file(WRITE "${DUI_SKIA_LIB_PATH_DEBUG}/args.gn" "${GN_ARGS_COMMON}is_debug = false\nextra_cflags += [ \"/MTd\", \"-D_ITERATOR_DEBUG_LEVEL=2\" ]\n")
            file(MAKE_DIRECTORY "${DUI_SKIA_LIB_PATH_RELEASE}")
            file(WRITE "${DUI_SKIA_LIB_PATH_RELEASE}/args.gn" "${GN_ARGS_COMMON}is_debug = false\n")
        else()
            set(_is_debug false)
            if(DUI_BUILD_TYPE STREQUAL "debug")
                set(_is_debug true)
            endif()
            file(MAKE_DIRECTORY "${DUI_SKIA_LIB_PATH}")
            file(WRITE "${DUI_SKIA_LIB_PATH}/args.gn" "${GN_ARGS_COMMON}is_debug = ${_is_debug}\n")
        endif()
    endif()
endfunction()

# ---- Make-time build targets ----
function(dui_deps_add_targets)
    get_property(_dui_deps_targets_done GLOBAL PROPERTY DUI_DEPS_TARGETS_ADDED)
    if(_dui_deps_targets_done)
        return()
    endif()
    set_property(GLOBAL PROPERTY DUI_DEPS_TARGETS_ADDED TRUE)

    # ---- Skia: gn gen + ninja (ninja is incremental: interrupted builds self-heal,
    # ---- args.gn changes re-trigger gn gen; no stamp file needed).
    if(DUI_BUILD_SKIA_FROM_SOURCE)
        # Main Skia output filename differs by platform (MSVC: skia.lib, others: libskia.a)
        if(MSVC)
            set(_skia_main_lib "skia.lib")
        else()
            set(_skia_main_lib "libskia.a")
        endif()

        # ---- ninja: required at make time to build gn and skia. Prefer PATH or skia's
        # ---- own locations; fetch it automatically when missing (like build_dui_all_in_one.bat).
        find_program(DUI_NINJA_BIN
            NAMES ninja ninja.exe
            HINTS "${DUI_SKIA_SRC_ROOT_DIR}/bin" "${DUI_SKIA_SRC_ROOT_DIR}/third_party/ninja")
        if(NOT DUI_NINJA_BIN)
            message(STATUS "ninja not found; fetching it with skia's bin/fetch-ninja ...")
            find_program(_ninja_python NAMES python3 python)
            if(_ninja_python)
                execute_process(
                    COMMAND "${_ninja_python}" "${DUI_SKIA_SRC_ROOT_DIR}/bin/fetch-ninja"
                    WORKING_DIRECTORY "${DUI_SKIA_SRC_ROOT_DIR}"
                    RESULT_VARIABLE _ninja_fetch_result
                )
            endif()
            if(WIN32)
                set(_ninja_bin "${DUI_SKIA_SRC_ROOT_DIR}/third_party/ninja/ninja.exe")
            else()
                set(_ninja_bin "${DUI_SKIA_SRC_ROOT_DIR}/third_party/ninja/ninja")
            endif()
            if(_ninja_fetch_result EQUAL 0 AND EXISTS "${_ninja_bin}")
                set(DUI_NINJA_BIN "${_ninja_bin}")
            endif()
        endif()
        if(NOT DUI_NINJA_BIN)
            message(FATAL_ERROR "ninja is required to build skia but was not found and could not be "
                    "fetched automatically. Install it (choco install ninja / scoop install ninja / "
                    "pacman -S ninja), or download it from https://github.com/ninja-build/ninja/releases "
                    "and place ninja(.exe) in ${DUI_SKIA_SRC_ROOT_DIR}/third_party/ninja/ or in PATH, "
                    "then re-run cmake configure.")
        endif()

        # ---- gn: built from source at make time (ordered before SDL3/skia) ----
        set(DUI_GN_BIN "")  # set when the gn source build is available; skia uses it below
        if(EXISTS "${DUI_ROOT}/third_party/gn/build/gen.py")
            find_program(DUI_GN_PYTHON NAMES python3 python)
            if(DUI_GN_PYTHON)
                if(WIN32)
                    set(DUI_GN_BIN "${DUI_ROOT}/third_party/gn/out/gn.exe")
                else()
                    set(DUI_GN_BIN "${DUI_ROOT}/third_party/gn/out/gn")
                endif()
                add_custom_command(
                    OUTPUT "${DUI_GN_BIN}"
                    COMMAND "${DUI_GN_PYTHON}" build/gen.py
                    COMMAND "${DUI_NINJA_BIN}" -C out
                    WORKING_DIRECTORY "${DUI_ROOT}/third_party/gn"
                    DEPENDS "${DUI_ROOT}/third_party/gn/build/gen.py"
                    COMMENT "Building gn (python build/gen.py + ninja -C out)..."
                    USES_TERMINAL VERBATIM
                )
                add_custom_target(dui_gn DEPENDS "${DUI_GN_BIN}")
            endif()
        endif()

        if(DUI_MULTI_CONFIG)
            # ---- Debug Skia ----
            if(NOT EXISTS "${DUI_SKIA_LIB_PATH_DEBUG}/${_skia_main_lib}")
                if(DUI_GN_BIN)
                    set(GN_EXECUTABLE_DEBUG "${DUI_GN_BIN}")
                else()
                    find_program(GN_EXECUTABLE_DEBUG
                        NAMES gn
                        HINTS "${DUI_ROOT}/third_party/gn/out" "${DUI_SKIA_SRC_ROOT_DIR}/bin"
                        REQUIRED)
                endif()
                set(NINJA_EXECUTABLE_DEBUG "${DUI_NINJA_BIN}")
                add_custom_command(
                    OUTPUT "${DUI_SKIA_LIB_PATH_DEBUG}/${_skia_main_lib}"
                    COMMAND ${GN_EXECUTABLE_DEBUG} gen "${DUI_SKIA_LIB_PATH_DEBUG}"
                    COMMAND ${NINJA_EXECUTABLE_DEBUG} -C "${DUI_SKIA_LIB_PATH_DEBUG}"
                    WORKING_DIRECTORY "${DUI_SKIA_SRC_ROOT_DIR}"
                    DEPENDS "${DUI_SKIA_LIB_PATH_DEBUG}/args.gn" "${DUI_SKIA_SRC_ROOT_DIR}/BUILD.gn" ${DUI_GN_BIN}
                    COMMENT "Building Skia (debug, gn gen + ninja)..."
                    USES_TERMINAL VERBATIM
                )
                add_custom_target(dui_skia_debug DEPENDS "${DUI_SKIA_LIB_PATH_DEBUG}/${_skia_main_lib}")
                if(TARGET dui_gn)
                    add_dependencies(dui_skia_debug dui_gn)
                endif()
            else()
                message(STATUS "Using prebuilt Skia (debug): ${DUI_SKIA_LIB_PATH_DEBUG}")
            endif()

            # ---- Release Skia ----
            if(NOT EXISTS "${DUI_SKIA_LIB_PATH_RELEASE}/${_skia_main_lib}")
                if(DUI_GN_BIN)
                    set(GN_EXECUTABLE_RELEASE "${DUI_GN_BIN}")
                else()
                    find_program(GN_EXECUTABLE_RELEASE
                        NAMES gn
                        HINTS "${DUI_ROOT}/third_party/gn/out" "${DUI_SKIA_SRC_ROOT_DIR}/bin"
                        REQUIRED)
                endif()
                set(NINJA_EXECUTABLE_RELEASE "${DUI_NINJA_BIN}")
                add_custom_command(
                    OUTPUT "${DUI_SKIA_LIB_PATH_RELEASE}/${_skia_main_lib}"
                    COMMAND ${GN_EXECUTABLE_RELEASE} gen "${DUI_SKIA_LIB_PATH_RELEASE}"
                    COMMAND ${NINJA_EXECUTABLE_RELEASE} -C "${DUI_SKIA_LIB_PATH_RELEASE}"
                    WORKING_DIRECTORY "${DUI_SKIA_SRC_ROOT_DIR}"
                    DEPENDS "${DUI_SKIA_LIB_PATH_RELEASE}/args.gn" "${DUI_SKIA_SRC_ROOT_DIR}/BUILD.gn" ${DUI_GN_BIN}
                    COMMENT "Building Skia (release, gn gen + ninja)..."
                    USES_TERMINAL VERBATIM
                )
                add_custom_target(dui_skia_release DEPENDS "${DUI_SKIA_LIB_PATH_RELEASE}/${_skia_main_lib}")
                if(TARGET dui_gn)
                    add_dependencies(dui_skia_release dui_gn)
                endif()
            else()
                message(STATUS "Using prebuilt Skia (release): ${DUI_SKIA_LIB_PATH_RELEASE}")
            endif()

            # Combined target so that `add_dependencies(${PROJECT_NAME} dui_skia)` builds both
            if(TARGET dui_skia_debug AND TARGET dui_skia_release)
                add_custom_target(dui_skia DEPENDS dui_skia_debug dui_skia_release)
            elseif(TARGET dui_skia_debug)
                add_custom_target(dui_skia DEPENDS dui_skia_debug)
            elseif(TARGET dui_skia_release)
                add_custom_target(dui_skia DEPENDS dui_skia_release)
            endif()
        else()
            # ---- Single-config Skia ----
            if(NOT EXISTS "${DUI_SKIA_LIB_PATH}/${_skia_main_lib}")
                # Look for gn/ninja inside the Skia source's bin/ first (the convention used by
                # scripts/build_dui_all_in_one.bat: bin/fetch-gn drops gn.exe there and
                # bin/fetch-ninja drops ninja.exe into third_party/ninja), then fall back to PATH.
                if(DUI_GN_BIN)
                    set(GN_EXECUTABLE "${DUI_GN_BIN}")
                else()
                    find_program(GN_EXECUTABLE
                        NAMES gn
                        HINTS "${DUI_ROOT}/third_party/gn/out" "${DUI_SKIA_SRC_ROOT_DIR}/bin"
                        REQUIRED)
                endif()
                set(NINJA_EXECUTABLE "${DUI_NINJA_BIN}")

                add_custom_command(
                    OUTPUT "${DUI_SKIA_LIB_PATH}/${_skia_main_lib}"
                    COMMAND ${GN_EXECUTABLE} gen "${DUI_SKIA_LIB_PATH}"
                    COMMAND ${NINJA_EXECUTABLE} -C "${DUI_SKIA_LIB_PATH}"
                    WORKING_DIRECTORY "${DUI_SKIA_SRC_ROOT_DIR}"
                    DEPENDS "${DUI_SKIA_LIB_PATH}/args.gn" "${DUI_SKIA_SRC_ROOT_DIR}/BUILD.gn" ${DUI_GN_BIN}
                    COMMENT "Building Skia (gn gen + ninja)..."
                    USES_TERMINAL VERBATIM
                )
                add_custom_target(dui_skia DEPENDS "${DUI_SKIA_LIB_PATH}/${_skia_main_lib}")
                if(TARGET dui_gn)
                    add_dependencies(dui_skia dui_gn)
                endif()
            else()
                message(STATUS "Using prebuilt Skia: ${DUI_SKIA_LIB_PATH}")
            endif()
        endif()
    endif()

    # ---- SDL3: cmake configure + build + install into the build dir (keeps the source dir clean).
    if(DUI_ENABLE_SDL AND DUI_BUILD_SDL_FROM_SOURCE)
        if(NOT EXISTS "${DUI_SDL_SRC_ROOT_DIR}/lib" AND NOT EXISTS "${DUI_SDL_SRC_ROOT_DIR}/lib64")
            set(SDL_BUILD_DIR "${CMAKE_BINARY_DIR}/sdl3-build")
            set(SDL_INSTALL_DIR "${CMAKE_BINARY_DIR}/sdl3-install")
            set(SDL_STAMP "${SDL_INSTALL_DIR}/.dui_built")

            set(_sdl_build_type "${CMAKE_BUILD_TYPE}")
            if(NOT _sdl_build_type)
                set(_sdl_build_type Release)
            endif()

            # On MSVC the linked library name is SDL3-static.lib -> build SDL statically there.
            # Note: SDL static builds default to /MD; with -DDUI_MD=OFF (/MT) there may be a
            # runtime library mismatch.
            set(_sdl_shared ON)
            set(_sdl_static OFF)
            if(DUI_OS_WINDOWS AND NOT MINGW)
                set(_sdl_shared OFF)
                set(_sdl_static ON)
            endif()

            add_custom_command(
                OUTPUT "${SDL_STAMP}"
                COMMAND ${CMAKE_COMMAND} -S "${DUI_SDL_SRC_ROOT_DIR}" -B "${SDL_BUILD_DIR}"
                    -DCMAKE_INSTALL_PREFIX=${SDL_INSTALL_DIR}
                    -DCMAKE_INSTALL_LIBDIR=lib
                    -DSDL_SHARED=${_sdl_shared} -DSDL_STATIC=${_sdl_static}
                    -DSDL_TEST_LIBRARY=OFF
                    -DCMAKE_BUILD_TYPE=${_sdl_build_type}
                COMMAND ${CMAKE_COMMAND} --build "${SDL_BUILD_DIR}" --target install
                COMMAND ${CMAKE_COMMAND} -E touch "${SDL_STAMP}"
                DEPENDS "${DUI_SDL_SRC_ROOT_DIR}/CMakeLists.txt"
                COMMENT "Building SDL3 (cmake --build --target install)..."
                USES_TERMINAL VERBATIM
            )
            add_custom_target(dui_sdl DEPENDS "${SDL_STAMP}")
            # Build order: gn first, then SDL3 (and skia after that)
            if(TARGET dui_gn)
                add_dependencies(dui_sdl dui_gn)
            endif()
        else()
            message(STATUS "Using prebuilt SDL3: ${DUI_SDL_SRC_ROOT_DIR}")
        endif()
    endif()
endfunction()

# ---- Download helper: verify cached archive, retry on failure (shared by skia/SDL3/CEF/WebView2) ----
# Checks the integrity of a cached archive before it is used, so a partial download left by an
# interrupted previous configure is detected and re-downloaded instead of failing at extraction.
# Retries the download up to 3 times and removes the partial file after each failure, so the
# next configure always starts clean.
#   _url  - download URL
#   _file - archive path in third_party/downloads/
#   _kind - "zip" (unzip -t) or "tarbz2" (tar -tjf); Windows uses tar -tf for both
function(dui_deps_download_retry _url _file _kind)
    if(WIN32)
        set(_test_command tar -tf "${_file}")  # bsdtar reads zip and tar.bz2
    elseif(_kind STREQUAL "zip")
        set(_test_command unzip -tq "${_file}")
    else()
        set(_test_command tar -tjf "${_file}")
    endif()

    if(EXISTS "${_file}")
        execute_process(COMMAND ${_test_command} RESULT_VARIABLE _test_result OUTPUT_QUIET ERROR_QUIET)
        if(_test_result EQUAL 0)
            message(STATUS "Archive verified: ${_file}")
            return()
        endif()
        message(WARNING "Cached archive is incomplete/corrupt (interrupted download?); re-downloading: ${_file}")
        file(REMOVE "${_file}")
    endif()

    set(_attempt 0)
    while(_attempt LESS 3)
        math(EXPR _attempt "${_attempt}+1")
        message(STATUS "Downloading (attempt ${_attempt}/3): ${_url}")
        file(DOWNLOAD "${_url}" "${_file}" STATUS _dl_status)
        list(GET _dl_status 0 _dl_code)
        if(_dl_code EQUAL 0)
            execute_process(COMMAND ${_test_command} RESULT_VARIABLE _test_result OUTPUT_QUIET ERROR_QUIET)
            if(_test_result EQUAL 0)
                return()
            endif()
            message(WARNING "Downloaded archive failed the integrity check; retrying")
        else()
            message(WARNING "Download failed (HTTP ${_dl_code}); retrying")
        endif()
        file(REMOVE "${_file}")  # never leave a partial file for the next configure
    endwhile()
    get_filename_component(_file_dir "${_file}" DIRECTORY)
    message(FATAL_ERROR "Download failed after ${_attempt} attempts: ${_url}\n"
            "Please retry cmake configure later, or download the file manually (see the URL\n"
            "above) and place it into ${_file_dir} keeping its original filename - the next\n"
            "configure re-extracts from the cache without downloading.")
endfunction()

# ---- Skia source zip download (idempotent; fetched at configure time, no shell scripts) ----
# The zip is cached in third_party/downloads/ (gitignored); only the extraction temp dir is
# removed, so a later configure re-extracts from the cached archive without re-downloading.
function(dui_deps_download_skia)
    if(EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/BUILD.gn")
        return()  # already present
    endif()

    set(_skia_url "https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.0.zip")
    set(_skia_topdir "skia-skia-dui-0.1.0")  # the single top-level folder inside the zip
    set(_skia_dl_dir "${DUI_ROOT}/third_party/downloads")
    set(_skia_archive "${_skia_dl_dir}/skia-dui-0.1.0.zip")

    file(MAKE_DIRECTORY "${_skia_dl_dir}")
    dui_deps_download_retry("${_skia_url}" "${_skia_archive}" "zip")

    message(STATUS "Extracting Skia source...")
    # Note: CMake's file(ARCHIVE_EXTRACT) fails on the skia zip ("ZIP decompression
    # failed"), so use the system tools and check the exit code explicitly.
    if(EXISTS "${DUI_SKIA_SRC_ROOT_DIR}")
        file(REMOVE_RECURSE "${DUI_SKIA_SRC_ROOT_DIR}")  # leftover (e.g. empty submodule dir)
    endif()
    set(_skia_tmp_dir "${_skia_dl_dir}/extract")
    file(REMOVE_RECURSE "${_skia_tmp_dir}")
    file(MAKE_DIRECTORY "${_skia_tmp_dir}")
    if(WIN32)
        # Windows 10 1803+ ships tar.exe (bsdtar), which reads zip archives.
        # bsdtar -C requires the target dir to exist, so create it first.
        file(MAKE_DIRECTORY "${DUI_SKIA_SRC_ROOT_DIR}")
        execute_process(
            COMMAND tar -xf "${_skia_archive}" --strip-components=1 -C "${DUI_SKIA_SRC_ROOT_DIR}"
            RESULT_VARIABLE _skia_extract_result
        )
    else()
        # unzip is a standard tool on macOS/Linux/FreeBSD
        execute_process(
            COMMAND unzip -q -o "${_skia_archive}" -d "${_skia_tmp_dir}"
            RESULT_VARIABLE _skia_extract_result
        )
        if(_skia_extract_result EQUAL 0)
            execute_process(
                COMMAND "${CMAKE_COMMAND}" -E rename "${_skia_tmp_dir}/${_skia_topdir}" "${DUI_SKIA_SRC_ROOT_DIR}"
                RESULT_VARIABLE _skia_extract_result
            )
        endif()
    endif()
    if(NOT _skia_extract_result EQUAL 0 OR NOT EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/BUILD.gn")
        file(REMOVE "${_skia_archive}")  # corrupt archive; force a fresh download next time
        file(REMOVE_RECURSE "${_skia_tmp_dir}")
        message(FATAL_ERROR "Skia archive extraction failed: ${_skia_archive}")
    endif()
    file(REMOVE_RECURSE "${_skia_tmp_dir}")  # keep the zip itself for offline re-extract
    message(STATUS "Skia source ready: ${DUI_SKIA_SRC_ROOT_DIR}")
endfunction()

# ---- SDL3 source zip download (idempotent; fetched at configure time, no shell scripts) ----
# The zip is cached in third_party/downloads/ (gitignored); only the extraction temp dir is
# removed, so a later configure re-extracts from the cached archive without re-downloading.
function(dui_deps_download_sdl)
    if(EXISTS "${DUI_SDL_SRC_ROOT_DIR}/CMakeLists.txt")
        return()  # already present
    endif()

    set(_sdl_url "https://github.com/libsdl-org/SDL/releases/download/release-3.4.14/SDL3-3.4.14.zip")
    set(_sdl_topdir "SDL3-3.4.14")  # the single top-level folder inside the zip
    set(_sdl_dl_dir "${DUI_ROOT}/third_party/downloads")
    set(_sdl_archive "${_sdl_dl_dir}/SDL3-3.4.14.zip")

    file(MAKE_DIRECTORY "${_sdl_dl_dir}")
    dui_deps_download_retry("${_sdl_url}" "${_sdl_archive}" "zip")

    message(STATUS "Extracting SDL3 source...")
    # Same approach as dui_deps_download_skia: system tools + explicit exit-code check.
    if(EXISTS "${DUI_SDL_SRC_ROOT_DIR}")
        file(REMOVE_RECURSE "${DUI_SDL_SRC_ROOT_DIR}")  # leftover (e.g. empty submodule dir)
    endif()
    set(_sdl_tmp_dir "${_sdl_dl_dir}/extract")
    file(REMOVE_RECURSE "${_sdl_tmp_dir}")
    file(MAKE_DIRECTORY "${_sdl_tmp_dir}")
    if(WIN32)
        # Windows 10 1803+ ships tar.exe (bsdtar), which reads zip archives.
        # bsdtar -C requires the target dir to exist, so create it first.
        file(MAKE_DIRECTORY "${DUI_SDL_SRC_ROOT_DIR}")
        execute_process(
            COMMAND tar -xf "${_sdl_archive}" --strip-components=1 -C "${DUI_SDL_SRC_ROOT_DIR}"
            RESULT_VARIABLE _sdl_extract_result
        )
    else()
        # unzip is a standard tool on macOS/Linux/FreeBSD
        execute_process(
            COMMAND unzip -q -o "${_sdl_archive}" -d "${_sdl_tmp_dir}"
            RESULT_VARIABLE _sdl_extract_result
        )
        if(_sdl_extract_result EQUAL 0)
            execute_process(
                COMMAND "${CMAKE_COMMAND}" -E rename "${_sdl_tmp_dir}/${_sdl_topdir}" "${DUI_SDL_SRC_ROOT_DIR}"
                RESULT_VARIABLE _sdl_extract_result
            )
        endif()
    endif()
    if(NOT _sdl_extract_result EQUAL 0 OR NOT EXISTS "${DUI_SDL_SRC_ROOT_DIR}/CMakeLists.txt")
        file(REMOVE "${_sdl_archive}")  # corrupt archive; force a fresh download next time
        file(REMOVE_RECURSE "${_sdl_tmp_dir}")
        message(FATAL_ERROR "SDL3 archive extraction failed: ${_sdl_archive}")
    endif()
    file(REMOVE_RECURSE "${_sdl_tmp_dir}")  # keep the zip itself for offline re-extract
    message(STATUS "SDL3 source ready: ${DUI_SDL_SRC_ROOT_DIR}")
endfunction()

# ---- GN source clone (idempotent; the build itself happens at make time) ----
# Building skia requires gn. Prebuilt CIPD binaries only cover amd64 reliably, so clone the
# gn source at configure time and build it at make time via the dui_gn target
# (build/gen.py + ninja -C out, per https://gn.googlesource.com/gn/+/refs/heads/main/README.md),
# ordered before the SDL3/skia builds. The binary lands in third_party/gn/out/gn (gn.exe on
# Windows). A system gn is preferred and skips the clone entirely; if the clone is
# unavailable, configure falls back to a system gn or skia's bin/ at make time.
function(dui_deps_download_gn)
    if(NOT DUI_BUILD_SKIA_FROM_SOURCE)
        return()
    endif()
    set(_gn_dir "${DUI_ROOT}/third_party/gn")
    if(EXISTS "${_gn_dir}/build/gen.py")
        return()  # already cloned
    endif()
    # Prefer a system gn (zero cost - already installed, no download needed): the
    # make-time skia steps fall back to find_program(gn) when the source build is
    # unavailable, so skip the clone entirely if gn is in PATH.
    find_program(_gn_system NAMES gn)
    if(_gn_system)
        message(STATUS "Using system gn: ${_gn_system} (skipping source clone)")
        return()
    endif()
    # Then try skia's own bin/fetch-gn: it downloads the skia-pinned prebuilt CIPD
    # binary into skia/bin/gn (gn.exe on Windows), avoiding a source clone+build.
    if(EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/bin/fetch-gn")
        find_program(_gn_python NAMES python3 python)
        if(_gn_python)
            message(STATUS "Fetching prebuilt gn with skia's bin/fetch-gn ...")
            execute_process(
                COMMAND "${_gn_python}" "${DUI_SKIA_SRC_ROOT_DIR}/bin/fetch-gn"
                WORKING_DIRECTORY "${DUI_SKIA_SRC_ROOT_DIR}"
                RESULT_VARIABLE _gn_fetch_result
            )
            if(_gn_fetch_result EQUAL 0 AND
               (EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/bin/gn" OR EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/bin/gn.exe"))
                message(STATUS "gn fetched: ${DUI_SKIA_SRC_ROOT_DIR}/bin/gn (skipping source clone)")
                return()
            endif()
            message(STATUS "fetch-gn failed; falling back to a source clone")
        endif()
    endif()
    # Try the Google source 2 times, then fall back to a GitHub mirror.
    # Abort slow/hung clones quickly: 10s connect timeout, and fail if the transfer
    # stays below 200 KB/s for 30s (git http.lowSpeed* options).
    set(_gn_google_url "https://gn.googlesource.com/gn")
    set(_gn_mirror_url "https://github.com/ArthurSonzogni/gn")
    set(_gn_clone_ok FALSE)
    foreach(_gn_attempt RANGE 1 2)
        message(STATUS "Cloning gn source (attempt ${_gn_attempt}/2): ${_gn_google_url}")
        execute_process(
            # NOTE: full clone required - build/gen.py runs `git describe --match initial-commit`
            # to generate last_commit_position.h, which fails on a shallow clone (the tag only
            # exists in the full history). The repo is small (~40MB, ~30s to clone).
            COMMAND git -c http.connectTimeout=10 -c http.lowSpeedLimit=204800 -c http.lowSpeedTime=30 clone "${_gn_google_url}" "${_gn_dir}"
            RESULT_VARIABLE _gn_clone_result
        )
        if(_gn_clone_result EQUAL 0 AND EXISTS "${_gn_dir}/build/gen.py")
            set(_gn_clone_ok TRUE)
            break()
        endif()
        file(REMOVE_RECURSE "${_gn_dir}")  # partial clone; retry from scratch
    endforeach()
    if(NOT _gn_clone_ok)
        message(STATUS "Google source failed after 2 attempts; trying GitHub mirror: ${_gn_mirror_url}")
        execute_process(
            COMMAND git -c http.connectTimeout=10 -c http.lowSpeedLimit=204800 -c http.lowSpeedTime=30 clone "${_gn_mirror_url}" "${_gn_dir}"
            RESULT_VARIABLE _gn_clone_result
        )
        if(_gn_clone_result EQUAL 0 AND EXISTS "${_gn_dir}/build/gen.py")
            set(_gn_clone_ok TRUE)
        endif()
    endif()
    if(NOT _gn_clone_ok)
        message(WARNING "gn clone failed; configure will look for a system gn (or skia's bin/).\n"
                        "Please install gn manually and re-run cmake configure. Known package names:\n"
                        "  Debian/Ubuntu: sudo apt install generate-ninja   (NOT \"gn\")\n"
                        "  Fedora/RHEL:   sudo dnf install gn\n"
                        "  Arch Linux:    sudo pacman -S gn\n"
                        "  FreeBSD:       pkg install gn\n"
                        "  MSYS2:         pacman -S mingw-w64-x86_64-gn\n"
                        "  macOS:         no package - build gn from source (see the gn README)\n"
                        "Or download a prebuilt gn binary from CIPD (a zip; version pinned in\n"
                        "${DUI_SKIA_SRC_ROOT_DIR}/bin/fetch-gn) and unzip it to\n"
                        "${DUI_SKIA_SRC_ROOT_DIR}/bin/gn (gn.exe on Windows):\n"
                        "  https://chrome-infra-packages.appspot.com/dl/gn/gn/windows-amd64/+/git_revision:b2afae122eeb6ce09c52d63f67dc53fc517dbdc8\n"
                        "  (use linux-amd64 / mac-amd64 / mac-arm64 / linux-arm64 as needed)\n"
                        "Or clone the gn source into ${DUI_ROOT}/third_party/gn (the build at\n"
                        "make time compiles it automatically; full history, not --depth 1):\n"
                        "  git clone https://gn.googlesource.com/gn ${DUI_ROOT}/third_party/gn\n"
                        "  or the GitHub mirror: git clone https://github.com/ArthurSonzogni/gn ${DUI_ROOT}/third_party/gn")
    endif()
endfunction()

# ---- CEF binary distribution download (only when missing) ----
# The tar.bz2 is cached in third_party/downloads/ (gitignored), like the skia/SDL3 zips;
# only the extraction target (third_party/libcef/cef_binary) is re-created when missing.
function(dui_deps_download_cef)
    if(DEFINED CEF_ROOT)
        return()
    endif()
    set(_cef_dest "${DUI_ROOT}/third_party/libcef/cef_binary")
    if(EXISTS "${_cef_dest}/CMakeLists.txt")
        return()
    endif()

    # Version + platform/arch mapping (URLs from docs/CEF.md)
    set(_cef_ver "142.0.10+g29548e2+chromium-142.0.7444.135")
    set(_cef_ver_enc "142.0.10%2Bg29548e2%2Bchromium-142.0.7444.135")
    if(DUI_CEF_109)
        set(_cef_ver "109.1.18+gf1c41e4+chromium-109.0.5414.120")
        set(_cef_ver_enc "109.1.18%2Bgf1c41e4%2Bchromium-109.0.5414.120")
    endif()

    if(DUI_OS_MACOS)
        if(DUI_SYSTEM_PROCESSOR STREQUAL "arm64")
            set(_cef_suffix "macosarm64")
        else()
            set(_cef_suffix "macosx64")
        endif()
    elseif(DUI_OS_LINUX)
        if(DUI_SYSTEM_PROCESSOR STREQUAL "arm64")
            set(_cef_suffix "linuxarm64")
        else()
            set(_cef_suffix "linux64")
        endif()
    elseif(DUI_OS_WINDOWS)
        if(DUI_BITS_64)
            set(_cef_suffix "windows64")
        else()
            set(_cef_suffix "windows32")
        endif()
    else()
        # Unsupported platform (e.g. FreeBSD): there is no official CEF binary
        # distribution. Skip the download instead of failing; the CEF module
        # stays disabled on such platforms.
        message(STATUS "CEF auto-download is not supported on this platform; the CEF module is disabled")
        return()
    endif()

    set(_cef_url "https://cef-builds.spotifycdn.com/cef_binary_${_cef_ver_enc}_${_cef_suffix}.tar.bz2")
    set(_cef_dl_dir "${DUI_ROOT}/third_party/downloads")
    set(_cef_archive "${_cef_dl_dir}/cef_binary_${_cef_ver}_${_cef_suffix}.tar.bz2")
    set(_cef_extracted "${DUI_ROOT}/third_party/libcef/cef_binary_${_cef_ver}_${_cef_suffix}")

    file(MAKE_DIRECTORY "${_cef_dl_dir}")
    dui_deps_download_retry("${_cef_url}" "${_cef_archive}" "tarbz2")

    message(STATUS "Extracting CEF binary distribution...")
    file(ARCHIVE_EXTRACT INPUT "${_cef_archive}" DESTINATION "${DUI_ROOT}/third_party/libcef")
    if(NOT EXISTS "${_cef_extracted}")
        # Fallback: system tar (e.g. CMake built without bzip2 support)
        execute_process(
            COMMAND tar -xjf "${_cef_archive}"
            WORKING_DIRECTORY "${DUI_ROOT}/third_party/libcef"
            RESULT_VARIABLE _cef_tar_result
        )
        if(NOT _cef_tar_result EQUAL 0 OR NOT EXISTS "${_cef_extracted}")
            file(REMOVE "${_cef_archive}")  # corrupt archive; force a fresh download next time
            message(FATAL_ERROR "CEF archive extraction failed: ${_cef_archive}\n"
                    "Download the archive manually from https://cef-builds.spotifycdn.com/ and\n"
                    "either place it into ${_cef_dl_dir} (keep the original filename), or\n"
                    "extract it into ${_cef_dest} (the extraction location), then re-run\n"
                    "cmake configure. If you do not need the CEF examples, set -DDUI_ENABLE_CEF=OFF.")
        endif()
    endif()

    if(EXISTS "${_cef_dest}")
        file(REMOVE_RECURSE "${_cef_dest}")
    endif()
    file(RENAME "${_cef_extracted}" "${_cef_dest}")
    # keep the archive itself in third_party/downloads/ for offline re-extract
    message(STATUS "CEF binary distribution ready: ${_cef_dest}")
endfunction()

# ---- WebView2 SDK NuGet package download (only when missing) ----
# The .nupkg is cached in third_party/downloads/ (gitignored), like the skia/SDL3 zips.
function(dui_deps_download_webview2)
    set(_wv2_dest "${DUI_ROOT}/third_party/Microsoft.Web.WebView2")
    if(EXISTS "${_wv2_dest}/build/native/${DUI_SYSTEM_PROCESSOR}/WebView2Loader.dll.lib")
        return()  # already installed
    endif()

    set(_wv2_ver "1.0.2903.40")
    set(_wv2_url "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${_wv2_ver}")
    set(_wv2_dl_dir "${DUI_ROOT}/third_party/downloads")
    set(_wv2_archive "${_wv2_dl_dir}/Microsoft.Web.WebView2.${_wv2_ver}.nupkg")

    file(MAKE_DIRECTORY "${_wv2_dl_dir}")
    dui_deps_download_retry("${_wv2_url}" "${_wv2_archive}" "zip")

    message(STATUS "Extracting WebView2 SDK...")
    file(MAKE_DIRECTORY "${_wv2_dest}")
    file(ARCHIVE_EXTRACT INPUT "${_wv2_archive}" DESTINATION "${_wv2_dest}")
    if(NOT EXISTS "${_wv2_dest}/build/native/${DUI_SYSTEM_PROCESSOR}/WebView2Loader.dll.lib")
        # Fallback: .nupkg is a zip; try tar (in case CMake lacks zip support)
        execute_process(
            COMMAND tar -xf "${_wv2_archive}"
            WORKING_DIRECTORY "${_wv2_dest}"
            RESULT_VARIABLE _wv2_tar_result
        )
        if(NOT _wv2_tar_result EQUAL 0 OR NOT EXISTS "${_wv2_dest}/build/native/${DUI_SYSTEM_PROCESSOR}/WebView2Loader.dll.lib")
            file(REMOVE "${_wv2_archive}")  # corrupt archive; force a fresh download next time
            file(REMOVE_RECURSE "${_wv2_dest}")
            message(FATAL_ERROR "WebView2 SDK extraction failed: ${_wv2_archive}")
        endif()
    endif()
    # keep the .nupkg itself in third_party/downloads/ for offline re-extract
    message(STATUS "WebView2 SDK ready: ${_wv2_dest}")
endfunction()
