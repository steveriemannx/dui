# dui Example Development Skill

This document is the implementation standard for examples in this repository.
When changing or adding an example, use `hello`, `hello_code`, and `hello_gen`
as the reference implementations. Keep the three modes behaviorally aligned
unless a mode-specific implementation is required.

## Example Modes

Each UI example may have three directories:

```text
examples/<name>/          # XML mode
examples/<name>_code/     # Pure C++ UI mode
examples/<name>_gen/      # XML-to-C++ generated UI mode
```

The three modes have the same application responsibilities:

```text
main.cpp       Start the application only
MainForm.h     Declare the window and its UI/behavior methods
MainForm.cpp   Configure the window, build/load the UI, bind events
CMakeLists.txt Select the resource and generation pipeline
```

Do not put application initialization, resource startup, window creation, or
the message loop in an example-specific `FrameworkThread` class. Use the
shared helpers in `dui/Utils/UiBuilder.h`.

## Entry Points

### XML Mode

`examples/<name>/main.cpp` uses `ui::Run`:

```cpp
#include "dui/dui.h"
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::Run<MainForm>("Example Title");
}
```

`ui::Run` loads resources from the executable directory, creates the window,
centers it by default, runs the message loop, and shuts down global resources.

### Pure Code and Generated Modes

`*_code/main.cpp` and `*_gen/main.cpp` use `ui::RunMemory`:

```cpp
#include "dui/dui.h"
#include "MainForm.h"
#include "embedded_resources.inc"
#include "dui/Utils/UiBuilder.h"

int main()
{
    return ui::RunMemory<MainForm>(
        "Example Title",
        EmbeddedResources());
}
```

Use the optional idle callback only when the example needs continuous idle
work, such as `ChildWindow` painting:

```cpp
return ui::RunMemory<MainForm>(
    "Example Title",
    EmbeddedResources(),
    [](MainForm* window) {
        window->PaintNextChildWindow();
    });
```

Do not reintroduce `DUI_APP_ENTRY`, a local `App` class, or a local
`FrameworkThread` for this purpose.

## MainForm Structure

Every `MainForm` derives from `ui::WindowImplBase` and declares the lifecycle
methods in the same order as the Hello examples.

```cpp
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    virtual std::string GetSkinFolder() override;
    virtual std::string GetSkinFile() override;
    virtual void OnInitWindow() override;

private:
    void SetupWindow();
    void BuildUI();
    void BindEvents();
};
```

The standard initialization order is:

```cpp
void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();

    BaseClass::OnInitWindow();
}
```

Use the following rules for the three modes:

| Mode | `SetupWindow()` | `BuildUI()` | Skin methods |
|---|---|---|---|
| XML | Not needed | Not needed; XML is loaded by the framework | Return the XML folder/file |
| Code | Set window properties | Create controls with `ui::Create` and `ui::Attach` | Return empty strings |
| Gen | Not needed for properties present in XML | Call generated `Init<Name>(this)` | Return empty strings |

The XML mode may still define `BindEvents()` when C++ event wiring is needed.
It should not add an empty `SetupWindow()` merely to match the other modes.

## XML Mode

The XML version is the design and runtime layout version:

```cpp
virtual std::string GetSkinFolder() override { return "hello"; }
virtual std::string GetSkinFile() override { return "hello.xml"; }
```

Its `OnInitWindow()` normally only binds application events and calls the base
implementation:

```cpp
void MainForm::OnInitWindow()
{
    BindEvents();
    BaseClass::OnInitWindow();
}
```

The XML file is stored in the repository resource tree:

```text
resources/themes/<platform-theme>/<name>/<name>.xml
```

Edit this file, not the copied file under `bin/resources`.

The XML `<Window>` node owns window-level settings such as size, minimum size,
caption, shadow, layered-window state, resize borders, icon, and menus.

## Pure Code Mode

The code version does not load a layout XML. Both skin methods return empty
strings:

```cpp
virtual std::string GetSkinFolder() override { return ""; }
virtual std::string GetSkinFile() override { return ""; }
```

`SetupWindow()` contains window-level configuration:

```cpp
void MainForm::SetupWindow()
{
    SetWindowSize(800, 600);
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    SetWindowIcon("public/caption/logo.ico");
}
```

Do not use `GetCreateWindowAttributes()` for normal pure-code examples. Use
`SetWindowSize()` and the ordinary window setter methods in `SetupWindow()`.
Override `GetCreateWindowAttributes()` only when a real pre-creation native
window requirement cannot be expressed by the normal setters.

