---
name: dui-theme
description: Customize dui themes (colors, fonts, common style classes, icons), modify global.xml, and create custom styles
---

# Theme and Style Customization in dui

## Important: resource file rules

### Resources a new project must copy (minimal set)
```
bin/resources/
├── themes/default/
│   ├── global.xml                    # required: global style definitions
│   ├── public/                       # required: shared image resources (133 files)
│   │   ├── button/                   # window button icons (SVG)
│   │   ├── caption/                  # caption bar resources
│   │   ├── checkbox/                 # checkbox icons
│   │   ├── combo/                    # combo box icons
│   │   ├── option/                   # option button icons
│   │   ├── scrollbar01/              # scrollbar style 1
│   │   ├── scrollbar02/              # scrollbar style 2
│   │   ├── shadow/                   # window shadow
│   │   ├── slider/                   # slider resources
│   │   ├── tooltip/                  # tooltips
│   │   ├── tree/                     # tree control expand/collapse icons
│   │   ├── menu/                     # menu resources
│   │   ├── progress/                 # progress bar resources
│   │   ├── animation/                # loading animation (JSON)
│   │   └── ...                       # other shared resources
│   └── <your_app>/                   # your application's own XML and images
│       └── your_form.xml
├── fonts/                            # optional: custom font files
│   └── RobotoMono-*.ttf
└── lang/                             # optional: multi-language files
    ├── zh_CN.txt
    └── en_US.txt
```

### Content that must NOT be copied
- **Do not copy** the example directories such as `themes/default/basic/`, `controls/`, `layout/`
- **Do not copy** the demo directories such as `themes/default/chat/`, `cef/`, `cef_browser/`
- **Do not copy** binary files such as `bin/*.exe`, `bin/*.dll`
- **Do not copy** `bin/bin.zip`
- Only create a skin directory **of your own application** (e.g., `themes/default/my_app/`)

### Rules for referencing resource paths
```xml
<!-- Reference a shared resource in the public directory (relative path) -->
normal_image="file='public/button/window-minimize.svg' width='24' height='24'"

<!-- Reference a resource in the same directory (no path prefix needed) -->
bkimage="my_background.png"

<!-- Reference a public resource in a parent directory -->
bkimage="file='../public/shadow/shadow_big.svg' corner='64,64,68,70'"
```

## Predefined color quick reference

### Window/background colors
| Color name | Value | Use |
|--------|------|------|
| bk_wnd_darkcolor | #FFF0F2F5 | window dark background (light gray) |
| bk_wnd_lightcolor | #FFFFFFFF | window light background (white) |
| bk_main_wnd_title | #FF238EFA | caption bar blue |
| bk_listitem_hovered | #FFF0F2F5 | list item hover |
| bk_listitem_selected | #FFE4E7EB | list item selected |
| bk_menuitem_hovered | #FFE1E6EB | menu item hover |

### Text colors
| Color name | Value | Use |
|--------|------|------|
| default_font_color | #FF000000 | default black text |
| disabled_font_color | #FFA1AEBC | disabled gray text |
| default_link_font_color | #FF0000FF | hyperlink blue |
| white | #FFFFFFFF | white |
| darkcolor | #FF333333 | dark gray |
| lightcolor | #FF888888 | light gray |
| blue | #FF006DD9 | blue |
| red | #FFC63535 | red |
| green | #FF00BB96 | green |

### Separator colors
| Color name | Value | Use |
|--------|------|------|
| splitline_level1 | #FFD2D4D6 | dark separator |
| splitline_level2 | #FFEBEDF0 | light separator |

### Custom colors
Add the following to global.xml:
```xml
<TextColor name="my_brand_color" value="#FF1890FF"/>
```
Color format: `#AARRGGBB` (ARGB), `#RRGGBB` (RGB), or a predefined name (Blue/Red/White, ...)

## Predefined font ID quick reference

