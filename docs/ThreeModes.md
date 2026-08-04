# Three Modes: XML / XML-to-code Generation / Pure Code

dui supports three UI development modes, and all 18 non-CEF examples provide versions of all three modes.

## Mode 1: XML Mode (traditional)

- Layout is written in `bin/resources/themes/default/<skin>/<file>.xml` and parsed at runtime by WindowBuilder
- Examples: 22 original examples (`examples/basic`, `examples/controls`, ...)
- The window overrides `GetSkinFolder()` / `GetSkinFile()` to return the layout file

## Mode 2: XML-to-code Generation (XML is the design-time format)

- **Workflow**: write/debug the UI in XML (parsed at runtime for immediate visual feedback) → at build time `cmake/xml_to_code.cpp` converts the XML into pure C++ code → the final program has zero layout XML parsing
- Examples: `examples/<name>_gen` (18 of them); build-time generation produces `generated_ui.inc`, and `MainForm::OnInitWindow` calls the generated `InitXxx(this)`
- CMake configuration (see `cmake/dui_gen_code.cmake`):
  ```cmake
  set(GEN_XML_FILES layout.xml)     # Layout XML list (multiple files supported)
  set(GEN_FUNC_NAME Init)           # Prefix of the generated function name
  include(dui_gen_code.cmake)
  ```
- The generator supports: all control class names (aligned with the WindowBuilder factory table), `<Include src="x.xml" count="n"/>` expansion, `<RichText>` rich text content, and window-level `<Class>/<Font>/<TextColor>/<DefaultFontFamilyNames>` definitions
- **Events**: `on_click`/`on_select` attributes and `<Event>` tags are skipped; they must be wired up manually in `MainForm::OnInitWindow` (`FindControl` + `AttachClick/AttachSelect`)
- **Item templates** (files whose root element is `ListBoxItem`/`TreeNode`): not generated; build the Item classes in code following the `examples/genlist` pattern
- `GEN_AUTO_EMBED` (image embedding) is available on Linux only (depends on memfd_create); keep it OFF on macOS, where images are read from disk

## Mode 3: Pure Code Mode

- The layout is built entirely in C++ code — no layout XML at all, no build-time generator
- Examples: `examples/<name>_code` (18 of them); `MainForm::BuildUI()` uses `new ui::Xxx(this)` + `SetAttribute()` + `AddItem()`, and finally `AttachBox(pRoot)`
- Window properties are set in `GetCreateWindowAttributes()`; the control `name` must match the `FindControl` references in the logic code
- The theme (fonts/colors/global Class) is provided automatically by `GlobalManager::Startup` parsing the `global.xml` on disk; window-level Classes are registered with `AddClass()`

## Common Conventions

- Window creation is identical in all three modes: `CreateWnd` → `ShowWindow`
- Title bar buttons (minbtn/maxbtn/restorebtn/closebtn/fullscreenbtn) are wired up automatically by `WindowImplBase`
- Images/fonts/themes are read from the `bin/resources/` directory on disk (`Startup(LocalFilesResParam(module directory + "resources\\"))`)

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
# Top-level build (default: configure + build everything at once; use --target <example name> to build only a single target)
./scripts/macos_build.sh                 # macOS
./scripts/linux_build.sh                 # Linux
./scripts/linux_build_wayland.sh         # Linux (Wayland)
./scripts/macos_build.sh --fresh         # clean and re-configure

# Per-example standalone build (old way)
./scripts/macos_build.sh --standalone
```

- Top-level mode is equivalent to: `cmake -S . -B scripts/build_temp/llvm_build/top` + `cmake --build ...`

Classic command-line flow (the repo's build scripts live in `scripts/`, so `build/` is free for your own output directory):
```bash
mkdir build && cd build
cmake ..          # Release by default when no build type is specified
make -j6          # builds everything at once; make basic etc. builds only a single target
```
- The third-party libraries are vendored under `third_party/`; skia and SDL3 are built automatically by the top-level build (`dui_skia` / `dui_sdl` targets) when their libraries are missing
- Platform notes: WebView2/WebView2Browser are Windows-only; codeui/embedxml/genlist/genui are Linux-only (no macOS entry; genlist depends on memfd)
- Each example directory is still an independent CMake project and can be built alone with `cmake -S examples/<name> -B build/...`
