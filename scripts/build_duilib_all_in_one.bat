echo OFF

:: build duilib: MSVC + LLVM
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
if not exist ".\nim_duilib\.git" (
    if exist "..\..\nim_duilib\.git" (
        cd ..\..\
    )
)
echo %cd%

set retry_delay=10

:retry_clone_duilib
if not exist ".\nim_duilib\.git" (
    git clone https://github.com/rhett-lee/nim_duilib
) else (
    git -C ./nim_duilib pull
)
if %errorlevel% neq 0 (
    timeout /t %retry_delay% >nul
    goto retry_clone_duilib
)

if not exist ".\nim_duilib\.git" (
    echo clone nim_duilib failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)

@REM Fetch skia: download the dui fork zip and extract it (replaces git clone + skia_compile patch)
set SKIA_ZIP_URL=https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.0.zip
set SKIA_ZIP_FILE=skia.zip

if exist ".\skia\BUILD.gn" goto fetch_skia_done
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
if not exist ".\skia" mkdir ".\skia"
tar -xf %SKIA_ZIP_FILE% --strip-components=1 -C .\skia
if %errorlevel% neq 0 (
    echo extract skia failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)
if not exist ".\skia\BUILD.gn" (
    echo fetch skia failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)
del %SKIA_ZIP_FILE%
:fetch_skia_done

@REM Fetch gn/ninja build tools with skia's own fetch scripts (python3, checked above)
if not exist ".\skia\bin\gn.exe" (
    python3 .\skia\bin\fetch-gn
)
if not exist ".\skia\third_party\ninja\ninja.exe" (
    python3 .\skia\bin\fetch-ninja
)

@REM build skia
cd skia
.\bin\gn.exe gen out/llvm.x64.debug --ide="%VS_VERSION%" --sln="skia" --args="target_cpu=\"x64\" cc=\"clang\" cxx=\"clang++\" clang_win=\"C:/LLVM\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"%RuntimeLibraryDebug%\"]"
.\third_party\ninja\ninja.exe -C out/llvm.x64.debug

.\bin\gn.exe gen out/llvm.x64.release --ide="%VS_VERSION%" --sln="skia" --args="target_cpu=\"x64\" cc=\"clang\" cxx=\"clang++\" clang_win=\"C:/LLVM\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"%RuntimeLibraryRelease%\"]"
.\third_party\ninja\ninja.exe -C out/llvm.x64.release

.\bin\gn.exe gen out/llvm.x86.release --ide="%VS_VERSION%" --sln="skia" --args="target_cpu=\"x86\" cc=\"clang\" cxx=\"clang++\" clang_win=\"C:/LLVM\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"%RuntimeLibraryRelease%\"]"
.\third_party\ninja\ninja.exe -C out/llvm.x86.release

.\bin\gn.exe gen out/llvm.x86.debug --ide="%VS_VERSION%" --sln="skia" --args="target_cpu=\"x86\" cc=\"clang\" cxx=\"clang++\" clang_win=\"C:/LLVM\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"%RuntimeLibraryDebug%\"]"
.\third_party\ninja\ninja.exe -C out/llvm.x86.debug
cd ..

@REM build nim_duilib
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

:: nim_duilib Runtime Library (/MT or /MD)
if "%RuntimeLibraryDebug%" == "/MDd" (
    call .\nim_duilib\msvc\PropertySheets\DuilibUseDynamicRuntime.bat /S
) else (
    call .\nim_duilib\msvc\PropertySheets\DuilibUseStaticRuntime.bat /S
)

devenv "./nim_duilib/scripts/examples.sln" /Build "Debug|x64"

cd /d %CURRENT_DIR%
echo.
echo finished.
