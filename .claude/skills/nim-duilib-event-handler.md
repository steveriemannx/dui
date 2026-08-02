---
name: nim-event-handler
description: Add event handling to nim_duilib controls (supports XML inline events and C++ event binding)
trigger: Triggered when the user needs to handle control events, respond to user actions, or bind callbacks
---

# Event Handling in nim_duilib

## Method 1: XML inline events (no C++ code needed)

### Syntax
```xml
<Control name="source">
    <Event type="event_type" receiver="target_control_name" apply_attribute="attribute_name='attribute_value'"/>
</Control>
```

### receiver syntax
| Format | Meaning |
|------|------|
| `receiver="target_name"` | Finds a control in the window by name |
| `receiver="./target_name"` | Finds a control within the current container |
| `receiver=""` | The control itself |
| `receiver="#window#"` | The window object |
| `receiver="name1 name2"` | Multiple controls (space-separated) |

### Common examples

**Show/hide a panel on button click:**
```xml
<Button name="btn_toggle" text="Toggle">
    <Event type="click" receiver="panel" apply_attribute="visible='true'"/>
</Button>
```

**Switch content with option buttons:**
```xml
<Option group="tabs" text="Tab 1" selected="true">
    <Event type="select" receiver="page1" apply_attribute="visible='true'"/>
    <Event type="select" receiver="page2" apply_attribute="visible='false'"/>
</Option>
<Option group="tabs" text="Tab 2">
    <Event type="select" receiver="page1" apply_attribute="visible='false'"/>
    <Event type="select" receiver="page2" apply_attribute="visible='true'"/>
</Option>
```

**Switch style on mouse hover:**
```xml
<Control name="card" bkcolor="white">
    <Event type="mouse_enter" receiver="" apply_attribute="bkcolor='AliceBlue'"/>
    <Event type="mouse_leave" receiver="" apply_attribute="bkcolor='white'"/>
</Control>
```

**Play animation on mouse hover:**
```xml
<Control bkimage="file='fan.gif' auto_play='false' play_count='-1'">
    <Event type="mouse_enter" receiver="" apply_attribute="start_image_animation={}"/>
    <Event type="mouse_leave" receiver="" apply_attribute="stop_image_animation={}"/>
</Control>
```

**Change the window shadow type:**
```xml
<Option group="shadow" text="Large shadow">
    <Event type="select" receiver="#window#" apply_attribute="shadow_type={big}"/>
</Option>
```

**Change a container layout attribute:**
```xml
<Option group="align" text="Center aligned">
    <Event type="select" receiver="container" apply_attribute="child_halign='center'"/>
</Option>
```

## Method 2: C++ event binding

### Commonly used Attach methods
```cpp
// Bind in OnInitWindow()

// Click
control->AttachClick([this](const ui::EventArgs& args) -> bool {
    return true;
});

// Select/deselect (CheckBox/Option/ListBox)
control->AttachSelect([](const ui::EventArgs&) { return true; });
control->AttachUnSelect([](const ui::EventArgs&) { return true; });

// Text change (RichEdit)
edit->AttachTextChange([](const ui::EventArgs&) { return true; });

// Enter key (RichEdit)
edit->AttachReturn([](const ui::EventArgs&) { return true; });

// Tab selection (TabCtrl)
tabCtrl->AttachTabSelect([](const ui::EventArgs& args) {
    size_t newIndex = args.wParam;
    size_t oldIndex = args.lParam;
    return true;
});

// Value change (Progress/Slider)
slider->AttachEvent(ui::kEventValueChange, [](const ui::EventArgs&) { return true; });

// Generic event binding
control->AttachEvent(ui::kEventMouseEnter, handler);
control->AttachEvent(ui::kEventMouseLeave, handler);
control->AttachEvent(ui::kEventKeyDown, handler);
control->AttachEvent(ui::kEventSetFocus, handler);
control->AttachEvent(ui::kEventKillFocus, handler);
control->AttachEvent(ui::kEventWindowClose, handler);
```

### EventArgs fields
```cpp
struct EventArgs {
    EventType eventType;     // Event type
    size_t wParam;           // Parameter 1
    size_t lParam;           // Parameter 2
    UiPoint ptMouse;         // Mouse position
    uint16_t vkCode;         // Key code
    uint16_t modifierKey;    // Modifier key
    int64_t eventData;       // Extra data
    Control* pSender;        // Sender control
};
```

### Full list of event types
Keyboard & mouse: `kEventKeyDown`, `kEventKeyUp`, `kEventChar`, `kEventMouseEnter`, `kEventMouseLeave`, `kEventMouseMove`, `kEventMouseButtonDown`, `kEventMouseButtonUp`, `kEventMouseDoubleClick`, `kEventMouseRButtonDown`, `kEventMouseRButtonUp`, `kEventMouseWheel`, `kEventContextMenu`

Actions: `kEventClick`, `kEventRClick`, `kEventSelect`, `kEventUnSelect`, `kEventChecked`, `kEventUnCheck`, `kEventTabSelect`, `kEventExpand`, `kEventCollapse`

Editing: `kEventTextChange`, `kEventSelChange`, `kEventReturn`, `kEventTab`, `kEventZoom`

States: `kEventSetFocus`, `kEventKillFocus`, `kEventStateChange`, `kEventVisibleChange`, `kEventResize`

Window: `kEventWindowClose`, `kEventWindowSize`, `kEventWindowMove`, `kEventWindowKillFocus`

Drag & drop: `kEventDropEnter`, `kEventDropOver`, `kEventDropLeave`, `kEventDropData`

Animation: `kEventImageAnimationStart`, `kEventImageAnimationStop`

### Notes
- An event callback returning `true` means the event has been handled
- Use `UiBind` to bind member functions with weak reference safety
- When capturing `this` in a lambda, wrap it with `ui::UiBind(this, lambda)` to prevent the callback from crashing after the control is destroyed
