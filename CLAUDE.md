# dui - Cross-Platform C++ UI Library

## Commit Convention
- Commit messages end with the co-author trailer exactly as:
  `Co-Authored-By: Claude <noreply@anthropic.com>` (the name is "Claude",
  never "Claude Code").

## Project Overview
dui is a cross-platform C++ UI framework based on the Skia rendering engine, using a development model of XML-described UI layout + C++ control logic.
- **Supported platforms**: Windows (7/10/11+), Linux, macOS (12+), FreeBSD
- **Rendering engine**: Skia (CPU/OpenGL)
- **Build tools**: CMake + Visual Studio / GCC / Clang
- **C++ standard**: C++17+

## Project Structure
```
dui/
├── include/dui/         # public headers (one directory per module)
│   ├── Core/            # window, control base classes, events, managers
│   ├── Control/         # UI controls (Button, Label, RichEdit, TreeView...)
│   ├── Box/             # container controls (VBox, HBox, ListBox, TabBox...)
│   ├── Layout/          # layout engine (HLayout, VLayout, GridLayout...)
│   ├── Animation/       # animation system
│   ├── Image/           # image processing (PNG/SVG/GIF/WEBP/APNG/Lottie/PAG)
│   ├── Render/          # rendering interfaces
│   ├── RenderSkia/      # Skia rendering implementation
│   ├── Utils/           # utility classes (WindowImplBase, FilePath...)
│   ├── Binding/         # optional data-binding module (DUI_ENABLE_MVVM; see docs/Binding.md)
│   ├── CEFControl/      # CEF browser integration
│   └── WebView2/        # WebView2 control
├── src/                 # implementation only (.cpp/.mm); module layout mirrors include/dui/
├── examples/            # example programs (XML, _gen and _code variants of each)
├── docs/                # full documentation
├── resources/           # theme resources (XML layouts, images, fonts)
├── tests/               # test suite (plain assert(); registered with CTest)
├── tools/               # xml_to_code generator
├── third_party/         # vendored dependencies (Skia, pugixml, libpng, libjpeg-turbo, ...)
├── cmake/               # CMake configuration
├── scripts/             # helper scripts
├── build/               # build tree (build/bin holds the example programs and the synced resources)
└── lib/                 # built libraries
```

## Development Modes (XML + C++)

### XML Layout Files
- Edit location: `resources/themes/default/<skin_folder>/<skin_file>.xml` (repo root — the single source of truth)
- Runtime location: `bin/resources/themes/default/<skin_folder>/<skin_file>.xml` — an automatic copy synced at configure time; never edit files under `bin/` (changes are overwritten at the next configure)
- Global resources: `resources/themes/default/global.xml` (fonts, colors, common styles)
- Encoding: UTF-8

### The Three C++ Files
Each window usually requires three files:
1. **MainThread** - inherits `ui::FrameworkThread`, responsible for initialization and window creation
2. **MainForm.h/cpp** - inherits `ui::WindowImplBase`, implements window logic
3. **XML layout file** - describes the UI structure and styles

### Key Code Patterns

**Initialize global resources:**
```cpp
ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
resourcePath += DUI_T("resources\\");
ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));
```

**Create a window:**
```cpp
MainForm* window = new MainForm();
window->CreateWnd(nullptr, ui::WindowCreateParam(DUI_T("WindowTitle"), true));
window->PostQuitMsgWhenClosed(true);
window->ShowWindow(ui::kSW_SHOW_NORMAL);
```

**Find a control:**
```cpp
ui::Button* btn = dynamic_cast<ui::Button*>(FindControl(DUI_T("btn_name")));
```

**Event binding:**
```cpp
btn->AttachClick([this](const ui::EventArgs& args) {
    // handle the click event
    return true;
});
```

**Single entry point (Qt-style):** every example/app has one `main.cpp` using the entry macro from `dui/Utils/AppEntry.h`; the platform entry function (wWinMain/WinMain/main) is generated automatically:

```cpp
#include "TestApplication.h"
#include "dui/Utils/AppEntry.h"

DUI_APP_ENTRY(TestApplication)        // AppClass must provide void Run();
// DUI_APP_ENTRY_ARGS(AppClass)       // AppClass must provide static Instance() + int Run(int argc, char** argv)
```

- Invoke the macro exactly once per executable, at global scope, in one `.cpp` file (never in a header)
- Exceptions: `cef`/`CefBrowser` keep `main_macos.mm` (Objective-C++ for CEF) and guard the macro with `#if !defined(__APPLE__)`; `controls` has a custom `main.cpp` (native backend video driver argument)

## Documentation References
- Full documentation: `docs/Summary.md` (documentation index)
- Control properties: `docs/Control.md`
- Containers/layouts: `docs/Box.md`
- Global resources: `docs/Global.md`
- Window properties: `docs/Window.md`
- Event system: `docs/Events.md`
- XML events: `docs/XmlEvents.md`
- XML node names: `docs/XmlNode.md`
- Detailed LLM reference: `.claude/docs/dui-llm-reference.md`

## Coding Standards
- Strings use the `DString` type; literals are wrapped with the `DUI_T("...")` macro
- Control lookup uses `FindControl(DUI_T("name"))` and requires `dynamic_cast` to the concrete type
- Event callbacks return `true` to indicate the event was handled
- Embedded quotes in XML attribute values use single quotes `'` or curly braces `{}` instead of double quotes
- Control classes support template variants: `Label` (Control-based), `LabelBox` (Box-based), `LabelHBox` (HBox-based), `LabelVBox` (VBox-based)
- Window destruction is managed by the framework; create with `new`, no manual `delete` needed

## Build
- Windows: open `scripts/examples.sln`, select Debug|x64 or Release|x64
- Cross-platform: `scripts/build_dui_all_in_one.sh` or `scripts/build_dui_all_in_one.bat`
- Dependencies: Skia must be compiled first (see `scripts/build.md`)
- Example mode selection (CMake): `-DDUI_EXAMPLES_MODE=ALL|XML|GEN|CODE` — builds only the examples of one development mode (XML / XML-to-code generation / pure code); default `ALL`
