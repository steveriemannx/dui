# nim_duilib

[nim_duilib](https://github.com/rhett-lee/nim_duilib) is a cross-platform C++ UI library (Windows/Linux/macOS/FreeBSD) based on the classic duilib, using **XML-described UI layout + Skia rendering**. It supports CPU/GPU rendering, DPI awareness, window shadows, multi-language, CEF/WebView2 integration, and more.

![GitHub](https://img.shields.io/badge/license-MIT-green.svg)

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

## Three UI Development Modes
XML layout (parsed at runtime) / XML-to-code generation / pure C++ code — every example provides all three variants: `examples/<name>`, `examples/<name>_gen`, `examples/<name>_code`. See [docs/ThreeModes.md](docs/ThreeModes.md).

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
