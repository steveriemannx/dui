# Build Process

For the list of build scripts, solution files and their usage, see [scripts/build.md](../scripts/build.md).
The CEF module setup is documented separately in [docs/CEF.md](CEF.md).

## A. Build Process (Windows platform)
### I. Prerequisites: Install the required software
1. Install python3 (the Python major version must be 3; it needs to be added to the Path environment variable)    
(1) First install python3    
(2) In the directory where `python.exe` is located, copy `python.exe` and rename the copy to `python3.exe`: so that `python3.exe` is accessible from the command line   
(3) Verify from the command line: `> python3.exe --version` displays the Python version number     
2. Install Git For Windows: version 2.44 (other versions work too); git needs to be added to the Path environment variable so that `git.exe` is accessible from the command line    
3. Install Visual Studio; during installation, be sure to also select the correct Windows SDK version   
   It is recommended to install the Windows 11 SDK, because the CEF module depends on the Windows 11 SDK; the Windows 10 SDK will cause CEF-related modules to fail to compile;    
   If you don't use CEF, the Windows 10 SDK is fine
4. Install LLVM: 21.1.4 Win64 version (other versions work too)    
(1) Install directory: `C:\LLVM`    
(2) Note: if installed in another directory, the install directory must not contain spaces, or the build will hit problems.

### II. Automated build with a script (recommended)
The script automatically handles downloading and building the relevant source code.    
Choose a working directory (note: the path must not contain spaces, or the build script will fail), create a script `build.bat`, copy the script below into it, and save the file.    
* For Visual Studio 2022/2026, the script content is as follows:    
```
REM For Visual Studio 2022/2026
echo OFF
set retry_delay=10

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
    exit /b 1
)
.\dui\scripts\build_dui_all_in_one.bat
```
The script above builds with static runtime libraries (MT and MTd) by default. To use dynamic runtime libraries (MD and MDd), append the `/MD` parameter to the last line, changing it to:    
`.\dui\scripts\build_dui_all_in_one.bat /MD`    
Note 1: if dui is ultimately built as a DLL, the dynamic runtime libraries must be used.    
Note 2: if dui uses dynamic runtime libraries, the Skia library must also use dynamic runtime libraries; see the skia_compile library documentation for the build method.    
    
* Once the script file is ready, open a command prompt and run the script: 
```
.\build.bat
```
The compiled example programs are in the bin directory.

### III. Notes for Visual Studio (sln) builds
1. CEF module notes:    
(1) The CEF module depends on the Windows 11 SDK; a lower SDK version will cause build errors.    
(2) The CEF module can be disabled if not needed: edit the `msvc\PropertySheets\CEFSettings.props` file and change the value of `LibCefEnabled` to `0`.    
(3) After disabling the CEF module, use the `dui_no_cef.sln` or `examples_no_cef.sln` projects to build, reducing the compilation of libCEF code.    
2. WebView2 module notes:    
(1) The WebView2 module can be disabled if not needed: edit the `msvc\PropertySheets\WebView2Settings.props` file and change the value of `WebView2Enabled` to `0`.    
For CMake builds, the CEF and WebView2 examples are included by default in the `ALL`/`XML` example modes and download large binary distributions at configure time (CEF ≈ 200 MB). To skip them (no CEF/WebView2 download):    
    `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDUI_BUILD_CEF_EXAMPLES=OFF -DDUI_BUILD_WEBVIEW2_EXAMPLES=OFF`    
The `GEN`/`CODE` example modes (`-DDUI_EXAMPLES_MODE=GEN|CODE`) never include the CEF/WebView2 examples.    
3. The dui library uses static runtime libraries by default (/MT and /MTd), and also supports dynamic runtime libraries (/MD and /MDd); to switch:    
(1) The runtime library used when building the Skia library must be the same as the one used by the dui library. See the skia_compile library documentation for how to build Skia.    
(2) To switch the dui library to dynamic runtime libraries, run the following script:    
    `.\dui\msvc\PropertySheets\DuiUseDynamicRuntime.bat`    
(3) To switch the dui library to static runtime libraries, run the following script:    
    `.\dui\msvc\PropertySheets\DuiUseStaticRuntime.bat`    

