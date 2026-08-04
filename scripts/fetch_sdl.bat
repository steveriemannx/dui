echo OFF
:: Fetch the SDL3 source (zip archive) into third_party\SDL3
:: Windows counterpart of scripts/fetch_sdl.sh (requires curl.exe and tar.exe,
:: both bundled with Windows 10 1803+).
::
:: Usage:
::   scripts\fetch_sdl.bat            fetch if not already present (idempotent)
::   scripts\fetch_sdl.bat -f         force re-download and re-extract

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

set SDL_ZIP_URL=https://github.com/libsdl-org/SDL/releases/download/release-3.4.14/SDL3-3.4.14.zip
set SDL_ZIP_FILE=%THIRD_PARTY_DIR%\.download\SDL3-3.4.14.zip
set SDL_DEST=%THIRD_PARTY_DIR%\SDL3

if exist "%SDL_DEST%\CMakeLists.txt" (
    if %FORCE%==0 (
        echo SDL3 already present at %SDL_DEST% ^(use -f to re-extract^)
        cd /d %CURRENT_DIR%
        exit /b 0
    )
)

if not exist "%THIRD_PARTY_DIR%\.download" mkdir "%THIRD_PARTY_DIR%\.download"

if not exist "%SDL_ZIP_FILE%" (
    echo === Downloading: %SDL_ZIP_URL% ===
    curl -L -f -o "%SDL_ZIP_FILE%" --retry 3 --connect-timeout 15 --retry-delay 10 --speed-limit 100 --speed-time 120 "%SDL_ZIP_URL%"
    if %errorlevel% neq 0 (
        echo download failed: %SDL_ZIP_URL%
        cd /d %CURRENT_DIR%
        exit /b 1
    )
) else (
    echo SDL3 archive already downloaded: %SDL_ZIP_FILE%
)

if not exist "%SDL_DEST%" mkdir "%SDL_DEST%"
tar -xf "%SDL_ZIP_FILE%" --strip-components=1 -C "%SDL_DEST%"
if %errorlevel% neq 0 (
    echo extract SDL3 failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)
if not exist "%SDL_DEST%\CMakeLists.txt" (
    echo extraction failed: CMakeLists.txt not found!
    cd /d %CURRENT_DIR%
    exit /b 1
)
echo SDL3 ready at %SDL_DEST%
cd /d %CURRENT_DIR%
