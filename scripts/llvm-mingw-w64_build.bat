@echo OFF
@REM Windows build script; compiler: MinGW-w64
@REM Make sure the MinGW-w64 build environment is already added to the PATH variable; example:
@REM Assume the MinGW-w64 binaries are located at: C:\mingw64\llvm-mingw-20250430-ucrt-x86_64\bin
@REM Add the MinGW-w64 build environment to the PATH variable with the following command:
@REM #SET PATH=%PATH%;C:\mingw64\llvm-mingw-20250430-ucrt-x86_64\bin
@REM You can run "g++ -v" to check the version

@REM Make sure cmake is installed on this machine (minimum version: v3.21)
@REM You can run "cmake --version" to check the version (this machine: cmake version 4.0.2)

:: Force enable SDL
if "%1" == "-sdl" (   
    set SDL_PARAM=-DDUI_ENABLE_SDL=ON
) else (
    set "SDL_PARAM="
)
echo SDL_PARAM: %SDL_PARAM%

@for %%i in ("%~dp0..\") do set "bat_parent_dir=%%~fi"
@REM Strip the trailing backslash: -S "%DUI_SRC_ROOT_DIR%" would otherwise end
@REM with \" which the CRT parses as an escaped quote, swallowing the rest of
@REM the command line into the -S argument.
SET DUI_SRC_ROOT_DIR=%bat_parent_dir:~0,-1%
echo DUI_SRC_ROOT_DIR: "%DUI_SRC_ROOT_DIR%"

@for %%i in ("%~dp0..\third_party\skia") do set "bat_parent_dir=%%~fi"
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

where clang++.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo clang++ found at:  
    where clang++
) else (
    echo clang++ not found in PATH
    exit /b 1
)

where clang.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo clang.exe found at:  
    where clang.exe
) else (
    echo clang.exe not found in PATH
    exit /b 1
)

@REM # Set the compiler
SET DUI_CC=clang
SET DUI_CXX=clang++
SET DUI_COMPILER_ID=llvm-mingw-w64

@REM Arguments:
@REM   --fresh       clean and re-configure (incremental by default)
@REM   --standalone  build each example as an independent CMake project (legacy mode); default uses top-level CMake management
SET DUI_CMAKE_REFRESH=
echo %* | findstr /C:"--fresh" >nul && set DUI_CMAKE_REFRESH=--fresh
SET STANDALONE=false
echo %* | findstr /C:"--standalone" >nul && set STANDALONE=true

SET DUI_CMAKE=cmake %DUI_CMAKE_REFRESH% -G"MinGW Makefiles" -DCMAKE_C_COMPILER=%DUI_CC% -DCMAKE_CXX_COMPILER=%DUI_CXX%
SET DUI_MAKE=cmake --build
SET DUI_BUILD_TYPE=Release
@REM Parallel build jobs: 3/4 of the CPU cores (leave the rest for the system)
set /a DUI_JOBS=%NUMBER_OF_PROCESSORS% * 3 / 4
if %DUI_JOBS% lss 1 set DUI_JOBS=1
SET DUI_MAKE_THREADS=-j %DUI_JOBS%

if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    SET DUI_SKIA_LIB_SUBPATH=mingw64-llvm.x64.release
) else (
    SET DUI_SKIA_LIB_SUBPATH=mingw64-llvm.x86.release
)
echo "DUI_SKIA_LIB_SUBPATH:%DUI_SKIA_LIB_SUBPATH%"

if not exist "%SKIA_SRC_ROOT_DIR%\out\%DUI_SKIA_LIB_SUBPATH%" (
    echo "Please compile the skia first or run build_dui_all_in_one_mingw-w64.bat."
    exit /b 1
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

@REM ============================================================
@REM Top-level CMake build (default): configure the whole repository at once
@REM NOTE: DUI_TOP_BUILD_DIR is set OUTSIDE the if block - cmd expands %VAR%
@REM at parse time for the whole block, so an in-block SET would be empty.
@REM ============================================================
SET DUI_TOP_BUILD_DIR=%DUI_BUILD_DIR%\top
if "%STANDALONE%"=="false" (
    if not exist "%DUI_TOP_BUILD_DIR%" mkdir "%DUI_TOP_BUILD_DIR%"

    %DUI_CMAKE% -S "%DUI_SRC_ROOT_DIR%" -B "%DUI_TOP_BUILD_DIR%" -DCMAKE_BUILD_TYPE=%DUI_BUILD_TYPE% -DDUI_SKIA_LIB_SUBPATH="%DUI_SKIA_LIB_SUBPATH%" %SDL_PARAM%
    if %errorlevel% neq 0 (
        echo Top-level cmake configure failed.
        exit /b 1
    )

    %DUI_MAKE% "%DUI_TOP_BUILD_DIR%" %DUI_MAKE_THREADS%
    exit /b %errorlevel%
)

@REM ============================================================
@REM --standalone: build each example as an independent CMake project (legacy mode)
@REM ============================================================
@REM # Build third-party libraries
SET DUI_THIRD_PARTY_LIBS=zlib,libpng,cximage,libwebp
for %%i in (%DUI_THIRD_PARTY_LIBS%) do (
    %DUI_CMAKE% -S "%DUI_SRC_ROOT_DIR%third_party\%%i" -B "%DUI_BUILD_DIR%\%%i" -DCMAKE_BUILD_TYPE=%DUI_BUILD_TYPE%
    %DUI_MAKE% "%DUI_BUILD_DIR%\%%i" %DUI_MAKE_THREADS%
)

@REM #Build dui
%DUI_CMAKE% -S "%DUI_SRC_ROOT_DIR%src" -B "%DUI_BUILD_DIR%\dui" -DCMAKE_BUILD_TYPE=%DUI_BUILD_TYPE% %SDL_PARAM%
%DUI_MAKE% "%DUI_BUILD_DIR%\dui" %DUI_MAKE_THREADS%

@REM #Build each program under examples
SET DUI_PROGRAMS=basic,controls,ColorPicker,DpiAware,chat,layout,ListBox,ListCtrl,MoveControl,MultiLang,render,RichEdit,VirtualListBox,threads,TreeView,WebView2,WebView2Browser,ChildWindow,XmlPreview
for %%i in (%DUI_PROGRAMS%) do (
    %DUI_CMAKE% -S "%DUI_SRC_ROOT_DIR%examples\%%i" -B "%DUI_BUILD_DIR%\%%i" -DCMAKE_BUILD_TYPE=%DUI_BUILD_TYPE% %SDL_PARAM%
    %DUI_MAKE% "%DUI_BUILD_DIR%\%%i" %DUI_MAKE_THREADS%
)

@REM # CEF is not supported, so these programs are not included: cef, CefBrowser
