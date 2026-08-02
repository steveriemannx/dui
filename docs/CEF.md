# CEF Control (CefControl)    
nim_duilib's CEF control (CefControl) is an integrated wrapper around libcef, encapsulating the functionality of the CEF component into a duilib control so that web browsing can be integrated with the UI library. libcef (Chromium Embedded Framework, abbreviated as CEF) is an open-source framework that allows developers to embed Chromium (the open-source foundation of the Google Chrome browser) browser functionality into their applications. With libcef, developers can integrate web page rendering, JavaScript execution and HTML5 support into their own applications without requiring users to install a separate browser.  

## 1. Core functionality and architecture of libcef
### Function definition
libcef.dll/libcef.so is the core dynamic link library of CEF, providing the browser kernel functionality of Chromium, including:    
* Web page rendering: supports modern web technologies such as HTML5 and CSS3
* JavaScript execution: enables interaction with web page scripts
* Network communication: handles HTTP requests, responses and resource loading
* Multi-process architecture: manages rendering, plug-ins and other modules through independent processes to improve stability
### System architecture
* libcef.dll/libcef.so sits between the application and the Chromium engine, interacting through interface calls. For example, when the application loads a web page through libcef.dll/libcef.so, Chromium's network module and rendering module are responsible for content fetching and display respectively.
* nim_duilib wraps the basic functionality of the libcef.dll/libcef.so module through the CEF control (CefControl), integrating web pages and the UI into one.

## 2. Classes related to the CEF control (CefControl)
| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| Interface class | [duilib/duilib_cef.h](../duilib/duilib_cef.h) | The interface class of the CEF control; applications need to include this header file: `#include "duilib/duilib_cef.h"` |
| CefManager | [duilib/CEFControl/CefManager.h](../duilib/CEFControl/CefManager.h) | CEF control manager, responsible for the initialization and uninitialization of the CEF module |
| CefControl | [duilib/CEFControl/CefControl.h](../duilib/CEFControl/CefControl.h) | CEF control interface, providing basic web browsing functionality and event acceptance |
| CefControlEvent | [duilib/CEFControl/CefControlEvent.h](../duilib/CEFControl/CefControlEvent.h) | Event reception interface for web browsing of the CEF control |
| CefControlNative | [duilib/CEFControl/CefControlNative.h](../duilib/CEFControl/CefControlNative.h) | Wrapper of the windowed mode of the CEF control |
| CefControlOffScreen | [duilib/CEFControl/CefControlOffScreen.h](../duilib/CEFControl/CefControlOffScreen.h) | Wrapper of the off-screen rendering mode of the CEF control |

## 3. Example programs related to the CEF control (CefControl)
| Example Program | Description |
| :---     | :--- |
| examples\cef           | A simple example of using the CEF control; this program uses the windowed mode of the CEF control |
| examples\CefBrowser    | An example of using the CEF control as a multi-tab browser; this program uses the off-screen rendering mode of the CEF control|


### In the following documentation, the source root directory of nim_duilib is assumed to be the variable: `${NIM_DUILIB_ROOT}`.   
## 4. Usage of the CEF control (CefControl) (Windows platform)

