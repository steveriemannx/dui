# Three Modes: XML / XML-to-code Generation / Pure Code

dui supports three UI development modes. The XML, XML-to-code, and pure-code example
variants are selected independently by the top-level CMake build.

## Mode 1: XML Mode (traditional)

- Layout is edited in `resources/themes/<theme>/<skin>/<file>.xml` and parsed at runtime by WindowBuilder
- Configure-time resource synchronization copies the source tree to the runtime `bin/resources/` directory
- The window overrides `GetSkinFolder()` / `GetSkinFile()` to return the layout file

## Mode 2: XML-to-code Generation (XML is the design-time format)

- **Workflow**: write/debug the UI in XML (parsed at runtime for immediate visual feedback) → at build time `tools/xml_to_code.cpp` converts the XML into pure C++ code → the final program has zero layout XML parsing
- Examples: `examples/<name>_gen`; build-time generation produces `generated_ui.inc` in the build tree, and `MainForm::OnInitWindow` calls the generated `InitXxx(this)`
- CMake configuration (see `cmake/dui_gen_code.cmake`):
  ```cmake
  set(GEN_XML_FILES layout.xml)     # Layout XML list (multiple files supported)
  set(GEN_FUNC_NAME Init)           # Prefix of the generated function name
  include(dui_gen_code.cmake)
  ```
- The generator supports: all control class names (aligned with the WindowBuilder factory table), `<Include src="x.xml" count="n"/>` expansion, `<RichText>` rich text content, and window-level `<Class>/<Font>/<TextColor>/<DefaultFontFamilyNames>` definitions
- **Events**: `on_click`/`on_select` attributes and `<Event>` tags are skipped; they must be wired up manually in `MainForm::OnInitWindow` (`FindControl` + `AttachClick/AttachSelect`)
- **Item templates** (files whose root element is `ListBoxItem`/`TreeNode`): not generated; build the Item classes in code
- Embedded-resource examples use `dui_embed_res.cmake`; generated and pure-code hello variants embed their selected theme resources on macOS and Windows

## Mode 3: Pure Code Mode

- The layout is built entirely in C++ code — no layout XML at all, no build-time generator
- Examples: `examples/<name>_code`; `MainForm::BuildUI()` uses `ui::Create`/`ui::Attach` (or equivalent `new ui::Xxx` + `SetAttribute` + `AddItem`) and finally attaches the root box
- Window properties are set in the window initialization code; the control `name` must match the `FindControl` references in the logic code
- The theme (fonts/colors/global Class) is provided by `GlobalManager::Startup`; embedded-resource examples use `MemoryResParam`, while disk-backed examples use `LocalFilesResParam`

## Common Conventions

- Window creation is identical in all three modes: `CreateWnd` → `ShowWindow`
- Title bar buttons (minbtn/maxbtn/restorebtn/closebtn/fullscreenbtn) are wired up automatically by `WindowImplBase`
- XML mode reads runtime resources from `bin/resources/`; generated/pure-code examples may embed resources and use `MemoryResParam`
- Generated control trees call `SetArrange(false)` after the root is attached so the first paint performs a complete final layout

## Internal Library Skin Assets (documented exception)

The following internal control skins continue to be read from XML on disk; none of the three modes generate or hand-write them:

| Asset | Description |
|---|---|
| `color/color_picker.xml`, `public/color/screen_color_picker.xml` | ColorPicker control window skin |
| `list_ctrl/loading*.xml`, `loading_progress*.xml` | ListCtrl loading overlay |
| Menu window templates (XML under `public/menu/`, `<skin>/menu/`) | In modes 2/3, menu content is built with the pure-code menu API (`ShowMenu("")` + `AddMenuItem`); the template files themselves are no longer loaded |

## Pure Code Menu API (used by modes 2/3)

```cpp
ui::Menu* menu = new ui::Menu(this, pRelatedControl);
menu->ShowMenu(_T(""), point);          // empty XML = pure code mode
ui::MenuItem* pItem = new ui::MenuItem(menu);
pItem->SetClass(_T("menu_element"));
pItem->SetText(_T("Menu item"));
pItem->SetFixedWidth(ui::UiFixedInt(180), true, true);
menu->AddMenuItem(pItem);               // sub-menus use pItem->AddSubMenuItem(pSub)
menu->AddMenuControl(pControl);         // add ordinary controls such as separators
```

Library support (on the dui side):
- `Menu::PreInitWindow`: builds the `MenuListBox` root node when the XML is empty
- `Menu::AddMenuControl`: adds regular controls to the menu
- `MenuBar::AddTopMenu(id, text, builder)`: the builder callback builds the menu items (no XML)
- `ListBox(Window*, Layout* = nullptr)` / `VirtualListBox(Window*, Layout* = nullptr)`: default layout; can be `new`ed directly in pure code

## Build

The repository uses **top-level CMake management** (following the develop2 branch): the root CMakeLists.txt uniformly manages dui, the third-party libraries, and all examples.

```bash
# Top-level build (default: configure + build everything at once; use --target to build selected targets)
./scripts/macos_build.sh                 # macOS
./scripts/linux_build.sh                 # Linux
./scripts/linux_build_wayland.sh         # Linux (Wayland)
./scripts/macos_build.sh --fresh         # clean and re-configure

# Per-example standalone build (legacy workflow)
./scripts/macos_build.sh --standalone
```

- Top-level mode is equivalent to: `cmake -S . -B scripts/build_temp/llvm_build/top` + `cmake --build ...`

Classic command-line flow (the repo's build scripts live in `scripts/`, so `build/` is free for your own output directory):
```bash
mkdir build && cd build
cmake ..          # Release by default when no build type is specified
make -j6          # builds everything at once; make basic etc. builds only a single target
```
- Skia is vendored under `third_party/` and built automatically by the top-level build (`dui_skia` target) when its library is missing
- Platform notes: WebView2/WebView2Browser are Windows-only
- Each example directory is still an independent CMake project and can be built alone with `cmake -S examples/<name> -B build/...`

### Selecting Example Modes

Use one top-level build directory and select the example family at configure time:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DDUI_BUILD_CEF_EXAMPLES=OFF -DDUI_BUILD_WEBVIEW2_EXAMPLES=OFF \
  -DDUI_EXAMPLES_MODE=ALL
cmake --build build --target hello hello_gen hello_code
```

`XML`, `GEN`, and `CODE` select only the corresponding example family. `ALL`
includes all three families. CEF and WebView2 examples are only considered in
the `ALL` and `XML` modes.

### Tests and Installation

Enable the default non-GUI tests with `DUI_BUILD_TESTS=ON`:

```bash
cmake -S . -B build -DDUI_BUILD_TESTS=ON
cmake --build build --target dui_core_tests
ctest --test-dir build --output-on-failure
```

The install package exports `dui::dui` and `dui::dui_entry`:

```bash
cmake --install build --prefix /path/to/dui-install
```

An installed CMake consumer can use `find_package(dui CONFIG REQUIRED)` and
link `dui::dui`.
