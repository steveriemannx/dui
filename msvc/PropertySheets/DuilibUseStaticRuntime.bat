@echo off
chcp 65001 >nul
setlocal

set "SCRIPT_DIR=%~dp0"
set "FLAG_FILE=%SCRIPT_DIR%duilib_dll.flag"

echo ==============================================
echo  "Switching to: static runtime library mode (/MT /MTd)"
echo ==============================================

if exist "%FLAG_FILE%" (
    del /f /q "%FLAG_FILE%"
    if exist "%FLAG_FILE%" (
        echo "Failed to delete the file: %FLAG_FILE%"
    ) else (
        echo "Deleted: %FLAG_FILE%"
    )
)
if exist "%FLAG_FILE%" (
    echo "Failed to switch the runtime library mode; please delete the file manually: %FLAG_FILE%!"
) else (
    echo "The runtime library mode has been switched to static runtime libraries (/MT /MTd)!"
)
echo.

if "%1" NEQ "/S" (
    echo "Please close VS and reopen the project."
    echo "This script will exit automatically in 7 seconds..."
    echo.
    timeout /t 7 >nul
)

endlocal
