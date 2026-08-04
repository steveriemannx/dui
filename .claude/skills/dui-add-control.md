---
name: nim-add-control
description: Add controls to a dui UI (XML + C++ event binding)
trigger: Triggered when the user asks to add controls such as buttons, text inputs, lists, checkboxes, etc.
---

# Add a Control in dui

## Steps

### 1. Confirm the control requirements
- Control type (button, text input, list, etc.)
- Placement (which XML file, which container)
- Whether C++ event handling is needed

### 2. Control XML quick reference

**Button:**
```xml
<Button name="btn_submit" text="Submit" width="80" height="32"
        class="btn_global_blue_80x30" tooltip_text="Click to submit"/>
```

**Label:**
```xml
<Label name="lbl_info" text="Info text" font="system_14"
       normal_text_color="default_font_color" text_align="left,vcenter"/>
```

**RichEdit (single-line):**
```xml
<RichEdit name="edit_input" width="200" height="30"
          single_line="true" prompt_text="Please enter..."
          font="system_12" text_padding="4,2,4,2"
          border_size="1" border_color="gray" border_round="4,4"
          hot_border_color="blue" focus_border_color="blue"/>
```

**RichEdit (multi-line):**
```xml
<RichEdit name="edit_content" width="stretch" height="200"
          multi_line="true" word_wrap="true" vscrollbar="true"
          want_return="true" prompt_text="Please enter content..."
          border_size="1" border_color="gray"/>
```

**Password input:**
```xml
<RichEdit name="edit_pwd" width="200" height="30"
          single_line="true" password="true" prompt_text="Please enter your password"
          border_size="1" border_color="gray" border_round="4,4"/>
```

**CheckBox:**
```xml
<CheckBox name="chk_agree" text="I agree to the terms" class="checkbox_2"/>
```

**Option button:**
```xml
<Option name="opt_male" text="Male" group="gender" class="option_1" selected="true"/>
<Option name="opt_female" text="Female" group="gender" class="option_1"/>
```

**Combo box:**
```xml
<Combo name="combo_type" class="combo" width="150" height="30"/>
```

**Progress bar:**
```xml
<Progress name="prog_download" width="stretch" height="16"
          min="0" max="100" value="0"
          bkcolor="lightgray" progress_color="blue" border_round="8,8"/>
```

**Slider:**
```xml
<Slider name="slider_volume" width="200" height="20"
        min="0" max="100" value="50" step="1"/>
```

**VListBox (list):**
```xml
<VListBox name="list_items" width="stretch" height="stretch"
          vscrollbar="true" class="list"/>
```

**TreeView:**
```xml
<TreeView name="tree_files" width="stretch" height="stretch"
          vscrollbar="true" class="tree_view"/>
```

**Line (separator):**
```xml
<Line line_color="gray" line_width="1" height="1"/>           <!-- Horizontal line -->
<Line vertical="true" line_color="gray" line_width="1" width="1"/>  <!-- Vertical line -->
```

**DateTime:**
```xml
<DateTime name="dt_start" width="160" height="30"
          format="%Y-%m-%d" edit_format="date_calendar"/>
```

**HyperLink:**
```xml
<HyperLink text="Visit the website" url="https://example.com"
           normal_text_color="blue" cursor_type="hand"/>
```

**IPAddress:**
```xml
<IPAddress name="ip_server" width="200" height="30" ip="192.168.1.1"/>
```

**TabCtrl + TabBox tabs:**
```xml
<VBox>
    <TabCtrl name="tab_ctrl" height="32">
        <TabCtrlItem text="Tab 1" selected="true"/>
        <TabCtrlItem text="Tab 2"/>
    </TabCtrl>
    <TabBox name="tab_box">
        <VBox><!-- Tab 1 content --></VBox>
        <VBox visible="false"><!-- Tab 2 content --></VBox>
    </TabBox>
</VBox>
```

### 3. C++ event binding code

Add the following in the window's `OnInitWindow()`:

```cpp
// Button click
if (auto* btn = dynamic_cast<ui::Button*>(FindControl(_T("btn_submit")))) {
    btn->AttachClick([this](const ui::EventArgs&) {
        // Handle the click
        return true;
    });
}

// CheckBox
if (auto* chk = dynamic_cast<ui::CheckBox*>(FindControl(_T("chk_agree")))) {
    chk->AttachSelect([this](const ui::EventArgs&) { /* selected */ return true; });
    chk->AttachUnSelect([this](const ui::EventArgs&) { /* deselected */ return true; });
}

// Text change in the input box
if (auto* edit = dynamic_cast<ui::RichEdit*>(FindControl(_T("edit_input")))) {
    edit->AttachTextChange([this](const ui::EventArgs&) {
        // Text changed
        return true;
    });
}

// Combo selection changed
if (auto* combo = dynamic_cast<ui::Combo*>(FindControl(_T("combo_type")))) {
    combo->AttachSelect([this](const ui::EventArgs& args) {
        size_t selIndex = args.wParam;
        return true;
    });
}

// List selection
if (auto* list = dynamic_cast<ui::ListBox*>(FindControl(_T("list_items")))) {
    list->AttachSelect([this](const ui::EventArgs& args) {
        size_t newSel = args.wParam;
        return true;
    });
}
```

### 4. Adding data dynamically

```cpp
// Add options to the Combo
if (auto* combo = dynamic_cast<ui::Combo*>(FindControl(_T("combo_type")))) {
    auto* treeView = combo->GetTreeView();
    auto* root = treeView->GetRootNode();
    for (int i = 0; i < 5; i++) {
        auto* node = new ui::TreeNode(this);
        node->SetClass(_T("tree_node"));
        node->SetText(ui::StringUtil::Printf(_T("Option %d"), i));
        root->AddChildNode(node);
    }
    combo->SetCurSel(0);
}

// Add items to the ListBox
if (auto* list = dynamic_cast<ui::ListBox*>(FindControl(_T("list_items")))) {
    for (int i = 0; i < 20; i++) {
        auto* item = new ui::ListBoxItem(this);
        item->SetClass(_T("listitem"));
        item->SetText(ui::StringUtil::Printf(_T("List item %d"), i));
        item->SetFixedHeight(ui::UiFixedInt(28), true, true);
        list->AddItem(item);
    }
}
```
