# Project Build Documentation and Scripts

This directory contains the build documentation and the dependency build scripts of the project. Assume the project root directory is the variable `${DUILIB_ROOT}`.

## Files in this directory

| FileName                  | OS      | Compiler                  | Skia (required) | CEF (optional) | WebView2 (optional) | Comment |
| :---                      | :---    | :---                      |:---             |:---            |:---                 |:---     |
| `build_duilib_all_in_one.bat` | Windows | Visual Studio          | Downloaded and built automatically | Supported | Supported | One-click build script; it downloads and builds all sources (including dependencies such as skia)<br>skia is built with the following options: <br>llvm.x64.debug/llvm.x64.release<br>llvm.x86.debug/llvm.x86.release<br>The CEF module must be downloaded manually, see [CEF.md](../docs/CEF.md)|
| `duilib.sln`              | Windows | Visual Studio              | Must be downloaded and built in advance | Supported, can be disabled | Supported, can be disabled | Visual Studio solution file; includes the lib but not the example programs|
| `duilib_no_cef.sln`       | Windows | Visual Studio              | Must be downloaded and built in advance | Not supported | Supported, can be disabled | Visual Studio solution file; includes the lib but not the example programs|
| `examples.sln`            | Windows | Visual Studio              | Must be downloaded and built in advance | Supported | Supported | Visual Studio solution file; includes the lib and the example programs<br>The CEF module must be downloaded manually, see [CEF.md](../docs/CEF.md)|
| `examples_no_cef.sln`     | Windows | Visual Studio              | Must be downloaded and built in advance | Not supported | Supported, can be disabled | Visual Studio solution file; includes the lib and the example programs<br>The CEF module must be downloaded manually, see [CEF.md](../docs/CEF.md)|
| `msvc_build.bat`          | Windows | cmake/MSVC                 | Must be downloaded and built in advance | Supported | Supported | Command-line build script; builds with cmake using the MSVC compiler|
| `gcc-mingw-w64_build.bat` | Windows | MinGW-w64 gcc/g++          | Must be downloaded and built in advance | Not supported | Supported | MinGW-w64 build script; compiler is gcc/g++|
| `llvm-mingw-w64_build.bat`| Windows | MinGW-w64 clang/clang++    | Must be downloaded and built in advance | Not supported | Supported | MinGW-w64 build script; compiler is clang/clang++|
| `build_duilib_all_in_one.sh` | Windows | MSYS2 gcc/g++ <br>MSYS2 clang/clang++ | Must be downloaded and built in advance | Not supported | Supported | One-click build script; downloads and builds all sources (including dependencies such as skia); compiler is gcc/g++ or clang/clang++|
| `msys2_build.sh`          | Windows | MSYS2 gcc/g++ <br>MSYS2 clang/clang++ | Must be downloaded and built in advance | Not supported | Supported | Build script for MSYS2; compiler is gcc/g++ or clang/clang++|
| `build_duilib_all_in_one.sh` | Linux | gcc/g++ <br> clang/clang++  | Downloaded and built automatically | Supported, auto-downloaded | Not supported | One-click build script; downloads and builds all sources (including dependencies such as skia); compiler is gcc/g++ or clang/clang++|
| `linux_build.sh`          | Linux | gcc/g++ <br> clang/clang++  | Must be downloaded and built in advance | Supported | Not supported | Build script for Linux; compiler is gcc/g++ or clang/clang++|
| `build_duilib_all_in_one.sh` | MacOS | clang/clang++            | Downloaded and built automatically | Supported, auto-downloaded | Not supported | One-click build script; downloads and builds all sources (including dependencies such as skia); compiler is clang/clang++|
| `macos_build.sh`          | MacOS | clang/clang++              | Must be downloaded and built in advance | Supported | Not supported | Build script for macOS; compiler is clang/clang++|
| `build_duilib_all_in_one.sh` | FreeBSD | clang/clang++          | Downloaded and built automatically | Not supported | Not supported | One-click build script; downloads and builds all sources (including dependencies such as skia); compiler is clang/clang++|
| `freebsd_build.sh`        | FreeBSD | clang/clang++             | Must be downloaded and built in advance | Not supported | Not supported | Build script for FreeBSD; compiler is clang/clang++|

## Build output files

1. lib files: `${DUILIB_ROOT}/lib`
2. bin files: `${DUILIB_ROOT}/bin`
3. Temporary files (can be cleaned up)    
（1）`${DUILIB_ROOT}/scripts/build_temp`: temporary build directory, can be cleaned up    
（2）`${DUILIB_ROOT}/scripts/.vs`: hidden directory; Visual Studio cache directory, occupies a lot of space and keeps growing; can be cleaned periodically.    
（3）`${DUILIB_ROOT}/cef_cache`: network cache directory of the libCEF module at runtime; can be deleted.    
（4）`${DUILIB_ROOT}/webview2_cache`: network cache directory of the WebView2 module at runtime; can be deleted.    

## Files needed for program release

1. The `${DUILIB_ROOT}/bin/resources` directory: the resource files (XML files, image resources, etc.)    
（1）`${DUILIB_ROOT}/bin/resources/fonts`: font directory; if no custom fonts are used, this directory can be deleted.    
（2）`${DUILIB_ROOT}/bin/resources/lang`: multi-language text files; if multi-language support is not used, this directory can be deleted.    
（3）`${DUILIB_ROOT}/bin/resources/themes/default`: the theme resource directory; only the `public` directory and the `global.xml` file need to be kept, the other directories can be deleted.    
2. On Windows, to pack the resources into a zip and embed it in the exe:    
   Pack the `bin/resources` directory prepared in step 1 into a `resources.zip` file, replace the current `bin/resources.zip`, then rebuild the exe.    
   For how to use `resources.zip`, refer to the `examples/basic` example program.    
3. If the CEF module is used: copy the files in CEF's Release and Resources directories to the `bin\libcef_win` or `bin\libcef_win_109` directory.    
   For details, see the CEF usage documentation: [docs/CEF.md](../docs/CEF.md).
