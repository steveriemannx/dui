@echo off
chcp 65001 >nul
setlocal

set "SCRIPT_DIR=%~dp0"
set "FLAG_FILE=%SCRIPT_DIR%dui_dll.flag"

echo ==============================================
echo  "Switching to: dynamic runtime library mode (/MD /MDd)"
echo ==============================================

echo. > "%FLAG_FILE%"
if exist "%FLAG_FILE%" (
    echo "Created: %FLAG_FILE%"
    echo "The runtime library mode has been switched to dynamic runtime libraries (/MD /MDd)!"
) else (
    echo "Failed to switch the runtime library mode; please create the file manually: %FLAG_FILE%!"
)
echo.

if "%1" NEQ "/S" (
    echo "Please close VS and reopen the project."
    echo "This script will exit automatically in 7 seconds..."
    echo.
    timeout /t 7 >nul
)

endlocal
