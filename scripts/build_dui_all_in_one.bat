echo OFF

:: build dui: MSVC + LLVM
set CURRENT_DIR=%cd%
set SCRIPT_DIR=%~dp0

:: Runtime Library (/MT or /MD)
set "RuntimeLibraryRelease=/MT"
set "RuntimeLibraryDebug=/MTd"
if "%1" == "/MD" (
    set "RuntimeLibraryRelease=/MD"
    set "RuntimeLibraryDebug=/MDd"
)
echo RuntimeLibraryRelease: %RuntimeLibraryRelease%
echo RuntimeLibraryDebug: %RuntimeLibraryDebug%

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

where curl.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo curl.exe found at:
    where curl.exe
) else (
    echo curl.exe not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

where tar.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo tar.exe found at:
    where tar.exe
) else (
    echo tar.exe not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

if exist "C:\LLVM\bin\clang.exe" (
    echo clang.exe found at: C:\LLVM\bin\clang.exe
) else (
    echo clang.exe not found in default location
    cd /d %CURRENT_DIR%
    exit /b 1
)

if exist "C:\LLVM\bin\clang++.exe" (
    echo clang++.exe found at: C:\LLVM\bin\clang++.exe
) else (
    echo clang++.exe not found in default location
    cd /d %CURRENT_DIR%
    exit /b 1
)

@REM # detect vs version
if exist "%SCRIPT_DIR%\detect_vs_version.bat" (
    call %SCRIPT_DIR%\detect_vs_version.bat
) else (
    echo detect_vs_version.bat not found in %SCRIPT_DIR%
    cd /d %CURRENT_DIR%
    exit /b 1
)

set MSVC_PATH="%VS_PATH%"
echo %MSVC_PATH%

if not exist %MSVC_PATH% (
    echo Microsoft Visual Studio not found.
    cd /d %CURRENT_DIR%
    exit /b 1
)

if %VS_MAJOR_VERSION% LSS 17 (
    echo.
    echo ==============================================
    echo "ERROR: Visual Studio 2022 (version 17.0) or newer is required!"
    echo "Detected VS Major Version: %VS_MAJOR_VERSION%"
    echo ==============================================
    echo.
    cd /d %CURRENT_DIR%
    exit /b 1
)

cd /d %SCRIPT_DIR%
echo %cd%
if not exist ".\dui\CMakeLists.txt" (
    if exist "..\..\dui\CMakeLists.txt" (
        cd ..\..\
    )
)
echo %cd%

set retry_delay=10

@REM Fetch skia: download the dui fork zip and extract it (replaces git clone + skia_compile patch)
set SKIA_ZIP_URL=https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.0.zip
set SKIA_ZIP_FILE=skia.zip

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
del %SKIA_ZIP_FILE%
:fetch_skia_done

@REM Fetch ninja with skia's own fetch script (python3, checked above)
if not exist ".\dui\third_party\skia\third_party\ninja\ninja.exe" (
    python3 .\dui\third_party\skia\bin\fetch-ninja
)

@REM Use a system gn if available; otherwise build gn from source (official instructions;
@REM needs the MSVC toolchain in PATH). The clone tries the Google source 2 times, then
@REM falls back to a GitHub mirror. Slow/hung clones abort quickly: 10s connect timeout,
@REM and fail if the transfer stays below 200 KB/s for 30s (git http.lowSpeed* options).
set GN_BIN=
where gn >nul 2>&1
if %errorlevel% equ 0 (
    echo Using system gn:
    where gn
    set GN_BIN=gn
) else (
    echo gn not found in PATH - building gn from source
    if not exist ".\dui\third_party\gn\out\gn.exe" (
        if not exist ".\dui\third_party\gn\.git" (
            @REM NOTE: full clone required - build/gen.py runs `git describe --match initial-commit`
            @REM to generate last_commit_position.h, which fails on a shallow clone (the tag only
            @REM exists in the full history). The repo is small (~40MB, ~30s to clone).
            for /l %%n in (1,1,2) do (
                if not exist ".\dui\third_party\gn\.git" (
                    echo Cloning gn from Google source (attempt %%n/2): https://gn.googlesource.com/gn
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
            echo   or build gn from source per https://gn.googlesource.com/gn/+/refs/heads/main/README.md
            cd /d %CURRENT_DIR%
            exit /b 1
        )
        for /f "delims=" %%i in ('where /R %MSVC_PATH% vcvarsall.bat') do set "MSVC_VAR_PATH=%%i"
        @call "%MSVC_VAR_PATH%" x64
        cd dui\third_party\gn
        python3 build/gen.py
        ..\skia\third_party\ninja\ninja.exe -C out
        cd ..\..\..
        if not exist ".\dui\third_party\gn\out\gn.exe" (
            echo gn build failed! Install gn or check the build log above.
            echo Hint: build gn from source per https://gn.googlesource.com/gn/+/refs/heads/main/README.md
            cd /d %CURRENT_DIR%
            exit /b 1
        )
    )
    set GN_BIN=..\gn\out\gn.exe
)

@REM build skia
cd dui\third_party\skia
%GN_BIN% gen out/llvm.x64.debug --ide="%VS_VERSION%" --sln="skia" --args="target_cpu=\"x64\" cc=\"clang\" cxx=\"clang++\" clang_win=\"C:/LLVM\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"%RuntimeLibraryDebug%\"]"
.\third_party\ninja\ninja.exe -C out/llvm.x64.debug

%GN_BIN% gen out/llvm.x64.release --ide="%VS_VERSION%" --sln="skia" --args="target_cpu=\"x64\" cc=\"clang\" cxx=\"clang++\" clang_win=\"C:/LLVM\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"%RuntimeLibraryRelease%\"]"
.\third_party\ninja\ninja.exe -C out/llvm.x64.release

%GN_BIN% gen out/llvm.x86.release --ide="%VS_VERSION%" --sln="skia" --args="target_cpu=\"x86\" cc=\"clang\" cxx=\"clang++\" clang_win=\"C:/LLVM\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"%RuntimeLibraryRelease%\"]"
.\third_party\ninja\ninja.exe -C out/llvm.x86.release

%GN_BIN% gen out/llvm.x86.debug --ide="%VS_VERSION%" --sln="skia" --args="target_cpu=\"x86\" cc=\"clang\" cxx=\"clang++\" clang_win=\"C:/LLVM\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"%RuntimeLibraryDebug%\"]"
.\third_party\ninja\ninja.exe -C out/llvm.x86.debug
cd ..\..\..

@REM build dui
for /f "delims=" %%i in ('where /R %MSVC_PATH% vcvarsall.bat') do set "MSVC_VAR_PATH=%%i"
echo vcvarsall.bat full path: %MSVC_VAR_PATH%
@call "%MSVC_VAR_PATH%" x64

where devenv >nul 2>&1
if %errorlevel% equ 0 (
    echo devenv found at:  
    where devenv
) else (
    echo devenv not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

:: dui Runtime Library (/MT or /MD)
if "%RuntimeLibraryDebug%" == "/MDd" (
    call .\dui\msvc\PropertySheets\DuiUseDynamicRuntime.bat /S
) else (
    call .\dui\msvc\PropertySheets\DuiUseStaticRuntime.bat /S
)

devenv "./dui/scripts/examples.sln" /Build "Debug|x64"

cd /d %CURRENT_DIR%
echo.
echo finished.
