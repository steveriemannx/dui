# dui LLM Quick Reference

> This document is optimized for LLM/AI agents, providing a quick reference to dui's core APIs.

## 1. XML Layout Structure

### Window Template
```xml
<?xml version="1.0" encoding="UTF-8"?>
<Window size="800,600" min_size="80,60"
        caption="0,0,0,36" use_system_caption="false"
        snap_layout_menu="true" sys_menu="true" sys_menu_rect="0,0,36,36"
        shadow_type="default" shadow_attached="true"
        layered_window="true" alpha="255" size_box="4,4,4,4"
        icon="../public/caption/logo.ico">
  <VBox bkcolor="bk_wnd_darkcolor">
    <!-- Caption bar -->
    <HBox name="window_caption_bar" width="stretch" height="36" bkcolor="bk_wnd_lightcolor">
      <Control />
      <Button class="btn_wnd_min_11" name="minbtn" height="32" width="40" margin="0,2,0,2"/>
      <Box height="stretch" width="40" margin="0,2,0,2">
        <Button class="btn_wnd_max_11" name="maxbtn" height="32" width="stretch"/>
        <Button class="btn_wnd_restore_11" name="restorebtn" height="32" width="stretch" visible="false"/>
      </Box>
      <Button class="btn_wnd_close_11" name="closebtn" height="stretch" width="40"/>
    </HBox>
    <!-- Content area -->
    <Box>
      <!-- Place your content here -->
    </Box>
  </VBox>
</Window>
```

### Window Attribute Quick Reference
| Attribute | Type | Description |
|------|------|------|
| size | size | Initial window size, supports percentages like "75%,75%" |
| min_size / max_size | size | Minimum/maximum size |
| caption | rect | Draggable area of the caption bar "0,0,0,36" |
| size_box | rect | Draggable resize margins "4,4,4,4" |
| shadow_type | string | Shadow type: default/big/big_round/small/small_round/menu/menu_round/none/none_round |
| shadow_attached | bool | Whether to attach a shadow |
| layered_window | bool | Whether it is a layered window |
| alpha | int | Transparency 0-255 |
| round_corner | size | Window corner radius "4,4" |
| icon | string | Window icon path (ico) |
| use_system_caption | bool | Use the system caption bar |
| text / textid | string | Window title / multi-language ID |

## 2. Container Type Quick Reference

| XML Node | Layout | Description |
|---------|---------|------|
| Box | Floating (Layout) | Free positioning; children use absolute or relative layout |
| VBox | Vertical (VLayout) | Children are arranged from top to bottom |
| HBox | Horizontal (HLayout) | Children are arranged from left to right |
| VFlowBox | Vertical flow | Arranged vertically, automatically wraps to new columns |
| HFlowBox | Horizontal flow | Arranged horizontally, automatically wraps to new rows |
| VTileBox | Vertical tile | Grid-style vertical arrangement, uses the columns attribute |
| HTileBox | Horizontal tile | Grid-style horizontal arrangement, uses the rows attribute |
| GridBox | Grid (GridLayout) | Grid layout, supports cell merging |
| TabBox | Floating | Multi-tab switching, only the current page is shown |
| ScrollBox | Floating + scrollbar | A scrollable Box |
| VScrollBox | Vertical + scrollbar | A scrollable VBox |
| HScrollBox | Horizontal + scrollbar | A scrollable HBox |
| VListBox | Vertical list | A selectable vertical list |
| HListBox | Horizontal list | A selectable horizontal list |
| VirtualVListBox | Virtual vertical list | Virtual list for large data sets (vertical) |
| VirtualHListBox | Virtual horizontal list | Virtual list for large data sets (horizontal) |

### Container Attributes
| Attribute | Type | Description |
|------|------|------|
| child_margin | int | Spacing between children (same for X and Y) |
| child_margin_x | int | Horizontal spacing between children |
| child_margin_y | int | Vertical spacing between children |
| child_halign | string | Horizontal alignment of children: left/center/right |
| child_valign | string | Vertical alignment of children: top/center/bottom |
| mouse_child | bool | Whether children respond to the mouse |
| padding | rect | Padding "L,T,R,B" |

## 3. Control Type Quick Reference