When `SetupWindow()` changes the size after native window creation, call
`CenterWindow()` unless the XML/code explicitly specifies a position.

`BuildUI()` creates the complete control tree and attaches its root:

```cpp
void MainForm::BuildUI()
{
    auto* root = ui::Create<ui::VBox>(this, {
        {"bkcolor", "bk_wnd_darkcolor"},
        {"visible", "true"}
    });

    auto* label = ui::Create<ui::Label>(this, {
        {"name", "hello_label"},
        {"text", "Hello, dui!"}
    });
    ui::Attach(root, label);
    ui::Attach(this, root);
}
```

Use `ui::Create<T>()` and `ui::Attach()` rather than manually mixing control
constructors and unrelated ownership patterns.

## Generated Mode

The generated version keeps the same `MainForm` lifecycle as pure code, but
`BuildUI()` calls the generated function:

```cpp
#include "generated_ui.inc"

void MainForm::BuildUI()
{
    InitHello(this);
}
```

`generated_ui.inc` is generated from the XML design file by
`tools/xml_to_code.cpp`. Do not edit it by hand. The generated file may be
checked into the example directory because it is also useful for inspection,
but the XML and generator are the source of truth.

The generator creates:

- The control tree and control attributes
- Window-level attributes from the root `<Window>` node
- Window-level shared resources supported by the generator
- The root attachment to the window

The generated window attributes are emitted as readable setter calls in the
generated function. This includes `SetWindowSize`, `SetWindowMinimumSize`,
`SetCaptionRect`, `SetShadowType`, `SetLayeredWindow`, `SetSizeBox`,
`SetWindowIcon`, menu settings, alpha, opacity, and render backend settings.
The generated function uses `auto& w = *pWindow` for concise window setter
calls, while numbered pointer names such as `p0` and `p1` keep generated
control variables short and collision-free.
For a root XML file under a skin directory, the generated function also sets
the window resource path to that skin directory so relative images resolve
against the active platform theme, for example `chat` resolves under
`themes/macos26/chat` on macOS.
`position`/`pos` is emitted as a final `MoveWindow` call. A window with a size
but no explicit position is centered after the size is applied.

Do not duplicate those settings in `MainForm::SetupWindow()`.

## Window Position and Defaults

The default window creation path uses centered placement:

```cpp
WindowCreateParam(title, true)
```

If the XML root does not specify a position, the window remains centered.

To specify a screen position, use either spelling:

```xml
<Window position="100,80" ...>
```

or:

```xml
<Window pos="100,80" ...>
```

The coordinates are the screen-space top-left position. An explicit position
overrides the default centering. If `size` is applied after native creation,
the framework re-centers only when there is no explicit position.

An omitted XML property is not emitted by `xml_to_code`; the framework default
is used instead. Do not add generated setter calls for omitted properties.

## Events

Bind events in `BindEvents()`, after the UI has been loaded or built:

```cpp
void MainForm::BindEvents()
{
    if (auto* button = ui::Find<ui::Button>(this, "hello_btn")) {
        button->AttachClick([this](const ui::EventArgs&) {
            if (auto* label = ui::Find<ui::Label>(this, "hello_label")) {
                label->SetText("Updated");
            }
            return true;
        });
    }
}
```

Use `ui::Find<T>()` instead of repeated `FindControl()` and
`dynamic_cast` code. Keep control names identical across XML, generated UI,
and pure-code UI so the event logic remains equivalent.

The generator intentionally skips event attributes and event nodes. Events
must remain in `MainForm::BindEvents()` or in a dedicated C++ helper.

Standard caption controls named `minbtn`, `maxbtn`, `restorebtn`, `closebtn`,
and `fullscreenbtn` are wired automatically by the framework. Do not manually
duplicate the framework's caption handling.

On macOS those caption buttons never render (the framework removes them and
injects the self-drawn traffic lights), so a macOS entry only needs the
caption bar placeholder. The macOS caption title is also framework-owned and
off by default: declare the window title with `text="..."` plus
`show_caption_title="true"` (styling via `caption_title_style`);
do not place a title Label in the macOS caption bar by hand. Keep names and
attributes identical across XML / generated / pure-code variants.

## Resources and CMake

### XML Mode

Use the common and binary resource CMake modules:

```cmake
include("${DUI_SRC_ROOT_DIR}/cmake/dui_app.cmake")

add_executable(<name>
    MainForm.cpp
    main.cpp
)
target_link_libraries(<name> PRIVATE dui::app)
dui_finalize_app(<name>)
```

