@echo OFF
@REM Windows build script; compiler: Visual Studio 2022 / Visual Studio 2026
set CURRENT_DIR=%cd%
set SCRIPT_DIR=%~dp0

@REM Make sure cmake is installed on this machine (minimum version: v3.21)
@REM If using Visual Studio 2026, cmake 4.2+ is required
@REM You can run "cmake --version" to check the version (this machine: cmake version 4.3.0)

@for %%i in ("%~dp0..\") do set "bat_parent_dir=%%~fi"
SET DUI_SRC_ROOT_DIR=%bat_parent_dir%
echo DUI_SRC_ROOT_DIR: "%DUI_SRC_ROOT_DIR%"

@for %%i in ("%~dp0..\..\skia") do set "bat_parent_dir=%%~fi"
SET SKIA_SRC_ROOT_DIR=%bat_parent_dir%
echo SKIA_SRC_ROOT_DIR: "%SKIA_SRC_ROOT_DIR%"

@REM # Detect the VS version
if exist "%SCRIPT_DIR%\detect_vs_version.bat" (
    call %SCRIPT_DIR%\detect_vs_version.bat
) else (
    echo detect_vs_version.bat not found in %SCRIPT_DIR%
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

@REM # Set the compiler
SET DUI_COMPILER_ID=msvc

if "%VS_VERSION%"=="vs2026" (
    SET DUI_CMAKE=cmake --fresh -G"Visual Studio 18 2026"
) else (
    SET DUI_CMAKE=cmake --fresh -G"Visual Studio 17 2022"
)

SET DUI_MAKE=cmake --build
SET DUI_BUILD_TYPE=Release
SET DUI_BUILD_PARAM=--config %DUI_BUILD_TYPE%

@REM # Skia library subdirectory, fixed to the LLVM build; comment out this block to use the default rules
SET CPU_ARCH=%VSCMD_ARG_TGT_ARCH%
if "%CPU_ARCH%"=="" (
    SET CPU_ARCH=x64
)
SET DUI_SKIA_LIB_SUBPATH=llvm.%CPU_ARCH%.release
echo "DUI_SKIA_LIB_SUBPATH:%DUI_SKIA_LIB_SUBPATH%"

if not exist "%SKIA_SRC_ROOT_DIR%\out\%DUI_SKIA_LIB_SUBPATH%" (
    echo "Please compile the skia first or run build_dui_all_in_one.bat."
    exit /b 1
)

if "%CPU_ARCH%"=="x86" (
    SET DUI_CMAKE=%DUI_CMAKE% -A Win32
)

@REM # lib directory
SET DUI_LIB_DIR=%DUI_SRC_ROOT_DIR%\lib\
SET target_dir=%DUI_LIB_DIR%
if not exist "%target_dir%" (
    @mkdir "%target_dir%"
)

@REM # Build temporary directory
SET DUI_BUILD_DIR=%DUI_SRC_ROOT_DIR%\scripts\build_temp\%DUI_COMPILER_ID%
SET target_dir=%DUI_SRC_ROOT_DIR%\scripts\build_temp\
if not exist "%target_dir%" (
    @mkdir "%target_dir%"
)
if not exist "%DUI_BUILD_DIR%" (
    @mkdir "%DUI_BUILD_DIR%"
)

@REM # Build third-party libraries
SET DUI_THIRD_PARTY_LIBS=zlib,libpng,cximage,libwebp,libcef\libcef_win
for %%i in (%DUI_THIRD_PARTY_LIBS%) do (
    %DUI_CMAKE% -S "%DUI_SRC_ROOT_DIR%dui\third_party\%%i" -B "%DUI_BUILD_DIR%\%%i" -DCMAKE_BUILD_TYPE=%DUI_BUILD_TYPE%
    %DUI_MAKE% "%DUI_BUILD_DIR%\%%i" %DUI_BUILD_PARAM%
)

@REM #Build dui
%DUI_CMAKE% -S "%DUI_SRC_ROOT_DIR%dui" -B "%DUI_BUILD_DIR%\dui" -DCMAKE_BUILD_TYPE=%DUI_BUILD_TYPE%
%DUI_MAKE% "%DUI_BUILD_DIR%\dui" %DUI_BUILD_PARAM%

@REM #Build each program under examples
SET DUI_PROGRAMS=basic,controls,ColorPicker,DpiAware,chat,layout,ListBox,ListCtrl,MoveControl,MultiLang,render,RichEdit,VirtualListBox,threads,TreeView,cef,CefBrowser,WebView2,WebView2Browser,ChildWindow,XmlPreview
for %%i in (%DUI_PROGRAMS%) do (
    %DUI_CMAKE% -S "%DUI_SRC_ROOT_DIR%examples\%%i" -B "%DUI_BUILD_DIR%\%%i" -DCMAKE_BUILD_TYPE=%DUI_BUILD_TYPE% -DDUI_SKIA_LIB_SUBPATH=%DUI_SKIA_LIB_SUBPATH%
    %DUI_MAKE% "%DUI_BUILD_DIR%\%%i" %DUI_BUILD_PARAM%
)