| XML Node | Base Class | Description |
|---------|------|------|
| Control | - | Base control / placeholder |
| Label | Control | Text label |
| Button | Label | Button |
| CheckBox | Button | Checkbox |
| Option | CheckBox | Option button (grouped via the group attribute) |
| Combo | Box | Drop-down combo box |
| FilterCombo | Combo | Filterable drop-down combo box |
| CheckCombo | Box | Multi-select combo box |
| ComboButton | Box | Button with a drop-down |
| RichEdit | ScrollBox | Text edit box (single-line/multi-line/password) |
| RichText | Control | Rich text display (HTML subset) |
| Progress | Label | Progress bar |
| Slider | Progress | Slider |
| CircleProgress | Progress | Circular progress bar |
| DateTime | Label | Date/time picker |
| TreeView | VListBox | Tree view control |
| TreeNode | Box | Tree node |
| ListCtrl | VBox | List control (Report/List/Icon views) |
| PropertyGrid | VListBox | Property grid |
| HyperLink | Label | Hyperlink |
| Line | Control | Line drawing control |
| Split / SplitBox | Control/Box | Splitter |
| ScrollBar | Control | Scrollbar |
| TabCtrl | HBox | Tab control |
| IPAddress | HBox | IP address input |
| HotKey | HBox | Hotkey input |
| GroupBox/GroupVBox/GroupHBox | Box | Grouping container |
| ColorControl/ColorPicker* | - | Color picker component |
| DirectoryTree | TreeView | Directory tree |

### Control Common Attributes (inherited by all controls)
| Attribute | Default | Type | Description |
|------|--------|------|------|
| name | | string | Control name (recommended to be unique within the window) |
| class | | string | References a common style in global.xml |
| width | stretch | int/string | Width: number/stretch/auto/"50%" |
| height | stretch | int/string | Height: number/stretch/auto/"50%" |
| min_width / min_height | -1 | int | Minimum width/height |
| max_width / max_height | MAX | int/string | Maximum width/height |
| margin | 0,0,0,0 | rect | Outer margin "L,T,R,B" |
| padding | 0,0,0,0 | rect | Padding "L,T,R,B" |
| halign | left | string | Horizontal alignment: left/center/right |
| valign | top | string | Vertical alignment: top/center/bottom |
| visible | true | bool | Whether visible |
| enabled | true | bool | Whether enabled |
| float | false | bool | Whether absolutely positioned |
| bkcolor | | string | Background color (color name/ARGB) |
| bkimage | | string | Background image |
| normal_image / hot_image / pushed_image / disabled_image | | string | Images for each state |
| normal_color / hot_color / pushed_color / disabled_color | | string | Colors for each state |
| border_size | 0 | int/rect | Border size |
| border_color | | string | Border color |
| border_round | 0,0 | size | Border corner radius |
| tooltip_text | | string | Mouse hover tooltip |
| alpha | 255 | int | Transparency 0-255 |
| cursor_type | arrow | string | Cursor: arrow/hand/ibeam/wait/cross/size_we/size_ns etc. |
| no_focus | false | bool | Whether focus cannot be obtained |
| tab_stop | true | bool | Whether TAB navigation is allowed |
| fade_visible | true | bool | Whether there is an animation when visibility changes |
| box_shadow | | string | Shadow "color='red' offset='0,0' blurradius='8' spreadradius='8'" |

### Label Attributes (inherits Control)
| Attribute | Default | Type | Description |
|------|--------|------|------|
| text | | string | Displayed text |
| text_id | | string | Multi-language ID |
| text_align | "left,top" | string | Alignment: left/hcenter/right/hjustify + top/vcenter/bottom |
| text_padding | 0,0,0,0 | rect | Text padding |
| font | | string | Font ID (defined in global.xml) |
| normal_text_color | | string | Normal text color |
| hot_text_color | | string | Hover text color |
| single_line | true | bool | Single-line display |
| multi_line | false | bool | Multi-line display |
| end_ellipsis | false | bool | Truncate with ellipsis |
| rich_text | false | bool | Supports HTML-subset rich text |

### CheckBox Extra Attributes (inherits Button)
| Attribute | Type | Description |
|------|------|------|
| selected | bool | Whether selected |
| selected_normal_image / selected_hot_image / ... | string | Images for each state when selected |

### Option Extra Attributes (inherits CheckBox)
| Attribute | Type | Description |
|------|------|------|
| group | string | Group name; mutually exclusive within the same group |

