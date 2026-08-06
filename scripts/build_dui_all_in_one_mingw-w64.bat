echo OFF

:: Force enable SDL
set ENABLE_SDL=0
if "%1" == "-sdl" (
    set ENABLE_SDL=1
)
echo ENABLE_SDL: %ENABLE_SDL%

:: build dui: MinGW-w64 gcc/g++ or clang/clang++
set CURRENT_DIR=%cd%
set SCRIPT_DIR=%~dp0

echo Checking the necessary software
where git.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo git.exe found at:  
    where git.exe
) else (
    echo git.exe not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

where python3.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo python3.exe found at:  
    where python3.exe
) else (
    echo python3.exe not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

where cmake.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo cmake.exe found at:  
    where cmake.exe
) else (
    echo cmake.exe not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

set has_gcc=0
set has_clang=0

:: gcc/g++
where gcc >nul 2>&1 && where g++ >nul 2>&1 && set has_gcc=1

:: clang/clang++
where clang >nul 2>&1 && where clang++ >nul 2>&1 && set has_clang=1

if %has_gcc%%has_clang% equ 00 (
    echo  "GCC/G++ - MinGW not found in PATH"
    echo  "Clang/Clang++ - LLVM not found in PATH"
    cd /d %CURRENT_DIR%
    exit /b 1
)

@REM Skia's gn selects the mingw toolchain only for a MinGW-targeting compiler
@REM (gn/is_mingw.py); an MSVC-targeting clang (e.g. C:\LLVM) would silently fall
@REM back to the MSVC cl.exe toolchain and fail on clang-only flags. Verify
@REM that the clang on PATH targets MinGW before building skia.
if %has_clang% equ 1 (
    clang --version | findstr /i "windows-msvc" >nul 2>&1
    if not errorlevel 1 (
        echo The clang on PATH targets MSVC - LLVM for Windows, not MinGW.
        echo Add the llvm-mingw bin directory to the FRONT of PATH, for example:
        echo   SET PATH=C:\mingw64\llvm-mingw-20250430-ucrt-x86_64\bin;%%PATH%%
        cd /d %CURRENT_DIR%
        exit /b 1
    )
)

if %has_clang% equ 1 (
    where clang
    where clang++
) else (
    where gcc
    where g++
)

cd /d %SCRIPT_DIR%
if not exist ".\dui\CMakeLists.txt" (
    if exist "..\..\dui\CMakeLists.txt" (
        cd ..\..\
    )
)

@REM Reuse the ninja vendored in the skia fork; fall back to ninja on PATH.
set NINJA_BIN=
if exist ".\dui\third_party\skia\third_party\ninja\ninja.exe" (
    set "NINJA_BIN=%CD%\dui\third_party\skia\third_party\ninja\ninja.exe"
)
if not defined NINJA_BIN (
    where ninja >nul 2>&1
    if errorlevel 1 (
        echo ninja not found - install the mingw-w64 ninja package
        cd /d %CURRENT_DIR%
        exit /b 1
    )
    set "NINJA_BIN=ninja"
)
echo NINJA_BIN: %NINJA_BIN%

set retry_delay=10

@REM gn acquisition order: existing source build -> system gn -> skia's bin/fetch-gn
@REM (prebuilt CIPD binary) -> clone Google source (2 attempts) -> GitHub mirror
@REM -> build from source.
@REM Slow/hung clones abort quickly: 10s connect timeout, and fail if the transfer
@REM stays below 200 KB/s for 30s (git http.lowSpeed* options).
set GN_BIN=
@REM 1) reuse an existing source-built gn
if exist ".\dui\third_party\gn\out\gn.exe" (
    set "GN_BIN=%CD%\dui\third_party\gn\out\gn.exe"
)
@REM 2) system gn (zero cost - already installed, no download needed)
if not defined GN_BIN (
    where gn >nul 2>&1
    if not errorlevel 1 (
        echo Using system gn:
        where gn
        for /f "delims=" %%i in ('where gn') do set "GN_BIN=%%i"
    )
)
@REM 3) try skia's own bin/fetch-gn (prebuilt CIPD binary into skia/bin)
if not defined GN_BIN (
    if exist ".\dui\third_party\skia\bin\fetch-gn" (
        echo Trying skia's bin/fetch-gn ...
        python3 .\dui\third_party\skia\bin\fetch-gn
        if exist ".\dui\third_party\skia\bin\gn.exe" (
            echo Using gn fetched by skia's bin/fetch-gn
            set "GN_BIN=%CD%\dui\third_party\skia\bin\gn.exe"
        )
    )
)
@REM 4) clone + build from source
if not defined GN_BIN (
    echo gn not found - building gn from source
    if not exist ".\dui\third_party\gn\out\gn.exe" (
        if not exist ".\dui\third_party\gn\.git" (
            @REM NOTE: full clone required - build/gen.py runs `git describe --match initial-commit`
            @REM to generate last_commit_position.h, which fails on a shallow clone (the tag only
            @REM exists in the full history). The repo is small (~40MB, ~30s to clone).
            for /l %%n in (1,1,2) do (
                if not exist ".\dui\third_party\gn\.git" (
                    echo Cloning gn from Google source - attempt %%n/2 - https://gn.googlesource.com/gn
                    git -c http.connectTimeout=10 -c http.lowSpeedLimit=204800 -c http.lowSpeedTime=30 clone https://gn.googlesource.com/gn .\dui\third_party\gn
                    if not exist ".\dui\third_party\gn\.git" (
                        if exist ".\dui\third_party\gn" rmdir /s /q ".\dui\third_party\gn"
                        if %%n lss 2 timeout /t %retry_delay% >nul
                    )
                )
            )
            if not exist ".\dui\third_party\gn\.git" (
                echo Google source failed after 2 attempts; trying GitHub mirror: https://github.com/ArthurSonzogni/gn
                git -c http.connectTimeout=10 -c http.lowSpeedLimit=204800 -c http.lowSpeedTime=30 clone https://github.com/ArthurSonzogni/gn .\dui\third_party\gn
            )
        )
        if not exist ".\dui\third_party\gn\.git" (
            echo clone gn failed from both sources!
            echo Please install gn manually and re-run this script.
            echo   MSYS2: pacman -S mingw-w64-x86_64-gn
            echo   or clone the gn source into .\dui\third_party\gn
            echo   or download a prebuilt gn binary and unzip it to .\dui\third_party\skia\bin\gn.exe
            cd /d %CURRENT_DIR%
            exit /b 1
        )
        @REM Normalize line endings to LF (Windows autocrlf converts text files to
        @REM CRLF, which breaks gn's own unit tests - format_test_data comparison)
        git -C .\dui\third_party\gn config core.autocrlf false
        git -C .\dui\third_party\gn rm --cached -r .
        git -C .\dui\third_party\gn reset --hard
        cd dui\third_party\gn
        python3 build/gen.py --platform mingw
        %NINJA_BIN% -C out
        cd ..\..\..
        if not exist ".\dui\third_party\gn\out\gn.exe" (
            echo gn build failed! Install gn or check the build output above.
            echo Hint: build gn from source per https://gn.googlesource.com/gn/+/refs/heads/main/README.md
            cd /d %CURRENT_DIR%
            exit /b 1
        )
    )
    set "GN_BIN=%CD%\dui\third_party\gn\out\gn.exe"
)
echo GN_BIN: %GN_BIN%

@REM Put gn on PATH so skia's find_headers.py (called during ninja) can find it
if "%GN_BIN%" neq "gn" (
    for %%F in ("%GN_BIN%") do set "PATH=%%~dpF;%PATH%"
)

@REM Parallel build jobs: 3/4 of the CPU cores (leave the rest for the system)
set /a DUI_JOBS=%NUMBER_OF_PROCESSORS% * 3 / 4
if %DUI_JOBS% lss 1 set DUI_JOBS=1

@REM Fetch SDL first, then skia (same layout as the CMake build; SDL clone is idempotent)
if %ENABLE_SDL% equ 1 (
echo - Cloning SDL ...
:retry_clone_SDL
    if not exist ".\dui\third_party\SDL3\.git" (
        git clone https://github.com/libsdl-org/SDL.git
    ) else (
        git -C ./dui/third_party/SDL3 pull
    )
    if %errorlevel% neq 0 (
        timeout /t %retry_delay% >nul
        goto retry_clone_SDL
    )
)

@REM Fetch skia: download the dui fork zip (same source as the CMake build; idempotent)
@REM The version marker (.dui_skia_version) triggers a re-fetch when the tag changes.
set SKIA_VERSION=skia-dui-0.1.1
set SKIA_ZIP_URL=https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.1.zip
set SKIA_ZIP_FILE=skia.zip
set SKIA_HAVE_VERSION=
set SKIA_REFETCH=0
if exist ".\dui\third_party\skia\.dui_skia_version" for /f "usebackq delims=" %%v in (".\dui\third_party\skia\.dui_skia_version") do set "SKIA_HAVE_VERSION=%%v"
if exist ".\dui\third_party\skia\BUILD.gn" if not "%SKIA_HAVE_VERSION%"=="%SKIA_VERSION%" set SKIA_REFETCH=1
if "%SKIA_REFETCH%"=="1" (
    echo Skia version mismatch - re-fetching ...
    rmdir /s /q ".\dui\third_party\skia"
)
if exist ".\dui\third_party\skia\BUILD.gn" goto fetch_skia_done
:retry_fetch_skia
curl -L -f -o %SKIA_ZIP_FILE% --retry 3 --connect-timeout 15 --retry-delay 10 --speed-limit 100 --speed-time 120 %SKIA_ZIP_URL%
if %errorlevel% neq 0 (
    timeout /t %retry_delay% >nul
    goto retry_fetch_skia
)
if not exist "%SKIA_ZIP_FILE%" (
    echo fetch skia failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)
@REM Windows 10+ ships tar.exe (bsdtar) which reads zip archives
if not exist ".\dui\third_party\skia" mkdir ".\dui\third_party\skia"
tar -xf %SKIA_ZIP_FILE% --strip-components=1 -C .\dui\third_party\skia
if %errorlevel% neq 0 (
    echo extract skia failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)
if not exist ".\dui\third_party\skia\BUILD.gn" (
    echo fetch skia failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)
> ".\dui\third_party\skia\.dui_skia_version" <nul set /p =%SKIA_VERSION%
del %SKIA_ZIP_FILE%
:fetch_skia_done

echo - Building skia ...
cd dui\third_party\skia

if %has_clang% equ 1 (
    if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
        %GN_BIN% gen out/mingw64-llvm.x64.release --args="target_cpu=\"x64\" cc=\"clang\" cxx=\"clang++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        %NINJA_BIN% -C out/mingw64-llvm.x64.release
        if errorlevel 1 (
            echo skia build failed - fix the errors above and re-run this script.
            cd /d %CURRENT_DIR%
            exit /b 1
        )
    ) else (
        %GN_BIN% gen out/mingw64-llvm.x86.release --args="target_cpu=\"x86\" cc=\"clang\" cxx=\"clang++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        %NINJA_BIN% -C out/mingw64-llvm.x86.release
        if errorlevel 1 (
            echo skia build failed - fix the errors above and re-run this script.
            cd /d %CURRENT_DIR%
            exit /b 1
        )
    )
) else (
    if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
        %GN_BIN% gen out/mingw64-gcc.x64.release --args="target_cpu=\"x64\" cc=\"gcc\" cxx=\"g++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        %NINJA_BIN% -C out/mingw64-gcc.x64.release
        if errorlevel 1 (
            echo skia build failed - fix the errors above and re-run this script.
            cd /d %CURRENT_DIR%
            exit /b 1
        )
    ) else (
        %GN_BIN% gen out/mingw64-gcc.x86.release --args="target_cpu=\"x86\" cc=\"gcc\" cxx=\"g++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        %NINJA_BIN% -C out/mingw64-gcc.x86.release
        if errorlevel 1 (
            echo skia build failed - fix the errors above and re-run this script.
            cd /d %CURRENT_DIR%
            exit /b 1
        )
    )
)
cd ..\..\..