`dui_app.cmake` includes `dui_common.cmake` itself. The application owns its target and
lists its sources — do not glob them. `dui_finalize_app` sets the output directory, the
Windows entry point, the macOS `.app` bundle and its code signature, and the build-order
dependencies. `examples/hello/CMakeLists.txt` is the smallest complete example.

The runtime resource directory is copied beside the executable.

### Code and Gen Modes

Embed the active platform theme, public assets, fonts, language resources, and
the example skin:

```cmake
set(EMBED_RES_DIR "${DUI_ROOT}/resources")
if(APPLE)
    set(EXAMPLE_THEME macos26)
elseif(WIN32)
    set(EXAMPLE_THEME windows11)
else()
    set(EXAMPLE_THEME default)
endif()
set(EMBED_RES_PATHS
    "themes/${EXAMPLE_THEME}/global.xml"
    "themes/${EXAMPLE_THEME}/public"
    "themes/${EXAMPLE_THEME}/<name>"
    "fonts"
    "lang")
include("${DUI_SRC_ROOT_DIR}/cmake/dui_embed_res.cmake")

add_executable(<name> ...)
target_link_libraries(<name> PRIVATE dui::app)
dui_finalize_app(<name>)
```

Every image referenced by an embedded layout must be present in the active
platform theme's embedded resource set. For macOS, use
`themes/macos26/<example>`; do not rely on a default-theme fallback. If an
asset is missing from the active theme, add the asset to that theme directory
and keep the XML references unchanged. For example, the chat modes embed:

```cmake
"themes/${CHAT_THEME}/chat"
```

The generated archive must be rebuilt after changing `EMBED_RES_PATHS` or
adding resource files. A successful compile alone does not prove that an image
can be loaded; verify the generated archive contains the referenced assets.

For gen mode, also configure generation before `dui_finalize_app()`:

```cmake
set(GEN_XML_FILES "${DUI_ROOT}/resources/themes/${EXAMPLE_THEME}/<name>/<name>.xml")
set(GEN_FUNC_NAME Init)
# Optional: one resource folder per XML file, relative to the active theme.
# set(GEN_RESOURCE_FOLDERS <name>)
include("${DUI_SRC_ROOT_DIR}/cmake/dui_gen_code.cmake")
```

The generated function name is `Init` plus the XML file stem, for example
`hello.xml` produces `InitHello()`.

## ChildWindow and Other Special Behavior

Specialized examples may add unique methods and interfaces, but their common
window lifecycle must still follow the Hello structure. For example,
ChildWindow keeps these specialized responsibilities:

- `CreateChildWindows()` and `CloseChildWindows()` manage child-window events
- `PaintChildWindow()` and `PaintNextChildWindow()` manage drawing
- `OnInitLayout()` creates child-window associations after layout exists
- `OnPreCloseWindow()` closes child windows before base cleanup
- `OnLayeredWindowChanged()` propagates layered state

Continuous drawing belongs in the shared `ui::Run`/`ui::RunMemory` idle callback:

```cpp
[](MainForm* window) {
    window->PaintNextChildWindow();
}
```

Do not move this logic into a duplicate application thread class.

### Library-Owned Window Exceptions

Some examples demonstrate a library-owned window rather than an application
window with three independently implemented layouts. `ColorPicker` is such an
example: `ui::ColorPicker` owns its private XML skin and its private control
construction. The example wrapper only supplies the initial color and public
callbacks.

For this kind of example:

- Keep the wrapper class small and derive from the library window class
- Do not invent an unrelated `MainForm::BuildUI()` that duplicates library UI
- XML mode uses `ui::Run` and disk resources
- Code/gen variants may use `ui::RunMemory` when their resource pipeline embeds
  the library-owned skin
- Treat the library class and its skin as the source of truth
- Document the exception in the example rather than pretending it is a normal
  three-layout example

## Change Checklist

Before completing an example change, verify:

- `main.cpp` uses `ui::Run` or `ui::RunMemory`, not a local app thread
- XML mode points `GetSkinFolder()` and `GetSkinFile()` to the correct skin
- Code mode uses `SetupWindow()` and `BuildUI()` with `SetWindowSize()`
- Code mode recenters after changing the native window size when no position is set
- Gen mode calls the generated `Init<Name>(this)` function
- Gen mode has no duplicated window properties already present in XML
- Events are bound in `BindEvents()` with stable control names
- Window position defaults to center unless `position` or `pos` is specified
- XML changes are made under `resources/`, never under `bin/`
- Gen changes regenerate `generated_ui.inc`
- Code and gen resources include the active theme and the example skin
- If an active theme relies on shared fallback assets, embed the fallback skin path too
- The affected target builds successfully