### RichEdit Attributes (inherits ScrollBox)
| Attribute | Default | Type | Description |
|------|--------|------|------|
| text | | string | Text content |
| font | | string | Font ID |
| text_align | "left,top" | string | Alignment |
| text_padding | | rect | Text padding |
| single_line | true | bool | Single-line mode |
| multi_line | false | bool | Multi-line mode |
| password | false | bool | Password mode |
| readonly | false | bool | Read-only |
| number_only | false | bool | Numbers only |
| max_number / min_number | | int | Numeric range |
| limit_text | | int | Maximum number of characters |
| prompt_text | | string | Placeholder prompt text |
| prompt_color | | string | Prompt text color |
| word_wrap | false | bool | Automatic word wrap |
| vscrollbar / hscrollbar | false | bool | Scrollbar |
| want_return | false | bool | Accept Enter key |
| want_tab | false | bool | Accept Tab key |
| caret_color | | string | Caret color |
| normal_text_color | | string | Text color |

### Progress Attributes (inherits Label)
| Attribute | Default | Type | Description |
|------|--------|------|------|
| min | 0 | int | Minimum value |
| max | 100 | int | Maximum value |
| value | 0 | int | Current value |
| horizontal | true | bool | Horizontal direction |
| progress_color | | string | Progress bar color |
| progress_image | | string | Progress bar image |

### Combo Attributes (inherits Box)
| Attribute | Default | Type | Description |
|------|--------|------|------|
| combo_type | "drop_down" | string | "drop_down" (editable) / "drop_list" (not editable) |
| dropbox_size | | string | Drop-down list size |
| popup_top | false | bool | Pop up upward |

## 4. Global Resources (global.xml)

### Font Definition
```xml
<DefaultFontFamilyNames value="Microsoft YaHei,SimSun"/>
<Font id="system_12" name="system" size="12"/>
<Font id="system_bold_14" name="system" size="14" bold="true"/>
<FontFile file="RobotoMono-Regular.ttf" desc="Roboto Mono Regular"/>
```

### Color Definition
```xml
<TextColor name="default_font_color" value="#FF333333"/>
<TextColor name="bk_wnd_darkcolor" value="#FF2B2B2B"/>
```
Color format: "#AARRGGBB" (ARGB), "#RRGGBB" (RGB), or a color name (Blue/Red/White...)

### Common Style (Class)
```xml
<Class name="btn_default" font="system_12" normal_text_color="white"
       normal_image="file='btn_normal.png'"
       hot_image="file='btn_hot.png'"
       pushed_image="file='btn_pushed.png'"/>
```
Usage: `<Button class="btn_default" text="Click"/>`
**The class attribute must be written before all other attributes**

## 5. Image Attributes
```xml
<!-- Simple usage -->
<Control bkimage="logo.png"/>

<!-- Full attributes -->
<Control bkimage="file='icon.svg' width='24' height='24' valign='center' halign='center'"/>
```

| Attribute | Type | Description |
|------|------|------|
| file | string | Image path (relative to the theme directory) |
| width / height | string | Image size (pixels or percentage) |
| src | rect | Source region crop "L,T,R,B" |
| dest | rect | Target draw region |
| corner | rect | Nine-patch parameters "L,T,R,B" |
| fade | int | Transparency 0-255 |
| halign / valign | string | Alignment |
| xtiled / ytiled | bool | Tiled drawing |
| auto_play | bool | Animation auto-play |
| play_count | int | Animation play count (-1 = infinite) |

Supported formats: PNG, SVG, JPG, GIF, BMP, APNG, WEBP, ICO, Lottie-JSON, PAG

## 6. XML Event System

### XML Inline Events
```xml
<Button name="my_btn" text="Click">
  <Event type="click" receiver="target_control_name" apply_attribute="visible='true'"/>
</Button>

<!-- Event target types -->
<!-- receiver="name"           Find a control by name -->
<!-- receiver="./name"         Find within the current container -->
<!-- receiver=""               The control itself -->
<!-- receiver="#window#"       The window -->
```

