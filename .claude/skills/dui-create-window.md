---
name: nim-create-window
description: Create a new dui window (generate a C++ window class + XML layout file + update the main thread)
trigger: Triggered when the user asks to create a new window, main form, dialog, or UI
---

# Create a New Window in dui

## Steps

### 1. Confirm the parameters
Confirm the following details with the user (if not already provided):
- **Window name**: the C++ class name (e.g., `SettingsForm`)
- **Skin directory name**: the name of the directory containing the XML file (e.g., `settings`)
- **Window title**: the title text displayed in the window
- **Window size**: defaults to "800,600"
- **Whether a caption bar is needed**: defaults to Yes (with minimize/maximize/close buttons)

### 2. Generate the C++ header file
Create `<FormName>.h` in `examples/<project>/` or a location specified by the user:

```cpp
#ifndef EXAMPLES_<FORM_NAME_UPPER>_H_
#define EXAMPLES_<FORM_NAME_UPPER>_H_

#include "dui/dui.h"

class <FormName> : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    <FormName>();
    virtual ~<FormName>() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void OnInitWindow() override;
};

#endif // EXAMPLES_<FORM_NAME_UPPER>_H_
```

### 3. Generate the C++ implementation file
Create `<FormName>.cpp`:

```cpp
#include "<FormName>.h"

<FormName>::<FormName>() {}
<FormName>::~<FormName>() {}

DString <FormName>::GetSkinFolder()
{
    return _T("<skin_folder>");
}

DString <FormName>::GetSkinFile()
{
    return _T("<skin_file>.xml");
}

void <FormName>::OnInitWindow()
{
    BaseClass::OnInitWindow();
    // TODO: initialize controls and bind events
}
```

### 4. Generate the XML layout file
Create `<skin_file>.xml` under `bin/resources/themes/default/<skin_folder>/`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Window size="<width>,<height>" min_size="240,100"
        caption="0,0,0,36" use_system_caption="false"
        snap_layout_menu="true" sys_menu="true" sys_menu_rect="0,0,36,36"
        shadow_type="default" shadow_attached="true"
        layered_window="true" alpha="255" size_box="4,4,4,4"
        icon="../public/caption/logo.ico">
    <VBox bkcolor="bk_wnd_darkcolor">
        <!-- Caption bar -->
        <HBox name="window_caption_bar" width="stretch" height="36" bkcolor="bk_wnd_lightcolor">
            <Label text="<WindowTitle>" margin="12,0,0,0" valign="center" normal_text_color="white"/>
            <Control />
            <Button class="btn_wnd_min_11" height="32" width="40" name="minbtn" margin="0,2,0,2"/>
            <Box height="stretch" width="40" margin="0,2,0,2">
                <Button class="btn_wnd_max_11" height="32" width="stretch" name="maxbtn"/>
                <Button class="btn_wnd_restore_11" height="32" width="stretch" name="restorebtn" visible="false"/>
            </Box>
            <Button class="btn_wnd_close_11" height="stretch" width="40" name="closebtn" margin="0,0,0,2"/>
        </HBox>
        <!-- Content area -->
        <Box padding="12,12,12,12">
            <VBox>
                <!-- TODO: add content controls here -->
                <Label text="<WindowTitle>" text_align="hcenter,vcenter" height="100%" width="100%"/>
            </VBox>
        </Box>
    </VBox>
</Window>
```

### 5. Code to create the window
Add the following wherever this window needs to be opened:

```cpp
#include "<FormName>.h"

// Create and show the window
<FormName>* window = new <FormName>();
window->CreateWnd(nullptr, ui::WindowCreateParam(_T("<WindowTitle>"), true));
window->ShowWindow(ui::kSW_SHOW_NORMAL);
// If this is the main window, add:
// window->PostQuitMsgWhenClosed(true);
```

### 6. Checklist
- [ ] The .h and .cpp files have been added to the VS project or CMakeLists.txt
- [ ] The XML file is encoded in UTF-8
- [ ] The XML file path matches the return values of GetSkinFolder()/GetSkinFile()
- [ ] If it is an example project, a reference must be added in the vcxproj
