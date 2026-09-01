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

### Mode 2: Resources embedded into the executable (single-file release)
```cpp
// Include the generated file in exactly one source file.
#include "embedded_resources.inc"
ui::MemoryResParam resParam(GetEmbeddedResourcesData(), GetEmbeddedResourcesSize());
ui::GlobalManager::Instance().Startup(resParam);
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
| bin/*.zip | external distribution archive, not a runtime resource format |

**Rule: only package global.xml + public/ + your own application directory + fonts/ (optional) + lang/ (optional)**

## Cross-platform notes

| Platform | Supported resource modes |
|------|--------------|
| Windows | Local files / custom embedded binary |
| Linux | Local files / custom embedded binary |
| macOS | Local files / custom embedded binary |
| FreeBSD | Local files / custom embedded binary |

The custom binary archive is platform-independent and can be embedded in any executable. A ZIP may still be used as an external distribution wrapper, but dui does not read ZIP files at runtime.