### Common Event Types
| type value | Description |
|--------|------|
| click | Click |
| rclick | Right-click |
| mouse_enter / mouse_leave | Mouse enter/leave |
| mouse_button_down / mouse_button_up | Mouse button press/release |
| mouse_double_click | Double-click |
| select / unselect | Select/unselect (ListBox/Combo) |
| check / uncheck | Check/uncheck (CheckBox) |
| tab_select | Tab switching |
| text_changed | Text changed |
| value_changed | Value changed |
| key_down / key_up | Key press |
| return | Enter key |
| visible_changed | Visibility changed |
| window_close | Window close |
| window_size | Window size changed |

## 7. C++ Code Mode

### Window Class Template
```cpp
// MyForm.h
#ifndef MY_FORM_H_
#define MY_FORM_H_
#include "dui/dui.h"

class MyForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MyForm();
    virtual ~MyForm() override;
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void OnInitWindow() override;
};
#endif

// MyForm.cpp
#include "MyForm.h"

MyForm::MyForm() {}
MyForm::~MyForm() {}

DString MyForm::GetSkinFolder() { return _T("my_skin"); }
DString MyForm::GetSkinFile() { return _T("my_form.xml"); }

void MyForm::OnInitWindow()
{
    BaseClass::OnInitWindow();
    // Initialize controls and bind events here
}
```

### Main Thread Template
```cpp
// MainThread.h
#include "dui/dui.h"
class MainThread : public ui::FrameworkThread
{
public:
    MainThread();
    virtual ~MainThread() override;
private:
    virtual void OnInit() override;
    virtual void OnCleanup() override;
};

// MainThread.cpp
#include "MainThread.h"
#include "MyForm.h"

MainThread::MainThread() : FrameworkThread(_T("MainThread"), ui::kThreadUI) {}
MainThread::~MainThread() {}

void MainThread::OnInit()
{
    ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
    resourcePath += _T("resources\\");
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    MyForm* window = new MyForm();
    window->CreateWnd(nullptr, ui::WindowCreateParam(_T("MyApp"), true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
}

void MainThread::OnCleanup()
{
    ui::GlobalManager::Instance().Shutdown();
}
```

### Entry Function (Windows)
```cpp
#include "MainThread.h"
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    MainThread thread;
    thread.RunMessageLoop();
    return 0;
}
```

### Control Operations
```cpp
// Find a control
ui::Button* btn = dynamic_cast<ui::Button*>(FindControl(_T("my_button")));
ui::Label* label = dynamic_cast<ui::Label*>(FindControl(_T("my_label")));
ui::RichEdit* edit = dynamic_cast<ui::RichEdit*>(FindControl(_T("my_edit")));
ui::CheckBox* check = dynamic_cast<ui::CheckBox*>(FindControl(_T("my_check")));
ui::Combo* combo = dynamic_cast<ui::Combo*>(FindControl(_T("my_combo")));
ui::ListBox* list = dynamic_cast<ui::ListBox*>(FindControl(_T("my_list")));
ui::Progress* progress = dynamic_cast<ui::Progress*>(FindControl(_T("my_progress")));

// Set attributes
label->SetText(_T("Hello"));
edit->SetText(_T("Input"));
DString text = edit->GetText();
check->SetSelected(true);
bool isChecked = check->IsSelected();
progress->SetValue(50);

// Visibility
btn->SetVisible(true);
btn->SetEnabled(false);
```

### Event Binding
```cpp
void MyForm::OnInitWindow()
{
    BaseClass::OnInitWindow();

    // Button click
    ui::Button* btn = dynamic_cast<ui::Button*>(FindControl(_T("btn_ok")));
    if (btn) {
        btn->AttachClick([this](const ui::EventArgs& args) {
            // Handle logic
            return true;
        });
    }

    // Checkbox state change
    ui::CheckBox* check = dynamic_cast<ui::CheckBox*>(FindControl(_T("my_check")));
    if (check) {
        check->AttachSelect([this](const ui::EventArgs& args) {
            // Selected
            return true;
        });
        check->AttachUnSelect([this](const ui::EventArgs& args) {
            // Unselected
            return true;
        });
    }

    // Text change
    ui::RichEdit* edit = dynamic_cast<ui::RichEdit*>(FindControl(_T("my_edit")));
    if (edit) {
        edit->AttachTextChange([this](const ui::EventArgs& args) {
            // Text changed
            return true;
        });
    }

    // List selection
    ui::ListBox* list = dynamic_cast<ui::ListBox*>(FindControl(_T("my_list")));
    if (list) {
        list->AttachSelect([this](const ui::EventArgs& args) {
            size_t newIndex = args.wParam;
            size_t oldIndex = args.lParam;
            return true;
        });
    }

    // Generic event binding
    btn->AttachEvent(ui::kEventMouseEnter, [](const ui::EventArgs&) {
        return true;
    });
}
```