## B. Build Process (Linux platform)
### I. Prerequisites: Install the required software
On different operating systems, install the required software following the list below.
| Operating system platform            |Desktop type        |Modules and install commands required (required) | 
| :---                    | :---           | :---                    |
|OpenEuler                |UKUI/DDE (X11) |`sudo dnf install -y gcc g++ gdb make git ninja-build gn python cmake llvm clang unzip fontconfig-devel mesa-libGL-devel mesa-libGLU-devel mesa-libGLES-devel mesa-libEGL-devel vulkan-devel libXext-devel libXcursor-devel libXi-devel libXrandr-devel dbus-devel ibus-devel`| 
|OpenKylin    | Wayland        |`sudo apt install -y gcc g++ gdb make git ninja-build generate-ninja python3 cmake llvm clang unzip libfontconfig-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libvulkan-dev libxext-dev libxcursor-dev libxi-dev libxrandr-dev libdbus-1-dev libibus-1.0-dev libwayland-dev libxkbcommon-dev`| 
|UbuntuKylin    | X11            |`sudo apt install -y gcc g++ gdb make git ninja-build generate-ninja python3 cmake llvm clang unzip libfontconfig-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libvulkan-devlibxext-dev libxcursor-dev libxi-dev libxrandr-dev libdbus-1-dev libibus-1.0-dev`| 
|Zhongke Fangde                 | X11            |`sudo apt install -y gcc g++ gdb make git ninja-build generate-ninja python3 cmake llvm clang unzip libfontconfig-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libvulkan-dev libxext-dev libxcursor-dev libxi-dev libxrandr-dev libdbus-1-dev libibus-1.0-dev`| 
|UnionTech UOS                  | X11            |`sudo apt install -y gcc g++ gdb make git cmake python3 ninja-build wget unzip libfontconfig1-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libvulkan-dev libxext-dev libxcursor-dev libxi-dev libxrandr-dev libdbus-1-dev libibus-1.0-dev`| 
|Ubuntu                   |GNOME (Wayland)|`sudo apt install -y gcc g++ gdb make git ninja-build generate-ninja python3 cmake llvm clang unzip bzip2 libfontconfig-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libvulkan-dev libxext-dev libxcursor-dev libxi-dev libxrandr-dev libdbus-1-dev libibus-1.0-dev libwayland-dev libxkbcommon-dev`| 
|Debian                   |GNOME (Wayland)|`sudo apt install -y gcc g++ gdb make git ninja-build generate-ninja python3 cmake llvm clang unzip libfontconfig-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev libvulkan-dev libxext-dev libxcursor-dev libxi-dev libxrandr-dev libdbus-1-dev libibus-1.0-dev libwayland-dev libxkbcommon-dev`| 
|Fedora                   |GNOME (Wayland)|`sudo dnf install -y gcc g++ gdb make git ninja-build gn python cmake llvm clang unzip fontconfig-devel mesa-libGL-devel mesa-libGLU-devel mesa-libGLES-devel mesa-libEGL-devel vulkan-devel libXext-devel libXcursor-devel libXi-devel libXrandr-devel dbus-devel ibus-devel wayland-devel libxkbcommon-devel`|
|OpenSuse                 |KDE (X11)      |`sudo zypper install -y gcc gcc-c++ gdb make git ninja gn python cmake llvm clang unzip fontconfig-devel Mesa-libGL-devel Mesa-libEGL-devel Mesa-libGLESv3-devel glu-devel vulkan-devel libXext-devel libXcursor-devel libXi-devel libXrandr-devel dbus-1-devel ibus-devel`|

### II. Automated build with a script (recommended)
The script automatically handles downloading and building the relevant source code.    
Choose a working directory (note: the path must not contain spaces, or the build script will fail), create a script `build.sh`, copy the script below into it, and save the file.    
Then in the console, add execute permission to the script file, and finally run the script: 
```
chmod +x build.sh
./build.sh
```

The script content is as follows:    
```
#!/bin/bash

# Retry clone dui
while true; do
    if [ ! -d "./dui/.git" ]; then
        git clone https://github.com/steveriemannx/dui dui
    else
        git -C ./dui pull
    fi
    if [ $? -ne 0 ]; then
        sleep 10
        continue
    fi
    break
done

chmod +x ./dui/scripts/build_dui_all_in_one.sh
./dui/scripts/build_dui_all_in_one.sh
```
The compiled example programs are in the bin directory.    
Note: on UOS systems, install the required development environment first, then install; see the document: [Compiling skia on UnionTech UOS](https://github.com/rhett-lee/skia_compile/blob/main/%E7%BB%9F%E4%BF%A1UOS%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)。

## C. Build Process (macOS platform)
### I. Prerequisites: Install the required software
After installing the system, the following needs to be done:    
#### Install the Xcode command-line tools
```
xcode-select --install
```
Verify the installation:
```
clang++ --version
```
#### Install Homebrew
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```
If it fails, look for other sources to install from.    
Update Homebrew:    
```
brew update
```
#### Software already included with the system (no installation needed)
`git make unzip python3`

#### Install cmake
```
brew install cmake
```
#### Install ninja
```
brew install ninja
```
#### Install gn (gn needs to be compiled from source)
```
mkdir ~/develop
cd ~/develop
git clone https://github.com/timniederhausen/gn
cd gn
python3 build/gen.py
ninja -C out
sudo cp out/gn /usr/local/bin/
gn --version
```

### II. Automated build with a script (recommended)
The script automatically handles downloading and building the relevant source code.    
Choose a working directory (note: the path must not contain spaces, or the build script will fail), create a script `build.sh`, copy the script below into it, and save the file.    
Then in the console, add execute permission to the script file, and finally run the script: 
```
chmod +x build.sh
./build.sh
```

The script content is as follows:    
```
#!/bin/bash

# Retry clone dui
while true; do
    if [ ! -d "./dui/.git" ]; then
        git clone https://github.com/steveriemannx/dui dui
    else
        git -C ./dui pull
    fi
    if [ $? -ne 0 ]; then
        sleep 10
        continue
    fi
    break
done

chmod +x ./dui/scripts/build_dui_all_in_one.sh
./dui/scripts/build_dui_all_in_one.sh
```
The compiled example programs are in the bin directory.    

## D. Build Process (FreeBSD platform)
### I. Prerequisites: Install the required software
```
sudo pkg install git unzip python3 cmake ninja gn llvm fontconfig freetype2
```
### II. Automated build with a script (recommended)
The script automatically handles downloading and building the relevant source code.    
Choose a working directory (note: the path must not contain spaces, or the build script will fail), create a script `build.sh`, copy the script below into it, and save the file.    
Then in the console, add execute permission to the script file, and finally run the script: 
```
chmod +x build.sh
./build.sh
```

The script content is as follows:    
```
#!/usr/bin/env bash

# Retry clone dui
while true; do
    if [ ! -d "./dui/.git" ]; then
        git clone https://github.com/steveriemannx/dui dui
    else
        git -C ./dui pull
    fi
    if [ $? -ne 0 ]; then
        sleep 10
        continue
    fi
    break
done

chmod +x ./dui/scripts/build_dui_all_in_one.sh
./dui/scripts/build_dui_all_in_one.sh
```
The compiled example programs are in the bin directory.

Note: CEF (Chromium Embedded Framework) is not supported on the FreeBSD platform.
