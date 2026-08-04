---
name: nim-xml-layout
description: Design and generate XML layouts for dui windows
trigger: Triggered when the user needs to design a UI layout, create an XML UI, or modify the UI structure
---

# XML Layout Design in dui

## Steps

### 1. Understand the requirements
Confirm the UI layout the user wants:
- Which areas the UI contains (caption bar, sidebar, content area, bottom bar, etc.)
- Which controls each area contains
- The layout style (vertical, horizontal, grid, etc.)

### 2. Choose the right container

**Layout selection guide:**
| Requirement | Recommended container |
|------|---------|
| Top-to-bottom arrangement | VBox |
| Left-to-right arrangement | HBox |
| Horizontal arrangement with automatic wrapping | HFlowBox |
| Equal-width grid | VTileBox / HTileBox |
| Row-and-column grid (supports merging) | GridBox |
| Scrolling needed | VScrollBox / HScrollBox |
| Selectable list | VListBox / HListBox |
| List with large amounts of data | VirtualVListBox |
| Multi-tab switching | TabBox |

**Size setting guide:**
| Requirement | width/height value |
|------|----------------|
| Fixed size | a number such as "200" |
| Fill the parent container | "stretch" (default) |
| Fit the content | "auto" |
| Percentage of the parent container | "50%" |

### 3. Common layout templates

**Form layout (label + input box):**
```xml
<VBox padding="20,20,20,20" child_margin_y="10">
    <HBox height="auto" child_margin_x="10">
        <Label text="User name:" width="80" height="30" text_align="right,vcenter"/>
        <RichEdit name="edit_username" width="stretch" height="30"
                  single_line="true" prompt_text="Please enter user name"
                  border_size="1" border_color="gray" border_round="4,4"/>
    </HBox>
    <HBox height="auto" child_margin_x="10">
        <Label text="Password:" width="80" height="30" text_align="right,vcenter"/>
        <RichEdit name="edit_password" width="stretch" height="30"
                  single_line="true" password="true" prompt_text="Please enter password"
                  border_size="1" border_color="gray" border_round="4,4"/>
    </HBox>
    <HBox height="auto" halign="right" child_margin_x="10" margin="0,20,0,0">
        <Button name="btn_cancel" text="Cancel" width="80" height="32" class="btn_global_white_80x30"/>
        <Button name="btn_ok" text="OK" width="80" height="32" class="btn_global_blue_80x30"/>
    </HBox>
</VBox>
```

**Left/right split layout:**
```xml
<HBox>
    <!-- Left panel -->
    <VBox width="200" bkcolor="bk_wnd_lightcolor">
        <VListBox name="nav_list" width="stretch" height="stretch"/>
    </VBox>
    <!-- Separator -->
    <Line vertical="true" line_color="gray" line_width="1" width="1"/>
    <!-- Right content -->
    <VBox width="stretch" padding="12,12,12,12">
        <Label text="Content area" text_align="hcenter,vcenter"/>
    </VBox>
</HBox>
```

**Toolbar + content:**
```xml
<VBox>
    <!-- Toolbar -->
    <HBox height="40" bkcolor="bk_wnd_lightcolor" padding="4,4,4,4" child_margin_x="4">
        <Button name="btn_new" text="New" width="auto" height="stretch"/>
        <Button name="btn_open" text="Open" width="auto" height="stretch"/>
        <Button name="btn_save" text="Save" width="auto" height="stretch"/>
        <Line vertical="true" line_color="gray" width="1" margin="4,0,4,0"/>
        <Control /><!-- flexible spacer -->
        <RichEdit name="search_edit" width="200" height="28" single_line="true"
                  prompt_text="Search..." valign="center" border_size="1" border_color="gray"/>
    </HBox>
    <!-- Content -->
    <Box width="stretch" height="stretch">
        <!-- Content area -->
    </Box>
</VBox>
```

