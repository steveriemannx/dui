# CEF Control (CefControl)    
dui's CEF control (CefControl) is an integrated wrapper around libcef, encapsulating the functionality of the CEF component into a dui control so that web browsing can be integrated with the UI library. libcef (Chromium Embedded Framework, abbreviated as CEF) is an open-source framework that allows developers to embed Chromium (the open-source foundation of the Google Chrome browser) browser functionality into their applications. With libcef, developers can integrate web page rendering, JavaScript execution and HTML5 support into their own applications without requiring users to install a separate browser.  

## 1. Core functionality and architecture of libcef
### Function definition
libcef.dll/libcef.so is the core dynamic link library of CEF, providing the browser kernel functionality of Chromium, including:    
* Web page rendering: supports modern web technologies such as HTML5 and CSS3
* JavaScript execution: enables interaction with web page scripts
* Network communication: handles HTTP requests, responses and resource loading
* Multi-process architecture: manages rendering, plug-ins and other modules through independent processes to improve stability
### System architecture
* libcef.dll/libcef.so sits between the application and the Chromium engine, interacting through interface calls. For example, when the application loads a web page through libcef.dll/libcef.so, Chromium's network module and rendering module are responsible for content fetching and display respectively.
* dui wraps the basic functionality of the libcef.dll/libcef.so module through the CEF control (CefControl), integrating web pages and the UI into one.

## 2. Classes related to the CEF control (CefControl)
| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| Interface class | [include/dui/dui_cef.h](../include/dui/dui_cef.h) | The interface class of the CEF control; applications need to include this header file: `#include "dui/dui_cef.h"` |
| CefManager | [include/dui/CEFControl/CefManager.h](../include/dui/CEFControl/CefManager.h) | CEF control manager, responsible for the initialization and uninitialization of the CEF module |
| CefControl | [include/dui/CEFControl/CefControl.h](../include/dui/CEFControl/CefControl.h) | CEF control interface, providing basic web browsing functionality and event acceptance |
| CefControlEvent | [include/dui/CEFControl/CefControlEvent.h](../include/dui/CEFControl/CefControlEvent.h) | Event reception interface for web browsing of the CEF control |
| CefControlNative | [include/dui/CEFControl/CefControlNative.h](../include/dui/CEFControl/CefControlNative.h) | Wrapper of the windowed mode of the CEF control |
| CefControlOffScreen | [include/dui/CEFControl/CefControlOffScreen.h](../include/dui/CEFControl/CefControlOffScreen.h) | Wrapper of the off-screen rendering mode of the CEF control |

## 3. Example programs related to the CEF control (CefControl)
| Example Program | Description |
| :---     | :--- |
| examples\cef           | A simple example of using the CEF control; this program uses the windowed mode of the CEF control |
| examples\CefBrowser    | An example of using the CEF control as a multi-tab browser; this program uses the off-screen rendering mode of the CEF control|


### In the following documentation, the source root directory of dui is assumed to be the variable: `${NIM_DUI_ROOT}`.   
## 4. Usage of the CEF control (CefControl) (Windows platform)

### 1. Getting the libcef binaries and resource files (automatic with CMake)
The CMake build downloads the libcef binaries automatically at configure time when they are missing (into the `${NIM_DUI_ROOT}\third_party\libcef\cef_binary` directory; see `cmake/dui_deps.cmake`).    
The directories used for compilation are:    
* Latest version: `third_party\libcef\libcef_win` (headers and import libraries)    
* libcef 109 (Windows 7 compatible): `third_party\libcef\libcef_win_109`    
Because libcef.dll is loaded at runtime, the binaries and resource files (libcef.dll, chrome_*.pak, icudtl.dat, locales, etc. — the files in the `Release` and `Resources` directories of the downloaded distribution) must also be placed in the runtime directory used by your project (for example the `${NIM_DUI_ROOT}\bin\libcef_win\x64` directory for the classic sln layout).

### 2. Using libcef 109 (for VC projects using the `CEFSettings.props` property file)
Open the `${NIM_DUI_ROOT}\msvc\PropertySheets\CEFSettings.props` file with VS, change the LibCefVersion109 property value to `true`, and recompile the code.    
The effect can be seen in the `${NIM_DUI_ROOT}\examples\cef` and `${NIM_DUI_ROOT}\examples\CefBrowser` projects (visit a website that shows the UA to confirm that the UA is displayed).

### 3. Using the latest libcef version (for VC projects using the `CEFSettings.props` property file)
Open the `${NIM_DUI_ROOT}\msvc\PropertySheets\CEFSettings.props` file with VS, change the LibCefVersion109 property value to `false`, and recompile the code.    
The effect can be seen in the `${NIM_DUI_ROOT}\examples\cef` and `${NIM_DUI_ROOT}\examples\CefBrowser` projects (visit a website that shows the UA to confirm that the UA is displayed).

