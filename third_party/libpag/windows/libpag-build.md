# How to Build libpag
## Windows platform
### I. Download the source code (use the main branch code; the current Release code has bugs and the build flow could not be completed successfully)
```
# Current directory: %PAG_ROOT%
git clone https://github.com/Tencent/libpag.git
# SHA-1 at the time: 3ebe074de5eb990a0448ae236d75cf6e90f96107
```

### II. Development environment build (requires Visual Studio 2022):
1. First make sure you have the latest version of node.js installed, then use npm to install the depsync command line tool:
   `npm install -g depsync`
2. Run the depsync command in the source root to sync the third-party dependency repositories:
   `depsync`
3. We plan to build libpag.dll, Release only, but this DLL uses the static runtime library (MT). The files that need to be modified before building are:
（1）`libpag\third_party\vendor_tools\win.msvc.cmake`
     Change: remove the DLL suffix
（2）`libpag\third_party\tgfx\third_party\vendor_tools\win.msvc.cmake`
     Change: remove the DLL suffix
（3）`third_party\tgfx\third_party\skcms\CMakeLists.txt`
     Change: change the cmake_minimum_required version to 3.18: `cmake_minimum_required(VERSION 3.18)`
（4）`third_party\tgfx\third_party\pathkit\CMakeLists.txt`
     Change: change the cmake_minimum_required version to 3.18: `cmake_minimum_required(VERSION 3.18)`
（5）`third_party\tgfx\third_party\highway\CMakeLists.txt`
     Change: change the cmake_minimum_required version to 3.18: `cmake_minimum_required(VERSION 3.18)`

### III. Sync the interface header files
Copy `libpag\include` to the directory `duilib\third_party\libpag\windows\`, overwriting the existing header files, to keep the interface consistent.

### IV. Build with cmake on the command line
1. x64 build
（1）First enter the VS command prompt: "x64 Native Tools Command Prompt for VS 2022"
     This step is required; otherwise cmake will report an error
（2）Run the following command in the VS command prompt to build:
```
# Current directory: %PAG_ROOT%
cmake --fresh -G "Visual Studio 17 2022" -A x64 -S ./libpag -B ./build-libpag-x64 -DCMAKE_PROJECT_INCLUDE=./third_party/vendor_tools/win.msvc.cmake -DPAG_USE_QT=OFF -DPAG_USE_PNG_ENCODE=OFF -DPAG_USE_JPEG_ENCODE=OFF -DPAG_USE_WEBP_ENCODE=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.18 -DPAG_USE_HARFBUZZ=OFF -DPAG_USE_RTTR=OFF
cmake --build ./build-libpag-x64 --config Release
```
After the build completes:
（3）Copy `build-libpag-x64\Release\libpag.lib` to the directory `duilib\third_party\libpag\windows\lib-vc-x64`
（4）Copy `build-libpag-x64\Release\libpag.dll` to the directory `bin`

2. Win32 build
（1）First enter the VS command prompt: "x86 Native Tools Command Prompt for VS 2022"
     This step is required; otherwise cmake will report an error
（2）Run the following command in the VS command prompt to build:
```
# Current directory: %PAG_ROOT%
cmake --fresh -G "Visual Studio 17 2022" -A Win32 -S ./libpag -B ./build-libpag-Win32 -DCMAKE_PROJECT_INCLUDE=./third_party/vendor_tools/win.msvc.cmake -DPAG_USE_QT=OFF -DPAG_USE_PNG_ENCODE=OFF -DPAG_USE_JPEG_ENCODE=OFF -DPAG_USE_WEBP_ENCODE=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.18 -DPAG_USE_HARFBUZZ=OFF -DPAG_USE_RTTR=OFF
cmake --build ./build-libpag-Win32 --config Release
```
After the build completes:
（3）Copy `build-libpag-Win32\Release\libpag.lib` to the directory `duilib\third_party\libpag\windows\lib-vc-x86`
（4）Copy `build-libpag-Win32\Release\libpag.dll` to the directory `bin`

3. View the local configuration (for reference)
```
cmake -B ./build-libpag-x64 -LAH
cmake -B ./build-libpag-Win32 -LAH
```

4. Official build reference
```
https://pag.io/docs/en/sdk-desktop.html
```