**Card grid:**
```xml
<VTileScrollBox columns="3" item_size="200,150" child_margin="10"
                padding="10,10,10,10" vscrollbar="true">
    <Box bkcolor="white" border_size="1" border_color="gray" border_round="8,8">
        <VBox padding="12,12,12,12">
            <Label text="Card 1" font="system_bold_14"/>
            <Label text="Description text" normal_text_color="gray"/>
        </VBox>
    </Box>
    <!-- More cards... -->
</VTileScrollBox>
```

### 4. Key rules
- The XML file encoding must be **UTF-8**
- The `class` attribute must be written before **all other attributes**
- For embedded quotes in attribute values, use **single quotes `'`** or **curly braces `{}`** instead of double quotes
- `<Control />` acts as a flexible placeholder, taking the remaining space in an HBox/VBox
- The `name` of the window caption bar buttons must be: `minbtn`, `maxbtn`, `restorebtn`, `closebtn`, `fullscreenbtn`
- Color value format: "#AARRGGBB" (ARGB) or a color name

### 5. `<Include>` shared XML fragments

dui supports reusing XML fragments with `<Include src="fragment.xml"/>`, letting multiple windows share the same piece of layout (for example, a common status bar or toolbar).

**Path resolution rules** (`WindowBuilder.cpp:1097-1108`)
- `src` is resolved relative to the **directory of the host XML**; if not found, it falls back to the window resource path
- It is recommended to place the shared fragment in the **same skin directory** as the host XML; writing `src="fragment.xml"` is the most trouble-free

**Root node of the included XML**
- It is recommended to wrap the fragment in a single root `<Window>` (to satisfy pugixml's single-root requirement); window attributes on `<Window>` are ignored by the `IsWindowAttributesApplied()` guard, so **do not write attributes such as size/caption/layered_window on the included `<Window>`**, otherwise it may mislead
- Alternatively, a Box can be used directly as the root (HBox/VBox/Box); it works in practice as well

**`FindControl` scope**
- Addressing is independent per Window; two different Windows using the same `name` **will not conflict** — this is the basis for reusing shared fragments across windows

**⚠️ Critical pitfall: `<Include>` must not be the only direct child of `<Window>`**

`WindowBuilder::ParseXmlNodeChildren` (`WindowBuilder.cpp:1077-1124`) uses `continue` to skip the `pReturn = pControl` assignment block that follows (lines 1229-1232) when processing `<Include>`. **When `<Include>` is the first non-style child node under the window root `<Window>`, `CreateControls` returns nullptr, so `GlobalManager` does not create a root Box for that Window, and the window is silently created completely empty — invisible, with no controls at all**. The framework raises no error and prints no warning; the window simply has nothing.

```xml
<!-- ❌ Wrong: the HUD/small window is invisible and all controls are lost -->
<Window size="900,32" layered_window="true" ...>
    <Include src="status_bar.xml"/>
</Window>

<!-- ✅ Correct: wrap it in an HBox/VBox/Box -->
<Window size="900,32" layered_window="true" ...>
    <HBox>
        <Include src="status_bar.xml"/>
    </HBox>
</Window>
```

If `<Include>` already has a Box/control sibling node before it, this pitfall is avoided (because the first non-style child node already occupies pReturn). **But as soon as another window with the same layout puts the Include directly under the root, it breaks** — always wrapping shared fragments in a Box layer is the safest practice.

**Complete example**

```xml
<!-- status_bar.xml: shared fragment -->
<?xml version="1.0" encoding="UTF-8"?>
<Window>
    <HBox name="status_bar_root" height="32" bkcolor="bg_statusbar">
        <Label name="status_state" text="Ready" />
        <!-- ... -->
    </HBox>
</Window>

<!-- host_a.xml: main window (Include embedded in a VBox, safe) -->
<Window ...>
    <VBox>
        <!-- ...main content... -->
        <Include src="status_bar.xml"/>
    </VBox>
</Window>

<!-- host_b.xml: HUD small window (Include must be wrapped in a Box) -->
<Window size="900,32" layered_window="true" ...>
    <HBox>
        <Include src="status_bar.xml"/>
    </HBox>
</Window>
```
