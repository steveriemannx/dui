# dui_deps.cmake
# Dependency management for Skia / CEF:
#   - Skia sources are vendored as zip downloads (third_party/skia);
#     when missing they are downloaded and extracted automatically at configure time
#     (see dui_deps_download_skia below), so they are present
#     at configure time (src/CMakeLists.txt compiles skia's tools/window sources directly
#     into the dui library). The zip archives are kept in third_party/downloads/
#     (gitignored) so that deleting the extracted source and reconfiguring re-extracts
#     from the cached archive without re-downloading.
#   - The actual BUILD happens at make time via add_custom_target (dui_skia).
#   - CEF is a binary distribution (not source); when missing it is downloaded and extracted
#     automatically at configure time (URLs follow docs/CEF.md). Its archive is also cached
#     in third_party/downloads/.
#
# Included from dui_common.cmake (both the top-level build and standalone example builds).

# ---- Configure-time: source guards, skia args.gn, CEF download ----
function(dui_deps_configure)
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
            "The automatic download failed. Retry cmake configure, or build the tree by\n"
            "hand: fetch\n"
            "  https://github.com/google/skia/archive/34aa71b8bee4648a442b7125680232d803374f19.zip\n"
            "extract it into ${DUI_SKIA_SRC_ROOT_DIR}, then apply\n"
            "  third_party/skia-patches/dui.patch  (patch -p1 -N -f)\n"
            "(or set -DDUI_BUILD_SKIA_FROM_SOURCE=OFF and provide Skia yourself)")
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

        # A sanitizer build changes the layout of Skia's own types. SkASAN.h defines
        # SK_SANITIZE_ADDRESS whenever a translation unit is compiled with
        # -fsanitize=address, and SkTArray has a member under it -- so an instrumented
        # dui linked against an uninstrumented Skia disagrees with it about every type
        # containing a TArray, SkSVGContainer::fChildren among them. The observable
        # result is heap-buffer-overflow reports inside objects Skia allocated, which
        # are artifacts of the mismatch rather than defects: measured, SkSVGSVG is 848
        # bytes in dui's translation units and 840 in Skia's.
        # The cost is that toggling DUI_ENABLE_SANITIZERS rewrites args.gn, which
        # re-runs gn and rebuilds Skia.
        if(DUI_ENABLE_SANITIZERS)
            string(APPEND _gn_extra "sanitize = \"ASAN\"\n")
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
            # is_debug is always false, matching the multi-config branch above and for
            # the same reason. It is not merely a preference: skia's own BUILDCONFIG.gn
            # contains `assert(!(is_debug && is_official_build))`, and GN_ARGS_COMMON
            # sets is_official_build = true, so writing is_debug = true here made
            # `gn gen` abort -- a Debug build of dui on macOS/Linux did not work at all.
            # The dui targets still build with -g and no optimisation; only Skia is
            # release-grade, which is the usual arrangement when debugging against a
            # prebuilt dependency.
            file(MAKE_DIRECTORY "${DUI_SKIA_LIB_PATH}")
            file(WRITE "${DUI_SKIA_LIB_PATH}/args.gn" "${GN_ARGS_COMMON}is_debug = false\n")
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
    # Windows: run every ninja build through a filter .bat that (a) sets CL=/utf-8
    # (C4819 on codepage-936 Windows: sources contain non-ASCII chars and the build
    # treats warnings as errors), (b) hides cl's /showIncludes notes ("Note:
    # including file") from the console - ninja still parses them internally for
    # dependency tracking - keeping only progress and error lines visible, and
    # (c) prepends the directory of the gn binary (%~3) to PATH, since skia's
    # find_headers.py action runs a bare "gn" (a dui change, carried by
    # third_party/skia-patches/dui.patch, not by upstream Skia).
    #   Usage: dui_ninja.bat <ninja> <build-dir> [gn-path] [target...]
    if(WIN32)
        set(DUI_NINJA_FILTER_BAT "${CMAKE_CURRENT_BINARY_DIR}/dui_ninja.bat")
        file(WRITE "${DUI_NINJA_FILTER_BAT}"
            "@echo off\r\n"
            "set \"CL=%CL% /utf-8\"\r\n"
            "if not \"%~3\"==\"\" set \"GN_BIN_DIR=%~dp3\"\r\n"
            "if defined GN_BIN_DIR set \"PATH=%GN_BIN_DIR%;%PATH%\"\r\n"
            "\"%~1\" -C \"%~2\" %~4 %~5 > \"%TEMP%\\dui_ninja.log\" 2>&1\r\n"
            "set \"RC=%errorlevel%\"\r\n"
            "findstr /i /r /c:\"^\\[[0-9]*/[0-9]*\\]\" /c:\"error\" /c:\"FAILED\" /c:\"ninja:\" /c:\"LINK\" \"%TEMP%\\dui_ninja.log\"\r\n"
            "if not \"%RC%\"==\"0\" echo [dui_ninja] ninja exited with code %RC% - full log: %TEMP%\\dui_ninja.log\r\n"
            "exit /b %RC%\r\n")
    endif()

    # Run the skia ninja build. Windows runs it through dui_ninja.bat (sets CL=/utf-8,
    # hides cl /showIncludes notes, puts the gn directory on PATH for skia's
    # find_headers.py action); other platforms use plain "ninja -C <dir>".
    macro(dui_skia_ninja_command _out _ninja _dir _gn)
        if(WIN32)
            set(${_out} COMMAND cmd /c "${DUI_NINJA_FILTER_BAT}" ${_ninja} "${_dir}" ${_gn})
        else()
            set(${_out} COMMAND ${_ninja} -C "${_dir}")
        endif()
    endmacro()

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

        # ---- gn: built from source at make time (ordered before Skia) ----
        set(DUI_GN_BIN "")  # set when the gn source build is available; skia uses it below
        if(EXISTS "${DUI_ROOT}/third_party/gn/build/gen.py")
            find_program(DUI_GN_PYTHON NAMES python3 python)
            if(DUI_GN_PYTHON)
                if(WIN32)
                    set(DUI_GN_BIN "${CMAKE_BINARY_DIR}/tools/gn/gn.exe")
                else()
                    set(DUI_GN_BIN "${CMAKE_BINARY_DIR}/tools/gn/gn")
                endif()
                # gen.py emits build.ninja that invokes bare "cl.exe", which requires
                # the MSVC toolchain in PATH (ninja fails with "CreateProcess failed:
                # The system cannot find the file specified" otherwise). Run the whole
                # build inside a temp .bat that calls vcvarsall first, through the
                # shared filter .bat (line endings are normalized to LF at configure
                # time, so the unit tests build and pass too).
                if(WIN32)
                    dui_find_vcvarsall(_gn_vcvarsall _gn_vc_arch)
                    if(_gn_vcvarsall)
                        set(_gn_bat "${CMAKE_CURRENT_BINARY_DIR}/build_gn.bat")
                        file(WRITE "${_gn_bat}"
                            "@call \"${_gn_vcvarsall}\" ${_gn_vc_arch} >nul\r\n"
                            "\"${DUI_GN_PYTHON}\" build/gen.py --out-path \"${CMAKE_BINARY_DIR}/tools/gn\"\r\n"
                            # gn's own integration tests invoke a bare "gn.exe" (see
                            # build/ninja_file.py: run_gn rule), which cmd resolves
                            # via PATH; without this the freshly built out/gn.exe is
                            # not found and the gn build fails.
                            "set \"PATH=${CMAKE_BINARY_DIR}/tools/gn;%PATH%\"\r\n"
                            "call \"${DUI_NINJA_FILTER_BAT}\" \"${DUI_NINJA_BIN}\" \"${CMAKE_BINARY_DIR}/tools/gn\"\r\n")
                        set(_gn_commands COMMAND cmd /c "${_gn_bat}")
                    else()
                        message(WARNING "vcvarsall not found - the gn source build will likely fail. "
                                "Install gn via a package manager or build it manually in a VS "
                                "Developer Command Prompt")
                        set(_gn_commands
                            COMMAND "${DUI_GN_PYTHON}" build/gen.py --out-path "${CMAKE_BINARY_DIR}/tools/gn"
                            COMMAND "${DUI_NINJA_BIN}" -C "${CMAKE_BINARY_DIR}/tools/gn")
                    endif()
                else()
                    # Build gn the way upstream does: in its own tree, with a
                    # RELATIVE --out-path. gen.py derives every path it emits
                    # from that argument, and it also hardcodes "../build/gen.py"
                    # into the regen rule - so an absolute (or out-of-tree) path
                    # makes ninja consider build.ninja perpetually dirty and it
                    # loops ("still dirty after 100 tries"). The binary is then
                    # copied where the rest of the build expects it.
                    set(_gn_commands
                        COMMAND "${DUI_GN_PYTHON}" build/gen.py --out-path "out"
                        COMMAND "${DUI_NINJA_BIN}" -C "${DUI_ROOT}/third_party/gn/out" gn
                        COMMAND "${CMAKE_COMMAND}" -E copy "${DUI_ROOT}/third_party/gn/out/gn" "${DUI_GN_BIN}")
                endif()
                add_custom_command(
                    OUTPUT "${DUI_GN_BIN}"
                    ${_gn_commands}
                    WORKING_DIRECTORY "${DUI_ROOT}/third_party/gn"
                    DEPENDS "${DUI_ROOT}/third_party/gn/build/gen.py"
                    COMMENT "Building gn (python build/gen.py + ninja)..."
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
                        HINTS "${CMAKE_BINARY_DIR}/tools/gn" "${DUI_SKIA_SRC_ROOT_DIR}/bin"
                        REQUIRED)
                endif()
                set(NINJA_EXECUTABLE_DEBUG "${DUI_NINJA_BIN}")
                dui_skia_ninja_command(_skia_ninja_cmd ${NINJA_EXECUTABLE_DEBUG} "${DUI_SKIA_LIB_PATH_DEBUG}" ${GN_EXECUTABLE_DEBUG})
                add_custom_command(
                    OUTPUT "${DUI_SKIA_LIB_PATH_DEBUG}/${_skia_main_lib}"
                    COMMAND ${GN_EXECUTABLE_DEBUG} gen "${DUI_SKIA_LIB_PATH_DEBUG}"
                    ${_skia_ninja_cmd}
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
                        HINTS "${CMAKE_BINARY_DIR}/tools/gn" "${DUI_SKIA_SRC_ROOT_DIR}/bin"
                        REQUIRED)
                endif()
                set(NINJA_EXECUTABLE_RELEASE "${DUI_NINJA_BIN}")
                dui_skia_ninja_command(_skia_ninja_cmd ${NINJA_EXECUTABLE_RELEASE} "${DUI_SKIA_LIB_PATH_RELEASE}" ${GN_EXECUTABLE_RELEASE})
                add_custom_command(
                    OUTPUT "${DUI_SKIA_LIB_PATH_RELEASE}/${_skia_main_lib}"
                    COMMAND ${GN_EXECUTABLE_RELEASE} gen "${DUI_SKIA_LIB_PATH_RELEASE}"
                    ${_skia_ninja_cmd}
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
                        HINTS "${CMAKE_BINARY_DIR}/tools/gn" "${DUI_SKIA_SRC_ROOT_DIR}/bin"
                        REQUIRED)
                endif()
                set(NINJA_EXECUTABLE "${DUI_NINJA_BIN}")
                dui_skia_ninja_command(_skia_ninja_cmd ${NINJA_EXECUTABLE} "${DUI_SKIA_LIB_PATH}" ${GN_EXECUTABLE})

                add_custom_command(
                    OUTPUT "${DUI_SKIA_LIB_PATH}/${_skia_main_lib}"
                    COMMAND ${GN_EXECUTABLE} gen "${DUI_SKIA_LIB_PATH}"
                    ${_skia_ninja_cmd}
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

    # Expose Skia as a target instead of relying on directory-wide linker
    # search paths. This keeps Debug/Release selection correct for multi-config
    # generators and makes the build-order dependency explicit.
    if(NOT TARGET dui_skia_libs)
        add_library(dui_skia_libs INTERFACE)
        if(MSVC)
            set(_skia_lib_prefix "")
            set(_skia_lib_suffix ".lib")
        else()
            set(_skia_lib_prefix "lib")
            set(_skia_lib_suffix ".a")
        endif()
        foreach(_skia_lib ${DUI_SKIA_LIBS})
            if(DUI_MULTI_CONFIG)
                target_link_libraries(dui_skia_libs INTERFACE
                    "$<$<CONFIG:Debug>:${DUI_SKIA_LIB_PATH_DEBUG}/${_skia_lib_prefix}${_skia_lib}${_skia_lib_suffix}>"
                    "$<$<NOT:$<CONFIG:Debug>>:${DUI_SKIA_LIB_PATH_RELEASE}/${_skia_lib_prefix}${_skia_lib}${_skia_lib_suffix}>"
                )
            else()
                target_link_libraries(dui_skia_libs INTERFACE
                    "${DUI_SKIA_LIB_PATH}/${_skia_lib_prefix}${_skia_lib}${_skia_lib_suffix}")
            endif()
        endforeach()
    endif()
    if(TARGET dui_skia)
        add_dependencies(dui_skia_libs dui_skia)
    endif()

endfunction()

# ---- Download helper: verify cached archive, retry on failure (shared by Skia/CEF/WebView2) ----
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
    # What is fetched is upstream Skia at one pinned commit, from google/skia, plus
    # dui's own changes held as a patch in this repository
    # (third_party/skia-patches/dui.patch). The marker records both, because the
    # extracted tree alone cannot say which upstream commit or which patch revision
    # produced it -- so changing either one has to re-extract, or it would be
    # silently ignored.
    set(_skia_upstream_commit "34aa71b8bee4648a442b7125680232d803374f19")
    set(_skia_patch_revision   "dui.1")  # bump whenever the patch file changes
    set(_skia_version "skia-${_skia_upstream_commit}+${_skia_patch_revision}")
    if(EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/.dui_skia_version")
        file(READ "${DUI_SKIA_SRC_ROOT_DIR}/.dui_skia_version" _skia_have_version)
        string(STRIP "${_skia_have_version}" _skia_have_version)
        if(_skia_have_version STREQUAL "${_skia_version}")
            return()  # already present and up to date
        endif()
        message(STATUS "Skia version changed (${_skia_have_version} -> ${_skia_version}); re-extracting")
    elseif(EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/BUILD.gn")
        message(STATUS "Skia present without a version marker; re-extracting")
    else()
        message(STATUS "Fetching Skia source ...")
    endif()
    if(EXISTS "${DUI_SKIA_SRC_ROOT_DIR}")
        file(REMOVE_RECURSE "${DUI_SKIA_SRC_ROOT_DIR}")
    endif()

    # Upstream zip, not a fork archive: what dui builds from is a public commit plus a
    # diff that anyone can read, and moving to a newer Skia is "rebase one file".
    set(_skia_url "https://github.com/google/skia/archive/${_skia_upstream_commit}.zip")
    set(_skia_topdir "skia-${_skia_upstream_commit}")  # the single top-level folder inside the zip
    set(_skia_dl_dir "${DUI_ROOT}/third_party/downloads")
    set(_skia_archive "${_skia_dl_dir}/skia-${_skia_upstream_commit}.zip")

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

    # ---- Apply dui's changes -------------------------------------------------------
    # Everything dui needs from Skia that upstream does not provide lives in this one
    # file, applied to the freshly extracted upstream tree. The post-condition is not
    # the exit code alone: a patch that matched nothing can still exit 0 with -N, so
    # the sentinel is a file the patch is known to add.
    set(_skia_patch "${DUI_ROOT}/third_party/skia-patches/dui.patch")
    if(NOT EXISTS "${_skia_patch}")
        message(FATAL_ERROR "Skia customization patch is missing: ${_skia_patch}")
    endif()
    find_program(_dui_patch_tool NAMES patch)
    if(_dui_patch_tool)
        # -N + -f keep it non-interactive; a mismatch still has to fail, which the
        # sentinel check below is for.
        set(_dui_apply_cmd "${_dui_patch_tool}" -p1 -N -f -i "${_skia_patch}")
    else()
        find_program(_dui_git_tool NAMES git)
        if(NOT _dui_git_tool)
            message(FATAL_ERROR
                "Applying ${_skia_patch} needs either 'patch' or 'git' on PATH. Both ship "
                "with Git for Windows; or apply the patch by hand into "
                "${DUI_SKIA_SRC_ROOT_DIR} and re-run configure.")
        endif()
        set(_dui_apply_cmd "${_dui_git_tool}" apply -p1 --whitespace=nowarn "${_skia_patch}")
    endif()
    message(STATUS "Applying dui's Skia customizations (${_skia_patch_revision})...")
    execute_process(
        COMMAND ${_dui_apply_cmd}
        WORKING_DIRECTORY "${DUI_SKIA_SRC_ROOT_DIR}"
        RESULT_VARIABLE _skia_patch_result
        OUTPUT_VARIABLE _skia_patch_out
        ERROR_VARIABLE _skia_patch_err
    )
    if(NOT _skia_patch_result EQUAL 0 OR NOT EXISTS "${DUI_SKIA_SRC_ROOT_DIR}/gn/is_mingw.py")
        string(REPLACE "\n" "\n    " _skia_patch_report "${_skia_patch_out}${_skia_patch_err}")
        file(REMOVE_RECURSE "${DUI_SKIA_SRC_ROOT_DIR}")  # never leave a half-patched tree
        message(FATAL_ERROR
            "Applying the Skia customization patch failed (exit ${_skia_patch_result}).\n"
            "The tree has been removed rather than left half-patched; fix the patch or\n"
            "regenerate it (see third_party/skia-patches/README.md) and re-run configure.\n"
            "    ${_skia_patch_report}")
    endif()

    file(WRITE "${DUI_SKIA_SRC_ROOT_DIR}/.dui_skia_version" "${_skia_version}")
    message(STATUS "Skia source ready: ${DUI_SKIA_SRC_ROOT_DIR} (${_skia_version})")
endfunction()

# ---- GN source clone (idempotent; the build itself happens at make time) ----
# Building skia requires gn. Prebuilt CIPD binaries only cover amd64 reliably, so clone the
# gn source at configure time and build it at make time via the dui_gn target
# (build/gen.py + ninja -C out, per https://gn.googlesource.com/gn/+/refs/heads/main/README.md),
# ordered before the Skia build. The binary lands in build/tools/gn/gn (gn.exe on
# Windows). A system gn is preferred and skips the clone entirely; if the clone is
# unavailable, configure falls back to a system gn or skia's bin/ at make time.
function(dui_deps_download_gn)
    if(NOT DUI_BUILD_SKIA_FROM_SOURCE)
        return()
    endif()
    # Resolve gn once per configure: dui_deps_configure runs per scope (each
    # example), which would otherwise repeat the "Using system gn" status
    # messages once per example.
    get_property(_gn_resolved GLOBAL PROPERTY DUI_GN_RESOLVED)
    if(_gn_resolved)
        return()
    endif()
    set_property(GLOBAL PROPERTY DUI_GN_RESOLVED TRUE)

    set(_gn_dir "${DUI_ROOT}/third_party/gn")
    # Windows checkouts convert text files to CRLF (core.autocrlf default), which
    # breaks gn's own unit tests (format_test_data byte comparison) and confuses
    # other tools. Normalize the clone to LF once (config is persisted, so this
    # only runs on the first configure).
    if(WIN32 AND EXISTS "${_gn_dir}/.git")
        execute_process(
            COMMAND git -C "${_gn_dir}" config --get core.autocrlf
            OUTPUT_VARIABLE _gn_autocrlf
            OUTPUT_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _gn_cfg_result
        )
        if(NOT _gn_autocrlf STREQUAL "false")
            message(STATUS "Normalizing gn checkout line endings to LF (core.autocrlf false)")
            execute_process(COMMAND git -C "${_gn_dir}" config core.autocrlf false)
            execute_process(COMMAND git -C "${_gn_dir}" rm --cached -r . OUTPUT_QUIET)
            execute_process(COMMAND git -C "${_gn_dir}" reset --hard OUTPUT_QUIET)
        endif()
    endif()
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
# The tar.bz2 is cached in third_party/downloads/ (gitignored), like the Skia zips;
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
# The .nupkg is cached in third_party/downloads/ (gitignored), like the Skia zips.
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
