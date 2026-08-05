echo OFF

:: Force enable SDL
set ENABLE_SDL=0
if "%1" == "-sdl" (
    set ENABLE_SDL=1
)
echo ENABLE_SDL: %ENABLE_SDL%

:: build dui: MinGW-w64 gcc/g++ or clang/clang++
set CURRENT_DIR=%cd%
set SCRIPT_DIR=%~dp0

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

where cmake.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo cmake.exe found at:  
    where cmake.exe
) else (
    echo cmake.exe not found in PATH
    cd /d %CURRENT_DIR%
    exit /b 1
)

set has_gcc=0
set has_clang=0

:: gcc/g++
where gcc >nul 2>&1 && where g++ >nul 2>&1 && set has_gcc=1

:: clang/clang++
where clang >nul 2>&1 && where clang++ >nul 2>&1 && set has_clang=1

if %has_gcc%%has_clang% equ 00 (
    echo  "GCC/G++ (MinGW) not found in PATH"
    echo  "Clang/Clang++ (LLVM) not found in PATH"
    cd /d %CURRENT_DIR%
    exit /b 1
)

where gn >nul 2>&1
if %errorlevel% neq 0 (
    echo gn not found in PATH - install it and re-run this script:
    echo   MSYS2: pacman -S mingw-w64-x86_64-gn
    echo   or build gn from source per https://gn.googlesource.com/gn/+/refs/heads/main/README.md
    cd /d %CURRENT_DIR%
    exit /b 1
)

where ninja >nul 2>&1
if %errorlevel% neq 0 (
    echo ninja not found in PATH - install the mingw-w64 ninja package
    cd /d %CURRENT_DIR%
    exit /b 1
)

if %has_clang% equ 1 (
    where clang
    where clang++
) else (
    where gcc
    where g++
)

cd /d %SCRIPT_DIR%
echo %cd%
if not exist ".\dui\.git" (
    if exist "..\..\dui\.git" (
        cd ..\..\
    )
)
echo %cd%

set retry_delay=10

echo - Cloning dui ...
:retry_clone_dui
if not exist ".\dui\.git" (
    git clone https://github.com/steveriemannx/dui dui
) else (
    git -C ./dui pull
)
if %errorlevel% neq 0 (
    timeout /t %retry_delay% >nul
    goto retry_clone_dui
)

if not exist ".\dui\.git" (
    echo clone dui failed!
    cd /d %CURRENT_DIR%
    exit /b 1
)

@REM Fetch SDL first, then skia (same layout as the CMake build; SDL clone is idempotent)
if %ENABLE_SDL% equ 1 (
echo - Cloning SDL ...
:retry_clone_SDL
    if not exist ".\dui\third_party\SDL3\.git" (
        git clone https://github.com/libsdl-org/SDL.git
    ) else (
        git -C ./dui/third_party/SDL3 pull
    )
    if %errorlevel% neq 0 (
        timeout /t %retry_delay% >nul
        goto retry_clone_SDL
    )
)

@REM Fetch skia: download the dui fork zip (same source as the CMake build; idempotent)
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

echo - Building skia ...
cd dui\third_party\skia

if %has_clang% equ 1 (
    if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
        gn gen out/mingw64-llvm.x64.release --args="target_cpu=\"x64\" cc=\"clang\" cxx=\"clang++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        ninja -C out/mingw64-llvm.x64.release
    ) else (
        gn gen out/mingw64-llvm.x86.release --args="target_cpu=\"x86\" cc=\"clang\" cxx=\"clang++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        ninja -C out/mingw64-llvm.x86.release
    )
) else (
    if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
        gn gen out/mingw64-gcc.x64.release --args="target_cpu=\"x64\" cc=\"gcc\" cxx=\"g++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        ninja -C out/mingw64-gcc.x64.release
    ) else (
        gn gen out/mingw64-gcc.x86.release --args="target_cpu=\"x86\" cc=\"gcc\" cxx=\"g++\" is_trivial_abi=false is_official_build=true skia_use_libwebp_encode=false skia_use_libwebp_decode=false skia_use_libpng_encode=false skia_use_libpng_decode=false skia_use_zlib=false skia_use_libjpeg_turbo_encode=false skia_use_libjpeg_turbo_decode=false skia_enable_fontmgr_win_gdi=false skia_use_icu=false skia_use_expat=false skia_use_xps=false skia_enable_pdf=false skia_use_wuffs=false skia_enable_svg=true skia_use_expat=true skia_use_system_expat=false is_debug=false extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\"]"
        ninja -C out/mingw64-gcc.x86.release
    )
)
cd ..\..\..

if %ENABLE_SDL% equ 1 (
    if not exist ".\dui\third_party\SDL3\lib" (
        echo - Building SDL ...
        if %has_clang% equ 1 (
            SET DUI_CC=clang
            SET DUI_CXX=clang++
            SET DUI_SDL_DIR=sdl3-build.mingw.llvm
        ) else (
            SET DUI_CC=gcc
            SET DUI_CXX=g++
            SET DUI_SDL_DIR=sdl3-build.mingw.gcc
        )

        cmake -S "./dui/third_party/SDL3/" -B ".\dui\scripts\build_temp\%DUI_SDL_DIR%" -DCMAKE_INSTALL_PREFIX="./dui/third_party/SDL3" -G"MinGW Makefiles" -DCMAKE_C_COMPILER=%DUI_CC% -DCMAKE_CXX_COMPILER=%DUI_CXX% -DSDL_SHARED=OFF -DSDL_STATIC=ON -DSDL_TEST_LIBRARY=OFF -DCMAKE_BUILD_TYPE=Release
        cmake --build .\dui\scripts\build_temp\%DUI_SDL_DIR% -j 6
        cmake --install .\dui\scripts\build_temp\%DUI_SDL_DIR%
    ) else (
        echo - SDL3 already installed: .\dui\third_party\SDL3\lib
    )
)

echo - Building dui ...
cd /d %SCRIPT_DIR%
if %has_clang% equ 1 (
    call .\llvm-mingw-w64_build.bat %1
) else (
    call .\gcc-mingw-w64_build.bat %1
)

cd /d %CURRENT_DIR%
echo.
echo finished.