### 4. How to manually set the libcef-related properties in your own project (for VC projects that do not use the `CEFSettings.props` property file)
#### (1) Support for the new libcef version
The newer versions of libcef (higher than 109) have more complete functionality. They support Windows 10 and above (Win10/Win11, etc.) and do not support operating systems lower than Win10, such as Win7.    
The basic steps are as follows (all directories are written as subdirectories relative to the dui root; the actual settings can be flexibly adjusted according to your own project structure):    
1. Modify the header include paths of the VC project: add `third_party\libcef\libcef_win` (entry: VS project properties -> C/C++ -> General -> Additional Include Directories)    
2. Modify the library include paths of the VC project: add `third_party\libcef\libcef_win\lib\$(Platform)` (entry: VS project properties -> Linker -> General -> Additional Library Directories)    
3. Modify the library dependencies of the VC project to include the following library files (entry: VS project properties -> Linker -> Input -> Additional Dependencies):    
* For the Debug build, add: `libcef.lib;libcef_dll_wrapper_d.lib`    
* For the Release build, add: `libcef.lib;libcef_dll_wrapper.lib`  
4. In the VC project, set libcef.dll to be delay-loaded by adding `libcef.dll` (entry: VS project properties -> Linker -> Input -> Delay Loaded Dlls)    
5. Place the libcef binaries and resource files (libcef.dll, etc.) in the runtime directory of your project (for example the x64 version in `${NIM_DUI_ROOT}\bin\libcef_win\x64` and the Win32 version in `${NIM_DUI_ROOT}\bin\libcef_win\Win32`).

#### (2) Support for libcef 109
libcef 109 supports Windows 7 and above (Win7/Win10/Win11, etc.) and does not support older operating systems such as Windows XP.    
The basic steps are as follows (all directories are written as subdirectories relative to the dui root; the actual settings can be flexibly adjusted according to your own project structure):    
1. Modify the header include paths of the VC project: add `third_party\libcef\libcef_win_109` (entry: VS project properties -> C/C++ -> General -> Additional Include Directories)    
2. Modify the library include paths of the VC project: add `third_party\libcef\libcef_win_109\lib\$(Platform)` (entry: VS project properties -> Linker -> General -> Additional Library Directories)    
3. Modify the library dependencies of the VC project to include the following library files (entry: VS project properties -> Linker -> Input -> Additional Dependencies):    
* For the Debug build, add: `libcef.lib;libcef_dll_wrapper_109_d.lib`    
* For the Release build, add: `libcef.lib;libcef_dll_wrapper_109.lib`  
4. In the VC project, set libcef.dll to be delay-loaded by adding `libcef.dll` (entry: VS project properties -> Linker -> Input -> Delay Loaded Dlls)    
5. Place the libcef binaries and resource files (libcef.dll, etc.) in the runtime directory of your project (for example the x64 version in `${NIM_DUI_ROOT}\bin\libcef_win_109\x64` and the Win32 version in `${NIM_DUI_ROOT}\bin\libcef_win_109\Win32`).

## 5. Usage of the CEF control (CefControl) (Linux platform)
The basic steps are as follows (all directories are written as subdirectories relative to the dui root `${NIM_DUI_ROOT}`; the actual settings can be flexibly adjusted according to your own project structure):    
### 1. Getting the libcef binaries and resource files (automatic with CMake)
The CMake build downloads the libcef binaries automatically at configure time when they are missing (into the `${NIM_DUI_ROOT}/third_party/libcef/cef_binary` directory); the `${NIM_DUI_ROOT}/bin/libcef_linux/` directory is used as the runtime library directory.    
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
* Add `third_party/libcef/libcef_linux` to the header include paths    
* Add `bin/libcef_linux` to the library include paths (this directory contains the shared library files of libcef, such as libcef.so)    
* Set the linked libraries and add ` libcef.so cef_dll_wrapper X11`    
* Place the libcef binaries and resource files (libcef.so, etc.) in the directory `bin/libcef_linux`.    

## 6. Usage of the CEF control (CefControl) (macOS platform)
The basic steps are as follows (all directories are written as subdirectories relative to the dui root `${NIM_DUI_ROOT}`; the actual settings can be flexibly adjusted according to your own project structure):    
### 1. Getting the libcef binaries and resource files (automatic with CMake)
The CMake build downloads the complete binary distribution automatically at configure time when it is missing, and extracts it to the `${NIM_DUI_ROOT}/third_party/libcef/cef_binary` directory (it contains the framework binaries in the `Release/` directory).    
Note: `third_party/libcef/libcef_macos` is released together with the dui source code (wrapper sources and headers only); `cef_binary` is the complete binary distribution.    
`cmake/dui_cef_macos.cmake` uses the `cef_binary` directory as the default `CEF_ROOT`; it can be overridden with `-DCEF_ROOT=...` when running CMake.
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
