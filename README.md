# nim_duilib

[nim_duilib](https://github.com/rhett-lee/nim_duilib) is a cross-platform UI library developed in C++. It originates from the classic duilib UI library and has been deeply optimized and extended, supporting Windows/Linux/macOS/FreeBSD platforms. Supported Linux systems include OpenEuler, OpenKylin, UbuntuKylin, UnionTech UOS, Zhongke Fangde, Ubuntu, Fedora, Debian, etc. It focuses on simplifying the efficient development of desktop applications. Its design incorporates the DirectUI concept, describing UI layout through XML to separate visuals from logic, significantly improving development flexibility and maintainability.

![GitHub](https://img.shields.io/badge/license-MIT-green.svg)

## Core Technical Features

 - XML-driven layout: XML files define the UI structure, fully separating layout from business logic. Developers can quickly adjust control position, size, and style by modifying the XML without touching C++ core code, greatly improving development and iteration efficiency.
 - Rich control system: built-in basic controls such as buttons, text boxes, list views, virtual list controls, tree controls, sliders, progress bars, menus, color pickers, property grids, and tab pages, while also supporting custom control development to meet diverse UI design needs.
 - Efficient rendering: Skia is used as the rendering engine to implement windowless control drawing, reducing system resource usage and improving UI refresh speed. Hardware-accelerated rendering is supported (the backends can use CPU or GPU drawing), ensuring smooth operation of complex UIs.
 - Event-driven: message-mechanism-based event handling keeps UI interaction logic clear, and event response code can be configured in XML files.
 - Multiple image formats: supports SVG/PNG/GIF/JPG/BMP/APNG/WEBP/ICO image formats.
 - Animation format support: supports GIF, APNG, WEBP, Lottie JSON, and PAG animation file formats.
 - Multi-language and internationalization: supports dynamic switching between multiple languages, making it easy to develop globalized applications.
 - Dynamic skinning: skin structure is defined via XML files, so the UI style can be changed easily; dynamic skinning is supported.
 - Window shadows: supports rounded-corner and right-angle window shadows, with selectable shadow size that updates in real time.
 - DPI awareness: four modes — Unaware, SystemAware, PerMonitorAware, PerMonitorAware_V2 — with independent DPI settings and high-DPI adaptation (Windows only).
 - CEF control support: supports libcef 109 for Windows 7 compatibility; supports libcef 142 for Windows 10 and above, and supports Linux and macOS.
 - WebView2 control support: a WebView2 control can display web pages; its interface is simple and easy to use (Windows only).
 - SDL3 support: SDL3 can serve as the provider of basic functionality such as window management and input/output, enabling cross-platform support (Windows/Linux/macOS/FreeBSD currently adapted).

## Directory Structure
| Directory          | Description |
| :---          | :--- |
| duilib        | The project source code|
| docs          | Project documentation, including feature introductions and property list documentation for each control|
| bin           | Output directory for example programs, containing preset skins, language files, and CEF dependencies|
| licenses      | License files corresponding to the referenced open-source code|
| cmake         | Common settings used by cmake at build time|
| build         | Build scripts and build projects for each platform (including VC build projects)|
| msvc          | Windows platform application manifest files and common VC project configuration|
| examples      | Source code of the example programs, covering basic usage of every control|
| duilib/third_party| Third-party libraries the project depends on; see later documentation for details|

## Main Modifications Based on NIM_Duilib_Framework Source
<table>
    <tr>
        <th>Category</th>
        <th>Modification content</th>
    </tr>
    <tr>
        <td rowspan="11">Overall improvements</td>
        <td align="left">1. Reorganized the code structure, dividing it by functional module; large files were split into multiple small files by class, making the library's architecture easier to understand</td>
    </tr>
    <tr><td align="left">2. Reorganized the interface files and added comments and feature documentation for each interface, making the code easier to read and understand</td></tr>
    <tr><td align="left">3. Optimized the configuration XML files, adjusted the attribute naming rules; control width and height can now be set as percentages; image attributes were extended; the image loading flow was optimized</td></tr>
    <tr><td align="left">4. Extended image resource support: added APNG/WEBP animations, Lottie JSON animations, PAG animations, and ICO support; optimized the image loading engine and its code implementation logic</td></tr>
    <tr><td align="left">5. Reimplemented the layout code and added common layout schemes, grouped as: float layout, horizontal layout, vertical layout, horizontal flow layout, vertical flow layout, grid layout, horizontal tile layout, vertical tile layout, virtual horizontal list layout, virtual vertical list layout, virtual horizontal tile list layout, virtual vertical tile list layout, making the layout concept easier to understand and extend; see the documentation for details (docs/Box.md)</td></tr>
    <tr><td align="left">6. Replaced the XML parsing engine with the pugixml parser for better performance</td></tr>
    <tr><td align="left">7. Removed the dependency on the base library; the message loop and thread communication functionality are now implemented in-house</td></tr>
    <tr><td align="left">8. Integrated the Skia engine as the default rendering engine</td></tr>
    <tr><td align="left">9. SDL3 support for cross-platform use (Windows, Linux, macOS, and FreeBSD platforms adapted)</td></tr>
    <tr><td align="left">10. Moved the CEF component into the duilib project and upgraded the CEF version (supports libcef 109 for Win7 compatibility; supports libcef 142 for Win10 and above)</td></tr>
    <tr><td align="left">11. Redesigned the image management interfaces and loading flow (Image directory), supporting multi-threaded image loading for better extensibility of other image formats</td></tr>
    <tr>
        <td rowspan="22">Feature improvements</td>
        <td align="left">1. Improved the window class (Window): enhanced DPI adaptation and code tolerance for window messages; the code structure was adjusted</td>
    </tr>
    <tr><td align="left">2. Added new properties to the window class (Window): use_system_caption, snap_layout_menu, sys_menu, sys_menu_rect, icon, providing the option to use the operating system's default title bar; the custom-drawn title bar has functionality similar to the system title bar</td></tr>
    <tr><td align="left">3. Introduced the WindowDropTarget helper class for the window (Window), providing window-based drag-and-drop support</td></tr>
    <tr><td align="left">4. Reorganized resource-management related code so that managing resources such as fonts, colors, and images is easier to understand</td></tr>
    <tr><td align="left">5. Optimized the ListBox control: subdivided into ListBox, VListBox, HListBox, VTileListBox, HTileListBox, VirtualListBox, VirtualVListBox, VirtualHListBox, VirtualVTileListBox, VirtualHTileListBox with more complete functionality; the usability of virtual-list-based ListBox was improved</td></tr>
    <tr><td align="left">6. Optimized the controls associated with Combo (CheckCombo, FilterCombo) for better usability</td></tr>
    <tr><td align="left">7. Improved the editing functionality of the date-time (DateTime) control</td></tr>
    <tr><td align="left">8. Optimized the menu (CMenuWnd) class code and reimplemented menus with a new structure, making controls inside menus fully compatible with the existing container/control system, easier to understand and maintain</td></tr>
    <tr><td align="left">9. Optimized and extended the text editing control (RichEdit), enriching many common features</td></tr>
    <tr><td align="left">10. Optimized and extended the tree control (TreeView), enriching many common features and improving usability</td></tr>
    <tr><td align="left">11. Optimized the global resource (GlobalManager) interfaces so that all resources are managed through this interface, easier to understand and maintain</td></tr>
    <tr><td align="left">12. Between different containers in the same window, controls can support being dragged out of and into containers via property settings</td></tr>
    <tr><td align="left">13. Control background colors support gradients; foreground color functionality was added</td></tr>
    <tr><td align="left">14. Improved multi-language functionality for better dynamic language switching, with the example program examples/MultiLang</td></tr>
    <tr><td align="left">15. Improved DPI awareness: supports four modes — Unaware, SystemAware, PerMonitorAware, PerMonitorAware_V2 — with independent DPI settings and high-DPI adaptation, with the example program examples/DpiAware</td></tr>
    <tr><td align="left">16. Removed the ui_components project; the CEF component code was reorganized and merged into the duilib project; other content was deleted</td></tr>
    <tr><td align="left">17. Optimized the window shadow feature: shadows use SVG images, with a new shadow type property (shadow_type) whose values are: <br> "default", default shadow <br> "big", large shadow, right-angle corners (suitable for normal windows) <br> "big_round", large shadow, rounded corners (suitable for normal windows) <br> "small", small shadow, right-angle corners (suitable for normal windows) <br> "small_round", small shadow, rounded corners (suitable for normal windows) <br> "menu", small shadow, right-angle corners (suitable for popup windows such as menus) <br> "menu_round", small shadow, rounded corners (suitable for popup windows such as menus) <br> "none", no shadow</td></tr>
    <tr><td align="left">18. Added support for APNG/SVG/WEBP/ICO/LOTTIE/PAG image formats</td></tr>
    <tr><td align="left">19. Redesigned the control loading functionality: Box containers display the loading UI, configured through XML files (including animated images), with interaction with the animated images supported</td></tr>
    <tr><td align="left">20. Enhanced the Label text display control: added "justified" text alignment, vertical text support (drawing direction from top to bottom, right to left), and support for setting line spacing and letter spacing</td></tr>
    <tr><td align="left">21. Control supports fullscreen display (implemented via the new Window::SetFullscreenControl function); CEF and WebView2 controls support F11 page fullscreen toggling</td></tr>
    <tr><td align="left">22. Improved control animation details and introduced easing functions; supports setting control animation properties, such as easing function type, total animation duration, and playback interval</td></tr>
    <tr>
        <td rowspan="25">New controls/new containers</td>
        <td align="left">1. GroupBox: group container</td>
    </tr>
    <tr><td align="left">2. HotKey: hotkey control</td></tr>
    <tr><td align="left">3. HyperLink: text with hyperlinks</td></tr>
    <tr><td align="left">4. IPAddress: IP address control</td></tr>
    <tr><td align="left">5. Line: line drawing control</td></tr>
    <tr><td align="left">6. RichText: formatted text (HTML-like format)</td></tr>
    <tr><td align="left">7. Split: split bar control/container</td></tr>
    <tr><td align="left">8. TabCtrl: multi-tab control (like browser tabs)</td></tr>
    <tr><td align="left">9. ListCtrl: list control (Report/Icon/List views)</td></tr>
    <tr><td align="left">10. PropertyGrid: property grid control, supporting text, number, checkbox, font, color, date, IP address, hotkey, file path, folder, and other properties</td></tr>
    <tr><td align="left">11. ColorPicker: color picker in an independent window; its child controls can be used individually as color controls</td></tr>
    <tr><td align="left">12. ComboButton: a button with a dropdown combo box</td></tr>
    <tr><td align="left">13. ShadowWnd: a subclass of WinImplBase, a base class implemented with an attached shadow window; it creates and attaches windows, providing window shadows without the kWS_EX_LAYERED property</td></tr>
    <tr><td align="left">14. DirectoryTree: directory tree control for displaying directories in the file system</td></tr>
    <tr><td align="left">15. AddressBar: address bar control for displaying paths in the local file system</td></tr>
    <tr><td align="left">16. WebView2Control: wraps the basic functionality of the WebView2 control</td></tr>
    <tr><td align="left">17. GridBox/GridScrollBox: controls based on grid layout</td></tr>
    <tr><td align="left">18. HFlowBox/VFlowBox/HFlowScrollBox/VFlowScrollBox: controls based on horizontal and vertical flow layouts</td></tr>
    <tr><td align="left">19. MenuBar: menu bar control</td></tr>
    <tr><td align="left">20. IconControl/BitmapControl: display in-memory small icons and bitmap data</td></tr>
    <tr><td align="left">21. ChildWindow: child window control; the Windows implementation is a native system child window (with the WS_CHILD property); other platforms use SDL popup windows, which are not native child windows because SDL does not support native child windows</td></tr>
    <tr><td align="left">22. ControlDragableT (template class, including the four standard controls ControlDragable/BoxDragable/HBoxDragable/VBoxDragable): supports reordering child controls within the same Box by dragging, and adjusting a control's container by dragging between different Boxes</td></tr>
    <tr><td align="left">23. ControlMovableT (template class, including the four standard controls ControlMovable/BoxMovable/HBoxMovable/VBoxMovable): supports adjusting a control's position by mouse dragging, and adjusting the parent container's position by dragging</td></tr>
    <tr><td align="left">24. ControlResizableT (template class, including the four standard controls ControlResizable/BoxResizable/HBoxResizable/VBoxResizable): supports resizing a control by mouse dragging, similar to resizing a window</td></tr>
    <tr><td align="left">25. XmlBox: a container that can load and preview XML files of the UI library, used to preview the display effects of controls defined in XML files</td></tr>
    <tr>
        <td rowspan="3">Performance optimizations</td>
        <td align="left">1. Optimized the memory usage of Control and its child controls; memory footprint is greatly reduced when there are many UI elements</td>
    </tr>
    <tr><td align="left">2. Optimized the animation drawing flow and merged timer-triggered events, avoiding severe UI lag while playing control animations or animated images</td></tr>
    <tr><td align="left">3. Virtual-list-based ListBox controls and related controls: usability and performance were greatly improved through implementation mechanism optimization</td></tr>
    <tr>
        <td rowspan="14">Example program improvements</td>
        <td align="left">1. examples/ColorPicker: added a color picker example program</td>
    </tr>
    <tr><td align="left">2. examples/ListCtrl: added a list example program demonstrating special list features</td></tr>
    <tr><td align="left">3. examples/render: added a rendering engine example program demonstrating most container, control, and resource management features</td></tr>
    <tr><td align="left">4. examples/TreeView: added a tree control example program demonstrating various tree control features</td></tr>
    <tr><td align="left">5. examples/RichEdit: added a rich text editing control example program demonstrating various rich text editing features</td></tr>
    <tr><td align="left">6. examples/MultiLang: demonstrates dynamic multi-language switching</td></tr>
    <tr><td align="left">7. examples/DpiAware: demonstrates DPI awareness</td></tr>
    <tr><td align="left">8. examples/threads: demonstrates multi-threading</td></tr>
    <tr><td align="left">9. examples/WebView2: demonstrates the WebView2 control</td></tr>
    <tr><td align="left">10. examples/WebView2Browser: demonstrates the WebView2 control (multi-tab)</td></tr>
    <tr><td align="left">11. examples/layout: demonstrates all layouts and containers</td></tr>
    <tr><td align="left">12. examples/ChildWindow: demonstrates the child window control</td></tr>
    <tr><td align="left">13. examples/XmlPreview: tests the preview of XML file UI effects (tests the XmlBox container)</td></tr>
    <tr><td align="left">14. Other example programs: most received code compatibility fixes and optimizations so they can also be used as test programs</td></tr>
    <tr>
        <td rowspan="8">Documentation improvements</td>
        <td align="left">1. Reorganized the README.md and docs subdirectory documentation so readers can more easily understand the UI library's features and usage and get started more easily</td>
    </tr>
    <tr><td align="left">2. The control interfaces are not documented separately, because reading the comments in the interface files serves the purpose; the interface comments are currently fairly complete</td></tr>
    <tr><td align="left">3. Build documentation and dependency build scripts for each platform</td></tr>
    <tr><td align="left">4. Reorganized the license files of the main project and third-party source dependencies, centrally managed in the licenses directory</td></tr>
</table>

## Third-Party Libraries Used
| Name     |Code subdirectory                 | Purpose                                 | License file        | License category          |
| :---     | :---                      |:---                                  |:---               |:---               |
|apng      |duilib/third_party/libpng  | APNG image format support                     |zlib/libpng License|zlib/libpng license, a permissive open-source license|
|libpng    |duilib/third_party/libpng  | PNG image format support                      |[libpng.LICENSE.txt](licenses/libpng.LICENSE.txt)|Custom BSD-style permissive license|
|zlib      |duilib/third_party/zlib    | PNG/APNG image format support<br>Zip file decompression  |[zlib.LICENSE.txt](licenses/zlib.LICENSE.txt)|zlib license, a permissive open-source license|
|cximage   |duilib/third_party/cximage | ICO image format support                      |[cximage.LICENSE.txt](licenses/cximage.LICENSE.txt)|MIT-style license (non-standard MIT)|
|giflib    |duilib/third_party/giflib  | GIF image format support                      |[giflib.LICENSE.txt](licenses/giflib.LICENSE.txt)|MIT license|
|libwebp   |duilib/third_party/libwebp | WebP image format support                     |[libWebP.LICENSE.txt](licenses/libwebp.LICENSE.txt)|BSD 3-clause license|
|stb_image |duilib/third_party/stb_image| BMP image format support<br>Image resizing     |[stb_image.LICENSE.txt](licenses/stb_image.LICENSE.txt)|MIT license/public domain|
|libjpeg-turbo|duilib/third_party/libjpeg-turbo| JPEG image format support             |[libjpeg-turbo.LICENSE.md](licenses/libjpeg-turbo.LICENSE.md)|IJG license and modified BSD 3-clause license|
|nanosvg   |duilib/third_party/svg      | SVG image format support                     |[nanosvg.LICENSE.txt](licenses/nanosvg.LICENSE.txt)|zlib license|
|pugixml   |duilib/third_party/xml      | Parsing resource-description XML               |[pugixml.LICENSE.txt](licenses/pugixml.LICENSE.txt)|MIT license|
|ConvertUTF|duilib/third_party/convert_utf| UTF-8/UTF-16 encoding conversion         |[llvm.LICENSE.txt](licenses/llvm.LICENSE.txt)|Apache License Version 2.0 primarily,<br>supplemented by the LLVM exception terms,<br>with legacy license compatibility for historical versions|
|skia      |Skia source not included in the project          | UI library rendering engine<br>SVG image format support<br>Lottie JSON animation support|[skia.LICENSE.txt](licenses/skia.LICENSE.txt)|BSD 3-clause license|
|SDL       |SDL source not included in the project           | Cross-platform window management                     |[SDL.LICENSE.txt](licenses/SDL.LICENSE.txt)|zlib license|
|duilib    |                            | NIM_Duilib_Framework<br>is developed based on duilib  |[duilib.LICENSE.txt](licenses/duilib.LICENSE.txt)|BSD 2-clause license|
|NIM_Duilib<br>Framework|               | This project is developed based on<br>NIM_Duilib_Framework   |[NIM_Duilib_Framework.LICENSE.txt](licenses/NIM_Duilib_Framework.LICENSE.txt)|MIT license|
|libcef    |duilib/third_party/libcef   | Loads the CEF module|[libcef.LICENSE.txt](licenses/libcef.LICENSE.txt)|BSD 3-clause license|
|udis86    |duilib/third_party/libudis86| Computes the shortest instruction length for complete disassembly         |[udis86.LICENSE.txt](licenses/udis86.LICENSE.txt)|BSD 2-clause license|
|WebView2  |duilib/third_party/<br>Microsoft.Web.WebView2| WebView2 control support |[Microsoft.Web.WebView2.LICENSE.txt](licenses/Microsoft.Web.WebView2.LICENSE.txt)|BSD 3-clause license|
|libpag    |duilib/third_party/libpag   | PAG animation file support<br>(disabled by default; see later documentation) |[libpag.LICENSE.txt](licenses/libpag.LICENSE.txt)|Apache License Version 2.0 (primary)<br>libpag depends on many third-party components with<br>various licenses; see the files in the<br>`duilib/third_party/libpag/licenses`<br>directory. If libpag's licensing<br>(including the primary license and third-party component licenses)<br>is a concern, libpag can be left disabled.|

## UI Preview
Example programs written with this UI library; this document shows the display effects of each control: [docs/Examples.md](docs/Examples.md) 

## About the PAG Animation File Format
* PAG animation files are currently supported only on Windows; other platforms are not supported yet
* PAG animation file format support is disabled by default (because libpag.lib and libpag.dll must be compiled yourself and placed in the project for it to build and run normally)
* How to enable PAG animation format support:    
(1) Open the [`msvc/PropertySheets/LibPagSettings.props`](msvc/PropertySheets/LibPagSettings.props) file with a text editor and change the `LibPagEnabled` variable's value to `1`    
(2) Compile the libpag library following the documentation: [`duilib/third_party/libpag/windows/libpag-build.md`](duilib/third_party/libpag/windows/libpag-build.md)     
* When building nim_duilib, use `scripts/duilib.sln` or `scripts/examples.sln`; builds by other means are not supported.
* The primary license of the libpag library is Apache License Version 2.0, and the third-party components it depends on have many licenses,<br>see the files in the `duilib/third_party/libpag/licenses` directory.<br>If libpag's licensing (including the primary license and third-party component licenses) is a concern, libpag can be left disabled.

## Programming Languages
- C/C++: compilers must support C++20 (`main` branch)
- C/C++: compilers must support C++17 (`develop-cpp17` branch; this branch is only for the VS2017/VS2019 compilers on Windows; use the main branch in other environments)

## Supported Operating Systems
- Windows: 7/10/11 and above
- Linux: OpenEuler, OpenKylin, UbuntuKylin, Zhongke Fangde, UnionTech UOS, Ubuntu, Debian, Fedora, OpenSuse, etc.
- macOS: 12+
- FreeBSD

## Supported Compilers
- Visual Studio 2022/2026 (Windows)
- Visual Studio 2017/2019 (Windows; these two compiler versions are supported only on the `develop-cpp17` branch; CEF module not supported with VS2017)
- LLVM (Windows)
- MinGW-W64: gcc/g++, clang/clang++ (Windows)
- gcc/g++ (Linux)
- clang/clang++ (Linux)
- clang/clang++ (macOS)
- clang/clang++ (FreeBSD)

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

:retry_clone_duilib
if not exist ".\nim_duilib\.git" (
    git clone https://github.com/rhett-lee/nim_duilib
) else (  
    git -C ./nim_duilib pull
)
if %errorlevel% neq 0 (
    timeout /t %retry_delay% >nul
    goto retry_clone_duilib
)
if not exist ".\nim_duilib\.git" (
    echo clone duilib failed!
    exit /b 1
)
.\nim_duilib\scripts\build_duilib_all_in_one.bat
```
The script above builds with static runtime libraries (MT and MTd) by default. To use dynamic runtime libraries (MD and MDd), append the `/MD` parameter to the last line, changing it to:    
`.\nim_duilib\scripts\build_duilib_all_in_one.bat /MD`    
Note 1: if nim_duilib is ultimately built as a DLL, the dynamic runtime libraries must be used.    
Note 2: if nim_duilib uses dynamic runtime libraries, the Skia library must also use dynamic runtime libraries; see the skia_compile library documentation for the build method.    
    
* For Visual Studio 2017/2019 (the develop-cpp17 branch code is required), the script content is as follows:    
```
REM For Visual Studio 2017/2019
echo OFF
set retry_delay=10

:retry_clone_duilib
if not exist ".\nim_duilib\.git" (
    git clone https://github.com/rhett-lee/nim_duilib
) else (  
    git -C ./nim_duilib pull
)
if %errorlevel% neq 0 (
    timeout /t %retry_delay% >nul
    goto retry_clone_duilib
)
if not exist ".\nim_duilib\.git" (
    echo clone duilib failed!
    exit /b 1
)

:retry_pull_duilib
git -C ./nim_duilib checkout develop-cpp17
git -C ./nim_duilib pull
if %errorlevel% neq 0 (
    timeout /t %retry_delay% >nul
    goto retry_pull_duilib
)
.\nim_duilib\scripts\build_duilib_all_in_one.bat
```
The script above builds with static runtime libraries (MT and MTd) by default. To use dynamic runtime libraries (MD and MDd), append the `/MD` parameter to the last line, changing it to:    
`.\nim_duilib\scripts\build_duilib_all_in_one.bat /MD`    
Note 1: if nim_duilib is ultimately built as a DLL, the dynamic runtime libraries must be used.    
Note 2: if nim_duilib uses dynamic runtime libraries, the Skia library must also use dynamic runtime libraries; see the skia_compile library documentation for the build method.    
    
* Once the script file is ready, open a command prompt and run the script: 
```
.\build.bat
```
The compiled example programs are in the bin directory.

### III. Manual build process (Windows platform)
1. Set the working directory: `D:\develop`    
2. Fetch the relevant code    
(1) `git clone https://github.com/rhett-lee/nim_duilib`      
(2) `git clone https://github.com/rhett-lee/skia_compile`    
(3) `git clone https://github.com/google/skia.git`  
3. Build the Skia source    
(1) nim_duilib uses Skia internally as its UI drawing engine, so build skia first; prefer compiling with LLVM for smoother runtime performance    
(2) Follow the method in the [Compiling skia on Windows](https://github.com/rhett-lee/skia_compile/blob/main/Windows%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) document in the skia_compile directory to build the skia .lib files      
4. If using Visual Studio 2017/2019, the develop-cpp17 branch code is required; run the following command:    
   `git -C ./nim_duilib checkout develop-cpp17`
5. Build nim_duilib: enter the `build` directory and open `examples.sln` (if using Visual Studio 2017, open `examples_vs2017.sln`), then build; the compiled example programs are in the bin directory.
6. CEF module notes:    
(1) The CEF module depends on the Windows 11 SDK; a lower SDK version will cause build errors.    
(2) The CEF module supports only Visual Studio 2019/2022/2026, not Visual Studio 2017.    
(3) The CEF module can be disabled if not needed: edit the `msvc\PropertySheets\CEFSettings.props` file and change the value of `LibCefEnabled` to `0`.    
(4) After disabling the CEF module, use the `duilib_no_cef.sln` or `examples_no_cef.sln` projects to build, reducing the compilation of libCEF code.    
7. WebView2 module notes:    
(1) The WebView2 module can be disabled if not needed: edit the `msvc\PropertySheets\WebView2Settings.props` file and change the value of `WebView2Enabled` to `0`.    
8. The nim_duilib library uses static runtime libraries by default (/MT and /MTd), and also supports dynamic runtime libraries (/MD and /MDd); to switch:    
(1) The runtime library used when building the Skia library must be the same as the one used by the nim_duilib library. See the skia_compile library documentation for how to build Skia.    
(2) To switch the nim_duilib library to dynamic runtime libraries, run the following script:    
    `.\nim_duilib\msvc\PropertySheets\DuilibUseDynamicRuntime.bat`    
(3) To switch the nim_duilib library to static runtime libraries, run the following script:    
    `.\nim_duilib\msvc\PropertySheets\DuilibUseStaticRuntime.bat`    

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

# Retry clone nim_duilib
while true; do
    if [ ! -d "./nim_duilib/.git" ]; then
        git clone https://github.com/rhett-lee/nim_duilib
    else
        git -C ./nim_duilib pull
    fi
    if [ $? -ne 0 ]; then
        sleep 10
        continue
    fi
    break
done

chmod +x ./nim_duilib/scripts/build_duilib_all_in_one.sh
./nim_duilib/scripts/build_duilib_all_in_one.sh
```
The compiled example programs are in the bin directory.    
Note: on UOS systems, install the required development environment first, then install; see the document: [Compiling skia on UnionTech UOS](https://github.com/rhett-lee/skia_compile/blob/main/%E7%BB%9F%E4%BF%A1UOS%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)。

### III. Manual build process (Linux platform)
1. Set the working directory: `~/develop`    
2. Fetch the relevant code    
(1) `git clone https://github.com/rhett-lee/nim_duilib`      
(2) `git clone https://github.com/rhett-lee/skia_compile`    
(3) `git clone https://github.com/google/skia.git`  
(4) `git clone https://github.com/libsdl-org/SDL.git`    
3. Build the Skia library    

| Operating system platform            |Reference document (online link)    |  Reference document (local file) |
| :---                    | :---       |:---   |
|OpenEuler                |[Compiling skia on OpenEuler](https://github.com/rhett-lee/skia_compile/blob/main/OpenEuler%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)|[Compiling skia on OpenEuler](../skia_compile/OpenEuler%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)|
|OpenKylin    |[Compiling skia on OpenKylin](https://github.com/rhett-lee/skia_compile/blob/main/OpenKylin%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)|[Compiling skia on OpenKylin](../skia_compile/OpenKylin%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)|
|UbuntuKylin    |[Compiling skia on UbuntuKylin](https://github.com/rhett-lee/skia_compile/blob/main/UbuntuKylin%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)  |[Compiling skia on UbuntuKylin](../skia_compile/UbuntuKylin%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)|
|Zhongke Fangde                 |[Compiling skia on Zhongke Fangde](https://github.com/rhett-lee/skia_compile/blob/main/%E4%B8%AD%E7%A7%91%E6%96%B9%E5%BE%B7%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)  |[Compiling skia on Zhongke Fangde](../skia_compile/%E4%B8%AD%E7%A7%91%E6%96%B9%E5%BE%B7%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) |
|UnionTech UOS                  |[Compiling skia on UnionTech UOS](https://github.com/rhett-lee/skia_compile/blob/main/%E7%BB%9F%E4%BF%A1UOS%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)|[Compiling skia on UnionTech UOS](../skia_compile/%E7%BB%9F%E4%BF%A1UOS%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)|
|Ubuntu                   |[Compiling skia on Ubuntu](https://github.com/rhett-lee/skia_compile/blob/main/Ubuntu%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) | [Compiling skia on Ubuntu](../skia_compile/Ubuntu%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) |
|Debian                   |[Compiling skia on Debian](https://github.com/rhett-lee/skia_compile/blob/main/Debian%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)  |[Compiling skia on Debian](../skia_compile/Debian%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) |
|Fedora                   |[Compiling skia on Fedora](https://github.com/rhett-lee/skia_compile/blob/main/Fedora%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)  |[Compiling skia on Fedora](../skia_compile/Fedora%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)|
|OpenSuse                 |[Compiling skia on OpenSuse](https://github.com/rhett-lee/skia_compile/blob/main/OpenSuse%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) | [Compiling skia on OpenSuse](../skia_compile/OpenSuse%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) |

    Note: when compiling the skia source, use LLVM to compile; the program runs more smoothly this way.
4. Build the SDL library 
```
#!/bin/bash
cd ~/develop
cmake -S "./SDL/" -B "./SDL.build" -DCMAKE_INSTALL_PREFIX="./SDL3/" -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST_LIBRARY=OFF -DSDL_X11_XSCRNSAVER=OFF -DSDL_X11_XTEST=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build ./SDL.build
cmake --install ./SDL.build
```
5. Build nim_duilib
```
#!/bin/bash
cd ~/develop/nim_duilib/
chmod +x linux_build.sh
./linux_build.sh
```
After the build, executable files are generated in the bin directory.    
To enable CEF support, see the relevant documentation [docs/CEF.md](docs/CEF.md).

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

# Retry clone nim_duilib
while true; do
    if [ ! -d "./nim_duilib/.git" ]; then
        git clone https://github.com/rhett-lee/nim_duilib
    else
        git -C ./nim_duilib pull
    fi
    if [ $? -ne 0 ]; then
        sleep 10
        continue
    fi
    break
done

chmod +x ./nim_duilib/scripts/build_duilib_all_in_one.sh
./nim_duilib/scripts/build_duilib_all_in_one.sh
```
The compiled example programs are in the bin directory.    

### III. Manual build process (macOS platform)
1. Set the working directory: `~/develop`    
2. Fetch the relevant code    
(1) `git clone https://github.com/rhett-lee/nim_duilib`      
(2) `git clone https://github.com/rhett-lee/skia_compile`    
(3) `git clone https://github.com/google/skia.git`  
(4) `git clone https://github.com/libsdl-org/SDL.git`  
3. Build the Skia library    

| Operating system platform |Reference document (online link) |  Reference document (local file) |
| :---         | :---                |:---                   |
|macOS         |[Compiling skia on macOS](https://github.com/rhett-lee/skia_compile/blob/main/macOS%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) | [Compiling skia on macOS](../skia_compile/macOS%E4%B8%8B%E7%BC%96%E8%AF%91skia.md) |

    Note: when compiling the skia source, use LLVM to compile; the program runs more smoothly this way.
4. Build the SDL library 
```
#!/bin/bash
cd ~/develop
cmake -S "./SDL/" -B "./SDL.build" -DCMAKE_INSTALL_PREFIX="./SDL3/" -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST_LIBRARY=OFF -DSDL_X11_XSCRNSAVER=OFF -DSDL_X11_XTEST=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build ./SDL.build
cmake --install ./SDL.build
```
5. Build nim_duilib
```
#!/bin/bash
cd ~/develop/nim_duilib/
chmod +x macos_build.sh
./macos_build.sh
```
After the build, executable files are generated in the bin directory.    
To enable CEF support, see the relevant documentation [docs/CEF.md](docs/CEF.md).

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

# Retry clone nim_duilib
while true; do
    if [ ! -d "./nim_duilib/.git" ]; then
        git clone https://github.com/rhett-lee/nim_duilib
    else
        git -C ./nim_duilib pull
    fi
    if [ $? -ne 0 ]; then
        sleep 10
        continue
    fi
    break
done

chmod +x ./nim_duilib/scripts/build_duilib_all_in_one.sh
./nim_duilib/scripts/build_duilib_all_in_one.sh
```
The compiled example programs are in the bin directory.

Note: CEF (Chromium Embedded Framework) is not supported on the FreeBSD platform.
### III. Manual build process (FreeBSD platform)
1. Set the working directory: `~/develop`    
2. Fetch the relevant code    
(1) `git clone https://github.com/rhett-lee/nim_duilib`      
(2) `git clone https://github.com/rhett-lee/skia_compile`    
(3) `git clone https://github.com/google/skia.git`  
(4) `git clone https://github.com/libsdl-org/SDL.git`  
3. Build the Skia library    

| Operating system platform |Reference document (online link) |  Reference document (local file) |
| :---         | :---                |:---                   |
|FreeBSD         |[Compiling skia on FreeBSD](https://github.com/rhett-lee/skia_compile/blob/main/FreeBSD下编译skia.md) | [Compiling skia on FreeBSD](../skia_compile/FreeBSD下编译skia.md) |

    Note: when compiling the skia source, only LLVM compilation is supported.
4. Build the SDL library 
```
#!/usr/bin/env bash
cd ~/develop
cmake -S "./SDL/" -B "./SDL.build" -DCMAKE_INSTALL_PREFIX="./SDL3/" -DSDL_SHARED=ON -DSDL_STATIC=OFF -DSDL_TEST_LIBRARY=OFF -DSDL_X11_XSCRNSAVER=OFF -DSDL_X11_XTEST=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build ./SDL.build
cmake --install ./SDL.build
```
5. Build nim_duilib
```
#!/usr/bin/env bash
cd ~/develop/nim_duilib/
chmod +x ./scripts/freebsd_build.sh
./scripts/freebsd_build.sh
```
After the build, executable files are generated in the bin directory.    

## Development Plans
 - Continue enriching the UI library's controls and improving its features
 - Continuous testing and refinement of the cross-platform (Windows/Linux/macOS/FreeBSD) window engine based on [SDL3.0](https://www.libsdl.org/) (currently stable under X11/XWayland desktop environments, but with more problems under pure Wayland)
 - Test the UI library, find and fix defects, and continuously improve the code

## AI-Assisted Development (Claude Code Integration)

nim_duilib provides AI-friendly documentation and Skills that work with [Claude Code](https://docs.anthropic.com/en/docs/claude-code) for AI-assisted UI development.

### Feature Description
After registration, Claude Code can use the following nim_duilib-specific skills in **any project**:

| Command / Skill | Description |
| :--- | :--- |
| `/nim-init` | Initializes the nim_duilib development environment for the current project (copies the LLM reference documentation, updates CLAUDE.md) |
| `/nim-duilib-create-window` | Creates a new window (auto-generates the C++ class + XML layout file) |
| `/nim-duilib-xml-layout` | Designs XML UI layouts (forms, split columns, toolbars, card grids, and other templates) |
| `/nim-duilib-add-control` | Adds controls (XML snippets and C++ event binding code for 15+ controls) |
| `/nim-duilib-event-handler` | Event handling (XML inline events and C++ Attach bindings) |
| `/nim-duilib-theme` | Theme customization (predefined colors, fonts, quick reference for 100+ common style classes) |
| `/nim-duilib-resource-pack` | Resource packaging and deployment (ZIP packaging, embedding in an EXE for single-file release) |

### Quick Start

**Prerequisites:** [Claude Code](https://docs.anthropic.com/en/docs/claude-code) installed

**Step 1: Register (run only once)**

Run the following in the nim_duilib root directory:
```bash
# Windows (CMD / PowerShell)
.claude\register.bat

# Linux / macOS
bash .claude/register.sh
```
The registration script installs all skills to `~/.claude/skills/`, taking effect globally.

**Step 2: Use in your application project**

Open Claude Code in any project that needs nim_duilib and type:
```
/nim-init
```
The AI automatically configures the LLM reference documentation and CLAUDE.md for that project. After that, you can directly instruct Claude in natural language to complete UI development, for example:
- "Create a settings window with a username input box and a save button"
- "Design a layout with a left navigation bar and a right content area"
- "Add a click event to this button"
- "Package the resources into a single EXE"

### Updating Skills
When nim_duilib's AI skill files (`.claude/skills/`) are updated, re-run the registration script:
```bash
cd nim_duilib
.claude\register.bat   # Windows
# bash .claude/register.sh  # Linux / macOS
```

### Unregistering
To remove all global skills:
```bash
bash nim_duilib/.claude/unregister.sh
```

### File Structure
```
nim_duilib/.claude/
├── register.bat / register.ps1 / register.sh   # global registration scripts
├── unregister.sh                                # unregistration script
├── docs/
│   └── nim-duilib-llm-reference.md              # full LLM reference manual
└── skills/                                      # AI skill definitions
    ├── nim-duilib-create-window.md
    ├── nim-duilib-xml-layout.md
    ├── nim-duilib-add-control.md
    ├── nim-duilib-event-handler.md
    ├── nim-duilib-theme.md
    └── nim-duilib-resource-pack.md
```

## Reference Documentation

 - [Getting Started](docs/Getting-Started.md)
 - [Example Programs](docs/Examples.md)
 - [Global styles: fonts, colors, images and other resources](docs/Global.md)
 - [Window styles](docs/Window.md)
 - [Container styles](docs/Box.md)
 - [Control styles](docs/Control.md)
 - [Menu styles](docs/Menu.md)
 - [Control events/messages](docs/Events.md)
 - [Node names of controls in XML files](docs/XmlNode.md)
 - [Responding to control events directly in XML files](docs/XmlEvents.md)
 - [CEF control](docs/CEF.md)
 - [Project build documentation and scripts](scripts/build.md)
 - [Reference documentation](docs/Summary.md)

## Related Links
1. Skia build documentation library, click to visit: [skia_compile](https://github.com/rhett-lee/skia_compile) :    
2. This project is developed directly on the basis of the NIM_Duilib_Framework project, project address: [NIM_Duilib_Framework](https://github.com/netease-im/NIM_Duilib_Framework/)
3. The NIM_Duilib_Framework project is developed based on duilib, project address: [duilib](https://github.com/duilib/duilib)
