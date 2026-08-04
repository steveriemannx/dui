# dui MSVC Property Configuration

This document describes in detail the purpose and usage of the Visual Studio property sheet files (`*.props`) and script files (`*.bat`) in the `dui\msvc\PropertySheets` directory, for maintainers' reference.

## Directory Overview

```
PropertySheets/
├── CommonSettings.props           # Base configuration (required)
├── DuiSettings.props           # dui library's own configuration
├── BinCommonSettings.props        # Executable common configuration
├── BinCommonSettingsCEF.props     # Executable configuration with CEF support
├── BinCommonSettingsWebView2.props # Executable configuration with WebView2 support
├── BinOutSettings.props           # Output directory configuration
├── BinManifestSettings.props      # Manifest configuration
├── SDLSettings.props              # SDL3 support configuration
├── SkiaSettings.props             # Skia graphics library configuration (required)
├── JpegTurboSettings.props        # libjpeg-turbo configuration
├── LibPagSettings.props           # libpag configuration
├── CEFSettings.props              # libCEF configuration
├── WebView2Settings.props         # WebView2 configuration
├── DuiUseDynamicRuntime.bat    # Switch to dynamic runtime mode
└── DuiUseStaticRuntime.bat     # Switch to static runtime mode
```

## Property Sheet Files in Detail

### 1. CommonSettings.props (Base Configuration)

**Purpose:** Base configuration shared by all projects; contains basic compiler and linker settings.

**Main contents:**

- **Platform toolset selection:** Automatically selects the matching toolset based on the VS version
  - VS2022 (17.0) → v143
  - VS2019 (16.0) → v142
  - VS2017 (15.0) → v141

- **SDK version:** Windows 10.0 target platform

- **C++ standard:**
  - VS2022/VS2026: C++20
  - VS2019/VS2017: C++17

- **Runtime library:** Automatically switches between MT/MTd and MD/MDd by detecting the `dui_dll.flag` file

- **Base library list:**
  ```xml
  dui.lib / dui_d.lib          # main library
  zlib.lib / zlib_d.lib              # compression library
  cximage.lib / cximage_d.lib        # image processing library
  libpng.lib / libpng_d.lib          # PNG image library
  libwebp.lib / libwebp_d.lib        # WebP image library
  ```

**Important variables:**
- `EnableDuiDll`: whether to use DLL mode (1=DLL, 0=static library)
- `DuiSystemLibs`: system dependency library list
- `DuiThirdLibs`: third-party dependency library list
- `DuiMainLib`: dui main library name

---

### 2. DuiSettings.props (dui Library's Own Configuration)

**Purpose:** Property configuration used specifically for building the dui library itself.

**Import relationships:**
```
DuiSettings.props
├── CommonSettings.props
├── SkiaSettings.props
├── SDLSettings.props
├── CEFSettings.props
├── WebView2Settings.props
├── JpegTurboSettings.props
└── LibPagSettings.props
```

**Main features:**

- **Project type switching:** Builds as DLL or static library depending on the `EnableDuiDll` variable
  ```xml
  <ConfigurationType>DynamicLibrary</ConfigurationType>  <!-- DLL mode -->
  <ConfigurationType>StaticLibrary</ConfigurationType>   <!-- static library mode -->
  ```

- **Output configuration:** DLL mode outputs to the `bin\` directory; static library mode outputs to `lib\$(Platform)\`

- **Feature module macros:**
  - `DUI_SDL`: SDL support switch
  - `DUI_WEBVIEW2`: WebView2 support switch
  - `DUI_CEF`: CEF support switch
  - `DUI_JPEG_TURBO`: libjpeg-turbo support switch
  - `DUI_LIB_PAG`: libpag support switch

---

### 3. BinCommonSettings.props (Executable Common Configuration)

**Purpose:** Common configuration for all executable (exe) projects.

**Import relationships:**
```
BinCommonSettings.props
├── CommonSettings.props
├── BinManifestSettings.props
├── BinOutSettings.props
├── SDLSettings.props
├── SkiaSettings.props
├── JpegTurboSettings.props
└── LibPagSettings.props
```

**Main features:**

- **Include directory configuration:**
  ```xml
  $(SolutionDir)\..;$(ProjectDir)
  ```

- **Library path configuration:** selects different library paths depending on whether DLL mode is enabled

- **Preprocessor definitions:**
  - `DUI_SDL`
  - `DUI_JPEG_TURBO`
  - `DUI_LIB_PAG`

---

### 4. BinCommonSettingsCEF.props (CEF Executable Configuration)

**Purpose:** Configuration for executables that need libCEF support.

**Import relationships:**
```
BinCommonSettingsCEF.props
└── BinCommonSettings.props (includes the full common configuration)
    └── CEFSettings.props