### 1. Downloading the latest libcef binaries and resource files (you need to download them yourself)
Place the libcef binaries and resource files (libcef.dll, etc.) in the following directories:    
Put the x64 version in the `${NIM_DUILIB_ROOT}\bin\libcef_win\x64` directory,    
and put the Win32 version in the `${NIM_DUILIB_ROOT}\bin\libcef_win\Win32` directory    
* Download link for the 64-bit version: [11/11/2025 - 142.0.10+g29548e2+chromium-142.0.7444.135 / Chromium 142.0.7444.135](https://cef-builds.spotifycdn.com/cef_binary_142.0.10%2Bg29548e2%2Bchromium-142.0.7444.135_windows64.tar.bz2)
* Download link for the 32-bit version: [11/11/2025 - 142.0.10+g29548e2+chromium-142.0.7444.135 / Chromium 142.0.7444.135](https://cef-builds.spotifycdn.com/cef_binary_142.0.10%2Bg29548e2%2Bchromium-142.0.7444.135_windows32.tar.bz2)    
  After downloading the archive, extract it.    
  For the 64-bit version, copy the files in the Release directory and the files in the Resources directory to the `${NIM_DUILIB_ROOT}\bin\libcef_win\x64` directory,    
  and for the 32-bit version, copy the files in the Release directory and the files in the Resources directory to the `${NIM_DUILIB_ROOT}\bin\libcef_win\Win32` directory.

### 2. Downloading the libcef 109 binaries and resource files (you need to download them yourself)
Place the libcef 109 binaries and resource files (libcef.dll, etc.) in the following directories:    
Put the x64 version in the `${NIM_DUILIB_ROOT}\bin\libcef_win_109\x64` directory,    
and put the Win32 version in the `${NIM_DUILIB_ROOT}\bin\libcef_win_109\Win32` directory    
* Download link for the 64-bit version: [02/03/2023 - 109.1.18+gf1c41e4+chromium-109.0.5414.120 / Chromium 109.0.5414.120](https://cef-builds.spotifycdn.com/cef_binary_109.1.18%2Bgf1c41e4%2Bchromium-109.0.5414.120_windows64.tar.bz2)
* Download link for the 32-bit version: [01/27/2023 - 109.1.18+gf1c41e4+chromium-109.0.5414.120 / Chromium 109.0.5414.120](https://cef-builds.spotifycdn.com/cef_binary_109.1.18%2Bgf1c41e4%2Bchromium-109.0.5414.120_windows32.tar.bz2)    
  After downloading the archive, extract it.    
  For the 64-bit version, copy the files in the Release directory and the files in the Resources directory to the `${NIM_DUILIB_ROOT}\bin\libcef_win_109\x64` directory,    
  and for the 32-bit version, copy the files in the Release directory and the files in the Resources directory to the `${NIM_DUILIB_ROOT}\bin\libcef_win_109\Win32` directory.    

### 3. Directory structure of the libcef binaries and resource files
The libcef binaries and resource files must be placed in the designated directory under bin for libcef.dll to be loaded properly.    
For example, for the latest version of CEF, the 32-bit files must be placed in the `${NIM_DUILIB_ROOT}\bin\libcef_win\Win32` directory and the 64-bit files in the `${NIM_DUILIB_ROOT}\bin\libcef_win\x64` directory.    
The basic directory structure of the libcef binaries and resource files is (using the 64-bit version as an example):
```
chrome_elf.dll
d3dcompiler_47.dll
dxcompiler.dll
dxil.dll
libcef.dll
libEGL.dll
libGLESv2.dll
vulkan-1.dll
vk_swiftshader.dll
v8_context_snapshot.bin
vk_swiftshader_icd.json
chrome_100_percent.pak
chrome_200_percent.pak
icudtl.dat
resources.pak
locales (directory containing language packs such as zh-CN.pak and en-US.pak)
```

### 4. Using libcef 109 (for VC projects using the `CEFSettings.props` property file)
Open the `${NIM_DUILIB_ROOT}\msvc\PropertySheets\CEFSettings.props` file with VS, change the LibCefVersion109 property value to `true`, and recompile the code.    
The effect can be seen in the `${NIM_DUILIB_ROOT}\examples\cef` and `${NIM_DUILIB_ROOT}\examples\CefBrowser` projects (visit a website that shows the UA to confirm that the UA is displayed).

### 5. Using the latest libcef version (for VC projects using the `CEFSettings.props` property file)
Open the `${NIM_DUILIB_ROOT}\msvc\PropertySheets\CEFSettings.props` file with VS, change the LibCefVersion109 property value to `false`, and recompile the code.    
The effect can be seen in the `${NIM_DUILIB_ROOT}\examples\cef` and `${NIM_DUILIB_ROOT}\examples\CefBrowser` projects (visit a website that shows the UA to confirm that the UA is displayed).

### 6. How to manually set the libcef-related properties in your own project (for VC projects that do not use the `CEFSettings.props` property file)
#### (1) Support for the new libcef version
The newer versions of libcef (higher than 109) have more complete functionality. They support Windows 10 and above (Win10/Win11, etc.) and do not support operating systems lower than Win10, such as Win7.    
The basic steps are as follows (all directories are written as subdirectories relative to the nim_duilib root; the actual settings can be flexibly adjusted according to your own project structure):    
1. Modify the header include paths of the VC project: add `duilib\third_party\libcef\libcef_win` (entry: VS project properties -> C/C++ -> General -> Additional Include Directories)    
2. Modify the library include paths of the VC project: add `duilib\third_party\libcef\libcef_win\lib\$(Platform)` (entry: VS project properties -> Linker -> General -> Additional Library Directories)    
3. Modify the library dependencies of the VC project to include the following library files (entry: VS project properties -> Linker -> Input -> Additional Dependencies):    
* For the Debug build, add: `libcef.lib;libcef_dll_wrapper_d.lib`    
* For the Release build, add: `libcef.lib;libcef_dll_wrapper.lib`  
4. In the VC project, set libcef.dll to be delay-loaded by adding `libcef.dll` (entry: VS project properties -> Linker -> Input -> Delay Loaded Dlls)    
5. Place the libcef binaries and resource files (libcef.dll, etc.) in the following directories:    
* Put the x64 version in the `${NIM_DUILIB_ROOT}\bin\libcef_win\x64` directory
* Put the Win32 version in the `${NIM_DUILIB_ROOT}\bin\libcef_win\Win32` directory

#### (2) Support for libcef 109
libcef 109 supports Windows 7 and above (Win7/Win10/Win11, etc.) and does not support older operating systems such as Windows XP.    
The basic steps are as follows (all directories are written as subdirectories relative to the nim_duilib root; the actual settings can be flexibly adjusted according to your own project structure):    
1. Modify the header include paths of the VC project: add `duilib\third_party\libcef\libcef_win_109` (entry: VS project properties -> C/C++ -> General -> Additional Include Directories)    
2. Modify the library include paths of the VC project: add `duilib\third_party\libcef\libcef_win_109\lib\$(Platform)` (entry: VS project properties -> Linker -> General -> Additional Library Directories)    
3. Modify the library dependencies of the VC project to include the following library files (entry: VS project properties -> Linker -> Input -> Additional Dependencies):    
* For the Debug build, add: `libcef.lib;libcef_dll_wrapper_109_d.lib`    
* For the Release build, add: `libcef.lib;libcef_dll_wrapper_109.lib`  
4. In the VC project, set libcef.dll to be delay-loaded by adding `libcef.dll` (entry: VS project properties -> Linker -> Input -> Delay Loaded Dlls)    
5. Place the libcef binaries and resource files (libcef.dll, etc.) in the following directories:    
* Put the x64 version in the `${NIM_DUILIB_ROOT}\bin\libcef_win_109\x64` directory
* Put the Win32 version in the `${NIM_DUILIB_ROOT}\bin\libcef_win_109\Win32` directory

## 5. Usage of the CEF control (CefControl) (Linux platform)
The basic steps are as follows (all directories are written as subdirectories relative to the nim_duilib root `${NIM_DUILIB_ROOT}`; the actual settings can be flexibly adjusted according to your own project structure):    
### 1. Downloading the latest libcef binaries and resource files (you need to download them yourself)
* Download link for the x64 version:  [11/11/2025 - 142.0.10+g29548e2+chromium-142.0.7444.135 / Chromium 142.0.7444.135](https://cef-builds.spotifycdn.com/cef_binary_142.0.10%2Bg29548e2%2Bchromium-142.0.7444.135_linux64.tar.bz2)    
* Download link for the ARM64 version: [11/11/2025 - 142.0.10+g29548e2+chromium-142.0.7444.135 / Chromium 142.0.7444.135](https://cef-builds.spotifycdn.com/cef_binary_142.0.10%2Bg29548e2%2Bchromium-142.0.7444.135_linuxarm64.tar.bz2)    
  After downloading the archive, extract it.    
  Then copy the files in the Release directory and the files in the Resources directory to the `${NIM_DUILIB_ROOT}/bin/libcef_linux/` directory (the libcef_linux folder needs to be created).    
### 2. Directory structure of the libcef binaries and resource files
```
chrome-sandbox
libcef.so
libEGL.so
libGLESv2.so
libvk_swiftshader.so
libvulkan.so.1
v8_context_snapshot.bin
vk_swiftshader_icd.json
chrome_100_percent.pak
chrome_200_percent.pak
icudtl.dat
resources.pak
locales (directory containing language packs such as zh-CN.pak and en-US.pak)
```
### 3. Content to add to the Makefile or CMakeLists.txt of the program
* Add `duilib/third_party/libcef/libcef_linux` to the header include paths    
* Add `bin/libcef_linux` to the library include paths (this directory contains the shared library files of libcef, such as libcef.so)    
* Set the linked libraries and add ` libcef.so cef_dll_wrapper X11`    
* Place the libcef binaries and resource files (libcef.so, etc.) in the directory `bin/libcef_linux`.    

## 6. Usage of the CEF control (CefControl) (macOS platform)
The basic steps are as follows (all directories are written as subdirectories relative to the nim_duilib root `${NIM_DUILIB_ROOT}`; the actual settings can be flexibly adjusted according to your own project structure):    
### 1. Downloading the latest libcef binaries and resource files (you need to download them yourself)
* Download link for the x64 version:  [11/12/2025 - 142.0.10+g29548e2+chromium-142.0.7444.135 / Chromium 142.0.7444.135](https://cef-builds.spotifycdn.com/cef_binary_142.0.10%2Bg29548e2%2Bchromium-142.0.7444.135_macosx64.tar.bz2)    
* Download link for the ARM64 version: [11/12/2025 - 142.0.10+g29548e2+chromium-142.0.7444.135 / Chromium 142.0.7444.135](https://cef-builds.spotifycdn.com/cef_binary_142.0.10%2Bg29548e2%2Bchromium-142.0.7444.135_macosarm64.tar.bz2)    
  After downloading the archive, extract it.    
  Then copy the entire contents of the directory to the `${NIM_DUILIB_ROOT}/third_party/libcef/cef_binary` directory (the cef_binary folder needs to be created).    
  Note: `third_party/libcef/libcef_macos` is released together with the nim_duilib source code (wrapper sources and headers only); `cef_binary` is the complete binary distribution downloaded separately (it contains the framework binaries in the `Release/` directory).    
  `cmake/duilib_cef_macos.cmake` uses this directory as the default `CEF_ROOT`; it can be overridden with `-DCEF_ROOT=...` when running CMake.
### 2. Contents of the cef_binary directory
```
BUILD.bazel
LICENSE.txt
README.md
bazel
cef_paths2.gypi
CMakeLists.txt
Doxyfile
MODULE.bazel
README.txt
WORKSPACE
cef_paths.gypi
cmake
libcef_dll
include
Debug
Release
tests
```  
After compilation, the cmake script copies the dependent files to the target path `*.app/Contents/Frameworks/Chromium Embedded Framework.framework/`.    
