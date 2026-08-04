---
name: dui-resource-pack
description: dui resource packaging and deployment (ZIP packaging, embedding into the executable, single-file release); configure how resources are loaded
---

# Resource Packaging and Deployment in dui

## Three resource loading modes

### Mode 1: Local folder (recommended during development)
```cpp
ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
resourcePath += _T("resources\\");
ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));
```
Directory structure:
```
MyApp.exe
resources/
├── themes/default/
│   ├── global.xml
│   ├── public/...
│   └── my_app/...
├── fonts/...
└── lang/...
```

### Mode 2: ZIP archive (recommended for release)
```cpp
ui::ZipFileResParam resParam;
resParam.resourcePath = _T("resources\\");     // Relative path inside the ZIP
resParam.zipFilePath = ui::FilePathUtil::GetCurrentModuleDirectory();
resParam.zipFilePath += _T("resources.zip");   // ZIP file path
resParam.zipPassword = _T("");                 // Optional password
ui::GlobalManager::Instance().Startup(resParam);
```
Release files:
```
MyApp.exe
resources.zip        # contains the resources/ directory structure
```

### Mode 3: Resources embedded into the executable (single-file release, Windows only)
```cpp
#include "resource.h"

ui::ResZipFileResParam resParam;
resParam.resourcePath = _T("resources\\");
resParam.hResModule = nullptr;                        // nullptr = the current EXE
resParam.resourceName = MAKEINTRESOURCE(IDR_THEME);   // Resource ID
resParam.resourceType = _T("THEME");                  // Resource type name
resParam.zipPassword = _T("");
ui::GlobalManager::Instance().Startup(resParam);
```

## Implementing a single-EXE release (detailed steps for Mode 3)

### Step 1: Create resource.h
```cpp
// resource.h
#ifndef RESOURCE_H_
#define RESOURCE_H_

#define IDR_THEME  101

#endif // RESOURCE_H_
```

### Step 2: Create the .rc resource file
```rc
// MyApp.rc
#include "resource.h"
IDR_THEME  THEME  "..\\..\\bin\\resources.zip"
```
**Note**: the path is relative to the location of the .rc file.

### Step 3: Create resources.zip
Use 7-Zip to pack it (recommended parameters to ensure UTF-8 file names):
```bash
cd bin
7z a -tzip -mcu=on resources.zip resources/
```

**ZIP requirements:**
- Compression algorithm: Deflate only (Deflate64 is not supported)
- File name encoding: UTF-8 (use the `-mcu=on` option in 7-Zip)
- Password encryption: ZipCrypto only (ZIP legacy encryption)

### Step 4: Switch the loading mode in MainThread
```cpp
void MainThread::OnInit()
{
#ifdef NDEBUG
    // Release: use the ZIP resource embedded in the EXE
    ui::ResZipFileResParam resParam;
    resParam.resourcePath = _T("resources\\");
    resParam.hResModule = nullptr;
    resParam.resourceName = MAKEINTRESOURCE(IDR_THEME);
    resParam.resourceType = _T("THEME");
    resParam.zipPassword = _T("");
    ui::GlobalManager::Instance().Startup(resParam);
#else
    // Debug: use the local folder (easier to modify and debug)
    ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    resourcePath += _T("resources\\");
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));
#endif

    // Create windows...
}
```

### Step 5: Add the .rc file to the project
- Visual Studio: right-click the project → Add → Existing Item → select the .rc file
- CMakeLists.txt:
```cmake
# Add the resource file on Windows
if(WIN32)
    target_sources(MyApp PRIVATE MyApp.rc)
endif()
```

## Packaging checklist: what to include

### Resources that must be packaged
```
resources/
├── themes/default/
│   ├── global.xml                  # required
│   ├── public/                     # required (all 133 files)
│   │   ├── button/                 # SVG for window buttons
│   │   ├── caption/                # caption bar icons
│   │   ├── checkbox/               # checkbox icons
│   │   ├── combo/                  # combo box icons
│   │   ├── option/                 # option button icons
│   │   ├── scrollbar01/            # scrollbar resources
│   │   ├── scrollbar02/            # scrollbar resources
│   │   ├── shadow/                 # window shadow
│   │   ├── slider/                 # slider resources
│   │   ├── tooltip/                # tooltips
│   │   ├── tree/                   # tree control icons
│   │   ├── menu/                   # menu resources
│   │   ├── progress/               # progress bar
│   │   ├── animation/              # loading animation JSON
│   │   └── ...
│   └── my_app/                     # your application's XML and images
│       ├── main_form.xml
│       └── ...
├── fonts/                          # optional: custom fonts
└── lang/                           # optional: multi-language files
```

### Content that must NOT be packaged
| Do not package | Reason |
|---------|------|
| themes/default/basic/ | example program directory |
| themes/default/controls/ | example program directory |
| themes/default/layout/ | example program directory |
| themes/default/render/ | example program directory |
| themes/default/chat/ | example program directory |
| themes/default/cef/ | example program directory |
| themes/default/cef_browser/ | example program directory |
| themes/default/webview2/ | example program directory |
| themes/default/webview2_browser/ | example program directory |
| themes/default/list_box/ | example program directory |
| themes/default/list_ctrl/ | example program directory |
| themes/default/tree_view/ | example program directory |
| themes/default/rich_edit/ | example program directory |
| themes/default/color_picker/ | example program directory |
| themes/default/dpi_aware/ | example program directory |
| themes/default/move_control/ | example program directory |
| themes/default/threads/ | example program directory |
| themes/default/virtual_list_box/ | example program directory |
| themes/default/child_window/ | example program directory |
| themes/default/xml_preview/ | example program directory |
| themes/default/MultiLang/ | example program directory |
| bin/*.exe, bin/*.dll | build artifacts |
| bin/bin.zip | archive of build artifacts |

**Rule: only package global.xml + public/ + your own application directory + fonts/ (optional) + lang/ (optional)**

## Cross-platform notes

| Platform | Supported resource modes |
|------|--------------|
| Windows | Local files / ZIP file / embedded EXE (single file) |
| Linux | Local files / ZIP file |
| macOS | Local files / ZIP file |
| FreeBSD | Local files / ZIP file |

macOS/Linux do not support the embedded-EXE mode (there is no Windows RC resource mechanism); use the ZIP file mode for release.