```

**Special configuration:**
- **Delay-loaded DLL:** `libcef.dll` uses delay-loading
- **Preprocessor definition:** `DUI_CEF=$(LibCefEnabled)`

---

### 5. BinCommonSettingsWebView2.props (WebView2 Executable Configuration)

**Purpose:** Configuration for executables that need WebView2 support.

**Import relationships:**
```
BinCommonSettingsWebView2.props
└── BinCommonSettings.props (includes the full common configuration)
    └── WebView2Settings.props
```

**Special configuration:**
- **Preprocessor definition:** `DUI_WEBVIEW2=$(WebView2Enabled)`

---

### 6. BinOutSettings.props (Output Directory Configuration)

**Purpose:** Manages the executable output and intermediate directories in a unified way.

**Configuration rules:**

| Configuration/Platform | Output directory | Intermediate directory | Target name |
|-----------|----------|----------|----------|
| Release x64 | `..\bin\` | `build_temp\x64\$(ProjectName)\Release\` | `$(ProjectName)` |
| Debug x64 | `..\bin\` | `build_temp\x64\$(ProjectName)\Debug\` | `$(ProjectName)_d` |
| Release Win32 | `..\bin\` | `build_temp\x86\$(ProjectName)\Release\` | `$(ProjectName)32` |
| Debug Win32 | `..\bin\` | `build_temp\x86\$(ProjectName)\Debug\` | `$(ProjectName)32_d` |

---

### 7. SDLSettings.props (SDL3 Support Configuration)

**Purpose:** Configures the SDL3 graphics/input library support options.

**Configuration items:**
- `SDLEnabled`: SDL support switch (0=disabled, 1=enabled)
- `SDLIncludeDir`: SDL include directory (`$(SolutionDir)\..\..\SDL3\include`)
- `SDLLibDir`: SDL lib directory (selected by platform and configuration)
- `SDLLibs`: SDL dependency library list (`SDL3-static.lib;Version.lib;Winmm.lib;Setupapi.lib`)

**Macro definition:** `DUI_SDL=$(SDLEnabled)`

---

### 8. SkiaSettings.props (Skia Graphics Library Configuration)

**Purpose:** Configures the Skia vector graphics rendering engine support options.

**Configuration items:**
- `SkiaPreprocessorDefinitions`: Skia preprocessor definitions (`SK_GANESH;SK_GL;SK_RELEASE`)
- `SkiaIncludeDir`: Skia include directory (`$(SolutionDir)\..\..\skia`)
- `SkiaLibDir`: Skia lib directory (release/debug selected by platform and configuration)
- `SkiaLibs`: Skia dependency library list (`skia.lib;svg.lib;skshaper.lib;skottie.lib;sksg.lib;jsonreader.lib`)

**Note:** Skia is a required component; every configuration depends on Skia.

---

### 9. JpegTurboSettings.props (libjpeg-turbo Configuration)

**Purpose:** Configures the libjpeg-turbo JPEG codec library support options.

**Configuration items:**
- `JpegTurboEnabled`: libjpeg-turbo support switch (0=disabled, 1=enabled)
- `JpegTurboIncludeDir`: libjpeg-turbo include directory
- `JpegTurboLibDir`: libjpeg-turbo lib directory
- `JpegTurboLibs`: dependency library list (`turbojpeg-static.lib`)

**Macro definition:** `DUI_JPEG_TURBO=$(JpegTurboEnabled)`

---

### 10. LibPagSettings.props (libpag Configuration)

**Purpose:** Configures the libpag animation codec library support options.

**Configuration items:**
- `LibPagEnabled`: libpag support switch (0=disabled, 1=enabled)
- `LibPagIncludeDir`: libpag include directory
- `LibPagLibDir`: libpag lib directory
- `LibPagLibs`: dependency library list (`libpag.lib`)

**Macro definition:** `DUI_LIB_PAG=$(LibPagEnabled)`

---

### 11. CEFSettings.props (libCEF Configuration)

**Purpose:** Configures the libCEF (Chromium Embedded Framework) support options.

**Configuration items:**
- `LibCefEnabled`: libCEF support switch (0=disabled, 1=enabled)
- `LibCefVersion109`: version selection (0=latest, 1=109)
  - Version 109: supports Win7 and later
  - Latest: supports Win10 and later only
- `LibCefSrcDir`: libCEF source subdirectory
- `LibCefDllWrapperName`: CEF wrapper library name
- `LibCefDllName`: CEF DLL name (`libcef.dll`)
- `LibCefIncludeDir`: libCEF include directory
- `LibCefLibDir`: libCEF lib directory
- `LibCefLibs`: dependency library list

**Macro definition:** `DUI_CEF=$(LibCefEnabled)`

---

### 12. WebView2Settings.props (WebView2 Configuration)

**Purpose:** Configures the Microsoft WebView2 control support options.

**Configuration items:**
- `WebView2Enabled`: WebView2 support switch (0=disabled, 1=enabled)
- `WebView2LibDir`: WebView2 lib directory
- `WebView2Libs`: dependency library list (`WebView2LoaderStatic.lib`)

**Macro definition:** `DUI_WEBVIEW2=$(WebView2Enabled)`

---

## Script File Descriptions

### 1. DuiUseDynamicRuntime.bat (Switch to Dynamic Runtime Mode)

**Purpose:** Switches the dui projects to dynamic runtime mode (MD/MDd).

**Steps:**
1. Creates the `dui_dll.flag` file
2. Prompts the user to close and reopen the VS projects

**Effects:**
- `EnableDuiDll` in `CommonSettings.props` becomes 1
- All projects compile with `/MD` (Release) or `/MDd` (Debug)
- dui itself is built as a DLL

---

### 2. DuiUseStaticRuntime.bat (Switch to Static Runtime Mode)

**Purpose:** Switches the dui projects to static runtime mode (MT/MTd).

**Steps:**
1. Deletes the `dui_dll.flag` file
2. Prompts the user to close and reopen the VS projects

**Effects:**
- `EnableDuiDll` in `CommonSettings.props` becomes 0
- All projects compile with `/MT` (Release) or `/MTd` (Debug)
- dui itself is built as a static library

---

## Usage Guide

### How to reference the property sheets in a project

#### Building the dui library itself
```xml
<Import Project="$(SolutionDir)\..\msvc\PropertySheets\DuiSettings.props" />
```

#### Building an executable (base version)
```xml
<Import Project="$(SolutionDir)\..\msvc\PropertySheets\BinCommonSettings.props" />
```

#### Building an executable (with CEF support)
```xml
<Import Project="$(SolutionDir)\..\msvc\PropertySheets\BinCommonSettingsCEF.props" />
```

#### Building an executable (with WebView2 support)
```xml
<Import Project="$(SolutionDir)\..\msvc\PropertySheets\BinCommonSettingsWebView2.props" />
```

---

### How to switch between static and dynamic runtime

1. **Switch to dynamic runtime mode:**
   ```batch
   cd dui\msvc\PropertySheets
   DuiUseDynamicRuntime.bat
   ```

2. **Switch to static runtime mode:**
   ```batch
   cd dui\msvc\PropertySheets
   DuiUseStaticRuntime.bat
   ```

3. **Reload the projects:**
   - Close Visual Studio
   - Reopen the solution file (*.sln)
   - Wait for the property sheets to be reloaded

---

### How to enable/disable optional features

Edit the corresponding configuration file and change the switch variable:

| Feature module | Configuration file | Variable | Values |
|----------|----------|--------|--------|
| SDL support | SDLSettings.props | `SDLEnabled` | 0 (disabled), 1 (enabled) |
| libjpeg-turbo | JpegTurboSettings.props | `JpegTurboEnabled` | 0 (disabled), 1 (enabled) |
| libpag | LibPagSettings.props | `LibPagEnabled` | 0 (disabled), 1 (enabled) |
| libCEF | CEFSettings.props | `LibCefEnabled` | 0 (disabled), 1 (enabled) |
| libCEF version | CEFSettings.props | `LibCefVersion109` | 0 (latest), 1 (109) |
| WebView2 | WebView2Settings.props | `WebView2Enabled` | 0 (disabled), 1 (enabled) |

---

## Configuration Dependency Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    DuiSettings.props                      │
│               (dui library's own configuration)          │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐   │
│  │   Common     │    │    Skia      │    │     SDL      │   │
│  │  Settings    │◄───│  Settings    │    │  Settings    │   │
│  └──────┬───────┘    └──────────────┘    └──────────────┘   │
│         │                     │                              │
│         │              ┌──────┴───────┐                     │
│         │              │  JpegTurbo   │                     │
│         │              │  Settings    │                     │
│         │              └──────────────┘                     │
│         │                                                   │
│         │              ┌──────────────┐                     │
│         │              │    LibPag    │                     │
│         │              │  Settings    │                     │
│         │              └──────────────┘                     │
│         │                                                   │
│         │              ┌──────────────┐                     │
│         │              │     CEF      │                     │
│         │              │  Settings    │                     │
│         │              └──────────────┘                     │
│         │                                                   │
│         │              ┌──────────────┐                     │
│         │              │   WebView2   │                     │
│         │              │  Settings    │                     │
│         │              └──────────────┘                     │
└─────────┼───────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────────────────────────────────────────┐
│                  BinCommonSettings.props                     │
│            (executable common configuration)                │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐   │
│  │   Common     │    │    BinOut    │    │   BinMani-   │   │
│  │  Settings    │    │  Settings    │    │   fest       │   │
│  └──────────────┘    └──────────────┘    └──────────────┘   │
│                                                              │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐   │
│  │     SDL      │    │    Skia      │    │  JpegTurbo   │   │
│  │  Settings    │    │  Settings    │    │  Settings    │   │
│  └──────────────┘    └──────────────┘    └──────────────┘   │
│                                                              │
│  ┌──────────────┐                                           │
│  │    LibPag    │                                           │
│  │  Settings    │                                           │
│  └──────────────┘                                           │
└─────────────────────────────────────────────────────────────┘
          │
          ▼
┌─────────────────────────┐    ┌─────────────────────────────┐
│BinCommonSettingsCEF.props│    │BinCommonSettingsWebView2.props│
│ (executable with CEF)    │    │ (executable with WebView2)   │
├─────────────────────────┤    ├─────────────────────────────┤
│ BinCommonSettings.props  │    │  BinCommonSettings.props    │
│ └─ CEFSettings.props      │    │  └─ WebView2Settings.props   │
└─────────────────────────┘    └─────────────────────────────┘
```