if %ENABLE_SDL% equ 1 (
    if not exist ".\dui\third_party\SDL3\lib" (
        echo - Building SDL ...
        if %has_clang% equ 1 (
            SET DUI_CC=clang
            SET DUI_CXX=clang++
            SET DUI_SDL_DIR=sdl3-build.mingw.llvm
        ) else (
            SET DUI_CC=gcc
            SET DUI_CXX=g++
            SET DUI_SDL_DIR=sdl3-build.mingw.gcc
        )

        cmake -S "./dui/third_party/SDL3/" -B ".\dui\scripts\build_temp\%DUI_SDL_DIR%" -DCMAKE_INSTALL_PREFIX="./dui/third_party/SDL3" -G"MinGW Makefiles" -DCMAKE_C_COMPILER=%DUI_CC% -DCMAKE_CXX_COMPILER=%DUI_CXX% -DSDL_SHARED=OFF -DSDL_STATIC=ON -DSDL_TEST_LIBRARY=OFF -DCMAKE_BUILD_TYPE=Release
        cmake --build .\dui\scripts\build_temp\%DUI_SDL_DIR% -j %DUI_JOBS%
        cmake --install .\dui\scripts\build_temp\%DUI_SDL_DIR%
    ) else (
        echo - SDL3 already installed: .\dui\third_party\SDL3\lib
    )
)

echo - Building dui ...
cd /d %SCRIPT_DIR%
if %has_clang% equ 1 (
    call .\llvm-mingw-w64_build.bat %1
) else (
    call .\gcc-mingw-w64_build.bat %1
)

cd /d %CURRENT_DIR%
echo.
echo finished.
