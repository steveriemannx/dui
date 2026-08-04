echo OFF
:: Fetch the Skia source (zip archive) into third_party\skia
:: Windows counterpart of scripts/fetch_skia.sh (requires curl.exe and tar.exe,
:: both bundled with Windows 10 1803+).
::
:: Usage:
::   scripts\fetch_skia.bat            fetch if not already present (idempotent)
::   scripts\fetch_skia.bat -f         force re-download and re-extract

set CURRENT_DIR=%cd%
set SCRIPT_DIR=%~dp0
set THIRD_PARTY_DIR=%SCRIPT_DIR%..\third_party

set FORCE=0
if "%1" == "-f" set FORCE=1

where curl.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo curl.exe not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

where tar.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo tar.exe not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

set SKIA_ZIP_URL=https://github.com/steveriemannx/skia/archive/refs/tags/skia-dui-0.1.0.zip
set SKIA_ZIP_FILE=%THIRD_PARTY_DIR%\.download\skia.zip
set SKIA_DEST=%THIRD_PARTY_DIR%\skia

if exist "%SKIA_DEST%\BUILD.gn" (
    if %FORCE%==0 (
        echo skia already present at %SKIA_DEST% ^(use -f to re-extract^)
        cd /d %CURRENT_DIR%
        exit /b 0
    )
)

if not exist "%THIRD_PARTY_DIR%\.download" mkdir "%THIRD_PARTY_DIR%\.download"

if not exist "%SKIA_ZIP_FILE%" (
    echo === Downloading: %SKIA_ZIP_URL% ===
    curl -L -f -o "%SKIA_ZIP_FILE%" --retry 3 --connect-timeout 15 --retry-delay 10 --speed-limit 100 --speed-time 120 "%SKIA_ZIP_URL%"
    if %errorlevel% neq 0 (
        echo download failed: %SKIA_ZIP_URL%
        cd /d %CURRENT_DIR%
        exit /b 1
    )
) else (
    echo skia archive already downloaded: %SKIA_ZIP_FILE%
)

if not exist "%SKIA_DEST%" mkdir "%SKIA_DEST%"
tar -xf "%SKIA_ZIP_FILE%" --strip-components=1 -C "%SKIA_DEST%"
if %errorlevel% neq 0 (
    echo extract skia failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)
if not exist "%SKIA_DEST%\BUILD.gn" (
    echo extraction failed: BUILD.gn not found!
    cd /d %CURRENT_DIR%
    exit /b 1
)
echo skia ready at %SKIA_DEST%
cd /d %CURRENT_DIR%