---

## Maintenance Notes

### 1. Configuration files must be reloaded after changes

After modifying any `.props` file, you **must close Visual Studio and reopen the solution**, otherwise the changes may not take effect.

### 2. Runtime mode switching

- After switching static/dynamic runtime mode, you **must rebuild the entire solution**
- It is recommended to clean all intermediate files (the `build_temp` directory) and output files (`bin`, `lib` directories) before rebuilding

### 3. Adding new dependency libraries

If a new third-party dependency needs to be added, it is recommended to:

1. Create a standalone `XXXSettings.props` configuration file in the `PropertySheets` directory
2. Add `Import` statements to `DuiSettings.props` and `BinCommonSettings.props`
3. Configure the corresponding Include directory, Lib directory, library file list, etc.
4. Add the matching macro definition switch

### 4. Path variable conventions

- `$(SolutionDir)`: points to the directory containing the `.sln` file
- `$(ProjectDir)`: points to the current project directory
- All third-party library paths should use relative paths, following the `$(SolutionDir)\..\` form

### 5. Debug/Release configuration differences

- In Debug mode, library names automatically get the `_d` suffix
- In Debug mode, the default `libcmt.lib` library is disabled
- Skia, SDL, JpegTurbo and other libraries use different output directories in Debug and Release

---

## Version Compatibility

| VS version | Toolset | C++ standard | Supported |
|---------|--------|----------|----------|
| VS2026 | v145 | C++20 | main branch |
| VS2022 | v143 | C++20 | main branch |
| VS2019 | v142 | C++17 | develop-cpp17 branch |
| VS2017 | v141 | C++17 | develop-cpp17 branch |

---

## Troubleshooting

### Q: Modified a .props file but the configuration did not take effect
**A:** You must close VS and reopen the solution so MSBuild reloads the property files.

### Q: Build fails with "cannot find xxx.lib"
**A:** Check the path configuration in the corresponding `XXXSettings.props` file and make sure the library files exist in the specified directory.

### Q: Want to enable an optional feature but don't know how
**A:** Edit the corresponding `XXXSettings.props` file and set the switch variable to 1.

### Q: Build fails after switching between different VS versions
**A:** Make sure to use the corresponding branch (main or develop-cpp17); different branches support different VS versions.