| Font ID | Size | Style |
|--------|------|------|
| system_12 ~ system_22 | 12-22 | Regular |
| system_bold_12 ~ system_bold_22 | 12-22 | Bold |
| system_underline_12 | 12 | Underline |
| system_italic_12 | 12 | Italic |
| system_strikeout_12 | 12 | Strikeout |
| arial_12 ~ arial_22 | 12-22 | Arial regular |
| arial_bold_12 ~ arial_bold_22 | 12-22 | Arial bold |

Default font (used when no font is specified): system_14

### Custom fonts
```xml
<!-- Define a new font ID -->
<Font id="my_title_font" name="Microsoft YaHei" size="24" bold="true"/>

<!-- Use a bundled font file (placed in the resources/fonts/ directory) -->
<FontFile file="MyFont-Regular.ttf" desc="My font, regular"/>
<Font id="my_font_16" name="MyFont" size="16"/>
```

## Predefined common style (class) quick reference

### Button styles
| Class name | Description |
|---------|------|
| btn_global_blue_80x30 | blue button (image) |
| btn_global_white_80x30 | white button (image) |
| btn_global_red_80x30 | red button (image) |
| btn_global_gray_80x30 | gray button (image) |
| btn_global_color_blue | blue button (solid color) |
| btn_global_color_white | white button (solid color) |
| btn_global_color_red | red button (solid color) |
| btn_global_color_gray | gray button (solid color) |
| btn_wnd_min_11 | window minimize button |
| btn_wnd_max_11 | window maximize button |
| btn_wnd_restore_11 | window restore button |
| btn_wnd_close_11 | window close button |
| btn_wnd_fullscreen_11 | fullscreen button |

### Input control styles
| Class name | Description |
|---------|------|
| simple | simple input box (no border) |
| simple_border | input box with border |
| simple_border_bottom | input box with bottom border (highlighted on focus) |
| combo | combo box |
| filter_combo | filterable combo box |
| check_combo | multi-select combo box |
| ip_address | IP address input |
| hot_key | hotkey input |

### Selection control styles
| Class name | Description |
|---------|------|
| checkbox_1 | checkbox style 1 |
| checkbox_2 | checkbox style 2 |
| checkbox_toggle_1 | toggle switch style 1 |
| checkbox_toggle_2 | toggle switch style 2 |
| option_1 | option button style 1 |
| option_2 | option button style 2 |

### List/tree styles
| Class name | Description |
|---------|------|
| list | list container |
| listitem | list item |
| tree_view | tree control |
| tree_node | tree node |
| tree_node_checkbox | tree node with checkbox |

### Progress bar/slider styles
| Class name | Description |
|---------|------|
| progress_horizontal_blue | horizontal blue progress bar |
| progress_vertical_blue | vertical blue progress bar |
| slider_horizontal_green | horizontal green slider |
| slider_vertical_green | vertical green slider |

### Menu styles
| Class name | Description |
|---------|------|
| menu | menu container |
| menu_element | menu item |
| menu_text | menu text |
| menu_split_line | menu separator |

### Others
| Class name | Description |
|---------|------|
| tab_ctrl / tab_ctrl_item | tabs |
| vscrollbar / hscrollbar | scrollbar style 1 (simple) |
| vscrollbar2 / hscrollbar2 | scrollbar style 2 (with buttons) |
| rich_text | rich text |
| hyper_link | hyperlink |
| splitline_hor_level1 | horizontal separator (dark) |
| splitline_hor_level2 | horizontal separator (light) |
| splitline_ver_level1 | vertical separator |

### Custom common styles
```xml
<!-- Define in global.xml -->
<Class name="my_card"
       bkcolor="white" border_size="1" border_color="light_gray"
       border_round="8,8" padding="12,12,12,12"
       box_shadow="color='#20000000' offset='0,2' blurradius='8' spreadradius='0'"/>

<!-- Use in an XML layout (class must come first) -->
<Box class="my_card" width="200" height="150">
    <Label text="Card content"/>
</Box>
```
**Note: the `class` attribute must be written before all other attributes.** Later attributes can override the same-named attributes in the class.
