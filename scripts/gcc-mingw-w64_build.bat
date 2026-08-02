@echo OFF
@REM Windows build script; compiler: MinGW-w64
@REM Make sure the MinGW-w64 build environment is already added to the PATH variable; example:
@REM Assume the MinGW-w64 binaries are located at: C:\mingw64\x86_64-15.1.0-release-win32-seh-ucrt-rt_v12-rev0\mingw64\bin\
@REM Add the MinGW-w64 build environment to the PATH variable with the following command:
@REM #SET PATH=%PATH%;C:\mingw64\x86_64-15.1.0-release-win32-seh-ucrt-rt_v12-rev0\mingw64\bin\
@REM You can run "g++ -v" to check the version

@REM Make sure cmake is installed on this machine (minimum version: v3.21)
@REM You can run "cmake --version" to check the version (this machine: cmake version 4.0.2)

:: Force enable SDL
if "%1" == "-sdl" (   
    set SDL_PARAM=-DDUILIB_ENABLE_SDL=ON
) else (
    set "SDL_PARAM="
)
echo SDL_PARAM: %SDL_PARAM%

@for %%i in ("%~dp0..\") do set "bat_parent_dir=%%~fi"
SET DUILIB_SRC_ROOT_DIR=%bat_parent_dir%
echo DUILIB_SRC_ROOT_DIR: "%DUILIB_SRC_ROOT_DIR%"

@for %%i in ("%~dp0..\..\skia") do set "bat_parent_dir=%%~fi"
SET SKIA_SRC_ROOT_DIR=%bat_parent_dir%
echo SKIA_SRC_ROOT_DIR: "%SKIA_SRC_ROOT_DIR%"

echo Checking the necessary software
where cmake.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo cmake.exe found at:  
    where cmake.exe
) else (
    echo cmake.exe not found in PATH
    exit /b 1
)

where g++.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo g++ found at:  
    where g++
) else (
    echo g++ not found in PATH
    exit /b 1
)

where gcc.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo gcc.exe found at:  
    where gcc.exe
) else (
    echo gcc.exe not found in PATH
    exit /b 1
)

@REM # Set the compiler
SET DUILIB_CC=gcc
SET DUILIB_CXX=g++
SET DUILIB_COMPILER_ID=gcc-mingw-w64

SET DUILIB_CMAKE=cmake --fresh -G"MinGW Makefiles" -DCMAKE_C_COMPILER=%DUILIB_CC% -DCMAKE_CXX_COMPILER=%DUILIB_CXX%
SET DUILIB_MAKE=cmake --build
SET DUILIB_BUILD_TYPE=Release
SET DUILIB_MAKE_THREADS=-j 6

if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    SET DUILIB_SKIA_LIB_SUBPATH=mingw64-gcc.x64.release
) else (
    SET DUILIB_SKIA_LIB_SUBPATH=mingw64-gcc.x86.release
)
echo "DUILIB_SKIA_LIB_SUBPATH:%DUILIB_SKIA_LIB_SUBPATH%"

if not exist "%SKIA_SRC_ROOT_DIR%\out\%DUILIB_SKIA_LIB_SUBPATH%" (
    echo "Please compile the skia first or run build_duilib_all_in_one_mingw-w64.bat."
    exit /b 1
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
SET DUILIB_THIRD_PARTY_LIBS=zlib,libpng,cximage,libwebp
for %%i in (%DUILIB_THIRD_PARTY_LIBS%) do (
    %DUILIB_CMAKE% -S "%DUILIB_SRC_ROOT_DIR%duilib\third_party\%%i" -B "%DUILIB_BUILD_DIR%\%%i" -DCMAKE_BUILD_TYPE=%DUILIB_BUILD_TYPE%
    %DUILIB_MAKE% "%DUILIB_BUILD_DIR%\%%i" %DUILIB_MAKE_THREADS%
)

@REM #Build duilib
%DUILIB_CMAKE% -S "%DUILIB_SRC_ROOT_DIR%duilib" -B "%DUILIB_BUILD_DIR%\duilib" -DCMAKE_BUILD_TYPE=%DUILIB_BUILD_TYPE% %SDL_PARAM%
%DUILIB_MAKE% "%DUILIB_BUILD_DIR%\duilib" %DUILIB_MAKE_THREADS%

@REM #Build each program under examples
SET DUILIB_PROGRAMS=basic,controls,ColorPicker,DpiAware,chat,layout,ListBox,ListCtrl,MoveControl,MultiLang,render,RichEdit,VirtualListBox,threads,TreeView,WebView2,WebView2Browser,ChildWindow,XmlPreview
for %%i in (%DUILIB_PROGRAMS%) do (
    %DUILIB_CMAKE% -S "%DUILIB_SRC_ROOT_DIR%examples\%%i" -B "%DUILIB_BUILD_DIR%\%%i" -DCMAKE_BUILD_TYPE=%DUILIB_BUILD_TYPE% %SDL_PARAM%
    %DUILIB_MAKE% "%DUILIB_BUILD_DIR%\%%i" %DUILIB_MAKE_THREADS%
)

@REM # CEF is not supported, so these programs are not included: cef, CefBrowser
