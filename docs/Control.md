## Control (Basic Control) Attributes

The `Control` control is the base class of all available controls and contains the commonly used attributes of controls, such as width, height, margin, etc. Generally it is not used as a concrete control, but it is often used as a placeholder; please refer to the examples.
`Control` is the base class of all controls and contains the common attributes available to all controls.

```xml
<!-- A Control that stretches automatically to the width of its parent container; usually used as a placeholder -->
<Control width="stretch"/>
```

| Attribute Name | Default Value | Parameter Type | Function in [Control.h](../include/duilib/Core/Control.h) | Purpose |
| :--- | :--- | :--- | :---| :--- |
| class |  | string | SetClass|Control style; sets this control's attributes using the attribute values from the style, e.g. (btn_default). Multiple styles can be specified at the same time, separated by spaces; it must be written in the first attribute position |
| name |  | string | SetName|Control name; it is recommended to keep it unique within the same window, otherwise query efficiency is affected and the correct result may not be found |
| width | stretch | int / string | SetFixedWidth|Can be set to an int or string value. When the value is int, it sets the width of the control, e.g. (100); when the value is string, stretch means the width is calculated by the parent container, auto means the width is calculated automatically based on the content; a percentage value like "50%" means the expected width of the control is 50% of the parent control's width |
| height | stretch | int / string | SetFixedHeight|Can be set to an int or string value. When the value is int, it sets the height of the control, e.g. (100); when the value is string, stretch means the height is calculated by the parent container, auto means the height is calculated automatically based on the content; a percentage value like "30%" means the expected height of the control is 30% of the parent control's height |
| min_width | -1 | int | SetMinWidth|The minimum width of the control, e.g. (30) |
| min_height | -1 | int | SetMinHeight|The minimum height of the control, e.g. (30) |
| max_width | INT32_MAX | int / string | SetMaxWidth|The maximum width of the control; description is the same as width |
| max_height | INT32_MAX | int / string | SetMaxHeight|The maximum height of the control; description is the same as height |
| margin | 0,0,0,0 | rect | SetMargin|Margin, e.g. (2,2,2,2) |
| padding | 0,0,0,0 | rect | SetPadding|Padding, e.g. (2,2,2,2) |
| control_padding | true | bool | SetEnableControlPadding|Whether the control is allowed to apply its own padding |
| halign | left | string | SetHorAlignType|The horizontal alignment of the control, e.g. (center); supports left, center and right |
| valign | top | string | SetVerAlignType|The vertical alignment of the control, e.g. (center); supports top, center and bottom |
| align | left,top | string | SetHorAlignType<br>SetVerAlignType |Sets both the horizontal and vertical alignment of the control at the same time; same function as valign and halign.<br>Valid values: left, right, hcenter, top, vcenter, bottom, separated by commas, e.g. "hcenter,vcenter" |
| float | false | bool | SetFloat|Whether to use absolute positioning, e.g. "true" |
| keep_float_pos | false | bool | SetKeepFloatPos|Sets whether to keep the floating control's position relative to the parent control unchanged when the parent control's position and size are adjusted, e.g. "true" |
| bkcolor |  | string | SetBkColor|Background color string constant, e.g. (white) |
| bkcolor2 |  | string | SetBkColor2|Second background color string constant; if set, background color gradient is supported, e.g. (blue) |
| bkcolor2_direction | "1" | string | SetBkColor2Direction|Direction of the second background color; "1": left->right, "2": top->bottom, "3": top-left->bottom-right, "4": top-right->bottom-left |
| fore_color |  | string | SetForeColor|Foreground color string; the foreground color is generally set to a semi-transparent color, e.g. "#10006DD9" |
| normal_color |  | string | SetStateColor|Normal state color, e.g. (white) |
| hot_color |  | string | SetStateColor|Hover state color, e.g. (white) |
| pushed_color |  | string | SetStateColor|Pushed state color, e.g. (white) |
| disabled_color |  | string | SetStateColor|Disabled state color, e.g. (white) |
| border_color |  | string | SetBorderColor|Sets the border color for all states, e.g. (blue) |
| normal_border_color |  | string | SetBorderColor|Border color in the normal state, e.g. (blue) |
| hot_border_color |  | string | SetBorderColor|Border color in the hover state, e.g. (blue) |
| pushed_border_color |  | string | SetBorderColor|Border color in the pushed state, e.g. (blue) |
| disabled_border_color |  | string | SetBorderColor|Border color in the disabled state, e.g. (blue) |
| focus_border_color |  | string | SetFocusBorderColor|Border color in the focused state, e.g. (blue) |
| border_size | 0 | int / rect | SetBorderSize|Can be set to an int or rect value. When the value is int, left, top, right and bottom all use this value as the width. When the value is rect, the left, top, right and bottom borders are set separately |
| border_dash_style |"solid"| string | SetBorderDashStyle |Line style; valid values:<br>"solid": solid line<br>"dash": dashed line made of short dashes<br>"dot": dotted line made of dots<br>"dash_dot": alternating dash and dot line<br>"dash_dot_dot": alternating dash, dot and dot line |
| borders_on_top | true | bool | SetBordersOnTop|Whether the border is on top (i.e. draw child controls first, then the border, to avoid the border being covered by child controls) |
| left_border_size | 0 | int | SetLeftBorderSize|Left border size, e.g. (1); if this value is greater than 0, the border_size attribute setting is ignored |
| top_border_size | 0 | int | SetTopBorderSize|Top border size, e.g. (1); if this value is greater than 0, the border_size attribute setting is ignored |
| right_border_size | 0 | int | SetRightBorderSize|Right border size, e.g. (1); if this value is greater than 0, the border_size attribute setting is ignored |
| bottom_border_size | 0 | int | SetBottomBorderSize|Bottom border size, e.g. (1); if this value is greater than 0, the border_size attribute setting is ignored |
| border_round | 0,0 | size | SetBorderRound|Border corner radius, e.g. (2,2) |
| bkimage |  | string | SetBkImage|Background image, e.g. (bk.bmp or file='aaa.jpg' res='' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' fade='255' xtiled='false' ytiled='false') |
| normal_image |  | string | SetStateImage|Normal state image |
| hot_image |  | string | SetStateImage|Mouse hover state image |
| pushed_image |  | string | SetStateImage|Mouse pushed state image |
| disabled_image |  | string | SetStateImage|Disabled state image |
| fore_normal_image |  | string | SetForeStateImage|Normal state foreground image |
| fore_hot_image |  | string | SetForeStateImage |Mouse hover state foreground image |
| fore_pushed_image |  | string | SetForeStateImage|Mouse pushed state foreground image |
| fore_disabled_image |  | string | SetForeStateImage|Disabled state foreground image |
| tooltip_text |  | string | SetToolTipText|Mouse hover tooltip, e.g. (please enter your password here) |
| tooltip_text_id |  | string | SetToolTipTextId|Mouse hover tooltip; specifies the ID of the multilingual module. When tooltip_text is empty, this attribute is displayed instead, e.g. (TOOL_TIP_ID) |
| tooltip_width |  | int | SetToolTipWidth| The width occupied by the mouse hover tooltip |
| data_id |  | string | SetDataID|Custom string data; a helper, provided for user use |
| user_data_id |  | size_t | SetUserDataID|Custom integer data, provided for user use |
| enabled | true | bool | SetEnabled|Whether it can respond to user operations, e.g. "true" |
| mouse_enabled | true | bool | SetMouseEnabled|Whether this control can respond to mouse operations, e.g. "true" |
| keyboard_enabled | true | bool | SetKeyboardEnabled|Whether this control can respond to keyboard messages; when false, the control does not process keyboard messages |
| visible | true | bool | SetVisible|Whether it is visible, e.g. "true" |
| fade_visible | true | bool | SetFadeVisible|Whether it is visible, e.g. (true); this attribute triggers the control's animation effect |
| menu | false | bool | |Whether a right-click menu is needed, e.g. "true" |
| no_focus | false | bool | SetNoFocus|Whether it can obtain focus, e.g. "true" |
| tab_stop | true | bool | SetTabStop| Whether the TAB key can be used to switch focus to this control |
| show_focus_rect | false| bool | SetShowFocusRect| Whether to show the focus state (a rectangle made of dashed lines) |
| focus_rect_color | | string | SetFocusRectColor| The color of the focus state rectangle |
| alpha | 255 | int | SetAlpha|The overall transparency of the control, e.g. alpha="128"; valid range is 0-255 |
| state | normal | string | SetState|The current state of the control: supports normal, hot, pushed and disabled states |
| cursor_type | arrow | string | SetCursorType|The mouse cursor when the mouse moves over the control: <br>"arrow": arrow<br>"hand": hand<br>"wait": busy<br>"cross": crosshair<br>"ibeam": I-beam cursor, text cursor<br>"size_we": horizontal resize<br>"size_ns": vertical resize<br>"size_nwse": diagonal resize, northwest-southeast<br>"size_nesw": diagonal resize, northeast-southwest<br>"size_all": move, four-direction resize<br>"no": not-allowed cursor<br>"progress": progress, application-start cursor |
| render_offset | 0,0 | size | SetRenderOffset|The offset when the control is drawn, e.g. (10,10); generally used for drawing animations |
| fade_hot | false | bool |SetFadeHot |Whether to enable the transparency fade animation of the control in the mouse hover state, e.g. "true" |
| fade_hot_frame_interval_ms | 16 | int |SetFadeHotFrameIntervalMillSeconds |Sets the timer interval (in milliseconds) for playing the Hot state animation |
| fade_hot_total_ms | 180 | int |SetFadeHotTotalMillSeconds |Sets the total duration (in milliseconds) of the Hot state animation |
| fade_hot_easing_function | EaseInOutCubic | string |SetFadeHotEasingFunctionType |Sets the easing function type of the Hot state animation; for supported easing function types, see the EasingFunctions::GetEasingFunctionType implementation function |
| fade_alpha | false | bool/int | GetAnimationManager().SetFadeAlpha|Whether to enable the control's transparency fade animation. Valid values are as follows:<br>fade_alpha="false": the control's transparency fade animation is not enabled <br>fade_alpha="true": the control's transparency fade animation is enabled, and the final Alpha value of the control is set to 255 <br>fade_alpha="128": the control's transparency fade animation is enabled, and the final Alpha value of the control is set to 128; in this case the valid range is 1-255 |
| fade_width | false | bool | GetAnimationManager().SetFadeWidth|Whether to enable the control's width fade animation, e.g. "true". The control width cannot be of stretch type; this attribute must be written after the width attribute |
| fade_height | false | bool | GetAnimationManager().SetFadeHeight|Whether to enable the control's height fade animation, e.g. "true". The control height cannot be of stretch type; this attribute must be written after the height attribute |
| fade_size | false | bool | GetAnimationManager().SetFadeSize|Whether to enable the fade animation of the control's size (height and width), e.g. "true". The control's height and width cannot be of stretch type; this attribute must be written after the height and width attributes |
| fade_in_out_x_from_left | false | bool | GetAnimationManager().SetFadeInOutX|Whether to enable the control's left-to-right animation, e.g. "true" |
| fade_in_out_x_from_right | false | bool | GetAnimationManager().SetFadeInOutX|Whether to enable the control's right-to-left animation, e.g. "true" | 
| fade_in_out_y_from_top | false | bool | GetAnimationManager().SetFadeInOutY|Whether to enable the control's top-to-bottom animation, e.g. "true" | 
| fade_in_out_y_from_bottom | false | bool | GetAnimationManager().SetFadeInOutY|Whether to enable the control's bottom-to-top animation, e.g. "true" |
| fade_frame_interval_ms | 16 | int |GetAnimationManager().SetFrameIntervalMillSeconds |Sets the timer interval (in milliseconds) for playing animations |
| fade_total_ms | 180 | int |GetAnimationManager().SetTotalMillSeconds |Sets the total duration (in milliseconds) of the animation |
| fade_easing_function | EaseInOutCubic | string |GetAnimationManager().SetEasingFunctionType |Sets the easing function type of the animation; for supported easing function types, see the EasingFunctions::GetEasingFunctionType implementation function |
| loading     | | string | SetLoadingAttribute| Sets the attributes related to the UI display of the control's loading state; usage is similar to the Image attribute.<br>Usage example: loading="file='loading.xml' width='0' height='0' offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255' animation_control='loading_animation' auto_stop='true'" <br>The available attributes of loading are as follows:<br> "file": the XML resource file name; the XML resource is loaded according to this setting and finally displayed in a Box container<br>"width": the display width of the loading control, in pixels<br>"height": the display height of the loading control, in pixels <br> "offset_x": the X-direction offset of the loading control's position, relative to the top-left corner of the associated control; valid values: >= 0 <br> "offset_y": the Y-direction offset of the loading control's position, relative to the top-left corner of the associated control; valid values: >= 0  <br> "halign": horizontal alignment; valid values: "left" "center" "right"; only takes effect when offset_x has no valid value <br>  "valign": vertical alignment; valid values: "top" "center" "bottom"; only takes effect when offset_y has no valid value<br>"fade": the transparency of the loading control; valid range: 0 - 255<br>"auto_stop": after the loading animation finishes playing, automatically stop the loading state (automatically calls the StopLoading() function)<br>"animation_control": the name of the animation control, used for the interaction between the Loading function and the animation control on the loading control<br><br>For a complete demonstration of the loading function, refer to the `examples/ListCtrl` example program |
| paint_order | | string | SetPaintOrder| Sets the paint order: 0 means normal painting; a non-zero value means a specified paint order, and a larger value means it is painted later |
| start_image_animation     | | string | StartImageAnimation   | Plays the animation; at most 3 parameters, each separated by ',', see the function's parameter list for details |
| stop_image_animation      | | string | StopImageAnimation    | Stops the animation; at most 3 parameters, each separated by ',', see the function's parameter list for details |
| set_image_animation_frame | | string | SetImageAnimationFrame| Sets the current frame of the animation; at most 2 parameters, each separated by ',', see the function's parameter list for details |
| box_shadow | | string | SetBoxShadow|Sets the control's shadow attributes, for example: boxshadow="color='red' offset='0,0' blurradius='8' spreadradius='8' |
| enable_drag_drop |false| bool | SetEnableDragDrop | Whether drag-and-drop operations are allowed, including dragging in files and dragging in text |
| enable_drop_file |false| bool | SetEnableDropFile | Whether dragging in files is allowed |
| drop_file_types  || string | SetDropFileTypes  | The list of file extensions supported by the drag-and-drop file operation, e.g. ".txt;.csv" means only txt and csv files are supported; if empty, all files are supported |
| row_span  | 1 | int | SetRowSpan  | Cell merge attribute; how many rows it spans (defaults to 1 row); only takes effect in a GridLayout layout |
| col_span  | 1 | int | SetColumnSpan  | Cell merge attribute; how many columns it spans (defaults to 1 column); only takes effect in a GridLayout layout |

## ScrollBar Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| button1_normal_image |  | string | Left or top button normal state image |
| button1_hot_image |  | string | Left or top button mouse hover state image |
| button1_pushed_image |  | string | Left or top button mouse pushed state image |
| button1_disabled_image |  | string | Left or top button disabled state image |
| button2_normal_image |  | string | Right or bottom button normal state image |
| button2_hot_image |  | string | Right or bottom button mouse hover state image |
| button2_pushed_image |  | string | Right or bottom button mouse pushed state image |
| button2_disabled_image |  | string | Right or bottom button disabled state image |
| thumb_normal_image |  | string | Thumb normal state image |
| thumb_hot_image |  | string | Thumb mouse hover state image |
| thumb_pushed_image |  | string | Thumb mouse pushed state image |
| thumb_disabled_image |  | string | Thumb disabled state image |
| rail_normal_image |  | string | Slider middle indicator normal state image |
| rail_hot_image |  | string | Slider middle indicator mouse hover state image |
| rail_pushed_image |  | string | Slider middle indicator mouse pushed state image |
| rail_disabled_image |  | string | Slider middle indicator disabled state image |
| bk_normal_image |  | string | Background normal state image |
| bk_hot_image |  | string | Background mouse hover state image |
| bk_pushed_image |  | string | Background mouse pushed state image |
| bk_disabled_image |  | string | Background disabled state image |
| horizontal | false | bool | Horizontal or vertical, e.g. "true" |
| line_size | 8 | int | The size of scrolling one line, e.g. (8) |
| thumb_min_length | 30 | int | The minimum length of the thumb |
| range | 100 | int | Scroll range, e.g. (100) |
| value | 0 | int | Scroll position, e.g. (0) |
| show_button1 | true | bool | Whether to show the left or top button, e.g. "true" |
| show_button2 | true | bool | Whether to show the right or bottom button, e.g. "true" |
| auto_hide_scroll | true | bool | Whether to automatically hide the scroll bar, e.g. "true" |

The ScrollBar control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## Label Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| text |  | string | Displayed text |
| text_id |  | string | The text ID for the multilingual feature |
| rich_text | false | bool | Whether the text content is RichText <br> Usage example: `<Label rich_text="true" text="A simple <b>window</b><br/>with a <u>caption bar</u> and <u>normal buttons</u>, <b>bold, <font color='#FF0000'>red text</font></b>" />` <br> Note: in RichText mode, the following features are not supported:<br> (1) Text alignment does not support justify<br> (2) The vertical_text attribute is not supported (nor are the vertical text related attributes)<br> (3) The end_ellipsis attribute is not supported<br> (4) The path_ellipsis attribute is not supported<br> (5) The auto_tooltip attribute is not supported<br> (6) The word_spacing attribute is not supported |
| text_align | "left,top" | string | Sets the horizontal and vertical alignment of the text, separated by a half-width comma, e.g. "hcenter,vcenter".<br>Horizontal alignment values: left (align left), hcenter (center), right (align right), hjustify (justify) <br>Vertical alignment values: top (align top), vcenter (center), bottom (align bottom), vjustify (justify) |
| text_padding | 0,0,0,0 | rect | The padding of the text display, in the format "left,top,right,bottom", which sets the padding values on the left, top, right and bottom of the target area respectively, e.g. "2,2,2,2" |
| font | | string | Font ID; this font ID must exist in global.xml |
| end_ellipsis | false | bool | Whether to use ... to replace the truncated text at the end |
| path_ellipsis | false | bool | For paths, whether to use ... to replace the middle part of the path when it cannot be fully displayed |
| normal_text_color |  | string | Normal text color; if not specified, the default color is used, e.g. "blue" |
| hot_text_color |  | string | Mouse hover text color; if not specified, the default color is used, e.g. "blue" |
| pushed_text_color |  | string | Mouse pushed text color; if not specified, the default color is used, e.g. "blue" |
| disabled_text_color |  | string | Disabled text color; if not specified, the default color is used, e.g. "blue" |
| single_line | true | bool | Whether to output the text in a single line |
| multi_line | false | bool | Whether to output the text in multiple lines; mutually exclusive with the single_line attribute |
| auto_tooltip | false | bool | Whether the tooltip text shown when the mouse hovers over the control is displayed only when the ellipsis appears |
| replace_newline | false | bool | Whether to replace newline characters in the text: replace the string "\\\\n" with the newline character "\n", so that these two characters (\n) can be used as newline characters in XML, thus supporting multi-line text. For example: the original string is "first line\\\\nsecond line"; when true, the two characters "\\\\n" are replaced with the newline character "\n", and the final string becomes "first line\nsecond line" |
| spacing_mul | 1.0f | float | The multiplier of the line (column) spacing; it is a ratio of the font size (default is usually 1.0, i.e. 100% of the font size), used to adjust the line spacing proportionally <br> After setting, the actual line spacing is: font size * spacing_mul + spacing_add |
| spacing_add | 0 | float | The additional amount of line (column) spacing; it is a fixed additional pixel value (default is usually 0), used to add a fixed offset (in pixels) on top of the proportional adjustment <br> After setting, the actual line spacing is: font size * spacing_mul + spacing_add |
| word_spacing | 0 | float | Sets the spacing between two adjacent characters (in pixels) |
| vertical_text | false | bool | Sets the text direction: true for vertical text, false for horizontal text <br> Horizontal text drawing direction: left to right, top to bottom <br> Vertical text drawing direction: top to bottom, right to left |
| use_font_height | true | bool | When drawing text vertically, set the character spacing to use the default height of the font instead of the actual height of each character (all fonts are displayed at equal height) |
| ascii_rotate_90 | true | bool | When drawing text vertically, characters such as letters and numbers are rotated 90 degrees clockwise for display |

The Label control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## LabelBox Attributes
LabelBox and Label are classes based on the same template; refer to the `Label` attributes    
The LabelBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## LabelHBox Attributes
LabelHBox and Label are classes based on the same template; refer to the `Label` attributes    
The LabelHBox control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## LabelVBox Attributes
LabelVBox and Label are classes based on the same template; refer to the `Label` attributes    
The LabelVBox control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes

## Button Attributes
The Button control inherits the `Label` attributes; for more available attributes, refer to the `Label` attributes

## ButtonBox Attributes
ButtonBox and Button are classes based on the same template; refer to the `Button` attributes    
The ButtonBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## ButtonHBox Attributes
ButtonHBox and Button are classes based on the same template; refer to the `Button` attributes    
The ButtonHBox control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## ButtonVBox Attributes
ButtonVBox and Button are classes based on the same template; refer to the `Button` attributes    
The ButtonVBox control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes

## CheckBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| selected | false | bool | Whether it is selected |
| selected_normal_image |  | string | Normal state image in the selected state |
| selected_hot_image |  | string | Mouse hover state image in the selected state |
| selected_pushed_image |  | string | Mouse pushed state image in the selected state |
| selected_disabled_image |  | string | Disabled state image in the selected state |
| selected_fore_normal_image |  | string | Foreground image in the selected state |
| selected_fore_hot_image |  | string | Mouse hover state foreground image in the selected state |
| selected_fore_pushed_image |  | string | Mouse pushed state foreground image in the selected state |
| selected_fore_disabled_image |  | string | Disabled state foreground image in the selected state |
| part_selected_normal_image |  | string | Normal state image in the partially selected state |
| part_selected_hot_image |  | string | Mouse hover state image in the partially selected state |
| part_selected_pushed_image |  | string | Mouse pushed state image in the partially selected state |
| part_selected_disabled_image |  | string | Disabled state image in the partially selected state |
| part_selected_fore_normal_image |  | string | Foreground image in the partially selected state |
| part_selected_fore_hot_image |  | string | Mouse hover state foreground image in the partially selected state |
| part_selected_fore_pushed_image |  | string | Mouse pushed state foreground image in the partially selected state |
| part_selected_fore_disabled_image |  | string | Disabled state foreground image in the partially selected state |
| selected_text_color |  | string | Text color in the selected state; if not specified, the default color is used, e.g. (blue) |
| selected_normal_text_color |  | string | Normal state text color in the selected state; if not specified, the default color is used, e.g. (blue) |
| selected_hot_text_color |  | string | Mouse hover state text color in the selected state; if not specified, the default color is used, e.g. (blue) |
| selected_pushed_text_color |  | string | Mouse pushed state text color in the selected state; if not specified, the default color is used, e.g. (blue) |
| selected_disabled_text_color |  | string | Disabled state text color in the selected state; if not specified, the default color is used, e.g. (blue) |
| normal_first | false | bool | When the control is in the selected state and no background color or background image is set, draw it using the corresponding attributes of the unselected state |

The CheckBox control inherits the `Button` attributes; for more available attributes, refer to the `Button` attributes

## CheckBoxBox Attributes
CheckBoxBox and CheckBox are classes based on the same template; refer to the `CheckBox` attributes    
The CheckBoxBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## CheckBoxHBox Attributes
CheckBoxHBox and CheckBox are classes based on the same template; refer to the `CheckBox` attributes    
The CheckBoxHBox control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## CheckBoxVBox Attributes
CheckBoxVBox and CheckBox are classes based on the same template; refer to the `CheckBox` attributes    
The CheckBoxVBox control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes

## Option Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| group |  | string | The name of the group it belongs to; within the same group name, single selection is maintained |

The Option control inherits the `CheckBox` attributes; for more available attributes, refer to the `CheckBox` attributes

## OptionBox Attributes
OptionBoxBox and OptionBox are classes based on the same template; refer to the `OptionBox` attributes    
The OptionBoxBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## OptionHBox Attributes
OptionBoxHBox and OptionBox are classes based on the same template; refer to the `OptionBox` attributes    
The OptionBoxHBox control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## OptionVBox Attributes
OptionBoxVBox and OptionBox are classes based on the same template; refer to the `OptionBox` attributes    
The OptionBoxVBox control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes

## GroupBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| corner_size | "0,0" | size | Corner radius size |
| line_width | 0 | int | Line width |
| line_color | | string | Line color |
| text | | string | Text content |

The GroupBox control inherits the `Label` attributes; for more available attributes, refer to the `Label` attributes

## GroupVBox Attributes
GroupVBox and GroupBox are implemented from the same template; refer to the `GroupBox` attributes for available attributes    
The GroupVBox control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes

## GroupHBox Attributes
GroupHBox and GroupBox are implemented from the same template; refer to the `GroupBox` attributes for available attributes    
The GroupHBox control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## Combo Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| combo_type | "drop_down" | string | The type of the combo box: "drop_list" means a non-editable list, "drop_down" means an editable list |
| dropbox_size | | string | The size (width and height) of the drop-down list |
| popup_top | false | bool | Whether the drop-down list pops up upward |
| combo_tree_view_class | | string | The Class attribute of the TreeView of the drop-down list; see the corresponding content in `global.xml` for how to define it |
| combo_tree_node_class | | string | The Class attribute of the nodes of the TreeView of the drop-down list; see the corresponding content in `global.xml` for how to define it |
| combo_icon_class | | string | The Class attribute for displaying the icon; see the corresponding content in `global.xml` for how to define it |
| combo_edit_class | | string | The Class attribute of the edit control; see the corresponding content in `global.xml` for how to define it |
| combo_button_class | | string | The Class attribute of the button control; see the corresponding content in `global.xml` for how to define it |
| shadow_type        | "menu" | string | Sets the shadow type of the drop-down window:<br> "default", default shadow <br> "big", big shadow, square corners (suitable for normal windows)<br> "big_round", big shadow, rounded corners (suitable for normal windows)<br> "small", small shadow, square corners (suitable for normal windows)<br> "small_round", small shadow, rounded corners (suitable for normal windows)<br> "menu", small shadow, square corners (suitable for pop-up windows, such as menus)<br> "menu_round", small shadow, rounded corners (suitable for pop-up windows, such as menus)<br> "none", no shadow |

The Combo control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## FilterCombo Attributes
The FilterCombo control does not support the "combo_type" attribute    
The FilterCombo control inherits the `Combo` attributes; for more available attributes, refer to the `Combo` attributes

## ComboButton Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| dropbox_size | | string | The size (width and height) of the drop-down list |
| popup_top | false | bool | Whether the drop-down list pops up upward |
| combo_box_class | | string | The Class attribute of the drop-down combo box; see the corresponding content in `global.xml` for how to define it |
| left_button_class | | string | The Class attribute of the left button control; see the corresponding content in `global.xml` for how to define it |
| left_button_top_label_class | | string | The Class attribute of the Label control above the left button; see the corresponding content in `global.xml` for how to define it |
| left_button_bottom_label_class | | string | The Class attribute of the Label control below the left button; see the corresponding content in `global.xml` for how to define it |
| left_button_top_label_text | | string | The text of the Label control above the left button |
| left_button_bottom_label_text | | string | The text of the Label control below the left button |
| left_button_top_label_bkcolor | | string | The background color of the Label control above the left button |
| left_button_bottom_label_bkcolor | | string | The background color of the Label control below the left button |
| right_button_class | | string | The Class attribute of the right button control; see the corresponding content in `global.xml` for how to define it |
| shadow_type        | "menu" | string | Sets the shadow type of the drop-down window:<br> "default", default shadow <br> "big", big shadow, square corners (suitable for normal windows)<br> "big_round", big shadow, rounded corners (suitable for normal windows)<br> "small", small shadow, square corners (suitable for normal windows)<br> "small_round", small shadow, rounded corners (suitable for normal windows)<br> "menu", small shadow, square corners (suitable for pop-up windows, such as menus)<br> "menu_round", small shadow, rounded corners (suitable for pop-up windows, such as menus)<br> "none", no shadow |

The ComboButton control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## CheckCombo Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| dropbox | | string | The attribute information of the drop-down box; see the example programs for the specific setting method |
| dropbox_size | | string | The size (width and height) of the drop-down list |
| popup_top | false | bool | Whether the drop-down list pops up upward |
| dropbox_item_class | | string | The attributes of each list item in the drop-down list; see the example programs for the specific setting method |
| selected_item_class | | string | The attributes of each sub-item in the selected item; see the example programs for the specific setting method |
| shadow_type        | "menu" | string | Sets the shadow type of the drop-down window:<br> "default", default shadow <br> "big", big shadow, square corners (suitable for normal windows)<br> "big_round", big shadow, rounded corners (suitable for normal windows)<br> "small", small shadow, square corners (suitable for normal windows)<br> "small_round", small shadow, rounded corners (suitable for normal windows)<br> "menu", small shadow, square corners (suitable for pop-up windows, such as menus)<br> "menu_round", small shadow, rounded corners (suitable for pop-up windows, such as menus)<br> "none", no shadow |

The CheckCombo control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## DateTime Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| format | | string | The format of the date; see the function documentation in `DateTime.h` for details |
| edit_format | | string | The edit format of the date in the editing state. Valid values: "date_calendar": year-month-day; the date is modified through a month calendar shown in a drop-down box; "date_up_down": when editing, displays year-month-day; the date is modified via an up-down control placed on the right side of the control; "date_time_up_down": when editing, displays year-month-day hour:minute:second; "date_minute_up_down": when editing, displays year-month-day hour:minute; "time_up_down": when editing, displays hour:minute:second; "minute_up_down": when editing, displays hour:minute |
| spin_class | | string | The Class attribute of the Spin control in the date; only valid when using SDL; default value: "rich_edit_spin_box,rich_edit_spin_btn_up,rich_edit_spin_btn_down" |

The DateTime control inherits the `Label` attributes; for more available attributes, refer to the `Label` attributes

## HotKey Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| default_text | | string | The text displayed by default |

The HotKey control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## HyperLink Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| url | | string | URL |

The HyperLink control inherits the `Label` attributes; for more available attributes, refer to the `Label` attributes

## IPAddress Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| ip | | string | IP address, e.g. "192.168.0.0" |

The HotKey control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## Line Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| vertical | false | bool | Whether it is a vertical line |
| line_color | | string | The color of the line |
| line_width | | int | The width of the line |
| dash_style | | string | The line style; valid values: "solid": solid line; "dash": dashed line made of short dashes; "dot": dotted line made of dots; "dash_dot": alternating dash and dot line; "dash_dot_dot": alternating dash, dot and dot line |

The Line control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## Menu Attributes
Menu is a window; see the menu in the example programs for the specific usage    
The Menu control inherits the `Window` attributes; for more available attributes, refer to the `Window` attributes

## Progress Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| horizontal | true | bool | Whether it is horizontal; true means horizontal, false means vertical |
| min | 0 | int | Minimum progress value, e.g. (0) |
| max | 100 | int | Maximum progress value, e.g. (100) |
| value | 0 | int | Progress value, e.g. (50) |
| progress_image |  | string | Foreground image of the progress bar |
| stretch_fore_image | true | bool | Specifies whether the foreground image of the progress bar is stretched for display |
| progress_color |  | string | Foreground color of the progress bar; if not specified, the default color is used, e.g. (blue) |
| marquee | true | bool | Whether to display in scrolling mode |
| marquee_width | | int | The width of the scrolling area |
| marquee_step | | int | The step size of the scrolling |
| reverse | false | bool | Whether the progress value counts down (progress goes from 100 to 0) |

The Progress control inherits the `Label` attributes; for more available attributes, refer to the `Label` attributes

## Slider Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| thumb_normal_image |  | string | Drag thumb normal state image |
| thumb_hot_image |  | string | Drag thumb mouse hover state image |
| thumb_pushed_image |  | string | Drag thumb mouse pushed state image |
| thumb_disabled_image |  | string | Drag thumb mouse disabled state image |
| thumb_size | 10,10 | size | The size of the drag thumb, e.g. (10,10) |
| step | 1 | int | Progress step, e.g. (1) |
| progress_bar_padding | 0,0,0,0 | rect | The padding by which the slider is reduced when drawn |

The Slider control inherits the `Progress` attributes; for more available attributes, refer to the `Progress` attributes

## CircleProgress Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| circular | true | bool | Function switch: whether it is a ring progress bar |
| circle_width | 1 | int | The width of the ring progress bar, e.g. (10) |
| indicator |  | string | Sets the moving icon of the progress indicator |
| clockwise | true | bool | Sets the increment direction |
| bgcolor |  | string | Sets the background color of the progress bar |
| fgcolor |  | string | Sets the foreground color of the progress bar |
| gradient_color |  | string | Sets the foreground gradient color of the progress bar; used together with fgcolor; if not set, there is no gradient effect |

The CircleProgress control inherits the `Progress` attributes; for more available attributes, refer to the `Progress` attributes

## RichEdit Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| vscrollbar | false | bool | Whether to use a vertical scroll bar, e.g. "true" |
| hscrollbar | false | bool | Whether to use a horizontal scroll bar, e.g. "true" |
| auto_vscroll | false | bool | Whether to scroll vertically with input, e.g. "true" (not valid for the SDL implementation) |
| auto_hscroll | false | bool | Whether to scroll horizontally with input, e.g. "true" (not valid for the SDL implementation) |
| want_tab | false | bool | Whether to accept the Tab key message, e.g. "true" |
| want_return | false | bool | Whether to accept the Enter (Return) key message, e.g. "true" |
| want_ctrl_return | false | bool | Whether to accept the Ctrl+Return key message, e.g. "true" |
| rich_text | false | bool | Whether to use rich text format, e.g. "true" (not valid for the SDL implementation) |
| single_line | true | bool | Whether to use a single line, e.g. "true" |
| multi_line | false | bool | Whether to use multiple lines; mutually exclusive with the single_line attribute, e.g. "true" |
| readonly | false | bool | Whether it is read-only, e.g. "false" |
| password | false | bool | Whether it is password mode, e.g. "true" |
| show_password | false | bool | Whether to show the password character, e.g. "true" |
| password_char || string | Sets the password character; the default is the " * " character, which can be changed through this attribute |
| flash_password_char | false | bool | Display the character first, then show the password character |
| number_only | false | bool | Whether only numbers are allowed, e.g. "false" |
| max_number | INT_MAX | int | The maximum number allowed (only valid when number_only is true) |
| min_number | INT_MIN | int | The minimum number allowed (only valid when number_only is true) |
| text_align | "left,top" | string | The horizontal and vertical alignment of the text; valid values: left, right, hcenter, top, vcenter, bottom, separated by commas, e.g. "hcenter,vcenter" |
| text_padding |  | rect | Text padding, e.g. "2,2,2,2" |
| text |  | string | Displayed text |
| text_id |  | string | The multilingual feature ID of the displayed text |
| font | | string | Font ID |
| normal_text_color |  | string | Normal state text color; if not specified, the default color is used, e.g. "blue" |
| disabled_text_color |  | string | Disabled state text color; if not specified, the default color is used, e.g. "blue" |
| caret_color |  | string | The color of the caret |
| prompt_mode | false | bool | Whether to show the prompt text, e.g. "true" |
| prompt_text |  | string | The prompt text inside the text box; displayed when the text of the text box is empty |
| prompt_text_id |  | string | The ID of the multilingual feature, e.g. (TEXT_OUT) |
| prompt_color |  | string | The color of the prompt text inside the text box |
| focused_image |  | string | Image in the focused state |
| auto_detect_url | false | bool | Whether to automatically detect URLs; if it is a URL, it is displayed as a hyperlink (not valid for the SDL implementation) |
| limit_text | | int | Limits the maximum number of characters |
| limit_chars | | string | Limits which characters are allowed; for example, "abc" means only the characters a, b and c can be entered, and other characters are not allowed |
| allow_beep | false | bool | Whether a Beep sound is allowed (not valid for the SDL implementation) |
| word_wrap | false| bool | Whether to wrap lines automatically |
| no_caret_readonly |false| bool | In read-only mode, do not show the caret |
| save_selection |false| bool | If true, the selection boundaries should be saved when the control is in an inactive state (not valid for the SDL implementation) |
| hide_selection | true | bool | Whether to hide the selection |
| zoom | | size | Sets the zoom ratio: wParam is the numerator of the zoom ratio and lParam is the denominator. "wParam,lParam" means zooming displayed at the numerator/denominator of the zoom ratio; the valid range is 1/64 < (wParam / lParam) < 64. For example: "0,0" means zoom is disabled, "2,1" means zoomed in to 200%, "1,2" means zoomed out to 50% |
| wheel_zoom | | bool | Whether Ctrl + mouse wheel is allowed to adjust the zoom ratio |
| default_context_menu | false | bool | Whether to use the default right-click menu |
| spin_class | | string | Sets the Class name of the Spin function; if not empty, the Spin button is displayed; see the example programs for detailed usage |
| clear_btn_class | | string | Sets the Class name of the clear button function; if not empty, the clear button is displayed; see the example programs for detailed usage |
| show_passowrd_btn_class | | string | Sets the Class name of the show-password button function; if not empty, the show-password button is displayed; see the example programs for detailed usage |
| selection_bkcolor | "CornflowerBlue" | string | The background color of the selected text (focused state); if set to empty, it is not displayed |
| inactive_selection_bkcolor | "DarkGray" | string | The background color of the selected text (non-focused state); if set to empty, it is not displayed |
| current_row_bkcolor | "" | string | The background color of the current row (focused state); if set to empty, the current row background color is not displayed in the focused state |
| inactive_current_row_bkcolor | "" | string | The background color of the current row (non-focused state); if set to empty, the current row background color is not displayed in the non-focused state |
| select_all_on_focus |false| bool | Whether to select all when focus is obtained |
| focus_bottom_border_size |0| int | The size of the bottom border in the focused state |
| focus_bottom_border_color || string | The color of the bottom border in the focused state |
| enable_drag_drop |false| bool   | Whether drag-and-drop operations are allowed |
| enable_drop_file |false| bool   | Whether file drag-and-drop operations are allowed |
| drop_file_types  |     | string | The list of file extensions supported by the drag-and-drop file operation, e.g. ".txt;.csv" means only txt and csv files are supported; if empty, all files are supported |
| row_spacing_mul  | 1.0 | float  | Line spacing multiplier, e.g. 1.5 means 1.5 times the line spacing<br>Windows platform: only valid when the rich_text attribute is "true", because the RichEdit control on Windows only supports setting line spacing in rich text mode;<br>when using SDL, it is always valid; that is, on other platforms, the line spacing attributes are always valid |
| row_spacing_add  |0    | float  | Line spacing additional amount: a fixed additional pixel value (default is usually 0), used to add a fixed offset (in pixels) on top of the proportional adjustment; only valid when using SDL |

The RichEdit control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

## RichText Attributes
RichText is text with formatting, similar to HTML tags; formatted text starts with `<RichText>` and ends with `</RichText>`.    
Example: `<RichText>Formatted text demo: <a href="URL">text</a></RichText>`    
Supported tag list:    
```cpp
   // Supported tag list (HTML-compatible tags):
   // Hyperlink:    <a href="URL">text</a>
   // Bold:         <b> </b>
   // Italic:       <i> </i>
   // Strikethrough: <s> </s> or <del> </del> or <strike> </strike>
   // Underline:    <u> </u>
   // Background color: <bgcolor color="#000000"> </bgcolor>
   // Font:         <font face="SimSun" size="12" color="#000000">
   // Line break:   <br/>
```
For specific usage, also refer to the example programs.

| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| text_align | "left,top" | string | The horizontal and vertical alignment of the text; valid values: left, right, hcenter, top, vcenter, bottom, separated by commas, e.g. "hcenter,vcenter" |
| text_padding |  | rect | Text padding, e.g. "2,2,2,2" |
| font | | string | Font ID |
| text_color | | string | Default text color |
| replace_brace | true | bool | When setting the text attribute, whether to allow replacing '{' with '<' and '}' with '>'; this attribute must be placed before the text attribute to take effect, e.g. replace_brace="false" means replacement is forbidden |
| text | | string | Sets the formatted text content, in which '{' may be used instead of '<' and '}' instead of '>', thus avoiding escape characters and making it easier to read |
| text_id | | string | Sets the ID of the formatted text content, in which '{' may be used instead of '<' and '}' instead of '>' in the corresponding content, thus avoiding escape characters and making it easier to read |
| trim_policy | "all" | string | Sets the text trimming policy: "all" means remove all spaces; "none" means no spaces are removed; "keep_one" means keep only one space |
| default_link_font_color | | string | Hyperlink: normal text color value |
| hover_link_font_color | | string | Hyperlink: text color value in the hover state |
| mouse_down_link_font_color | | string | Hyperlink: text color value in the mouse pushed state |
| link_font_underline | true | bool | Hyperlink: whether to use underlined font |
| row_spacing_mul | 1.0 | float | Line spacing multiplier, e.g. 1.5 means 1.5 times the line spacing |
| row_spacing_add  |0    | float  | Line spacing additional amount: a fixed additional pixel value (default is usually 0), used to add a fixed offset (in pixels) on top of the proportional adjustment |
| word_wrap | true| bool | Whether to wrap lines automatically; if false, a new line only occurs at the `<br/>` tag |

The RichText control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## RichTextBox Attributes
RichTextBox and RichText are classes based on the same template; refer to the `RichText` attributes    
The RichTextBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## RichTextHBox Attributes
RichTextHBox and RichText are classes based on the same template; refer to the `RichText` attributes    
The RichTextHBox control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## RichTextVBox Attributes
RichTextVBox and RichText are classes based on the same template; refer to the `RichText` attributes    
The RichTextVBox control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes

## Split Attributes
A splitter control; by dragging the splitter, you can change the width or height of the two controls on the left/right or top/bottom. Usage:    
If placed in a horizontal layout (HLayout), drag left and right    
If placed in a vertical layout (VLayout), drag up and down    
Note: if both controls are set to the stretch type, the splitter will not work properly.

| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| enable_split_single | false | bool | When there is only one control, whether its width is allowed to be adjusted |

The Split control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## SplitBox Attributes
SplitBox and Split are implemented from the same template; refer to the `Split` attributes for available attributes    
The SplitBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## TabCtrl Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| selected_id | | int | The sub-item selected by default |
| tab_box_name| | string | The name of the bound TabBox control; after binding, when the selected item of the TabCtrl changes, the selected item of the TabBox follows |
| drag_order  | true | bool | Whether dragging to adjust the order is supported (within the same tab); enabled by default |
| drag_out_id | 0 | int | Sets whether dragging out of this container is supported: if not 0, dragging out is supported; otherwise it is not supported (dragged out to the container where drop_in_id==drag_out_id) |
| drop_in_id | 0 | int | Sets whether dragging into this container is supported: if not 0, dragging in is supported; otherwise it is not supported (dragged into this container from the container where drag_out_id==drop_in_id) |

The TabCtrl control inherits the `ListBox` attributes; for more available attributes, refer to the `ListBox` attributes

## TabCtrlItem Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :---     | :---   | :---     | :--- |
| tab_box_item_index | | int | The sub-item index of the bound TabBox (i.e. clicking this tab switches to the TabBox page with this index) |
| title | | string | The title text of the tab page |
| title_id | | string | The title text ID of the tab page (for multilingual support) |
| title_class | | string | The Class value of the title text resource of the tab page |
| icon | | string | The icon resource string of the tab page |
| icon_class | | string | The Class value of the icon resource of the tab page |
| close_button_class | | string | The Class value of the close button resource of the tab page |
| line_class | | string | The Class value of the separator line resource of the tab page |
| selected_round_corner | | size | The corner radius of the tab page in the selected state |
| hot_round_corner | | size | The corner radius of the tab page in the hover state |
| hot_padding | | size | The padding of the background color of the tab page in the Hot state |
| auto_hide_close_button | false | bool | Whether the close button is automatically hidden |

The TabCtrlItem control inherits the `ControlDragableT` attributes; for more available attributes, refer to the `ControlDragableT` attributes

## ControlDragableT Attributes (Template Class)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :---     | :---   | :---     | :--- |
| drag_order  | true | bool | Whether dragging to adjust the order is supported (within the same container); enabled by default |
| drag_out    | true| bool | Whether dragging out is supported (between different containers in the same window); enabled by default |
| drag_alpha  | 216 | uint8_t | The transparency of the control when dragging to reorder |

## ControlDragable Attributes
The ControlDragable control inherits the `ControlDragableT` and `Control` attributes; for more available attributes, refer to the `ControlDragableT` and `Control` attributes

## BoxDragable Attributes
The BoxDragable control inherits the `ControlDragableT` and `Box` attributes; for more available attributes, refer to the `ControlDragableT` and `Box` attributes

## HBoxDragable Attributes
The HBoxDragable control inherits the `ControlDragableT` and `HBox` attributes; for more available attributes, refer to the `ControlDragableT` and `HBox` attributes

## VBoxDragable Attributes
The VBoxDragable control inherits the `ControlDragableT` and `VBox` attributes; for more available attributes, refer to the `ControlDragableT` and `VBox` attributes

## ControlMovableT Attributes (Template Class)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :---     | :---   | :---     | :--- |
| enable_move_pos  | true | bool    | Whether dragging to adjust the control's position is supported; enabled by default |
| move_pos_draggable_border |   | UiPadding | The border range of the movable rectangle of the control (the four edges can be clicked and dragged, but the center area cannot be dragged) |
| move_pos_non_draggable_margin |  | UiMargin | The margin of the movable rectangle of the control (the surrounding area defined by the margin cannot be clicked and dragged; only the center area can be dragged) |
| move_parent_pos  | false| bool    | When dragging to adjust the control's position, whether to adjust the parent container's position; "true" means adjust the parent container's position, "false" means adjust the control's own position |
| move_pos_alpha   | 216  | uint8_t | The transparency of the control when dragging to adjust its position |
| move_pos_reserve_width   | 20  | int | When moving horizontally, the height reserved inside the parent container, to prevent the control from completely overflowing the parent container (not DPI-scaled) |
| move_pos_reserve_height   | 20  | int | When moving vertically, the width reserved inside the parent container, to prevent the control from completely overflowing the parent container (not DPI-scaled) |
| move_pos_keep_within_parent   | false  | bool | When moving the control, ensure the child control stays within the parent container without overflow |

## ControlMovable Attributes
The ControlMovable control inherits the `ControlMovableT` and `Control` attributes; for more available attributes, refer to the `ControlMovableT` and `Control` attributes

## BoxMovable Attributes
The BoxMovable control inherits the `ControlMovableT` and `Box` attributes; for more available attributes, refer to the `ControlMovableT` and `Box` attributes

## HBoxMovable Attributes
The HBoxMovable control inherits the `ControlMovableT` and `HBox` attributes; for more available attributes, refer to the `ControlMovableT` and `HBox` attributes

## VBoxMovable Attributes
The VBoxMovable control inherits the `ControlMovableT` and `VBox` attributes; for more available attributes, refer to the `ControlMovableT` and `VBox` attributes

## ControlResizableT Attributes (Template Class)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :---     | :---   | :---     | :--- |
| enable_resize   | true  | bool | Whether the mouse can be dragged to change the control's size |
| enable_move_pos  | false | bool    | Whether dragging to adjust the control's position is supported; disabled by default; if enabled, see the `ControlMovableT` attributes for the related attributes |
| resize_size_box   | | UiRect | Sets the size of the stretchable range when resizing the four edges of the control |
| resize_reserve_width   | 10| int | Sets the minimum width reserved when resizing (not DPI-scaled) |
| resize_reserve_height   | 10| int | Sets the minimum height reserved when resizing (not DPI-scaled) |
| resize_keep_within_parent| false | bool | When resizing the control, whether to ensure the child control stays within the parent container without overflow |

The ControlResizableT control inherits the `ControlMovableT` attributes; for more available attributes, refer to the `ControlMovableT` attributes

## ControlResizable Attributes
The ControlResizable control inherits the `ControlResizableT` and `Control` attributes; for more available attributes, refer to the `ControlResizableT` and `Control` attributes

## BoxResizable Attributes
The BoxResizable control inherits the `ControlResizableT` and `Box` attributes; for more available attributes, refer to the `ControlResizableT` and `Box` attributes

## HBoxResizable Attributes
The HBoxResizable control inherits the `ControlResizableT` and `HBox` attributes; for more available attributes, refer to the `ControlResizableT` and `HBox` attributes

## VBoxResizable Attributes
The VBoxResizable control inherits the `ControlResizableT` and `VBox` attributes; for more available attributes, refer to the `ControlResizableT` and `VBox` attributes

## ListBoxItem Attributes
ListBoxItem is a concrete implementation of the ListBoxItemTemplate template class, defined in the `duilib/Box/ListBoxItem.h` file; there are three related type definitions:    
```
typedef ListBoxItemTemplate<Box> ListBoxItem;
typedef ListBoxItemTemplate<HBox> ListBoxItemH;
typedef ListBoxItemTemplate<VBox> ListBoxItemV;
```
As a sub-item in the ListBox container, ListBoxItem itself does not define any attributes.
The ListBoxItem inherits the `Option` attributes; for more available attributes, refer to the `Option` attributes

## TreeView Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| indent | | int | The indentation of tree nodes (each level of nodes is indented by one indent unit) |
| multi_select | false | bool | Whether multiple selection is supported |
| check_box_class | | string | The Class attribute for displaying the CheckBox; see the corresponding content in `global.xml` and the example programs for how to define it |
| expand_image_class | | string | The Class attribute for displaying the expand/collapse icon; see the corresponding content in `global.xml` and the example programs for how to define it |
| show_icon | | string | The Class attribute for displaying the icon; see the corresponding content in `global.xml` and the example programs for how to define it |

The TreeView control inherits the `ListBox` attributes; for more available attributes, refer to the `ListBox` attributes

## TreeNode Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| expand_normal_image | | string | Normal state image when expanded; see the corresponding content in `global.xml` and the example programs for how to define it |
| expand_hot_image | | string | Hover state image when expanded; see the corresponding content in `global.xml` and the example programs for how to define it |
| expand_pushed_image | | string | Pushed state image when expanded; see the corresponding content in `global.xml` and the example programs for how to define it |
| expand_disabled_image | | string | Disabled state image when expanded; see the corresponding content in `global.xml` and the example programs for how to define it |
| collapse_normal_image | | string | Normal state image when collapsed; see the corresponding content in `global.xml` and the example programs for how to define it |
| collapse_hot_image | | string | Hover state image when collapsed; see the corresponding content in `global.xml` and the example programs for how to define it |
| collapse_pushed_image | | string | Pushed state image when collapsed; see the corresponding content in `global.xml` and the example programs for how to define it |
| collapse_disabled_image | | string | Disabled state image when collapsed; see the corresponding content in `global.xml` and the example programs for how to define it |
| expand_image_right_space | | int | The space to the right of the expand image |
| check_box_image_right_space | | int | The space to the right of the CheckBox image |
| icon_image_right_space | | int | The space to the right of the icon |

The TreeNode control inherits the `ListBoxItem` attributes; for more available attributes, refer to the `ListBoxItem` attributes

## DirectoryTree Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| small_icon_size | 16 | int | The icon size of tree nodes |
| large_icon_size | 32 | int | The large icon size, used to display the contents inside the directory; the tree node itself does not use this attribute |
| show_hiden_files | false | bool | Whether hidden files are displayed |
| show_system_files | false | bool | Whether system files are displayed |

The DirectoryTree control inherits the `TreeView` attributes; for more available attributes, refer to the `TreeView` attributes

## ListCtrl Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| type | "report" | string | Type; valid values: "report", "icon", "list" |
| header_class | | string | The Class attribute of ListCtrlHeader; see the corresponding content in `global.xml` and the example programs for how to define it |
| header_item_class | | string | The Class attribute of ListCtrlHeaderItem; see the corresponding content in `global.xml` and the example programs for how to define it |
| header_split_box_class | | string | The Class attribute of ListCtrlHeader/SplitBox; see the corresponding content in `global.xml` and the example programs for how to define it |
| header_split_control_class | | string | The Class attribute of ListCtrlHeader/SplitBox/Control; see the corresponding content in `global.xml` and the example programs for how to define it |
| enable_header_drag_order | true | bool | Whether dragging the list header to change the column order is supported |
| check_box_class | | string | The Class attribute of the CheckBox (applied to the Header and ListCtrl data); see the corresponding content in `global.xml` and the example programs for how to define it |
| data_item_class | | string | The Class attribute of ListCtrlItem; see the corresponding content in `global.xml` and the example programs for how to define it |
| data_sub_item_class | | string | The Class attribute of ListCtrlItem/ListCtrlSubItem; see the corresponding content in `global.xml` and the example programs for how to define it |
| row_grid_line_width | | int | The width of the horizontal grid line |
| row_grid_line_color | | int | The color of the horizontal grid line |
| column_grid_line_width | | int | The width of the vertical grid line |
| column_grid_line_color | | int | The color of the vertical grid line |
| report_view_class | | string | The Class attribute of the ListBox in the data Report view; see the corresponding content in `global.xml` and the example programs for how to define it |
| header_height | | int | The height of the header control |
| data_item_height | | int | The default height of data items (row height) |
| show_header | true | bool | Whether the header control is displayed |
| multi_select | true | bool | Whether multiple selection is supported |
| enable_column_width_auto | true | bool | Whether double-clicking the splitter of the Header to automatically adjust the column width is supported |
| auto_check_select | false | bool | Whether the selected data items are automatically checked (applies to the Header and each row) |
| show_header_checkbox | false | bool | Whether a CheckBox is shown at the far left of the header |
| show_data_item_checkbox | false | bool | Whether a CheckBox is shown at the beginning of each row |
| icon_view_class | | string | The Class attribute of the ListBox in the data Icon view; see the corresponding content in `global.xml` and the example programs for how to define it |
| icon_view_item_image_class | | string | The Class attribute of the image in the sub-items of the ListBox in the data Icon view; see the corresponding content in `global.xml` and the example programs for how to define it |
| icon_view_item_label_class | | string | The Class attribute of the Label in the sub-items of the ListBox in the data Icon view; see the corresponding content in `global.xml` and the example programs for how to define it |
| list_view_class | | string | The Class attribute of the ListBox in the data List view; see the corresponding content in `global.xml` and the example programs for how to define it |
| list_view_item_class | | string | The Class attribute of the sub-items of the ListBox in the data List view; see the corresponding content in `global.xml` and the example programs for how to define it |
| list_view_item_image_class | | string | The Class attribute of the image of the sub-items of the ListBox in the data List view; see the corresponding content in `global.xml` and the example programs for how to define it |
| list_view_item_label_class | | string | The Class attribute of the Label of the sub-items of the ListBox in the data List view; see the corresponding content in `global.xml` and the example programs for how to define it |
| enable_item_edit | true | bool | Whether sub-item editing is supported |
| list_ctrl_richedit_class | | string | The Class attribute of the edit box; see the corresponding content in `global.xml` and the example programs for how to define it |

The ListCtrl control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes    
The views of the ListCtrl control inherit the `ListBox` attributes; for more available attributes, refer to the `ListBox` attribute settings: [Box.md](Box.md); the view attributes need to be set in `global.xml`.

## PropertyGrid Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| property_grid_xml | | string | The configuration file XML; if empty, the default is "public/property_grid/property_grid.xml" |
| row_grid_line_width | | int | The width of the horizontal grid line |
| row_grid_line_color | | int | The color of the horizontal grid line |
| column_grid_line_width | | int | The width of the vertical grid line |
| column_grid_line_color | | int | The color of the vertical grid line |
| header_class | | string | The Class attribute of the header; see the corresponding content in `global.xml` and the example programs for how to define it |
| group_class | | string | The Class attribute of the group; see the corresponding content in `global.xml` and the example programs for how to define it |
| propterty_class | | string | The Class attribute of the property; see the corresponding content in `global.xml` and the example programs for how to define it |
| left_column_width | | int | The width of the left column |

The PropertyGrid control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes

## ColorPicker Attributes
ColorPicker is a window; see the menu in the example programs for the specific usage    
The ColorPicker control inherits the `Window` attributes; for more available attributes, refer to the `Window` attributes

## ControlDragable Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| drag_order | true | bool | Whether dragging to adjust the order is supported (within the same container) |
| drag_alpha | 216 | int | Sets the transparency of the control when dragging to reorder (0 - 255) |
| drag_out | true | bool | Whether dragging out is supported (between different containers in the same window) |

The ControlDragable control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## CefControl Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| url |  | string | After the control is created, navigate to this URL |
| url_is_local_file |  | string | Whether the URL specified by url is a local file; if it is a local file and a relative path is specified, the root directory is the directory of the executable |
| F11 | true | bool | Whether the F11 shortcut key is allowed (fullscreen / exit fullscreen) |
| F12 | true | bool | Whether the F12 shortcut key is allowed (show/hide developer tools) |
| download_favicon_image | false | bool | Whether to download the website's FavIcon |

The CefControl control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## WebView2Control Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| url |  | string | After the control is created, navigate to this URL |
| url_is_local_file |  | string | Whether the URL specified by url is a local file; if it is a local file and a relative path is specified, the root directory is the directory of the executable |
| F11 | true | bool | Whether the F11 shortcut key is allowed (fullscreen / exit fullscreen) |
| F12 | true | bool | Whether the F12 shortcut key is allowed (show/hide developer tools) |
| devtools_enabled | true | bool | Whether developer tools are allowed to be opened |

The WebView2Control control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## IconControl Attributes
The IconControl control inherits the `Control` attributes; for more available attributes, refer to the `Control` attributes

## BitmapControl Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| bitmap_halign  | left | string | The horizontal alignment of the image; valid values: "left", "center", "right" |
| bitmap_valign  | top | string | The vertical alignment of the image; valid values: "top", "center", "bottom" |
| bitmap_alpha  | 255 | int | The transparency when the image is drawn; valid range: 0 - 255 |
| bitmap_dest  |  | rect | The position and size of the destination area for drawing the image (position relative to the control area) |
| bitmap_src  |  | rect | The position and size of the source area for drawing the image |
| bitmap_margin  |  | rect | The margin within the drawing destination area (invalid if a dest value is specified) |
| bitmap_adaptive_dest_rect  | false | bool | Whether to automatically fit the destination area when drawing (scale the image proportionally) |
| bitmap_stretch  | false | bool | Whether to stretch the image when drawing (mutually exclusive with IsAdaptiveDestRect(); lower priority than IsAdaptiveDestRect()) |
| bitmap_multi_thread  | true | bool | Whether multi-threaded bitmap data operations are supported (if not specified, the default is true; multi-threaded bitmap data operations are supported by default) |

The BitmapControl control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

## AddressBar Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| path_tooltip | true | bool | Sets whether to show the tooltip of the path |
| return_update_ui | true | bool | Sets whether to automatically update the displayed control when Enter is pressed |
| esc_update_ui | true | bool | Sets whether to automatically update the displayed control when ESC is pressed |
| kill_focus_update_ui | true | bool | Sets whether to automatically update the displayed control when focus is lost |
| rich_edit_class | "address_bar_edit" | string | Sets the Class of the edit box |
| rich_edit_clear_btn_class | "rich_edit_clear_btn" | string | Sets the Class of the clear button of the edit box |
| sub_path_hbox_class | "address_bar_sub_path_hbox"| string | Sets the Class of the address bar path container (HBox); one HBox container per sub-path |
| sub_path_button_class | "address_bar_sub_path_button"| string | Sets the Class of the address bar sub-path button |
| sub_path_root_class | "address_bar_sub_path_root" | string | Sets the Class of the address bar root path ("/" path) |
| path_separator_class | "address_bar_path_separator" | string | Sets the Class of the address bar path separator |

The AddressBar control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

## ChildWindow Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| child_window_margin | rect | UiMargin | Sets the margin of the child window; the space of the margin can hold other controls |

The ChildWindow control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes
