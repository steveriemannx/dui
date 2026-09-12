# Data Binding (`ui::binding`)

An optional module that binds a control's attributes to an object's properties, and
refreshes them when that object says something changed.

It is what makes an **MVVM** style possible on top of dui, but it is not tied to that
pattern: it binds *any* observable object, so an ordinary MVC-style model class can be
bound the same way. MVVM is one way to use it, not a requirement of it.

The module is **purely additive**. It adds files under `src/Binding` and
`include/dui/Binding` and modifies no existing library source. Code that does not call
it behaves exactly as before, and the imperative style (`FindControl` + `AttachClick` +
`SetText`) keeps working — the two can be mixed freely in the same window, and an
application can migrate one control at a time, or never.

## 1. Enabling it

| Option | Default | Meaning |
| :--- | :--- | :--- |
| `DUI_ENABLE_MVVM` | `OFF` | Compile the module into the `dui` library |
| `DUI_BUILD_MVVM_EXAMPLES` | `ON` | Build the `Binding` example |

The library is compiled once, at root scope, so an example cannot switch the module on
for the library by itself. `src/CMakeLists.txt` therefore builds the module when *either*
option is on — the same arrangement CEF uses (`DUI_ENABLE_CEF` / `DUI_BUILD_CEF_EXAMPLES`).

So `-DDUI_ENABLE_MVVM=ON` is enough, and the default build already contains the module
because the example is on by default.

## 2. Quick start

The source side is any class deriving from `ui::binding::ObservableObject`:

```cpp
class CounterViewModel : public ui::binding::ObservableObject
{
public:
    CounterViewModel()
    {
        // Registering makes the property readable and writable by name. C++ has no
        // reflection, so binding needs this small table to be built by hand.
        RegisterProperty(DUI_T("countText"), m_sCountText);
    }

    void Increase()
    {
        ++m_nCount;
        m_sCountText = ui::StringUtil::Printf(DUI_T("Count: %d"), m_nCount);
        RaisePropertyChanged(DUI_T("countText"));   // the only place that notifies
    }

private:
    int     m_nCount = 0;
    DString m_sCountText;
};
```

The view holds a `BindingGroup` as a member and declares its bindings in one place:

```cpp
class MyForm : public ui::WindowImplBase
{
    std::shared_ptr<CounterViewModel> m_pViewModel;
    ui::binding::BindingGroup        m_bindings;   // member: unbinds automatically
};

void MyForm::OnInitWindow()
{
    BaseClass::OnInitWindow();
    m_pViewModel = std::make_shared<CounterViewModel>();

    m_bindings.SetContext(m_pViewModel);
    m_bindings.Bind(this, DUI_T("count_label"), DUI_T("text"), DUI_T("countText"));
    m_bindings.BindCommand(this, DUI_T("btn_inc"), m_pViewModel, &CounterViewModel::Increase);
}
```

Nothing refreshes the label after that. Changing the counter and raising the property is
enough: every control bound to `countText` follows, and adding another display point is
one more `Bind` line rather than another place that has to remember to refresh.

## 3. Bindable attributes

The target name is the same string the layout XML and `Control::SetAttribute` use, so a
binding reads like the XML does.

| Attribute | Readable on | Reports changes (two-way possible) |
| :--- | :--- | :--- |
| `text` | anything implementing `LabelOwner`: `Label`, `LabelBox`/`LabelHBox`/`LabelVBox`, `Button`, `CheckBox`, `GroupBox`, `Option`, ... | only `RichEdit` (fires `kEventTextChanged`) |
| `value` | `Progress`, and `Slider` which derives from it | only `Slider` (fires `kEventValueChanged`) |
| `visible` | every `Control` | yes, every `Control` (`kEventVisibleChanged`) |
| `enabled` | every `Control` | no |
| `bkimage` | every `Control` | no |

Two-way binding is therefore a per **control type** capability, not a per attribute one:
`BindTwoWay` on a `Slider`'s `value` works, and on a plain `Progress` — which never reports
a change — it returns `false` and only the one-way half would have made sense.

This table is also a whitelist. `Bind` refuses an unknown attribute name by returning
`false`, because `Control::SetAttribute` silently ignores names it does not recognize
(it only asserts in debug builds), so without the whitelist a typo in a binding would be a
no-op that looks like it works.

## 4. Bindings

```cpp
// One-way: the control follows the source.
m_bindings.Bind(pControl, DUI_T("text"), DUI_T("title"));

// Two-way: user edits write back. Returns false when the control cannot report changes.
m_bindings.BindTwoWay(pControl, DUI_T("text"), DUI_T("title"));

// Targets can be resolved by name instead of held as a pointer:
m_bindings.Bind(this,        DUI_T("label_title"), DUI_T("text"), DUI_T("title"));  // in the window
m_bindings.Bind(pItemAsBox,  DUI_T("label_title"), DUI_T("text"), DUI_T("title"));  // in a subtree

// Commands: a click calls a member function. The member pointer is checked at compile
// time, and the source is held weakly, so a dead source is a no-op rather than a crash.
m_bindings.BindCommand(pButton, pViewModel, &MyViewModel::DoSomething);

m_bindings.RefreshAll();   // re-pull everything now
m_bindings.UnbindAll();    // detach everything; the destructor does this too
```

Name lookup behaves exactly like `FindControl`: case-sensitive, first match in traversal
order, with no uniqueness guarantee. An item template repeats its inner names
(`label_title`, ...) across every instance, so a list item must bind against itself —
never against the window.

## 5. Lists and recycled items

Bindings declared before a context exists are remembered and applied as soon as
`SetContext` is called. That is what lets a recycled list item declare its bindings once,
in its constructor, and only swap contexts on each fill:

```cpp
Item::Item(Window* pWindow) : ui::ListBoxItem(pWindow)
{
    ui::GlobalManager::Instance().FillBoxWithCache(this, ui::FilePath(DUI_T("list_box/item.xml")));

    // No context yet -- these are recorded and applied later.
    m_bindings.Bind(this, DUI_T("label_title"), DUI_T("text"), DUI_T("title"));
    m_bindings.Bind(this, DUI_T("progress"),    DUI_T("value"), DUI_T("progress"));
}

void DataProvider::FillElement(ui::Control* pControl, size_t nIndex)
{
    dynamic_cast<Item*>(pControl)->m_bindings.SetContext(m_itemViewModels[nIndex]);
}
```

Swapping the context re-resolves every binding against it, which is exactly the semantics
control recycling needs. This works with the existing `VirtualListBoxElement` provider
interface — no new collection abstraction is involved.

## 6. Using it next to the imperative style

Nothing has to be migrated. In the same window, and against the same model object:

```cpp
void MyForm::OnInitWindow()
{
    // Bound controls: no stored pointer, no refresh call.
    m_bindings.Bind(this, DUI_T("count_label"), DUI_T("text"), DUI_T("countText"));

    // Hand-wired control: unchanged from before this module existed.
    m_pLegacyLabel = ui::Find<ui::Label>(this, DUI_T("legacy_label"));
    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("btn_legacy"))) {
        pButton->AttachClick([this](const ui::EventArgs&) {
            m_pViewModel->Increase();
            UpdateLegacyLabel();      // has to be remembered at every mutation site
            return true;
        });
    }
}
```

The example program (`examples/Binding`) shows both side by side, including the failure
mode the imperative path invites: the hand-wired label goes stale when the counter is
changed from a place that forgot to refresh it.

## 7. Lifetime

- The `BindingGroup` must not outlive the controls it binds. Holding it as a member of the
  window class satisfies this: the window's members are destroyed before the framework
  frees the control tree, so `UnbindAll` runs while the controls are still alive.
- Each binding additionally remembers its target control's weak flag, so if a control dies
  first the binding skips its work instead of touching freed memory.
- The source object is held by `shared_ptr` (the group owns it), and commands capture it
  weakly. Destroying a `BindingGroup` releases the source it was holding.

## 8. Design boundaries

These are deliberate limits, not gaps to be filled later:

- **No expressions.** There is no `width * 0.5`. A value is a property path and nothing
  more. Qt needed a whole JavaScript engine for QML bindings; WinUI's `x:Bind` gets its
  power from a compile-time code generator and still has no arithmetic. This module has
  neither, so expressions are out of scope.
- **No type system.** Values cross the boundary as strings, the same way
  `Control::SetAttribute` takes them. A non-string property overrides `GetProperty` /
  `SetProperty` and parses with `StringUtil::StringToInt32` and friends, or the source
  exposes a second, pre-formatted property. `QDataWidgetMapper` makes the same trade.
- **Conversions are `std::function`.** A converter is ordinary C++ — checked by the
  compiler, debuggable, no string DSL to parse.
- **No XML syntax.** Bindings are declared in C++ only. Adding a `<Binding>` node later
  would be a thin layer over the same `Bind` call; the reverse (removing it) would not be.
- **Bindings are observers.** They always return `true` from the event handlers they
  attach, so they never consume an event, never suppress bubbling, and never change the
  event flow of the code they observe.

## 9. Example

`examples/Binding` — a counter bound to two labels, a progress bar, a visibility toggle
and three command buttons, with one hand-wired button/label pair alongside them.

## See also

- [Three Development Modes](ThreeModes.md) — binding works in all three (XML, generated, pure code); it is an orthogonal axis to how the UI is built.
- [Control Events/Messages](Events.md) — the events two-way binding relies on.
- [Control Styles](Control.md) — the attribute names used as binding targets.
