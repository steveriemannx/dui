# dui-test

**dui-test** is the English version of [nim_duilib](https://github.com/rhett-lee/nim_duilib), a cross-platform C++ UI library (Windows/Linux/macOS/FreeBSD) based on the classic duilib, using **XML-described UI layout + Skia rendering**. It supports CPU/GPU rendering, DPI awareness, window shadows, multi-language, CEF/WebView2 integration, and more.

Besides the English translation, the project structure has been reorganized compared with the original:

- **Source layout**: library sources in `src/`, public headers in `include/duilib/`
- **Vendored dependencies**: everything under `third_party/` — Skia and SDL3 are built automatically, the CEF binary distribution is downloaded automatically
- **CMake-based builds**: top-level CMake management (`cmake -S . -B build`), one-click scripts for each platform, and the Windows Visual Studio solutions
- **Three development modes**: every example is available in three variants — XML layout, XML-to-code generation, and pure C++ code

![GitHub](https://img.shields.io/badge/license-MIT-green.svg)

## Three UI Development Modes

nim_duilib supports three ways to build a UI; every example provides all three variants (`examples/<name>`, `examples/<name>_gen`, `examples/<name>_code`):

1. **XML mode** — the UI is described in XML (`bin/resources/themes/default/<skin>/<file>.xml`) and parsed at runtime; the classic workflow.
2. **XML-to-code generation mode** (`*_gen`) — the XML is converted into pure C++ code at build time (`generated_ui.inc`), so the final program has no layout-XML parsing while the XML remains the design-time format.
3. **Pure code mode** (`*_code`) — the UI is built entirely in C++ (`new ui::Xxx` + `SetAttribute` + `AddItem`), with no XML involved.

See [docs/ThreeModes.md](docs/ThreeModes.md) for details.

## Features

 - **XML-driven layout**: UI structure is defined in XML, fully separating layout from business logic; no C++ changes needed to adjust control position, size, or style.
 - **Rich control system**: buttons, labels, list views (including virtual lists), tree controls, sliders, progress bars, menus, color pickers, property grids, tabs, rich text, and more; custom controls are supported.
 - **Efficient rendering**: Skia rendering engine, windowless control drawing, CPU or GPU backends.
 - **Event-driven**: message-mechanism-based event handling; event responses can also be configured directly in XML.
 - **Multiple image formats**: SVG/PNG/GIF/JPG/BMP/APNG/WEBP/ICO, plus GIF/APNG/WEBP/Lottie/PAG animations.
 - **Multi-language & dynamic skinning**: runtime language switching; XML-defined skins.
 - **Window shadows**: rounded/right-angle shadows with selectable, real-time-updating size.
 - **DPI awareness**: four modes (Unaware / SystemAware / PerMonitorAware / PerMonitorAware_V2), independent DPI settings (Windows only).
 - **CEF control**: libcef 109 (Windows 7) and libcef 142 (Windows 10+/Linux/macOS).
 - **WebView2 control** (Windows only) and **SDL3** window/input backend.

## Directory Structure
| Directory     | Description |
| :---          | :--- |
| src / include | Library source code / public header files |
| examples      | Example programs (each with `_code` / `_gen` variants; see the three development modes) |
| bin           | Build output: example programs, `resources/` (skins, fonts, languages), `resources.zip` |
| cmake         | Common CMake configuration |
| msvc          | Windows VC project property sheets |
| scripts       | Build scripts and Visual Studio solutions |
| third_party   | Vendored third-party libraries (built/downloaded automatically) |
| docs          | Project documentation |
| licenses      | License files of the project and its dependencies |

## Quick Start
```bash
# One-click build (downloads and builds all dependencies automatically)
./scripts/build_duilib_all_in_one.sh    # Linux / macOS / FreeBSD
scripts\build_duilib_all_in_one.bat     # Windows

# Or the CMake way
cmake -S . -B build
cmake --build build
```
The compiled example programs are in `bin/`. Platform prerequisites and detailed build steps: [docs/Build.md](docs/Build.md); script reference: [scripts/build.md](scripts/build.md).

## Documentation
- [Getting Started](docs/Getting-Started.md) · [Example Programs](docs/Examples.md)
- [Global styles](docs/Global.md) · [Window](docs/Window.md) · [Box](docs/Box.md) · [Control](docs/Control.md) · [Menu](docs/Menu.md)
- [Events](docs/Events.md) · [XML node names](docs/XmlNode.md) · [XML inline events](docs/XmlEvents.md)
- [CEF control](docs/CEF.md)
- [Third-party libraries & licenses](docs/ThirdParty.md)
- [History: modifications vs. NIM_Duilib_Framework](docs/History.md)
- [AI-assisted development (Claude Code skills)](docs/ClaudeCode.md)
- [Class reference](docs/Summary.md)

## Requirements
- **C++20**: Visual Studio 2022/2026, LLVM, MinGW-w64, gcc/g++, clang/clang++
- **Platforms**: Windows 7+, Linux (OpenEuler, OpenKylin, UbuntuKylin, UOS, Ubuntu, Debian, Fedora, OpenSuse, ...), macOS 12+, FreeBSD

## Related Links
1. Skia build documentation library: [skia_compile](https://github.com/rhett-lee/skia_compile)
2. Base project: [NIM_Duilib_Framework](https://github.com/netease-im/NIM_Duilib_Framework/)
3. Original project: [duilib](https://github.com/duilib/duilib)
