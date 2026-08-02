@echo OFF
@REM Windows build script; compiler: Visual Studio 2022 / Visual Studio 2026
set CURRENT_DIR=%cd%
set SCRIPT_DIR=%~dp0

@REM Make sure cmake is installed on this machine (minimum version: v3.21)
@REM If using Visual Studio 2026, cmake 4.2+ is required
@REM You can run "cmake --version" to check the version (this machine: cmake version 4.3.0)

@for %%i in ("%~dp0..\") do set "bat_parent_dir=%%~fi"
SET DUILIB_SRC_ROOT_DIR=%bat_parent_dir%
echo DUILIB_SRC_ROOT_DIR: "%DUILIB_SRC_ROOT_DIR%"

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
SET DUILIB_COMPILER_ID=msvc

if "%VS_VERSION%"=="vs2026" (
    SET DUILIB_CMAKE=cmake --fresh -G"Visual Studio 18 2026"
) else (
    SET DUILIB_CMAKE=cmake --fresh -G"Visual Studio 17 2022"
)

SET DUILIB_MAKE=cmake --build
SET DUILIB_BUILD_TYPE=Release
SET DUILIB_BUILD_PARAM=--config %DUILIB_BUILD_TYPE%

@REM # Skia library subdirectory, fixed to the LLVM build; comment out this block to use the default rules
SET CPU_ARCH=%VSCMD_ARG_TGT_ARCH%
if "%CPU_ARCH%"=="" (
    SET CPU_ARCH=x64
)
SET DUILIB_SKIA_LIB_SUBPATH=llvm.%CPU_ARCH%.release
echo "DUILIB_SKIA_LIB_SUBPATH:%DUILIB_SKIA_LIB_SUBPATH%"

if not exist "%SKIA_SRC_ROOT_DIR%\out\%DUILIB_SKIA_LIB_SUBPATH%" (
    echo "Please compile the skia first or run build_duilib_all_in_one.bat."
    exit /b 1
)

if "%CPU_ARCH%"=="x86" (
    SET DUILIB_CMAKE=%DUILIB_CMAKE% -A Win32
)

@REM # lib directory
SET DUILIB_LIB_DIR=%DUILIB_SRC_ROOT_DIR%\lib\
SET target_dir=%DUILIB_LIB_DIR%
if not exist "%target_dir%" (
    @mkdir "%target_dir%"
)

@REM # Build temporary directory
SET DUILIB_BUILD_DIR=%DUILIB_SRC_ROOT_DIR%\scripts\build_temp\%DUILIB_COMPILER_ID%
SET target_dir=%DUILIB_SRC_ROOT_DIR%\scripts\build_temp\
if not exist "%target_dir%" (
    @mkdir "%target_dir%"
)
if not exist "%DUILIB_BUILD_DIR%" (
    @mkdir "%DUILIB_BUILD_DIR%"
)

@REM # Build third-party libraries
SET DUILIB_THIRD_PARTY_LIBS=zlib,libpng,cximage,libwebp,libcef\libcef_win
for %%i in (%DUILIB_THIRD_PARTY_LIBS%) do (
    %DUILIB_CMAKE% -S "%DUILIB_SRC_ROOT_DIR%duilib\third_party\%%i" -B "%DUILIB_BUILD_DIR%\%%i" -DCMAKE_BUILD_TYPE=%DUILIB_BUILD_TYPE%
    %DUILIB_MAKE% "%DUILIB_BUILD_DIR%\%%i" %DUILIB_BUILD_PARAM%
)

@REM #Build duilib
%DUILIB_CMAKE% -S "%DUILIB_SRC_ROOT_DIR%duilib" -B "%DUILIB_BUILD_DIR%\duilib" -DCMAKE_BUILD_TYPE=%DUILIB_BUILD_TYPE%
%DUILIB_MAKE% "%DUILIB_BUILD_DIR%\duilib" %DUILIB_BUILD_PARAM%

@REM #Build each program under examples
SET DUILIB_PROGRAMS=basic,controls,ColorPicker,DpiAware,chat,layout,ListBox,ListCtrl,MoveControl,MultiLang,render,RichEdit,VirtualListBox,threads,TreeView,cef,CefBrowser,WebView2,WebView2Browser,ChildWindow,XmlPreview
for %%i in (%DUILIB_PROGRAMS%) do (
    %DUILIB_CMAKE% -S "%DUILIB_SRC_ROOT_DIR%examples\%%i" -B "%DUILIB_BUILD_DIR%\%%i" -DCMAKE_BUILD_TYPE=%DUILIB_BUILD_TYPE% -DDUILIB_SKIA_LIB_SUBPATH=%DUILIB_SKIA_LIB_SUBPATH%
    %DUILIB_MAKE% "%DUILIB_BUILD_DIR%\%%i" %DUILIB_BUILD_PARAM%
)