### ListBox Dynamically Adding Items
```cpp
ui::ListBox* list = dynamic_cast<ui::ListBox*>(FindControl(_T("my_list")));
for (int i = 0; i < 100; i++) {
    ui::ListBoxItem* item = new ui::ListBoxItem(this);
    item->SetText(ui::StringUtil::Printf(_T("Item %d"), i));
    item->SetClass(_T("listitem"));
    item->SetFixedHeight(ui::UiFixedInt(20), true, true);
    list->AddItem(item);
}
```

### TreeView Dynamically Adding Nodes
```cpp
ui::TreeView* tree = dynamic_cast<ui::TreeView*>(FindControl(_T("my_tree")));
ui::TreeNode* root = tree->GetRootNode();
ui::TreeNode* node = new ui::TreeNode(this);
node->SetClass(_T("tree_node"));
node->SetText(_T("New Node"));
root->AddChildNode(node);
```

### Combo Dynamically Adding Options
```cpp
ui::Combo* combo = dynamic_cast<ui::Combo*>(FindControl(_T("my_combo")));
ui::TreeView* treeView = combo->GetTreeView();
ui::TreeNode* treeNode = treeView->GetRootNode();
for (int i = 0; i < 10; i++) {
    ui::TreeNode* node = new ui::TreeNode(this);
    node->SetClass(_T("tree_node"));
    node->SetText(ui::StringUtil::Printf(_T("Option %d"), i));
    treeNode->AddChildNode(node);
}
combo->SetCurSel(0); // Select the first item by default
```

### Inter-thread Communication
```cpp
// Post a task to the worker thread
ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadWorker,
    UiBind(&MyForm::DoBackgroundWork, this));

// Post a task to the UI thread
ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI,
    UiBind(&MyForm::UpdateUI, this));

// Scheduled repeating task
ui::GlobalManager::Instance().Thread().PostRepeatedTask(ui::kThreadWorker,
    ui::UiBind(this, [this]() {
        // Logic executed each time
    }),
    200 // interval in milliseconds
);
```

### Weak Callback Protection
```cpp
// Use UiBind to bind a member function (automatic weak reference protection)
UiBind(&MyForm::OnButtonClick, this);

// Wrap a lambda with ui::UiBind
ui::UiBind(this, [this]() {
    // Will not execute after the control is destroyed
});
```

## 8. Layout Attribute Quick Reference

### Tile Layout (HTileBox/VTileBox)
| Attribute | Type | Description |
|------|------|------|
| item_size | size | Item size "100,40" |
| rows (HTileBox) | int/"auto" | Number of rows |
| columns (VTileBox) | int/"auto" | Number of columns |
| scale_down | bool | Shrink when overflowing |

### Grid Layout (GridBox)
| Attribute | Type | Description |
|------|------|------|
| rows | int | Number of grid rows (0 = auto) |
| columns | int | Number of grid columns (0 = auto) |
| grid_width | int | Cell width (0 = auto) |
| grid_height | int | Cell height (0 = auto) |
Child controls use the `row_span`/`col_span` attributes to merge cells.

### ScrollBox Extra Attributes
| Attribute | Type | Description |
|------|------|------|
| vscrollbar | bool | Vertical scrollbar |
| hscrollbar | bool | Horizontal scrollbar |
| vscrollbar_class | string | Vertical scrollbar style |
| hscrollbar_class | string | Horizontal scrollbar style |

## 9. File Path Conventions
- Theme resource root directory: `bin/resources/themes/default/`
- Global configuration: `bin/resources/themes/default/global.xml`
- Window XML: `bin/resources/themes/default/<skin_folder>/<skin_file>.xml`
- Common images: `bin/resources/themes/default/public/`
- Font files: `bin/resources/fonts/`
- Language files: `bin/resources/themes/default/lang/` (zh_CN.txt, en_US.txt)
- Example source code: `examples/<example_name>/`
