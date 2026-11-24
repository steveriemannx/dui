## Control (Basic Control) Attributes

The `Control` control is the base class of all available controls and contains the commonly used attributes of controls, such as width, height, margin, etc. Generally it is not used as a concrete control, but it is often used as a placeholder; please refer to the examples.
`Control` is the base class of all controls and contains the common attributes available to all controls.

```xml
<!-- A Control that stretches automatically to the width of its parent container; usually used as a placeholder -->
<Control width="stretch"/>
```

| Attribute Name | Default Value | Parameter Type | Function in [Control.h](../include/dui/Core/Control.h) | Purpose |
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

## ControlLoading and Skeleton Screens

Every control derives from `Control`, so all of them support the `loading` attribute plus the `StartLoading` / `StopLoading` interfaces, which can be used for loading placeholders over lists, tables and card areas.

### How skeleton screens differ from other loading states

| State | When to use | How to implement |
| :--- | :--- | :--- |
| Spinner | Layout structure unknown, or loading takes only a few hundred ms | `loading*.xml` + auto-playing json animation |
| Progress-bar loading | Progress is computable (download, upload, batch tasks) | Static-frame json + `SetImageAnimationFrame` driven manually in `kEventLoading` |
| **Skeleton screen** | Structure is known and the layout should be previewed up front | Grey placeholder blocks (see below); **the business side must implement a timeout fallback** |
| Empty state | Loading finished but there is no data | A real control, not loading |
| Error state | Loading failed | A real control plus a retry entry |

### Four hard constraints for skeleton screens

All four have been hit in practice; follow them when writing skeleton XML or code.

**① The loading layer is painted on top, not used as a mask.**
`ControlLoading` creates a standalone Box via `GlobalManager::CreateBox`. It is attached to the Window but is not part of the control tree, and is drawn above the target control through `Control::PaintLoading` → `AlphaPaint`. Therefore the **root container must have an opaque `bkcolor`**, otherwise the existing content underneath shows through.

**② Size: use `width='0' height='0'` together with a `stretch` root container to fill the target control.**
In `UpdateLoadingPos`, `stretch` is neither `IsAuto()` nor `IsInt32()`, so the computed size stays 0 and is then replaced by `if (nBoxWidth <= 0 || nBoxWidth > ctrlWidth) nBoxWidth = ctrlWidth`, i.e. the full control size — exactly the "fill the control including the header" effect, with no size calculation needed in C++. Use `auto` only when the size should follow the content (that path goes through `EstimateSize()`).

**③ A skeleton screen has no "animation finished" moment, so the business side must time it out.**
Spinners and progress bars can stop automatically via `auto_stop='true'`, but a skeleton is a static placeholder. So **do not set `animation_control` or `auto_stop`**; instead check the timeout inside `kEventLoading`:

```cpp
const uint64_t nTimeoutMs = 5000;
if (pLoadingStatus->m_nElapsedTimeMs >= nTimeoutMs) {
    pLoadingStatus->m_bStopLoading = true;
}
```

Note that `nMaxCount` in `StartLoading(nIntervalMs, nMaxCount)` is a **callback count limit, not a timeout**; the only way to time out is to inspect `ControlLoadingStatus::m_nElapsedTimeMs`. A skeleton screen without a timeout fallback is worse than a spinner, because the user loses the certainty that loading is still in progress.

**④ Do not call `SetAlpha` on the root container for the breathing animation.**
`Control::SetAlpha` applies to the **entire subtree**, and the root container's `bkcolor` is the only thing masking the content below (see ①). As soon as alpha drops the mask fails and the old content shows through. Keep the root at 255 and only animate the placeholder children:

```cpp
// Keep the root opaque; only the placeholder blocks breathe
const uint8_t nAlpha = (uint8_t)(197.5 + 57.5 * std::sin(fSeconds * 3.1415926));
for (size_t i = 0; i < pLoadingUiRootBox->GetItemCount(); ++i) {
    ui::Control* pChild = pLoadingUiRootBox->GetItemAt(i);
    if (pChild != nullptr) {
        pChild->SetAlpha(nAlpha);
    }
}
```

The child-control API on `Box` is `GetItemCount()` / `GetItemAt(i)` — there is **no** `GetChildCount()` / `GetChildAt()`. An alpha range of roughly 140~255 keeps the placeholders clearly readable. Changing a child's alpha does not trigger layout recalculation.

### Two more XML gotchas

- **`valign` only accepts `top` / `center` / `bottom`.** `vcenter` is only valid for the `text_align` attribute. An invalid value hits `else { ASSERT(0); }` at the end of `Control::SetAttribute`, which crashes debug builds outright. `halign` likewise only accepts `left` / `center` / `right`.
- **Sibling nodes at the same level should uniformly use `height="stretch"`, or uniformly use `auto` — do not mix them.** Mixing causes one row to be stretched while another follows its content height, producing asymmetric whitespace. Use margin for row spacing (top row `margin="0,0,0,12"`, bottom row `margin="0,12,0,0"`).

### Sample code

Starting a skeleton screen from C++:

```cpp
const DString strSkin = _T("loading_skeleton_list.xml");
const DString strLoading = _T("file='") + strSkin + _T("' width='0' height='0' "
    _T("offset_x='-1' offset_y='-1' valign='center' halign='center' fade='255'"));
pListCtrl->SetLoadingAttribute(strLoading);   // returns bool — check for failure

pListCtrl->AttachLoading([pListCtrl](const ui::EventArgs& args) {
    ui::ControlLoadingStatus* pStatus = (ui::ControlLoadingStatus*)args.wParam;
    if (pStatus == nullptr) {
        return false;
    }
    if (pStatus->m_nElapsedTimeMs >= 5000) {   // timeout fallback
        pStatus->m_bStopLoading = true;
        return true;
    }
    ui::ControlPtrT<ui::Box> pRootBox = pStatus->m_pLoadingUiRootBox;
    const uint8_t nAlpha = (uint8_t)(197.5 + 57.5 * std::sin(
        (double)pStatus->m_nElapsedTimeMs / 1000.0 * 3.1415926));
    for (size_t i = 0; i < pRootBox->GetItemCount(); ++i) {
        ui::Control* pChild = pRootBox->GetItemAt(i);
        if (pChild != nullptr) {
            pChild->SetAlpha(nAlpha);
        }
    }
    return true;
});

pListCtrl->StartLoading(33, -1);   // 33 ms heartbeat, -1 means "no count-based stop"
```

Skeleton XML skeleton (see `bin/resources/themes/default/list_ctrl/loading_skeleton_list.xml` for the full version):

```xml
<Loading>
    <VBox height="stretch" width="stretch" bkcolor="bg_list" mousechild="false" padding="8,6,8,6">
        <HBox height="24" width="stretch" margin="0,0,0,6">
            <Control width="16" height="16" bkcolor="bg_progress_track" border_round="3,3" valign="center"/>
            <VBox width="stretch" height="auto" margin="12,0,0,0" valign="center">
                <Control width="180" height="11" bkcolor="bg_progress_track" border_round="3,3"/>
                <Control width="110" height="9" bkcolor="bg_progress_track" border_round="3,3" margin="0,5,0,0"/>
            </VBox>
        </HBox>
    </VBox>
</Loading>
```

### Available skeleton skins

Located under `bin/resources/themes/default/list_ctrl/`:

- `loading_skeleton_list.xml` — list skeleton (toolbar placeholder + 12 rows)
- `loading_skeleton_card.xml` — card skeleton (3 columns × 2 rows, each card = large image slot + title + subtitle)

See the "Skeleton Test (List)", "Skeleton Test (Card)" and "Stop Loading" buttons in the `examples/ListCtrl` demo.

**Note:** `StopLoading()` destroys the loading Box, and the next `StartLoading` re-parses the XML, so a skeleton skin must be independently parseable every time. Also, `Control::StartLoading` internally calls `SetEnabled(false)`; if loading gets stuck, the control stays disabled and the whole area becomes unclickable, so provide an unconditional "StopLoading" escape hatch while debugging.

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

## Marquee Attributes
Marquee is a text marquee (scrolling text) control derived from Label (reusing all text attributes: `text`/`text_id`/`font`/`text_color`/`vertical_text`/`text_padding`, etc.). Header file: [Marquee.h](../include/dui/Control/Marquee.h). It scrolls text horizontally/vertically in a loop: when the text exceeds the control's display area it scrolls automatically in a seamless loop; otherwise it is displayed statically (aligned by `text_align`).

In addition to the Label attributes, the following attributes are added:

| Attribute | Default | Type | Description |
| :--- | :--- | :--- | :--- |
| scroll_direction | left | string | Scroll direction: "left" horizontal leftward / "right" horizontal rightward / "up" vertical upward / "down" vertical downward. Also determines the text orientation (left/right = horizontal text, up/down = vertical text) |
| scroll_speed | 50 | int | Scroll speed (pixels/second, auto DPI-scaled); 0 means no scrolling |
| scroll_interval | 30 | int | Scroll timer frame interval (ms, 5~1000), affects smoothness and CPU usage |
| hover_pause | true | bool | Whether to pause scrolling when the mouse hovers over the control |

Notes:
- **Loop scrolling**: when text exceeds the display area, two copies of the text are drawn end-to-end for a seamless loop; when it fits, the text is displayed statically (aligned by `text_align`) without using a timer.
- **Text orientation**: `left`/`right` scroll horizontal text, `up`/`down` scroll vertical text (sets `vertical_text` internally).
- **Dynamic update**: `SetText`/`SetTextId` resets the scroll position automatically; language switching via `text_id` works as well.
- **Pause/resume**: `Pause()`/`Resume()`/`SetPaused()` control manual pause; hover pause is controlled by `hover_pause` (enabled by default).

C++ API: `SetScrollDirection/GetScrollDirection`, `SetScrollSpeed/GetScrollSpeed`, `SetScrollInterval/GetScrollInterval`, `SetHoverPause/IsHoverPause`, `Pause/Resume/SetPaused/IsPaused`, `IsScrolling`.

### XML Usage Example

```xml
<!-- Horizontal leftward scroll (loops when text exceeds the control width) -->
<Marquee width="200" height="28" text_id="STRID_NEWS_TICKER"
          scroll_direction="left" scroll_speed="60"
          text_color="text_default"/>

<!-- Vertical upward scroll (vertical text) -->
<Marquee width="28" height="300" text="A vertically scrolling announcement"
          scroll_direction="up" scroll_speed="40" font="system_14"/>

<!-- Horizontal rightward scroll, no hover pause -->
<Marquee width="300" height="24" text="Text scrolling to the right"
          scroll_direction="right" hover_pause="false"/>
```

### C++ Usage Example

```cpp
ui::Marquee* pMarquee = dynamic_cast<ui::Marquee*>(pWindow->FindControl(_T("marquee")));
if (pMarquee != nullptr) {
    // Dynamically update text (resets scroll position automatically)
    pMarquee->SetText(_T("New scrolling announcement content"));

    // Adjust speed and direction
    pMarquee->SetScrollSpeed(80);
    pMarquee->SetScrollDirection(ui::MarqueeDirection::kLeft);

    // Pause / resume
    pMarquee->Pause();
    pMarquee->Resume();
}
```

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

## Switch Attributes
Switch is a toggle switch control derived from CheckBox, used for on/off state switching. Header file: [Switch.h](../include/dui/Control/Switch.h).
Both the track and the thumb can be self-drawn with semantic colors or skinned with SVG images: when images are configured, the off/on images cross-fade with the animation progress while the thumb image slides along the track; when no images are configured, colors are self-drawn.

In addition to the common attributes of CheckBox, the following new attributes are added:

| Attribute | Default | Type | Purpose |
| :--- | :--- | :--- | :--- |
| switch_animation_ms | 160 | int | Transition animation duration in milliseconds (0 disables animation) |
| track_off_color | bg_switch_track_off | string | Track color in unselected state (semantic color name, effective without track images) |
| track_on_color | bg_switch_track_on | string | Track color in selected state (semantic color name, effective without track images) |
| thumb_off_color | bg_switch_thumb_off | string | Thumb color in unselected state (semantic color name, effective without thumb images) |
| thumb_on_color | bg_switch_thumb_on | string | Thumb color in selected state (semantic color name, effective without thumb images) |
| track_off_image | | string | Track image in unselected state (image attribute string, paired with track_on_image) |
| track_on_image | | string | Track image in selected state (image attribute string, paired with track_off_image) |
| thumb_off_image | | string | Thumb image in unselected state (square image, paired with thumb_on_image) |
| thumb_on_image | | string | Thumb image in selected state (square image, paired with thumb_off_image) |
| thumb_padding | 0 | int | Distance between the thumb image square box and the track edges (auto DPI scaled). The square box side equals the track height; shadow padding must be drawn inside the image itself |

Notes:
- Track images are stretched to fill the whole track rectangle; thumb images translate inside a square box as high as the track. Do not set width/height/valign/halign in the image attribute string, otherwise the draw position will be fixed.
- Fixed colors inside images can be replaced with semantic colors via `svg_replace_colors` (multiple replacements separated by semicolons) to adapt to light/dark themes.
- Track and thumb layers are independent: when only thumb images are configured, the track is still self-drawn with colors (this is how the iOS-style skin works).

Corresponding C++ interfaces: `SetAnimationDuration/GetAnimationDuration`, `SetTrackOffColor/GetTrackOffColor`, `SetTrackOnColor/GetTrackOnColor`, `SetThumbOffColor/GetThumbOffColor`, `SetThumbOnColor/GetThumbOnColor`, `SetTrackOffImage/GetTrackOffImage`, `SetTrackOnImage/GetTrackOnImage`, `SetThumbOffImage/GetThumbOffImage`, `SetThumbOnImage/GetThumbOnImage`, `SetThumbPadding/GetThumbPadding`.
Selection state changes trigger `kEventSelect`/`kEventUnSelect` events (same as CheckBox).

### XML Usage Example (see the controls and color_theme example programs)

```xml
<!-- Using default skins (classes defined in global.xml) -->
<Switch class="switch" selected="false"/>
<Switch class="switch_fluent" selected="true"/>
<Switch class="switch_material" selected="true"/>

<!-- Pure color self-drawing: no images configured, specify track/thumb colors (semantic color names or color values) -->
<Switch width="51" height="31" selected="true"
        track_off_color="bg_switch_track_off" track_on_color="bg_switch_track_on"
        thumb_off_color="bg_switch_thumb_off" thumb_on_color="bg_switch_thumb_on"/>

<!-- Custom image skin: off/on track images filling the track + off/on square thumb images (see switch_fluent) -->
<Switch width="48" height="24" padding="2,2,2,2" cursor_type="hand"
        track_off_image="file='public/switch/switch_fluent_track_off.svg' svg_replace_colors='#E9E9EA|bg_switch_track_off;#8A8886|bg_switch_thumb_off'"
        track_on_image="file='public/switch/switch_fluent_track_on.svg' svg_replace_colors='#0078D4|bg_switch_track_on'"
        thumb_off_image="file='public/switch/switch_fluent_thumb_off.svg' svg_replace_colors='#8A8886|bg_switch_thumb_off'"
        thumb_on_image="file='public/switch/switch_fluent_thumb_on.svg'"/>

<!-- XML events: link other controls on state change (type is select/unselect, same as CheckBox) -->
<Switch class="switch">
    <Event type="select" receiver="target_control" apply_attribute="visible='true'"/>
    <Event type="unselect" receiver="target_control" apply_attribute="visible='false'"/>
</Switch>
```

Three skin classes are defined in global.xml by default (SVG assets are in the `public/switch/` directory):
- `switch` (alias `switch_ios`): iOS style, 51x31, large white thumb with soft shadow, track color follows the theme accent color;
- `switch_fluent`: Windows 11 Fluent style, 48x24 (padding=2, track 44x20), outlined small track + 14px small thumb;
- `switch_material`: Material Design 3 style, 52x32; unselected shows an outlined track with a 16px small thumb, selected shows an accent-filled track with a 24px large thumb carrying a check mark.

Semantic colors are defined in the `color_light` and `color_dark` global.xml files as `bg_switch_track_on/off` and `bg_switch_thumb_on/off`; SVG colors in the Fluent/Material skins are mapped to these semantic colors or `color_accent` via `svg_replace_colors`.

### C++ Usage Example

```cpp
// Get the control pointer (name="switch_test" in XML)
ui::Switch* pSwitch = dynamic_cast<ui::Switch*>(pWindow->FindControl(_T("switch_test")));
if (pSwitch != nullptr) {
    // Listen for state change events (same as CheckBox)
    pSwitch->AttachSelect([](const ui::EventArgs& args) {
        // Switched to selected state
        return true;
        });
    pSwitch->AttachUnSelect([](const ui::EventArgs& args) {
        // Switched to unselected state
        return true;
        });

    // Read/toggle the selected state dynamically (plays the slide animation and fires select/unselect events)
    bool bSelected = pSwitch->IsSelected();
    pSwitch->Selected(!bSelected);

    // Set the appearance dynamically (color self-drawing; use SetTrackXxxImage/SetThumbXxxImage for image skins)
    pSwitch->SetTrackOnColor(_T("#FF0078D4"));
    pSwitch->SetAnimationDuration(200);
}
```

## Badge Attributes
Badge is a badge control derived from Label, used to display unread counts/new message counts/red dots on TabCtrl tabs, buttons, icons and other controls. Header file: [Badge.h](../include/dui/Control/Badge.h).
Two forms: count badge (rounded capsule background + number, shows "99+" when exceeding the limit) and dot badge (dot mode, a small round dot). The count badge is automatically hidden when count <= 0 (no badge when there are no unread items); the dot badge is unaffected by count, and its visibility is controlled by the visible attribute.
Badge inherits the text attributes of Label (font, normal_text_color, text_padding, etc.); the background is a rounded capsule self-drawn with semantic colors (corner radius = half the height), automatically adapting to light/dark themes.

| Attribute | Default | Type | Purpose |
| :--- | :--- | :--- | :--- |
| count | 0 | int64 | Badge count: in count mode, shows the badge when greater than 0 and automatically hides when less than or equal to 0 (dot mode is unaffected) |
| max_count | 99 | int64 | Count limit, shows "limit+" (e.g. "99+") when exceeded |
| dot | false | bool | Dot mode: true shows a pure small round dot without a number |
| badge_color | bg_badge | string | Badge background color (semantic color name or color value) |

Notes:
- Badge is used as a child control inside any Box container; to float it on the corner of a host control, use `float="true"` with the `margin` attribute.
- The count bubble width adapts to the number of digits (`width="auto"`); a single digit appears as a circle; the dot size is determined by the XML width/height (default skin 8x8).
- Child controls are clipped at the parent container boundary; ensure the parent container is large enough for a floating badge (see the controls example: parent Box width = button width + badge overhang).
- Badge does not respond to the mouse (`mouse_enabled="false"`), so it does not block clicks on the host control.

Corresponding C++ interfaces: `SetCount/GetCount`, `SetMaxCount/GetMaxCount`, `SetDotMode/IsDotMode`, `SetBadgeColor/GetBadgeColor`.
Usage example (see the controls example program):

```xml
<!-- Default skin: count badge (automatically hidden when count <= 0) -->
<Badge class="badge" count="5"/>
<Badge class="badge" count="120"/>   <!-- Shows "99+" -->

<!-- Dot badge -->
<Badge class="badge_dot"/>

<!-- Floating on the button's top-right corner: parent Box width = button width + badge overhang, positioned with float+margin -->
<Box width="66" height="30">
    <Button class="btn_global" text="Messages" width="48" height="30"/>
    <Badge class="badge" count="6" float="true" margin="42,0,0,0"/>
</Box>

<!-- Badge on a TabCtrl tab: TabCtrlItem has built-in badge_count/badge_dot/badge_class attributes, no manual nesting needed -->
<TabCtrlItem class="tab_ctrl_item" title="Messages" badge_count="3"/>
```

Updating the unread count dynamically in C++:

```cpp
ui::Badge* pBadge = dynamic_cast<ui::Badge*>(pWindow->FindControl(_T("badge_msg")));
if (pBadge != nullptr) {
    pBadge->SetCount(pBadge->GetCount() + 1);   // Unread +1, shows "99+" when exceeding 99
    pBadge->SetCount(0);                        // Reset to zero, the badge is hidden automatically
}
```

Two skin classes are defined in global.xml by default:
- `badge`: count badge, height 18, white text on red background (semantic colors `text_badge`/`bg_badge`), width adapts to the digits, text_padding="5,0,5,0";
- `badge_dot`: dot badge, 8x8, dot="true".

The semantic color `bg_badge` is defined in the color_light and color_dark global.xml files, derived from `color_error` (red); the count text color `text_badge` is white.

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
| spin_class | | string | The Class attribute of the Spin control in the date; only valid when using native backend; default value: "rich_edit_spin_box,rich_edit_spin_btn_up,rich_edit_spin_btn_down" |

The DateTime control inherits the `Label` attributes; for more available attributes, refer to the `Label` attributes

## Calendar Attributes
| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| calendar_mode | "single" | string | Selection mode: "single" for single date, "range" for date range selection |
| first_day_of_week | "monday" | string | First day of week: "monday" or "sunday" |
| min_date | | string | Minimum selectable date (yyyy-mm-dd), empty means no limit |
| max_date | | string | Maximum selectable date (yyyy-mm-dd), empty means no limit |

The Calendar control inherits the `Control` attributes. For more available attributes, please refer to the `Control` attributes.

The Calendar control triggers the `kEventViewModeChanged` event when its view mode (month/year/decade) changes (wParam = new view mode 0/1/2, lParam = old view mode 0/1/2), which can be subscribed via `AttachViewModeChanged`; it triggers the `kEventDisplayDateChanged` event when the displayed period (the month in month-view, the year in year/decade-view) changes, which can be subscribed via `AttachDisplayDateChanged` (typically used to refresh the flyout title bar on page-turn or cross-month focus moves); it also triggers `kEventValueChanged` when the selected date or date range changes (consistent with DateTime).

The calendar flyout supports the following keyboard interactions (effective once the flyout opens and takes focus; Esc closes the flyout):

| Shortcut | Month view | Year view | Decade view |
| :--- | :--- | :--- | :--- |
| ← / → | Move focus ±1 day | Move focus ±1 month | Move focus ±1 year |
| ↑ / ↓ | Move focus ±1 week | Move focus ±3 months | Move focus ±3 years |
| Ctrl + ← / → / ↑ / ↓ | Move focus ±1 year | Move focus ±10 years (prev/next decade) | Move focus ±100 years (prev/next century) |
| Home | Jump to first day of month | Jump to first month of year (Jan) | Jump to first year of decade |
| End | Jump to last day of month | Jump to last month of year (Dec) | Jump to last year of decade |
| PageUp / PageDown | Prev / next month | Prev / next year | Prev / next decade |
| Enter / Space | Select focused date (single: confirm directly; range: enter "awaiting end" or confirm range) | Drill down to month view | Drill down to year view |
| Shift + ← / → / ↑ / ↓ (range mode) | Extend range from focused anchor (live highlight of start/end), Enter/Space again to confirm | — | — |

Additional notes:
- Focus movement and page-turning are both clamped to the `min_date`/`max_date` limits, so the focus ring never lands on a disabled date; page-turning stops at the earliest/latest selectable period boundary.
- Keyboard range extension in range mode shares the same start/end logic and live-preview drawing with mouse dragging.

## CalendarFlyout Attributes
CalendarFlyout is a date picker flyout based on Flyout hosting a Calendar control, corresponding to header [CalendarFlyout.h](../include/dui/Control/CalendarFlyout.h). It displays a calendar panel around an anchor control, supporting single/range selection and month/year/decade navigation.

Key features:
- Inherits Flyout: 8 placement directions, auto-flip on insufficient space, click-outside/Esc to close;
- Built-in XML layout: header navigation (prev/title/next) + Calendar + footer buttons (Today/Clear);
- Single mode: auto-close and trigger `AttachDateSelected` after date selection;
- Range mode: drag to select start/end dates, auto-close and trigger `AttachDateSelected`;
- Three-level navigation: click title to cycle month/year/decade views.

### C++ Interfaces

| Interface | Description |
| :--- | :--- |
| `CalendarFlyout(Window* pParentWindow)` | Construct flyout with parent window (anchor must belong to it) |
| `bool ShowAt(Control* anchor, const struct tm& initDate, Placement = Bottom)` | Show calendar flyout around anchor; returns false if creation failed (object auto-destroyed) |
| `SetMode(int32_t mode)` | Set selection mode: 0=single, 1=range |
| `GetMode()` | Get selection mode |
| `SetInitRange(start, end)` | Set initial range for range mode |
| `SetFirstDayOfWeek(dayOfWeek)` | Set first day of week: 0=Sunday, 1=Monday (default 1) |
| `SetDateLimit(minDate, maxDate)` | Set min/max date limit (yyyy-mm-dd) |
| `AttachDateSelected(callback)` | Register date selection callback (two-arg signature, single mode only) with `(WPARAM wParam, LPARAM lParam)`; wParam=0 single, 1 range; lParam is time_t for single, meaningless for range |
| `AttachDateSelectedEx(callback)` | Register date selection callback (three-arg signature, recommended) with `(WPARAM wParam, LPARAM lParam, const Calendar::DateRange* pRange)`; read full 64-bit start/end from pRange for range mode |
| `AttachDateCleared(callback)` | Register clear callback (triggered by "Clear" button) |

### Usage Example

```cpp
//Single date selection
ui::CalendarFlyout* pFlyout = new ui::CalendarFlyout(this);
pFlyout->SetMode(0);
struct tm today = ui::Calendar::GetToday();
pFlyout->ShowAt(pAnchor, today, ui::Flyout::Placement::Bottom);
pFlyout->AttachDateSelected([this](WPARAM wParam, LPARAM lParam) {
    if (wParam == 0) {
        time_t t = (time_t)lParam;
        struct tm date = ui::Calendar::TimeTToDate(t);
        DString text = ui::Calendar::FormatDateString(date);
        //Handle selected date
    }
});

//Range selection
ui::CalendarFlyout* pFlyout = new ui::CalendarFlyout(this);
pFlyout->SetMode(1);
pFlyout->ShowAt(pAnchor, today, ui::Flyout::Placement::Bottom);
pFlyout->AttachDateSelectedEx([this](WPARAM wParam, LPARAM lParam, const ui::Calendar::DateRange* pRange) {
    if (wParam == 1) {
        //Range mode: read full 64-bit start/end from pRange
        struct tm start = ui::Calendar::TimeTToDate(pRange->start);
        struct tm end = ui::Calendar::TimeTToDate(pRange->end);
        //Handle selected range
    }
});
```

The DateTime control (`edit_format="date_calendar"`) already uses CalendarFlyout internally; no manual creation needed.

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
| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| horizontal | true | bool | Whether horizontal; "true" means horizontal, "false" means vertical |
| min | 0 | int | Minimum progress value, e.g. "0" |
| max | 100 | int | Maximum progress value, e.g. "100" |
| value | 0 | double | Progress value (supports floating point), e.g. "50" or "0.5" |
| progress_image |  | string | Foreground image of the progress bar |
| stretch_fore_image | true | bool | Specify whether the progress bar foreground image is scaled for display |
| progress_color |  | string | Foreground color of the progress bar (semantic color name or value), e.g. "bg_progress" |
| progress_radius | 0 | int | Corner radius of the solid-color foreground (px, auto DPI-scaled, 0 = square) |
| buffer_value | -1 | double | Secondary progress value (buffer, usually slightly ahead of the main value, e.g. cached position; not drawn when <= min) |
| buffer_color |  | string | Secondary progress color (semantic color name or value; empty = a lighter shade of the foreground color) |
| show_percent | false | bool | Whether to auto-display the percentage text (overrides the control text) |
| animation_enabled | false | bool | Whether to enable value-change animation (default off) |
| animation_duration | 300 | int | Value-change animation duration (ms) |
| animation_easing | EaseOutQuad | string | Easing curve of the value-change animation; values are exactly the `EasingFunctionType` enum names, e.g. "EaseLinear", "EaseOutQuad", "EaseOutElastic", "EaseInOutBounce" (first letter case-insensitive; unrecognized values fall back to `EaseLinear`) |
| marquee | false | bool | Whether to scroll/marquee display |
| marquee_width | | int | Marquee width |
| marquee_step | | int | Marquee step |
| reverse | false | bool | Whether the fill direction is inverted: with `false`, value=min is empty and value=max is full; with `true` it is the opposite (value=min is full). The ratio is converted correctly even when `min != 0` |

The Progress control inherits the `Label` attributes. For more available attributes, please refer to the `Label` attributes.

Notes:
- `value` supports floating-point values (XML parsed as double; `SetValue(double)` likewise), suitable for 0~1 range scenarios such as download progress or volume.
- Use either `progress_color` or `progress_image` as the foreground; the solid-color foreground can be rounded via `progress_radius`. The track background uses `bkcolor` (a common Control attribute).
- `buffer_value` sets the secondary progress (the "buffered + played" dual progress of video/download), drawn as a lighter bar under the main progress; when `buffer_color` is not specified, a lighter shade (about 40% opacity) of the foreground color is used.
- With `show_percent` enabled, the control text is auto-updated to "xx%" (following the value-change animation); customize the format via `SetTextFormatCallback` (e.g. "5/10", "3.2 GB").
- With `animation_enabled` enabled, `SetValue` eases from the current value to the new value (duration `animation_duration`, easing curve given by `animation_easing`, default `EaseOutQuad`); Slider drags are instant and normally do not need this.
- Changing `min`/`max` at runtime **automatically clamps the current value back into the new range** (including `buffer_value` and the animation intermediate value), so `GetValue()` is always within `[min, max]` and the member value never diverges from what is drawn. Call `ClampCurrentValue()` to clamp manually.
- `progress_radius` now applies to the **solid main progress, the buffer progress, and the marquee block alike**, so all three share the same style.
- The marquee wraps with a period of "track length + block width" (modulo, with negative-value handling) and resets to 0 when the track length is <= 0.
- Value-change event: `AttachValueChanged` (`EventArgs::wParam` = new value, `lParam` = old value, both truncated to integer; read the exact floating-point value via `GetValue()`).
- When syncing values inside the framework or application code, use `SetValueNoEvent()`: it sets the value without firing `kEventValueChanged` (same semantics as `SetTextNoEvent` for text controls). Intended for the case where one event source drives several controls at once, so value-change callbacks are not re-triggered into a loop; `Slider` and `ColorSlider` inherit this API.
- Completion event: `AttachProgressCompleted` — fired once when the value reaches the completion endpoint (max for forward, min for countdown).
- Recommended semantic colors: foreground `bg_progress`, track `bg_progress_track` (auto-switch with light/dark theme).

### Animation easing curve
`animation_easing` reuses the framework's existing `ui::EasingFunctionType` enum (see [EasingFunctions.h](../include/dui/Animation/EasingFunctions.h)), which provides 31 curves:

```xml
<!-- linear -->
<Progress name="p1" min="0" max="100" value="70" animation_enabled="true" animation_duration="600" animation_easing="EaseLinear"/>
<!-- ease out quad (default) -->
<Progress name="p2" min="0" max="100" value="70" animation_enabled="true" animation_duration="600" animation_easing="EaseOutQuad"/>
<!-- elastic -->
<Progress name="p3" min="0" max="100" value="70" animation_enabled="true" animation_duration="900" animation_easing="EaseOutElastic"/>
```

Curve groups: `EaseLinear`, `Ease{In|Out|InOut}{Sine|Quad|Cubic|Quart|Quint|Expo|Circ|Back|Elastic|Bounce}`.
It can be changed at any time and takes effect on the next animation (the currently playing one is unaffected).

C++ interfaces: `SetAnimationEasing(EasingFunctionType)`, `SetAnimationEasingName(const DString&)`, `GetAnimationEasing()`, `GetAnimationEasingName()`.

### Reverse fill (reverse)
With `reverse="true"` the fill direction is inverted: `value=min` is full and `value=max` is empty.
Internally the value is **first normalized to a 0~1 ratio and then inverted**, rather than flipping the absolute value as `(max-min)-value` — the latter produces out-of-range or even negative ratios when `min != 0` (e.g. `min="10" max="100"`).

`Progress` and `CircleProgress` share the same reverse conversion semantics.

### Runtime range clamping
`SetMinValue`/`SetMaxValue` pull the current value back into the new range automatically, avoiding divergence between the member value and the rendered result:

```cpp
pProgress->SetValue(80.0);
pProgress->SetMaxValue(50);
// GetValue() == 50 here, not 80
```

`m_fCurrentValue`, the animation intermediate value `m_fDisplayValue`, and `buffer_value` are all clamped (an unset buffer value stays untouched).
When the range is invalid (`min > max`) nothing is changed, and the existing rendering logic handles it.
C++ interface: `ClampCurrentValue()`.

## Slider Attributes
| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| thumb_normal_image |  | string | Normal state image of the drag slider |
| thumb_hovered_image |  | string | State image of the drag slider in hovered state |
| thumb_pressed_image |  | string | State image of the drag slider in pressed state |
| thumb_disabled_image |  | string | Disabled state image of the drag slider |
| thumb_size | 10,10 | size | Size of the drag slider, e.g. "10,10" |
| step | 1 | int | Progress step, e.g. "1" |
| page_step | 0 | int | Big step for PageUp/PageDown (and Shift+arrow); 0 means auto = (max-min)/10 |
| click_to_jump | true | bool | Whether clicking the track jumps **on mouse down**; false keeps the legacy "jump on mouse up" behavior |
| progress_bar_padding | 0,0,0,0 | rect | Padding reduced when drawing the slider bar |
| show_ticks | false | bool | Whether tick marks (a ruler) are drawn |
| tick_count | 5 | int | Number of ticks (at least 2, including both endpoints) |
| tick_length | 4 | int | Tick length in pixels (DPI-scaled internally) |
| tick_color | text_muted | string | Tick color (semantic color name or color value) |
| tick_pos | bottom | string | Tick position: bottom/top (horizontal), right/left (vertical) |
| show_tick_labels | false | bool | Whether tick value labels are drawn (requires show_ticks) |
| tick_label_interval | 1 | int | Draw a value label every N ticks |
| tick_font | system_12 | string | Font ID for tick value labels |
| tick_label_color | text_muted | string | Color for tick value labels |

The Slider control inherits the `Progress` attributes. For more available attributes, please refer to the `Progress` attributes.

### Keyboard operation (accessibility)
Slider joins the Tab focus loop (`GetControlFlags` returns `UIFLAG_TABSTOP`) and handles `kEventKeyDown`:

| Key | Behavior |
| :--- | :--- |
| Left/Right (horizontal), Up/Down (vertical) | Step forward/backward by `step` |
| Shift + arrow | Step by `page_step` (big step) |
| PageUp / PageDown | Step by `page_step` (big step), independent of the slider axis |
| Home / End | Jump directly to `min` / `max` |
| Mouse wheel | Step forward/backward by `step` |

All keyboard operations clamp to `[min, max]` and fire `kEventValueChanged`.
When `page_step` is 0 (the default) it is computed as `(max-min)/10`, and never smaller than `step`.

### Ticks and ruler
Slider can draw tick lines and tick values like Chart axis ticks — typical for volume bars and timeline handles:

```xml
<Slider name="volume" min="0" max="100" value="30" height="42" width="stretch"
        show_ticks="true" tick_count="11" tick_length="5" tick_pos="bottom"
        tick_color="border_control_normal"
        show_tick_labels="true" tick_label_interval="2"
        tick_font="system_12" tick_label_color="text_muted"/>
```

Tick positions are computed over the thumb's **travel range**, so they align exactly with the thumb center; horizontal sliders draw ticks above or below the track, vertical sliders draw them to the left or right.
Integer tick values drop the decimal part by default (showing `20` instead of `20.00`); non-integer ticks keep two decimals.

Visual rules:
- **Major/minor tick layering**: ticks that carry a label (`i % tick_label_interval == 0`) are drawn as longer major ticks; the rest are minor ticks at roughly half the length, producing the visual hierarchy of a ruler
- **Adaptive label width**: the label half-width shrinks to the actual tick spacing (never more than half a spacing), which by design prevents adjacent labels from overlapping
- **Edge alignment for first/last**: the first label is left-aligned and the last is right-aligned (top/bottom for vertical sliders), with the text expanding inward. This keeps end labels from being shifted out of bounds — which would otherwise encroach on the neighbouring label's space — and guarantees the text is never clipped

Customize tick text (e.g. `03:20` for a timeline) with `SetTickLabelCallback`:

```cpp
pSlider->SetTickLabelCallback([](double value, int32_t /*index*/) {
    int nTotal = (int)value;
    return ui::StringUtil::Printf(_T("%02d:%02d"), nTotal / 60, nTotal % 60);
    });
```

### Track click behavior
`click_to_jump=true` (the default) jumps **on mouse down**: pressing the left mouse button anywhere on the track immediately moves the value there and enters the dragging state (you can keep holding to drag). This is the more common desktop interaction.
`click_to_jump=false` keeps the legacy behavior: the value jumps to the release position on mouse up.

## RangeSlider (inherits Progress attributes) Attributes
| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| low_value | 0 | double | Low value (floating point), e.g. "200" |
| high_value | max | double | High value (floating point), equivalent to the base `value` |
| min_range | 0 | double | Minimum distance between low and high (low must not exceed high - min_range) |
| range_color |  | string | Color of the highlighted selected range (semantic color name or color value; falls back to `progress_color` when empty) |
| range_radius | 0 | int | Corner radius of the highlighted range (pixels, auto DPI scaled, 0 = square corners) |
| click_to_jump | true | bool | Whether clicking the track jumps **on mouse down**; false keeps the legacy "jump on mouse up" behavior |
| thumb_size | 10,10 | size | Shared size of both thumbs, e.g. "10,10" |
| thumb_normal_image / thumb_hovered_image / thumb_pressed_image / thumb_disabled_image |  | string | State images of the low thumb |
| high_thumb_normal_image / high_thumb_hovered_image / high_thumb_pressed_image / high_thumb_disabled_image |  | string | State images of the high thumb (fall back to the low thumb images when empty) |

RangeSlider inherits the `Progress` attributes; for more available attributes, please refer to the `Progress` attributes.

A range slider places two thumbs (low / high) on a single track and highlights the selected range; it is commonly used for price / time range filtering.
The high value reuses the base progress value (`value` / `GetValue`), while the low value is maintained separately (`low_value` / `GetLowValue`); the `min` / `max` bounds reuse the base attributes.
Both values are clamped automatically: the low value stays within `[min, high - min_range]` and the high value within `[low + min_range, max]`; dragging, track clicks and `SetXXX` all follow this rule.

XML example:
```xml
<RangeSlider class="range_slider_horizontal" name="price_range" min="0" max="1000"
             low_value="200" value="800" min_range="50" width="stretch"/>
```

Corresponding C++ API:
```cpp
double GetLowValue() const;                          //low value
void SetLowValue(double fValue);                     //set low value (clamped, fires value-change event)
void SetLowValueNoEvent(double fValue);              //set low value (no event)
double GetHighValue() const;                         //high value (same as GetValue())
void SetHighValue(double fValue);                    //set high value (clamped, fires event)
void SetHighValueNoEvent(double fValue);             //set high value (no event)
void SetRange(double fLowValue, double fHighValue);  //set both low and high (fires one event)
void SetRangeNoEvent(double fLowValue, double fHighValue);
double GetMinRange() const;                          //minimum distance
void SetMinRange(double fMinRange);
void SetThumbSize(UiSize szXY, bool bNeedDpiScale);  //thumb size
UiSize GetThumbSize() const;
UiRect GetLowThumbRect() const;                      //low thumb rect
UiRect GetHighThumbRect() const;                     //high thumb rect
DString GetThumbStateImage(ControlStateType stateType) const;       //low thumb image
void SetThumbStateImage(ControlStateType stateType, const DString& strImage);
DString GetHighThumbStateImage(ControlStateType stateType) const;   //high thumb image
void SetHighThumbStateImage(ControlStateType stateType, const DString& strImage);
void SetRangeColor(const DString& strColor);         //highlighted range color
const DString& GetRangeColor() const;
void SetRangeRadius(int32_t nRadius);                //highlighted range corner radius
int32_t GetRangeRadius() const;
void SetClickToJump(bool bEnable);                   //whether clicking the track jumps on mouse down
bool IsClickToJump() const;
void AttachRangeChanged(const EventCallback& callback, EventCallbackID callbackID = 0); //range-change event
```

Value-change event: `AttachRangeChanged` — fired when the range changes; encoded as `kEventValueChanged` (`wParam` is `1`), with the low and high values carried through `EventArgs::pEventData` (pointing to `RangeSlider::RangeValue` with `lowValue` / `highValue`; valid only during the callback dispatch, do not store the pointer):
```cpp
pRange->AttachRangeChanged([](const ui::EventArgs& msg) {
    if (msg.pEventData != nullptr) {
        auto* pRange = static_cast<const ui::RangeSlider::RangeValue*>(msg.pEventData);
        // read pRange->lowValue / pRange->highValue
    }
    return true;
});
```

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

## Stepper (wizard step indicator) Attributes
| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| step_title |  | string | Title of each step, items separated by `|`, e.g. "Choose location\|License\|Installing\|Finish" |
| step_title_id |  | string | Multi-language ID of each step title (`|` separated), refreshed on language change |
| step_desc |  | string | Description of each step, items separated by `|` |
| step_desc_id |  | string | Multi-language ID of each step description (`|` separated), refreshed on language change |
| current / current_index | 0 | int | Current step index (0-based); steps before it are shown as completed |
| direction / orientation | horizontal | string | Layout direction: `horizontal` (default) / `vertical` |
| click_to_jump | true | bool | Whether clicking a step jumps to it |
| node_size | 24 | int | Node diameter (pixels, auto DPI scaled) |
| disabled_steps |  | string | Disabled step index list, separated by `|` or `,`, e.g. "2\|3" (disabled steps cannot be jumped to) |
| node_color_pending / node_color_active / node_color_completed / node_color_disabled | text_muted / color_accent / color_success / border_control_normal | string | Node color of each state (semantic color name or color value) |
| connector_color | border_control_normal | string | Connector color of unfinished segments |
| connector_active_color | color_success | string | Connector color of finished segments |
| title_color_active / title_color_completed / title_color_pending | text_default / color_success / text_muted | string | Title color of each state |
| desc_color | text_muted | string | Description text color |
| title_font / desc_font | system_bold_14 / system_12 | string | Title / description font ID |
| node_hovered_image (alias node_hot_image) |  | string | Hovered-state image of the node, optional; shown when the mouse hovers a clickable step (painted below the node circle) |
| node_pressed_image (alias node_pushed_image) |  | string | Pressed-state image of the node, optional; shown when the mouse presses a clickable step, falls back to the hovered image when unset |

Stepper inherits the `Control` attributes; for more available attributes, please refer to the `Control` attributes.

A stepper is used for install / register / config wizards, approval flows and other "process state machine" scenarios (`Timeline` is time-axis semantics and does not carry process steps).
The step state is derived from the "current index + disabled set": steps before the current index are completed (the node shows a check mark), the current step is active, and the following ones are pending; an explicitly disabled step takes precedence and is shown as disabled. Connectors are colored by finished segments.

The hovered/pressed images are shown only for "interactive steps" (control enabled, `click_to_jump` on, and the step not disabled) when the mouse hovers/presses, providing an accent glow/outline feedback without affecting the node color or the number/check mark. The default skin's `stepper_node` class already ships a set of glow-ring icons (colored via `svg_replace_colors` to follow the theme accent color); just reference it as `class="stepper_node"`.

XML example:
```xml
<Stepper class="stepper_node" name="stepper_demo" width="stretch" height="stretch" node_size="28" current="1"
         step_title_id="STRID_S1_TITLE|STRID_S2_TITLE|STRID_S3_TITLE|STRID_S4_TITLE"
         step_desc_id="STRID_S1_DESC|STRID_S2_DESC|STRID_S3_DESC|STRID_S4_DESC"/>
```

Corresponding C++ API:
```cpp
size_t GetStepCount() const;                          //number of steps
const Step* GetStepAt(size_t index) const;            //step data
void AddStep(const Step& step);                       //append a step
void AddStep(const DString& title, const DString& description = DString());
bool InsertStep(size_t index, const Step& step);
bool RemoveStepAt(size_t index);
void ClearSteps();
void SetSteps(const std::vector<Step>& steps);
size_t GetCurrentIndex() const;                       //current step index (clamped)
void SetCurrentIndex(size_t nIndex);                  //set current step (clamped, fires event when it changes)
void SetCurrentIndexNoEvent(size_t nIndex);           //set current step (no event)
bool Next();  bool NextNoEvent();                     //advance to next step (skips disabled steps, false on failure)
bool Back();  bool BackNoEvent();                     //go back to previous step
bool GoTo(size_t nIndex);  bool GoToNoEvent(size_t nIndex); //jump to a step (false for a disabled step)
void SetStepDisabled(size_t index, bool bDisabled);   //set whether a step is disabled
bool IsStepDisabled(size_t index) const;
void SetDisabledSteps(const std::vector<size_t>& disabledSteps);
StepStatus GetStepStatus(size_t index) const;         //derived state: kPending/kActive/kCompleted/kDisabled
void SetHorizontal(bool bHorizontal);                 //layout direction
bool IsHorizontal() const;
void SetNodeSize(int32_t nSize);                      //node diameter
int32_t GetNodeSize() const;
void SetClickToJump(bool bEnable);                    //whether clicking a step jumps
bool IsClickToJump() const;
void SetStateColor(StepStatus status, const DString& strColor);   //node color of each state
void SetTitleColor(StepStatus status, const DString& strColor);   //title color of each state
void SetConnectorColor(const DString& strColor);                  //connector color of unfinished segments
void SetConnectorActiveColor(const DString& strColor);            //connector color of finished segments
void SetDescColor(const DString& strColor);                       //description text color
void SetNodeStateImage(ControlStateType stateType, const DString& strImage); //node hovered/pressed image
DString GetNodeStateImage(ControlStateType stateType) const;
void AttachStepChanged(const EventCallback& callback, EventCallbackID callbackID = 0); //step-change event
```

Value-change event: `AttachStepChanged` — fired when the current step changes; encoded as `kEventValueChanged` (`wParam` is `2`), with the new / old index carried through `EventArgs::pEventData` (pointing to `Stepper::StepValue` with `currentIndex` / `oldIndex`; valid only during the callback dispatch, do not store the pointer):
```cpp
pStepper->AttachStepChanged([](const ui::EventArgs& msg) {
    if (msg.pEventData != nullptr) {
        auto* pValue = static_cast<const ui::Stepper::StepValue*>(msg.pEventData);
        // read pValue->currentIndex / pValue->oldIndex
    }
    return true;
});
```

## RichEdit Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| vscrollbar | false | bool | Whether to use a vertical scroll bar, e.g. "true" |
| hscrollbar | false | bool | Whether to use a horizontal scroll bar, e.g. "true" |
| auto_vscroll | false | bool | Whether to scroll vertically with input, e.g. "true" (not valid for the native backend implementation) |
| auto_hscroll | false | bool | Whether to scroll horizontally with input, e.g. "true" (not valid for the native backend implementation) |
| want_tab | false | bool | Whether to accept the Tab key message, e.g. "true" |
| want_return | false | bool | Whether to accept the Enter (Return) key message, e.g. "true" |
| want_ctrl_return | false | bool | Whether to accept the Ctrl+Return key message, e.g. "true" |
| rich_text | false | bool | Whether to use rich text format, e.g. "true" (not valid for the native backend implementation) |
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
| auto_detect_url | false | bool | Whether to automatically detect URLs; if it is a URL, it is displayed as a hyperlink (not valid for the native backend implementation) |
| limit_text | | int | Limits the maximum number of characters |
| limit_chars | | string | Limits which characters are allowed; for example, "abc" means only the characters a, b and c can be entered, and other characters are not allowed |
| allow_beep | false | bool | Whether a Beep sound is allowed (not valid for the native backend implementation) |
| word_wrap | false| bool | Whether to wrap lines automatically |
| no_caret_readonly |false| bool | In read-only mode, do not show the caret |
| save_selection |false| bool | If true, the selection boundaries should be saved when the control is in an inactive state (not valid for the native backend implementation) |
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
| row_spacing_mul  | 1.0 | float  | Line spacing multiplier, e.g. 1.5 means 1.5 times the line spacing<br>Windows platform: only valid when the rich_text attribute is "true", because the RichEdit control on Windows only supports setting line spacing in rich text mode;<br>when using native backend, it is always valid; that is, on other platforms, the line spacing attributes are always valid |
| row_spacing_add  |0    | float  | Line spacing additional amount: a fixed additional pixel value (default is usually 0), used to add a fixed offset (in pixels) on top of the proportional adjustment; only valid when using native backend |

The RichEdit control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

## SpinBox Attributes
SpinBox is a numeric input control derived from RichEdit. It enables `number_only` mode by default and provides numeric input plus spin buttons and up/down arrow key adjustment.    
In addition to RichEdit's common attributes (`spin_class`, `min_number`, `max_number`, `limit_text`, etc.), it adds the following attributes:    

| Attribute | Default | Type | Purpose |
| :--- | :--- | :--- | :--- |
| step | 1 | int | Step value (positive integer); the amount added/subtracted by each spin button click or up/down arrow key press |
| value | 0 | int64 | Initial numeric value (clamped to the min/max range if out of bounds) |

Corresponding C++ interfaces: `SetStep/GetStep`, `SetValue/GetValue`, `SetRange(min, max)`.    
The `kEventTextChanged` event is fired when the value changes (same as RichEdit).    
Example (see the rich_edit example program):    

```xml
<SpinBox class="simple simple_border rich_edit_spin" min_number="0" max_number="100" step="5" value="50"/>
```

The `spin_class` styles (rich_edit_spin_box / rich_edit_spin_btn_up / rich_edit_spin_btn_down) are defined in the `rich_edit_spin` class of global.xml.    
Note: the spin buttons are only shown when the `spin_class` attribute is set; otherwise only the up/down arrow keys can adjust the value.

## SearchBox Attributes
SearchBox is a composite search input control derived from HBox. It combines a search icon on the left, a text edit box (RichEdit), and a clear button on the right, intended for keyword input and search scenarios. Header file: [SearchBox.h](../include/dui/Control/SearchBox.h).

| Attribute | Default | Type | Purpose |
| :--- | :--- | :--- | :--- |
| text | "" | string | Initial text of the search box |
| text_id | "" | string | Multi-language string ID of the initial search box text (when set together with `text`, the last-set attribute takes effect) |
| prompt_text | "" | string | Placeholder text shown when the edit box is empty |
| prompt_text_id | "" | string | Multi-language string ID of the placeholder text |
| prompt_color | "" | string | Color of the placeholder text (semantic color name) |
| debounce_interval | 0 | int(ms) | Debounce interval (milliseconds): when search text changes continuously, `kEventTextChanged` is fired only after typing pauses for this interval; 0 = fire immediately on every change (default, backward compatible), used to avoid triggering a search request on every keystroke |
| suggestions | "" | string | Suggestion data source, multiple items separated by ";". (Search history is recorded automatically by the control when a search is committed, and need not be configured here) |
| max_visible | 8 | int | Maximum number of visible candidate rows in the dropdown (a scrollbar appears beyond this) |
| history_enabled | true | bool | Whether to record and display search history (history is also shown as candidates) |
| max_history | 10 | int | Maximum number of retained search history entries (oldest dropped beyond this, most-recently-used promoted to top) |
| filter_enabled | true | bool | Whether to filter candidates by the typed text: when true (default), candidates are filtered by `match_mode`/`ignore_case` only after the input reaches `min_chars`; when false the full list is always shown (no filtering), suitable when there are few candidates or a full overview is wanted |
| min_chars | 1 | int | Minimum input length to trigger filtering in filter mode; when empty or shorter, the full list is shown (focus state) |
| match_mode | prefix | string | Filter matching mode: `prefix` matches candidates starting with the input; `contains` matches candidates containing the input anywhere |
| ignore_case | true | bool | Whether to ignore case while filtering |

Corresponding C++ interfaces: `SetSearchText/GetSearchText`, `GetEditControl()` (returns the inner RichEdit control), `SetDebounceInterval/GetDebounceInterval` (debounce interval in milliseconds).
Suggestion (suggestion + history) related interfaces: `SetCandidates/GetCandidates/AddCandidate/ClearCandidates` (suggestion data source), `SetMaxVisible/GetMaxVisible`, `SetHistoryEnabled/IsHistoryEnabled`, `SetMaxHistory/GetMaxHistory`, `GetHistory/ClearHistory/AddSearchHistory` (search history), `SetFilterEnabled/IsFilterEnabled`, `SetMinChars/GetMinChars`, `SetMatchMode/GetMatchMode`, `SetIgnoreCase/IsIgnoreCase` (filter parameters).
The `kEventTextChanged` event is fired when the text changes (use `debounce_interval` to set a debounce interval so it fires only after typing pauses, avoiding a search on every keystroke); **Enter semantics**: if the user has not navigated candidates with the Up/Down arrow keys (i.e. no candidate is highlighted), Enter commits the "current text in the edit box" and fires `kEventReturn` (can be used to start a search); if the user has navigated and highlighted a candidate with the arrow keys, Enter chooses that candidate (writing it back to the edit box) and fires `kEventAutoCompleteSelect`, with its WPARAM being the candidate's index in the current candidate list and `GetSearchText()` already filled with the chosen candidate text.
The dropdown suggestion flyout reuses `AutoComplete`'s skin classes `auto_complete_flyout`/`auto_complete_list`/`auto_complete_item` (defined in global.xml), popping up below the search box following its width, without stealing focus.

Dropdown flyout display rules (clear and consistent):
- **Evaluate on focus**: when the edit box gains focus it is evaluated immediately; the flyout opens if there is content to show, otherwise it does not pop up.
- **Refresh on typing**: the flyout refreshes on every text change; it closes when there is no content to show.
- **Filter switch `filter_enabled` (default true)**: when enabled, candidates are filtered by `match_mode`/`ignore_case` only after the input reaches `min_chars`; when the input is empty or shorter than `min_chars`, the full list is shown (history first, most-recently-used on top). When disabled, the full merged/deduplicated list (history + suggestions) is always shown without filtering, suitable when there are few candidates.
- **No focus stealing**: the flyout does not take focus, and edit-box blur is not a close condition; the flyout closes automatically when a mouse button is pressed outside the flyout and the search box.
- **Focus returned after select**: when a candidate is clicked with the mouse, the framework first moves the native keyboard focus to the flyout window; before the flyout closes, the focus is returned to the search box's window, so keyboard input continues to work right after selection (the same applies when dismissing the flyout with Esc, which also does not re-pop it).
- **No reopen after select**: choosing a candidate writes the text back and will not re-pop the flyout due to the regained focus (avoids flicker right after selection).
- **No mis-selection on Enter**: pressing Enter without navigating candidates commits the typed text, so the list's default-highlighted first item is never mistaken for the search term.
- Re-submitting the same search term does not create duplicate entries in the list (deduplication).
The clear button reuses RichEdit's built-in `clear_btn_class` mechanism: it is shown when the edit box is focused and has text, clears the text when clicked, and is automatically hidden after losing focus.
Example (see the rich_edit example program):

```xml
<SearchBox class="search_box" prompt_text="Type to search"/>
```

The default skin defines four classes in global.xml: `search_box` (outer frame), `search_box_icon` (magnifier icon), `search_box_edit` (inner edit box), and `search_box_clear_btn` (clear button).

## TagInput Attributes
TagInput is a composite tag input control derived from Box. Using a horizontal flow layout, it combines a set of tags (HBox + text Label + close button) and a text edit box (RichEdit). Type text and press Enter to create a tag; click the X button on a tag or press Backspace in the empty edit box to remove a tag. When tags fill a row the control wraps automatically and grows in height. It is commonly used for recipients, tag picking, and keyword entry. Header file: [TagInput.h](../include/dui/Control/TagInput.h).

| Attribute | Default | Type | Purpose |
| :--- | :--- | :--- | :--- |
| prompt_text | "" | string | Placeholder text shown when the edit box is empty |
| prompt_text_id | "" | string | Multi-language string ID of the placeholder text |
| prompt_color | "" | string | Color of the placeholder text (semantic color name) |
| allow_duplicate | false | bool | Whether duplicate tags are allowed (when false, tags with duplicated text are not added) |
| max_count | 0 | int | Maximum number of tags; 0 means unlimited |
| delimiter | "" | string | Delimiter that commits a tag while typing (e.g. `,`); empty means tags are only created with Enter |
| tags | "" | string | Initial tag texts, multiple tags separated by semicolons `;` |

Corresponding C++ interfaces: `AddTag` (adds a tag; returns false when text is empty, duplicated, or the limit is reached), `RemoveTagAt`, `RemoveAllTags`, `GetTagCount`, `GetTagText`, `SetAllowDuplicate/IsAllowDuplicate`, `SetMaxCount/GetMaxCount`, `GetEditControl` (returns the inner RichEdit), `SetFocus` (sets focus to the inner edit box).

Events:
- `kEventTagAdded`: fired after a tag is added, WPARAM is the new tag index (starting from 0);
- `kEventTagRemoved`: fired before a tag is removed, WPARAM is the removed tag index; its text is still readable via `GetTagText` inside the callback;
- `kEventTagClick`: fired when the tag body (not the close button) is clicked, WPARAM is the tag index;
- `kEventTextChanged`: forwarded when the inner edit box text changes.

Example (see the controls example program):

```xml
<!-- Commit tags with Enter, with 3 initial tags -->
<TagInput class="tag_input" tags="nim_duilib;C++;desktop-ui" prompt_text="Type and press Enter to add a tag"/>

<!-- Comma or Enter commits tags; up to 5 tags, duplicates allowed -->
<TagInput class="tag_input" delimiter="," max_count="5" allow_duplicate="true" prompt_text="Press Enter or comma to add a tag"/>
```

The default skin defines five classes in global.xml: `tag_input` (outer container), `tag_input_tag` (tag background), `tag_input_tag_text` (tag text), `tag_input_tag_close` (tag close button), and `tag_input_edit` (inner edit box). Tag backgrounds use the semantic colors `bg_tag`/`bg_tag_hover`; on hover/press the close button shows the `bg_tag_close_hover`/`bg_tag_close_pressed` circular background with a white X icon. All adapt to light/dark themes automatically.

## AutoComplete Attributes
AutoComplete is an auto-complete composite control derived from Box, containing a single-line edit box (RichEdit) that fills the whole control. Once the input reaches `min_chars` characters, a non-focus-stealing suggestion popup (`AutoCompleteFlyout`, built on Flyout) opens below the edit box. The popup is as wide as the edit box, takes no layout space, and resizes its height as the number of candidates changes. The Up/Down keys move the selection, Enter confirms, Esc closes the popup; mouse hover highlights and a click confirms directly. On confirmation the chosen candidate text is filled back into the edit box (this programmatic fill does not re-trigger suggestions). Difference from FilterCombo: AutoComplete is edit-box-centric, filters in real time while typing and the popup follows the edit box, whereas FilterCombo is combo-box-centric with a fixed option list opened by a button. Typical uses include search history and city/address completion. Header file: [AutoComplete.h](../include/dui/Control/AutoComplete.h).

| Attribute | Default | Type | Purpose |
| :--- | :--- | :--- | :--- |
| items | "" | string | Initial candidate data source, multiple items separated by semicolons `;` (can also be set via `SetCandidates`) |
| prompt_text | "" | string | Placeholder text shown when the edit box is empty |
| prompt_text_id | "" | string | Multi-language string ID of the placeholder text |
| prompt_color | "" | string | Color of the placeholder text (semantic color name) |
| min_chars | 1 | int | Number of characters required before suggestions appear |
| max_visible | 8 | int | Maximum number of visible candidate rows in the popup; a scrollbar appears when exceeded |
| match_mode | prefix | string | Matching mode: `prefix` (candidate starts with the input) or `contains` (candidate contains the input anywhere) |
| ignore_case | true | bool | Whether matching is case-insensitive |

Corresponding C++ interfaces: `SetCandidates`/`GetCandidates` (set/get the full candidate data source), `AddCandidate` (appends a candidate; duplicates are ignored), `ClearCandidates`, `SetText`/`GetText` (setting text programmatically does not open the popup), `SetMinChars`/`GetMinChars`, `SetMaxVisible`/`GetMaxVisible`, `SetMatchMode`/`GetMatchMode` (parameter is `AutoCompleteMatchMode::kPrefix/kContains`), `SetIgnoreCase`/`IsIgnoreCase`, `GetEditControl` (returns the inner RichEdit), `SetFocus` (sets focus to the inner edit box).

Events:
- `kEventAutoCompleteSelect`: fired after a candidate is chosen (mouse click or Enter), WPARAM is the selected item index within the current candidate list (starting from 0); the inline XML event name is `select`;
- `kEventTextChanged`: forwarded when the inner edit box text changes (not fired for programmatic fill-back).

Example (see the controls example program):

```xml
<!-- Prefix matching over 18 cities; type a prefix such as b / sh -->
<AutoComplete class="auto_complete" name="auto_complete"
              items="Beijing;Berlin;Budapest;Bangkok;Boston;Brisbane;Chicago;London"
              prompt_text="Type a city prefix, e.g. b / sh"/>

<!-- Contains matching, at most 6 rows; the data source can also be provided entirely via SetCandidates -->
<AutoComplete class="auto_complete" name="auto_complete2" width="260" margin="8,2,0,2"
              match_mode="contains" max_visible="6" prompt_text="Type a keyword (contains match)"/>
```

The default skin defines two control classes in global.xml, `auto_complete` (outer container) and `auto_complete_edit` (inner edit box), plus three popup classes: `auto_complete_flyout` (popup card), `auto_complete_list` (candidate list), and `auto_complete_item` (candidate row). Candidate hover/selection backgrounds reuse the list semantic colors `bg_list_item_hovered`/`bg_list_item_selected`, and the popup card uses `bg_window_card`/`border_window`, adapting to light/dark themes automatically.

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

## Chart Attributes
Chart is a lightweight chart control derived from Control, self-drawn (no third-party chart library, no CEF embedding). Header file: [Chart.h](../include/dui/Control/Chart.h). It supports three forms — line, bar and pie, with data bound via XML attributes or C++ API.

In addition to the common Control attributes, the following attributes are added:

| Attribute | Default | Type | Description |
| :--- | :--- | :--- | :--- |
| chart_type | line | string | Chart type: "line" / "bar" / "pie" |
| data | empty | string | Comma-separated numeric series, e.g. data="12,35,28,40" (single series) |
| data_labels | empty | string | Comma-separated category labels, used for the category axis (X axis) of line/bar charts and the legend of pie charts |
| title | empty | string | Chart title (centered at the top) |
| x_axis_title | empty | string | X-axis title (below the category axis) |
| y_axis_title | empty | string | Y-axis title (vertical, on the left) |
| title_id | empty | string | Language ID (text_id) for the chart title (multi-language, takes precedence over title) |
| x_axis_title_id | empty | string | Language ID for the X-axis title (multi-language) |
| y_axis_title_id | empty | string | Language ID for the Y-axis title (multi-language) |
| data_labels_id | empty | string | Language ID for category labels (comma-separated STRIDs, multi-language, takes precedence over data_labels) |
| series_color | color_accent | string | Series color (semantic color name or color value); pie slices derive same-hue brightness gradients from it by index |
| axis_color | border_control_normal | string | Axis/grid line color (line and bar charts) |
| label_color | text_default | string | Text label color |
| show_value | true | bool | Whether to show the value next to data points / bar tops / slices |
| show_grid | true | bool | Whether to show grid lines (line and bar charts) |
| show_axis_values | true | bool | Whether to show Y-axis tick values |
| axis_divisions | 4 | int | Number of Y-axis divisions (1~10) |
| legend_visible | pie:true, others:false | bool | Whether to show the legend (pie shows by default; line/bar can enable it for multi-series) |
| line_width | 2 | int | Line thickness (line chart, auto DPI-scaled) |
| line_mode | straight | string | Line drawing mode: "straight" / "curve" (Bézier smooth) |
| area_fill | false | bool | Whether to fill a semi-transparent area under the line (line chart) |
| show_data_points | true | bool | Whether to draw data-point dots (line chart) |
| bar_mode | grouped | string | Bar layout: "grouped" side-by-side / "stacked" cumulative |
| donut | false | bool | Whether the pie chart is a donut ring |
| show_percent | false | bool | Whether the pie chart shows percentages (instead of raw values) |
| bar_radius | 3 | int | Bar corner radius (px, 0 for square corners, auto DPI-scaled) |
| bar_gradient | true | bool | Whether bars use gradient fill (bright top, dark bottom) |
| line_glow | false | bool | Whether the line chart draws a glow/shadow effect |
| animation_enabled | true | bool | Whether data-update animation is enabled (bar grow / line fade-in) |
| animation_duration | 300 | int | Data-update animation duration (ms) |

Notes:
- The three forms share the same data (`data`); switching `chart_type` changes the presentation without rebuilding the data.
- **Multi-series**: line/bar charts support multiple overlaid series (C++ `SetSeriesData`/`AddSeries`); when a series has no explicit color it is auto-assigned from a built-in 8-color palette; the legend can be enabled to show series names.
- Colors use semantic color names (ThemeColor in the color_light/color_dark global.xml files), automatically adapting to light/dark themes; a direct color value (e.g. `#FF0078D4`) is also accepted.
- Bar charts support negative values: a zero baseline is drawn automatically, negative bars extend downward; grouped mode lays series side-by-side, stacked mode accumulates positive values and subtracts negative ones.
- Negative values in a pie chart are ignored (not summed, not drawn); an empty or all-zero dataset renders an empty chart.
- The data range of line/bar charts is computed automatically (with headroom), no manual axis range needed.
- Value labels are auto-formatted: integers drop decimals, non-integers keep two decimal places.
- **Interactivity**: ① data-point hover tooltip (shows "series / category: value"); ② click-to-select (pie slice separates with highlight + leader-line external label, bar gets an outline highlight; click again to deselect); ③ line chart wheel zoom + drag pan (for real-time curves). Tooltip/select can be toggled via `SetEnableTooltip`/`SetEnableSelect`.
- **Visual polish**: bars support rounded corners (`bar_radius`) and gradient fill (`bar_gradient`); line charts support glow/shadow (`line_glow`); data updates animate bar growth / line fade-in (`animation_enabled`/`animation_duration`).
- **Incremental update (real-time monitoring)**: in addition to the full `SetSeriesData`, per-series point-level incremental APIs are provided — `AddDataPoint`/`InsertDataPoint`/`SetDataPoint`/`RemoveDataPoint` (return `false` safely on out-of-range), plus `ClearSeries`/`RemoveSeries`, `GetDataPoint`/`GetSeriesData` (read-only; return empty / 0.0 on out-of-range). Combined with `SetMaxDataPoints` you can set a rolling window (oldest points are dropped automatically beyond the cap) to avoid unbounded growth, ideal for real-time curves / monitoring.
- **Axis label formatter**: `SetYAxisLabelFormatter`/`SetXAxisLabelFormatter` (signature `std::function<DString(double value, int32_t index)>`, analogous to Slider's TickLabelCallback) let you custom-format Y-axis tick values (e.g. `"50%"`, `"1.2k"`) and X-axis category labels (e.g. relative time). When the callback is cleared, the default `FormatValue` numeric format is restored (X axis falls back to the `data_labels` string).

C++ API: `SetChartType/GetChartType`, `SetTitle`, `SetXAxisTitle`, `SetYAxisTitle`, `SetTitleId`, `SetXAxisTitleId`, `SetYAxisTitleId`, `SetData/AddData/ClearData/GetData`, `SetSeriesData/AddSeries/GetSeries/GetSeriesCount`, `SetDataLabels/GetDataLabels`, `SetDataLabelsId`, `SetSeriesColor`, `SetAxisColor`, `SetLabelColor`, `SetShowValue`, `SetShowGrid`, `SetShowAxisValues`, `SetAxisDivisions`, `SetLegendVisible`, `SetLineWidth`, `SetLineMode`, `SetAreaFill`, `SetShowDataPoints`, `SetBarMode`, `SetDonut`, `SetShowPercent`, `SetBarRadius`, `SetBarGradient`, `SetLineGlow`, `SetAnimationEnabled`, `SetAnimationDuration`; incremental update: `AddDataPoint/InsertDataPoint/SetDataPoint/RemoveDataPoint/GetDataPoint/GetSeriesData/ClearSeries/RemoveSeries/SetMaxDataPoints/GetMaxDataPoints`; axis formatter: `SetYAxisLabelFormatter/GetYAxisLabelFormatter/SetXAxisLabelFormatter/GetXAxisLabelFormatter`; interactivity: `SetEnableTooltip/IsEnableTooltip`, `SetEnableSelect/IsEnableSelect`, `GetSelected/ClearSelection`, `AttachPointClick/AttachPointHover`, `GetHitFromEvent`.

### XML Usage Example

```xml
<!-- Line chart: weekly visits -->
<Chart width="stretch" height="200" chart_type="line"
       data="120,356,280,410,398,452,380"
       data_labels="Mon,Tue,Wed,Thu,Fri,Sat,Sun"
       series_color="color_accent" show_value="true"/>

<!-- Bar chart: quarterly sales (with title and axis titles) -->
<Chart width="stretch" height="220" chart_type="bar"
       title="Quarterly Sales" x_axis_title="Quarter" y_axis_title="Sales (10k)"
       data="82,116,98,135"
       data_labels="Q1,Q2,Q3,Q4"
       series_color="color_accent"/>

<!-- Bar chart: with negative values (auto zero baseline) -->
<Chart width="stretch" height="220" chart_type="bar"
       data="45,-18,62,-25,38,80" data_labels="Jan,Feb,Mar,Apr,May,Jun"/>

<!-- Pie chart: donut + percentage (legend auto-drawn on the right) -->
<Chart width="300" height="300" chart_type="pie"
       data="45,30,15,10"
       data_labels="Product A,Product B,Product C,Others"
       series_color="color_accent" legend_visible="true"
       donut="true" show_percent="true"/>
```

### C++ Usage Example

```cpp
// Get the control pointer (name="chart_sales" in XML)
ui::Chart* pChart = dynamic_cast<ui::Chart*>(pWindow->FindControl(_T("chart_sales")));
if (pChart != nullptr) {
    // Single series: dynamically bind data (replaces existing data)
    pChart->SetData({ 82.0, 116.0, 98.0, 135.0 });
    pChart->SetDataLabels({ _T("Q1"), _T("Q2"), _T("Q3"), _T("Q4") });

    // Multi-series: overlaid comparison (line/bar)
    std::vector<ui::Chart::Series> series;
    ui::Chart::Series s1; s1.name = _T("This year"); s1.data = { 82.0, 116.0, 98.0, 135.0 }; s1.color = _T("color_accent");
    ui::Chart::Series s2; s2.name = _T("Last year"); s2.data = { 70.0, 90.0, 110.0, 120.0 }; //color auto-assigned
    series.push_back(s1); series.push_back(s2);
    pChart->SetSeriesData(series);
    pChart->SetLegendVisible(true);

    // Switch chart type / curve + area fill
    pChart->SetChartType(ui::ChartType::kLine);
    pChart->SetLineMode(ui::ChartLineMode::kCurve);
    pChart->SetAreaFill(true);

    // Append a data point (real-time curve scenario)
    pChart->AddData(152.0);
}
```

### Incremental Update & Axis Formatter Example

```cpp
ui::Chart* pChart = dynamic_cast<ui::Chart*>(pWindow->FindControl(_T("chart_rt")));
if (pChart != nullptr) {
    // ① Incremental update: build two series, then append points one by one
    //    (no need to replace the full dataset each time)
    pChart->SetSeriesData({
        { _T("CPU"),   { 30.0, 32.0, 28.0 }, _T("color_accent") },
        { _T("Memory"), { 55.0, 57.0, 53.0 }, _T("color_warning") }
    });
    pChart->AddDataPoint(0, 36.0);    // append to series 0 (returns true)
    pChart->AddDataPoint(1, 60.0);    // append to series 1
    pChart->SetDataPoint(0, 1, 40.0); // in-place update of point 1 in series 0
    double v = pChart->GetDataPoint(0, 2); // read point 2 in series 0

    // Rolling window: keep at most the latest 60 points, dropping the oldest
    // automatically beyond the cap (bounded memory for real-time monitoring)
    pChart->SetMaxDataPoints(60);

    // ③ Axis formatter callbacks: Y ticks as percent, X labels as relative time
    pChart->SetYAxisLabelFormatter([](double value, int32_t /*index*/) {
        return ui::StringUtil::Printf(_T("%.0f%%"), value);
    });
    pChart->SetXAxisLabelFormatter([](double /*value*/, int32_t index) {
        return ui::StringUtil::Printf(_T("%.1fs"), static_cast<double>(index) * 0.25);
    });
}
```

> Note: incremental APIs (e.g. `AddDataPoint`) safely return `false` on out-of-range; `GetDataPoint`/`GetSeriesData` return `0.0`/empty on out-of-range. Clearing the axis formatter restores the default numeric format.

### Interaction Usage Example

```cpp
// Hover tooltip and click-select are enabled by default; toggle via these setters
pChart->SetEnableTooltip(true);
pChart->SetEnableSelect(true);

// Listen for data-point clicks (line point / bar / pie slice / legend item)
pChart->AttachPointClick([](const ui::EventArgs& args) {
    ui::Chart::HitResult hit = ui::Chart::GetHitFromEvent(args);
    if (hit.type == ui::Chart::HitType::kSlice) {
        // The hit.dataIndex-th slice of the pie chart was clicked
        // ... business logic
    }
    else if (hit.type == ui::Chart::HitType::kBar) {
        // The hit.dataIndex-th bar of the hit.seriesIndex-th series was clicked
    }
    return true;
});

// Query the current selection
ui::Chart::HitResult sel = pChart->GetSelected();
if (sel.type != ui::Chart::HitType::kNone) {
    pChart->ClearSelection(); // clear selection
}
```

> Note: `HitResult` contains `type` (kPoint/kBar/kSlice/kLegend), `seriesIndex` (series index) and `dataIndex` (data index). Line charts support wheel zoom (around the mouse position) and left-button drag pan.

## Timeline Attributes
Timeline is a vertical timeline control derived from Control and fully self-drawn. Header file: [Timeline.h](../include/dui/Control/Timeline.h). It is used for logs, task flows and approval flows: nodes and status connector lines on the left, content (time/title/body) on the right. Data-driven rendering, supports forward/reverse ordering, node status colors, line styles, node click events, empty/loading states, and scrolling.

In addition to the common Control attributes, the following attributes are added:

| Attribute | Default | Type | Description |
| :--- | :--- | :--- | :--- |
| item_time | empty | string | Time/date text, `\|`-separated for multiple items (data binding) |
| item_title | empty | string | Title, `\|`-separated (data binding) |
| item_content | empty | string | Body content (supports line breaks), `\|`-separated (data binding) |
| item_status | empty | string | Node status, `\|`-separated: pending / inprogress / completed / failed (data binding) |
| item_color | empty | string | Custom node color, `\|`-separated (used when status is custom) |
| item_time_id | empty | string | Multi-language data binding: time field, `\|`-separated STRID list, auto-refreshed on language switch |
| item_title_id | empty | string | Multi-language data binding: title field, `\|`-separated STRID list |
| item_content_id | empty | string | Multi-language data binding: body field, `\|`-separated STRID list |
| node_color_pending | text_muted | string | Pending node color (semantic color name or value) |
| node_color_inprogress | color_accent | string | In-progress node color |
| node_color_completed | color_success | string | Completed node color |
| node_color_failed | color_error | string | Failed node color |
| line_color | border_control_normal | string | Connector line color |
| line_style | solid | string | Line style: "solid" / "dashed" |
| scrollbar_track_color | bg_scrollbar_normal | string | Scrollbar track color (semantic color name or value) |
| scrollbar_thumb_color | bg_scrollbar_thumb_normal | string | Scrollbar thumb color (semantic color name or value) |
| auto_hide_scrollbar | true | bool | Whether to show the scrollbar only on mouse hover (default on, cleaner look) |
| reverse | false | bool | Whether to display in reverse order (default false = forward) |
| node_size | 12 | int | Node diameter (px, auto DPI-scaled, 4~64) |
| item_spacing | 24 | int | Spacing between adjacent items (px, auto DPI-scaled) |
| empty_text | empty | string | Empty-state hint text (centered when there are no items) |
| empty_text_id | empty | string | Multi-language ID of the empty-state hint text (auto-refreshed on language switch) |
| loading | false | bool | Whether to show a loading state |

Notes:
- **Data binding**: `item_time`/`item_title`/`item_content`/`item_status` are `\|`-separated item lists, aligned by the longest one (missing fields become empty); alternatively drive them dynamically via the C++ `AddItem`/`SetItems` APIs.
- **Multi-language data binding**: `item_time_id`/`item_title_id`/`item_content_id` (and `empty_text_id`) take `\|`-separated STRID lists, resolved to actual texts from the language file; they are re-parsed and refreshed automatically on language switch (via the `OnLanguageChanged` override). When mixed with the direct-text attributes, whichever is set later takes effect.
- **Node status**: four states — pending (gray), in-progress (accent), completed (green), failed (red), all customizable.
- **Scrolling**: mouse-wheel scrolling when the content exceeds the control height, with a right-side scrollbar (with a draggable thumb) shown automatically; the track/thumb colors are customizable via `scrollbar_track_color`/`scrollbar_thumb_color`, defaulting to the theme scrollbar semantic colors (`bg_scrollbar_normal`/`bg_scrollbar_thumb_normal`); by default the scrollbar is only shown on mouse hover (`auto_hide_scrollbar`); body width wraps responsively with the control width.
- **Append visibility**: after `AddItem`, the view scrolls automatically to make the new item visible (scrolls to the bottom in forward order, to the top in reverse order).
- **Events**: clicking a node fires `kEventTimelineItemClick` (`EventArgs::wParam` is the item index).
- **Custom rendering**: `SetItemDrawCallback` can override the default rendering of a single item.

C++ API: `AddItem`, `InsertItem`, `RemoveItemAt`, `ClearItems`, `SetItems`, `GetItemCount`, `GetItemAt`, `GetItems`, `SetNodeColor/GetNodeColor`, `SetLineColor/GetLineColor`, `SetLineStyle`, `SetScrollbarTrackColor/GetScrollbarTrackColor`, `SetScrollbarThumbColor/GetScrollbarThumbColor`, `SetAutoHideScrollbar/IsAutoHideScrollbar`, `SetReverse/IsReverse`, `SetNodeSize`, `SetItemSpacing`, `SetEmptyText`, `SetEmptyTextId`, `SetLoading/IsLoading`, `SetItemDrawCallback`, `AttachItemClick`, `GetScrollPos/SetScrollPos`, `ScrollToItem`.

### XML Usage Example

```xml
<!-- Approval-flow timeline (data binding with status colors) -->
<Timeline width="360" height="320"
          item_time="2026-10-01|2026-10-02|2026-10-03"
          item_title="Submitted|Approved|Rejected"
          item_content="Application submitted, awaiting review.|Materials complete, passed.|Missing supplementary materials, rejected."
          item_status="completed|completed|failed"
          line_style="solid" reverse="false"/>

<!-- Task-flow timeline (reverse order + dashed line) -->
<Timeline width="stretch" height="260"
          item_time="09:00|10:30|11:00"
          item_title="Started|Running|Pending"
          item_content="Task started.|Executing core logic.|Waiting for the next stage."
          item_status="completed|inprogress|pending"
          line_style="dashed" reverse="true"/>
```

### C++ Usage Example

```cpp
ui::Timeline* pTimeline = dynamic_cast<ui::Timeline*>(pWindow->FindControl(_T("timeline")));
if (pTimeline != nullptr) {
    // Data-driven rendering
    pTimeline->AddItem(_T("2026-10-01"), _T("Submitted"), _T("Application submitted, awaiting review."), ui::TimelineNodeStatus::kCompleted);
    pTimeline->AddItem(_T("2026-10-02"), _T("Approved"), _T("Materials complete, passed."), ui::TimelineNodeStatus::kCompleted);
    pTimeline->AddItem(_T("2026-10-03"), _T("Reviewing"), _T("Under review."), ui::TimelineNodeStatus::kInProgress);

    // Reverse order + dashed line
    pTimeline->SetReverse(true);
    pTimeline->SetLineStyle(ui::TimelineLineStyle::kDashed);

    // Node click event (args.wParam is the item index)
    pTimeline->AttachItemClick([](const ui::EventArgs& args) {
        size_t index = static_cast<size_t>(args.wParam);
        // ... handle the click
        return true;
    });

    // Empty hint and loading state
    pTimeline->SetEmptyText(_T("No data"));
    pTimeline->SetLoading(true);
}
```

## Mask Attributes
Mask (the mask/overlay layer) is a container control derived from `Box` (XML node `Mask`) used to overlay a semi-transparent mask on top of other controls. It covers three typical popup-system scenarios:

- **Modal-dialog background dimming**: the mask fills the whole area and blocks mouse input (default `mouse_enabled=true`), making the controls underneath non-interactive;
- **Onboarding mask**: use `highlight_control` to "punch out" a highlight over a target control while the rest stays semi-transparent; with `click_through="true"`, clicks inside the highlight pass through to the target control to guide the user;
- **Loading overlay**: the mask covers the content area and blocks interaction; you can place any child controls on it (loading animation, hint text, buttons, etc.).

Mask is a container, so any child controls can be placed on top of it. When used as a floating overlay, configure `float="true" width="stretch" height="stretch"` (the built-in `mask` Class already does this) and place it as the last child of its parent (or use `paint_order`) so it is drawn on top and hit-tested first.

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| mask_color | String | semantic color `bg_overlay` | Mask color (semantic color name or color value); empty means no mask fill is drawn |
| highlight_control | String | empty | Name of the control to punch out (highlight), looked up by name across the whole window (the target may be any control outside the Mask); empty means no hole — the whole area is masked |
| highlight_padding | int | 4 | Inset of the highlight rect relative to the highlighted control (DPI-aware) |
| highlight_corner | int | 0 | Corner radius of the highlight rect (DPI-aware) |
| click_through | bool | false | Whether clicks inside the highlight pass through to the control underneath; typically used with `highlight_control` for onboarding |

> Note: `mouse_enabled` (inherited from the container base) controls whether the mask blocks the mouse; the default `true` makes it a modal mask, while `false` means "visual dimming only, mouse not blocked".

### C++ API

| Method | Description |
|--------|-------------|
| `SetMaskColor(strColor)` / `GetMaskColor()` | Set/get the mask color (semantic color name) |
| `SetHighlightControl(strName)` / `GetHighlightControl()` | Set/get the name of the control to punch out |
| `SetHighlightPadding(nPadding)` / `GetHighlightPadding()` | Set/get the highlight inset (DPI-aware) |
| `SetHighlightCorner(nCorner)` / `GetHighlightCorner()` | Set/get the highlight corner radius (DPI-aware) |
| `SetClickThrough(bool)` / `IsClickThrough()` | Set/get whether clicks inside the highlight pass through |
| `Mask::ShowOverWindow(pWindow)` (static) | Convenience API: overlay a full-window, mouse-blocking mask on the given window; returns the mask pointer (or `nullptr` on failure) |
| `Remove()` | Detach this mask from its parent (if the parent auto-destroys children, the object is destroyed right after the call and must not be accessed again) |

### XML example

```xml
<!-- (1) Modal mask: fills the parent and blocks the mouse; a centered card is placed on the mask -->
<Mask class="mask">
    <VBox bkcolor="bg_window_card" border_round="6,6" float="true" halign="center" valign="center"
          width="300" height="170" padding="20,20,20,20">
        <Label text_id="STRID_MASK_MODAL_TITLE" width="stretch" height="auto" font="system_bold_14"/>
        <Button class="btn_global_blue_80x30" name="mask_modal_close" text_id="STRID_MASK_CLOSE"
                float="true" halign="center" valign="bottom" width="90" height="30" border_round="3,3"/>
    </VBox>
</Mask>

<!-- (2) Onboarding mask: punch out a highlight over btn_guide_target; clicks inside it pass through -->
<Mask class="mask" highlight_control="btn_guide_target" highlight_padding="6" highlight_corner="6"
      click_through="true">
    <Label text="Click the highlighted button to continue" float="true" halign="center" valign="bottom"
           width="auto" height="auto" margin="0,0,0,60"/>
</Mask>

<!-- (3) Loading overlay: covers the content area and blocks interaction; a loading animation is placed on it -->
<Mask class="mask" visible="false">
    <VBox float="true" halign="center" valign="center" width="220" height="70">
        <Progress class="progress_horizontal" marquee="true" marquee_width="60" height="6"/>
        <Label text="Loading..." width="stretch" height="auto" margin="0,16,0,0" text_align="center"/>
    </VBox>
</Mask>
```

### C++ example

```cpp
// Programmatically overlay a mask on the whole window (default color bg_overlay, blocks the mouse)
ui::Mask* pMask = ui::Mask::ShowOverWindow(this);
if (pMask != nullptr) {
    // Append content: a hint label at the center of the mask
    ui::Label* pLabel = new ui::Label(this);
    pLabel->SetText(_T("Loading..."));
    pLabel->SetFloat(true);
    pLabel->SetHorAlignType(ui::HorAlignType::kAlignCenter);
    pLabel->SetVerAlignType(ui::VerAlignType::kAlignCenter);
    pMask->AddItem(pLabel);

    // Remove when no longer needed (if the parent auto-destroys children, the object is destroyed)
    // pMask->Remove();
}
```

See the controls demo (the "Mask / Overlay" group with three buttons: modal mask / onboarding / loading mask) for a complete example.

## Flyout Window
Flyout is a general-purpose popup (flying card) window derived from WindowImplBase. Header file: [Flyout.h](../include/dui/Control/Flyout.h). It is used to pop up arbitrary Box content around an anchor control (action panels, confirmation cards, rich-content tips, etc.), and serves as the common base for calendar popups, search suggestions and settings bubbles.

Key features:
- Anchor positioning: 8 placements (bottom/top/left/right x start/end aligned); when the preferred side has no room it automatically flips to the opposite side, and is finally clamped inside the monitor work area;
- Non-focus-stealing by default (`WS_EX_NOACTIVATE`): the parent window stays active while the flyout is shown and can keep receiving input;
- Automatically closed by clicking outside or pressing Esc by default; `Dismiss` closes it manually;
- Modeless window: the framework deletes the object automatically after the window closes, callers do not need to delete it;
- Only one active flyout at a time: showing a new flyout closes the previous one;
- The flyout is a layered popup window with shadow (`WS_POPUP | WS_EX_LAYERED`); the content XML is drawn with current-theme semantic colors, adapting to light/dark themes automatically.

### C++ Interfaces

| Interface | Description |
| :--- | :--- |
| `Flyout(Window* pParentWindow)` | Constructs a flyout with the parent window (the anchor control must belong to it) |
| `SetSkinFolder(path)` | Resource folder of the content XML; defaults to the parent window's resource path |
| `bool ShowAt(Control* anchor, const DString& xmlFile, Placement = Bottom)` | Shows the flyout around the anchor; on failure the object is already deleted and must not be accessed |
| `Dismiss()` | Closes the flyout manually (reason kManual) |
| `SetAutoDismiss(bool)` / `IsAutoDismiss()` | Whether click-outside/Esc closes automatically, default true |
| `SetNoFocus(bool)` / `IsNoFocus()` | Whether to avoid taking focus, default true; when false, focus loss is used to detect outside clicks |
| `SetGap(int)` / `GetGap()` | Gap between the flyout and the anchor (unscaled logical pixels), default 6 |
| `SetAllowFlip(bool)` / `IsAllowFlip()` | Whether flipping to the opposite side is allowed, default true |
| `GetAnchor()` | Returns the anchor control (may be nullptr after close or anchor destruction) |
| `GetPlacement()` | Returns the preferred placement |
| `IsOpen()` | Whether the flyout is open |
| `AttachOpened(callback)` / `AttachClosed(callback)` | Registers opened/about-to-close callbacks with a `CloseReason` argument |
| `static GetActiveFlyout()` | Returns the current active flyout, or nullptr |
| `static DismissActive()` | Dismisses the current active flyout (no-op if none) |

`Placement` enum: `Bottom` (below, start-aligned, default), `BottomEnd` (below, end-aligned), `Top`, `TopEnd`, `Right`, `RightEnd`, `Left`, `LeftEnd`.

`CloseReason` enum: `kManual` (manual close), `kClickOutside` (click outside / focus loss), `kEscape` (Esc key), `kAnchorLost` (anchor or parent window invalidated: destroyed/hidden/minimized/DPI changed).

### Writing the Content XML

The flyout window itself is created and styled in C++; the caller only provides a **content XML**. Its root is an ordinary `<Window>` with shadow and layered window enabled, and a fixed size fitting the content (the framework wraps a shadow around it):

```xml
<Window size="240,210" caption="0,0,0,0" use_system_caption="false"
        shadow_type="default" shadow_attached="true"
        layered_window="true" size_box="0,0,0,0">
    <VBox class="flyout" width="240" height="auto">
        <Label class="flyout_title" text="Flyout Title"/>
        <Label class="flyout_desc" text="Description text..." margin="0,6,0,10"/>
        <Button class="btn_primary_global" name="flyout_action" text="Primary Action"
                width="stretch" height="30" border_round="4,4"/>
    </VBox>
</Window>
```

Notes:
- The window size is in unscaled logical pixels; make the height large enough for all content (overflow would be painted over the shadow area);
- Name interactive controls with `name` and bind events via `FindControl` after `ShowAt` succeeds;
- The default skin defines 3 classes in global.xml: `flyout` (240-wide card, `bg_window_card` background, `border_window` border, corner radius 8), `flyout_title` (14pt bold title), `flyout_desc` (12pt `text_muted` multiline description).

### Usage Example

```cpp
//Called when the anchor button is clicked (clicking the same anchor again closes it)
void ShowFlyout(ui::Control* pAnchor)
{
    ui::Flyout* pActive = ui::Flyout::GetActiveFlyout();
    if (pActive != nullptr) {
        if (pActive->GetAnchor() == pAnchor) {
            pActive->Dismiss();      // Same anchor: toggle off
            return;
        }
        pActive->Dismiss();          // Different anchor: the old flyout closes
    }

    ui::Flyout* pFlyout = new ui::Flyout(this);
    pFlyout->SetSkinFolder(GetResourcePath().ToString());
    //The XML file name is resolved relative to SkinFolder, without a subfolder prefix
    if (!pFlyout->ShowAt(pAnchor, _T("my_flyout.xml"), ui::Flyout::Placement::Bottom)) {
        return;                      // On failure the object is already deleted
    }

    ui::Button* pAction = dynamic_cast<ui::Button*>(pFlyout->FindControl(_T("flyout_action")));
    if (pAction != nullptr) {
        pAction->AttachClick([pFlyout](const ui::EventArgs&) {
            //Do the action, then close the flyout
            pFlyout->Dismiss();
            return true;
            });
    }
}
```

Remarks:
- In no-focus mode, controls inside the flyout still receive clicks, but keyboard focus stays on the parent window; call `SetNoFocus(false)` when text input inside the flyout is required;
- Outside-click detection polls the global mouse button state (Windows only); on non-Windows platforms use the focus mode via `SetNoFocus(false)`;
- When the anchor is nested inside scrollable containers, the accumulated scroll offset is automatically removed, so the flyout always pops at the control's visual position after scrolling;
- When the parent window uses a private color theme via `OpenColorTheme` (e.g. the dark window in the ColorTheme demo), the flyout automatically inherits the same color set without extra setup;
- When the display DPI changes (e.g. moving across monitors), the flyout closes automatically with reason `kAnchorLost` to avoid wrong position/size.

See the controls demo (the "Flyout" group with 4 placement buttons) and the ColorTheme demo for complete examples.

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
| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| selected_id | | int | Default selected child item |
| tab_box_name| | string | Name of the bound TabBox control; after binding, when the TabCtrl's selected item changes, the TabBox's selected item changes accordingly |
| drag_order  | true | bool | Whether to support drag to adjust order (within the same tab); enabled by default |
| drag_out_id | 0 | int | Whether to support drag-out of this container: if not 0, dragging out is supported; otherwise it is not (dragged out to a container whose drop_in_id == drag_out_id)|
| drop_in_id | 0 | int | Whether to support drag-and-drop into this container: if not 0, dragging in is supported; otherwise it is not (dragged in from a container whose drag_out_id == drop_in_id)|
| selected_tab_item_outline_width  | 0 | float  | Width of the outer outline of the selected tab item |
| selected_tab_item_outline_color  |   | string | Color of the outer outline of the selected tab item |
| tab_ctrl_bottom_line_height      | 0 | float  | Height of the bottom line of the tab bar (this line is not drawn in the selected tab area, but drawn in other areas) |
| tab_ctrl_bottom_line_color       |   | string | Set the color of the bottom line of the tab bar |
| tab_ctrl_orientation | horz | string | Layout direction of the tab bar: `horz` (horizontal, default) / `vert` (vertical). In vertical mode, tabs are on the left and the content area on the right, and the selected-item outline and divider are automatically adapted to the right side |
| overflow_scroll | true | bool | Whether to enable overflow scrolling: when the total tab width (horizontal) or total height (vertical) exceeds the visible area, a scrollbar is shown automatically. The scrollbar floats above child controls and does not take up layout space; dragging a tab near the visible edge auto-scrolls |

**Corresponding C++ interfaces:** `SetTabCtrlOrientation`/`GetTabCtrlOrientation`, `SetEnableOverflowScroll`/`IsEnableOverflowScroll`.

The TabCtrl control inherits the `ListBox` attributes. For more available attributes, please refer to the `ListBox` attributes.

## TabCtrlItem Attributes
| Attribute name | Default value | Parameter type | Purpose |
| :---     | :---   | :---     | :--- |
| tab_box_item_index | | int | Index number of the bound TabBox child item (i.e. clicking this tab switches to the TabBox page with this index number) |
| title | | string | Title text of the tab page |
| title_id | | string | Title text ID of the tab page (for multilingual support) |
| title_class | | string | Class value of the title text resource attribute of the tab page |
| icon | | string | Icon resource string of the tab page |
| icon_class | | string | Class value of the icon resource attribute of the tab page |
| close_button_class | | string | Class value of the close button resource attribute of the tab page |
| line_class | | string | Class value of the divider resource attribute of the tab page |
| selected_round_corner | | size | Rounded corner size of the tab page in selected state|
| hovered_round_corner | | size | Rounded corner size of the tab page in hovered state|
| hovered_padding | | UiPadding | Padding of the background color of the tab page in hovered state|
| auto_hide_close_button | false | bool | Whether the close button is automatically hidden|
| fixed | false | bool | Whether the tab is a fixed tab: a fixed tab cannot be dragged, cannot be a drop target, and shows no close button; multiple fixed tabs always stay at the front of the tab bar, forming a "fixed block" (normal tabs can only move after the fixed block and cannot cross it)|
| badge_count | 0 | int64 | Badge count: shows the badge when greater than 0, automatically hidden when less than or equal to 0; the badge control is not created unless badge_class is set |
| badge_max_count | 99 | int64 | Badge count limit; when exceeded, "limit+" is displayed (e.g. "99+") |
| badge_dot | false | bool | Badge dot mode: true for a plain dot without digits; should be used together with badge_class or the badge attribute |
| badge_class | | string | Class value of the badge control (e.g. `tab_ctrl_item_badge`); the badge control is not created when empty |

**Notes:**
- TabCtrlItem has built-in badge support: specify a `Badge`-based Class via `badge_class`, then configure the display with `badge_count`/`badge_dot`/`badge_max_count`. Without `badge_class`, badge attributes only store values and no Badge child control is created.
- The default skin defines `tab_ctrl_item_badge` (count badge, height 16, white text on red background) and `tab_ctrl_item_badge_dot` (dot badge, 8x8) in global.xml, already attached to `badge_class` of `tab_ctrl_item`.
- The badge child control does not handle mouse events (`mouse_enabled="false"`), so it does not block tab clicks.

**Corresponding C++ interfaces:** `SetBadgeCount/GetBadgeCount`, `SetBadgeMaxCount/GetBadgeMaxCount`, `SetBadgeDot/IsBadgeDot`, `SetBadgeClass/GetBadgeClass`, `GetBadgeControl`.

**XML example:**

```xml
<TabCtrl class="tab_ctrl" width="stretch" height="36">
    <!-- Count badge (count=5) -->
    <TabCtrlItem class="tab_ctrl_item" title="Messages" badge_count="5"/>
    <!-- Overflow badge (shows "99+") -->
    <TabCtrlItem class="tab_ctrl_item" title="Notifications" badge_count="120"/>
    <!-- Dot badge (use the dedicated dot Class) -->
    <TabCtrlItem class="tab_ctrl_item" title="Feed" badge_class="tab_ctrl_item_badge_dot" badge_dot="true"/>
    <!-- No badge -->
    <TabCtrlItem class="tab_ctrl_item" title="Settings"/>
</TabCtrl>
```

Updating the badge dynamically in C++:

```cpp
ui::TabCtrlItem* pTabItem = dynamic_cast<ui::TabCtrlItem*>(pWindow->FindControl(_T("tab_msg")));
if (pTabItem != nullptr) {
    pTabItem->SetBadgeCount(pTabItem->GetBadgeCount() + 1);   // Unread +1
    pTabItem->SetBadgeCount(0);                               // Reset to zero, the badge is hidden automatically
}
```

**Vertical tab bar, fixed tabs and overflow scrolling:**

```xml
<!-- Vertical tab bar: tabs on the left, content area on the right; the selected-item outline and divider adapt to the right side automatically -->
<TabCtrl class="tab_ctrl" tab_ctrl_orientation="vert" width="160" height="stretch">
    <!-- Fixed tabs: always at the front, not draggable, no close button -->
    <TabCtrlItem class="tab_ctrl_item" title="Home" fixed="true"/>
    <TabCtrlItem class="tab_ctrl_item" title="Favorites" fixed="true"/>
    <!-- Normal tabs: draggable to reorder, but cannot cross the front fixed block -->
    <TabCtrlItem class="tab_ctrl_item" title="Messages" badge_count="5"/>
    <TabCtrlItem class="tab_ctrl_item" title="Settings"/>
</TabCtrl>

<!-- Horizontal tab bar + overflow scrolling: a scrollbar appears automatically when there are too many tabs -->
<TabCtrl class="tab_ctrl" overflow_scroll="true" width="stretch" height="36">
    <TabCtrlItem class="tab_ctrl_item" title="Tab1"/>
    <TabCtrlItem class="tab_ctrl_item" title="Tab2"/>
    <TabCtrlItem class="tab_ctrl_item" title="Tab3"/>
    ...
</TabCtrl>
```

- `tab_ctrl_orientation="vert"` switches the tab bar to a vertical layout (internally uses `VLayout`; the selected-item outline and divider adapt to the right side automatically).
- `fixed="true"` tabs form the front "fixed block": they cannot be dragged, cannot be drop targets, and show no close button; normal tabs can only move after the fixed block and cannot cross it; fixed tabs always stay at the front of the tab bar (even when appended via `AddItem`, they are inserted at the end of the fixed block).
- `overflow_scroll="true"` (default) shows a scrollbar automatically when the total tab size exceeds the visible area. The scrollbar floats above child controls and does not take up layout space; dragging a tab near the visible edge auto-scrolls.

**Prerequisite for overflow: tab items need a definite width**

The default skin's `tab_ctrl_item` class does not define `width`, and the control base class defaults to `stretch`
(see the `PlaceHolder` constructor). Tab items are therefore **divided equally to fill** the tab bar — in that case the
content width never exceeds the visible width no matter how many tabs exist, so the scrollbar never appears.
To make overflow scrolling work, give each tab item a definite width (e.g. `width="96"`) or let it size to its content (`width="auto"`).

- Horizontal overflow: `TabCtrl` width < sum of tab item widths.
- Vertical overflow: `TabCtrl` height < sum of tab item heights (with `tab_ctrl_orientation="vert"`, `SetTabCtrlOrientation` enables the vertical scrollbar automatically).

```xml
<!-- Horizontal overflow: tab bar is 300 wide, items are 96 wide, overflow starts at the 4th tab -->
<TabCtrl class="tab_ctrl" width="300" height="36" overflow_scroll="true">
    <TabCtrlItem class="tab_ctrl_item" title="Tab1" width="96"/>
    ...
</TabCtrl>
```

**Fixed block positioning rules** (also honored when toggling at runtime via `SetFixed`):

- The fixed block is always a contiguous range at the front of the tab bar; if the XML does not list fixed tabs first, they are normalized during initialization (their relative order is preserved).
- Calling `TabCtrlItem::SetFixed(true)` at runtime moves that tab to the end of the fixed block; `SetFixed(false)` moves it to the first position after the fixed block.
- Insertion and moving are constrained: `AddItem` inserts fixed tabs at the end of the fixed block and normal tabs at the end; `AddItemAt`/`SetItemIndex` only allow fixed tabs within `[0, fixed count]` and normal tabs within `[fixed count, item count]`.

```cpp
ui::TabCtrl* pTabCtrl = dynamic_cast<ui::TabCtrl*>(pWindow->FindControl(_T("tab_ctrl")));
if (pTabCtrl != nullptr) {
    // Toggle fixed: the tab moves to the corresponding block boundary automatically
    ui::TabCtrlItem* pItem = dynamic_cast<ui::TabCtrlItem*>(pTabCtrl->GetItemAt(0));
    if (pItem != nullptr) {
        pItem->SetFixed(true);
    }
    const size_t nFixedCount = pTabCtrl->GetFixedItemCount();   // number of fixed tabs
}
```

The TabCtrlItem control inherits the `ControlDragableT` attributes. For more available attributes, please refer to the `ControlDragableT` attributes.

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
ListBoxItem is a concrete implementation of the ListBoxItemTemplate template class, defined in the `dui/Box/ListBoxItem.h` file; there are three related type definitions:    
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


### ListCtrl Table Enhancements

For report-style scenarios (similar to Qt QTableView / WPF DataGrid), ListCtrl provides the following enhancements in Report view:

| Capability | Interface / Attribute | Description |
| :--- | :--- | :--- |
| Drag to resize columns | Header split bar (`enable_column_width_auto` controls double-click auto-fit) | Existing capability: drag the header split bar to resize a column; double-click to auto-fit by content |
| Drag header to reorder | `enable_header_drag_order` | Existing capability: drag the header to change column order (enabled by default) |
| Cell icon | `SetSubItemImageId(ById)` | Existing capability: set an icon for a cell |
| Frozen columns (freeze first column) | `frozen_column_count` / `SetFrozenColumnCount` / `GetFrozenColumnCount` | Freeze the first N columns on the left; they stay fixed when scrolling horizontally. The header is frozen as well, and grid lines and mouse hit testing are adapted |
| Cell progress bar | `SetSubItemProgress(ById)` / `GetSubItemProgress(ById)` | Displays a cell as a progress bar. Valid range is `[0, 100]`; `-1` turns off the progress bar (renders as plain text) |
| Cell rich text | `SetSubItemRichText(ById)` / `IsSubItemRichText(ById)` | Renders cell text as rich text (supports tags such as `<b>/<i>/<u>/<font>/<a>`, see [RichText attributes](#richtext-attributes)) |
| Cell word wrap | `SetSubItemSingleLine(ById)` / `IsSubItemSingleLine(ById)` | Control single/multi-line per cell: `true` = single line (ellipsis on overflow, default), `false` = word wrap (row height must be enlarged via `SetDataItemHeight`, otherwise clipped). Report view only |
| Single-cell refresh | `RefreshSubItem` / `RefreshSubItemById` | Refresh only the specified cell (partial update for progress/status columns), avoiding a full row refresh |
| Column show/hide | `SetColumnVisible(ById)` / `IsColumnVisible(ById)` | ListCtrl-level column visibility API (equivalent to the header item's visibility; the data columns are synchronized automatically) |
| Column layout events | `AttachColumnWidthChanged` / `AttachColumnOrderChanged` / `AttachColumnVisibleChanged` / `AttachColumnSorted` | Notify the application when column width/order/visibility/sort changes, for persisting the column layout |
| Cell selection mode | `selection_mode` (XML, `row`/`cell`) / `SetSelectionMode` / `GetSelectionMode` | Selection mode: `row` (row selection, default, existing behavior unchanged) / `cell` (cell selection); switching to `cell` clears the row selection, switching back to `row` clears the cell selection |
| Cell selection | `SetCurrentCell` / `GetCurrentCell` / `SetCellSelection` / `ClearCellSelection` / `GetCellSelection` / `SetSelectedCells` / `GetSelectedCells` / `IsCellSelected` / `IsCellCurrent` | Current cell and selection set in cell selection mode: the selected state is stored per cell at the data layer, indexed by row identity (RowId) + column ID (decoupled from display order, so it does not drift after sorting), supporting **discontiguous** selection. A plain click replaces the selection with that cell (anchor = focus), `Ctrl+click` toggles that cell, `Shift+click`/dragging replaces the selection with a rectangle, `Ctrl+drag`/`Ctrl+Shift+click` appends a rectangle; `GetSelectedCells` expands it to a cell list, and `SetSelectedCells` is the cell equivalent of the row API `SetSelectedDataItems` |
| Cell selection events | `AttachCurrentCellChanged` / `AttachCellSelectionChanged` | `kEventCurrentCellChanged` (current cell changed, wParam = row display index, lParam = column ID) / `kEventCellSelectionChanged` (selection area changed, wParam = number of areas, pEventData points to `std::vector<ui::ListCtrlCellRange>*` (the list of selected areas, possibly multiple, empty means no selection)); XML inline event names `on_current_cell_changed` / `on_cell_selection_changed` |
| Row/cell mode dispatch | `SetSelectAll` / `SetSelectNone`; `SetSelectedDataItems` / `GetSelectedDataItems` | `SetSelectAll` / `SetSelectNone` dispatch by the current mode (`row` selects/clears rows, `cell` selects/clears cells); the row-only APIs `SetSelectedDataItems` / `GetSelectedDataItems` **keep their original behavior** (they still read/write the underlying row-selection state normally), and `SetSelectedCells` / `GetSelectedCells` are available as cell equivalents. In `cell` mode isolation is not achieved by suppressing APIs but by suppressing the row-selected visual at the **paint layer** (`ListCtrlReportView::CanPaintSelectedColors` returns false), so no matter which entry point changes the row-selection state (right-click, keyboard, APIs), the UI never shows a row as selected |
| Copy / export | `enable_copy` / `SetEnableCopy` / `IsEnableCopy`, `enable_csv_export` / `SetEnableCsvExport` / `IsEnableCsvExport`, `CopySelectionToClipboard(bCsvFormat)`, `ExportToCsvFile(path, bSelectedOnly)` | TSV copy (`Ctrl+C`: row mode copies the selected rows, cell mode copies the selected area) and CSV export (`Ctrl+Shift+C` copies CSV to the clipboard; `ExportToCsvFile` writes a UTF-8 BOM CSV file that Excel opens directly without garbled Chinese). For multi-area selection in `kCell` mode the **union bounding rectangle** is used, and unselected cells inside it are written as empty strings to keep rows/columns aligned; whole-table export does not include the header row; mutually exclusive with the editing state (commit the edit first, then copy). Both switches default to on; when off, the corresponding shortcuts and APIs are inert (return `false` and change neither the editing state nor the data) |
| Column-level compare mode | `SetColumnSortFlag(ById)` / `GetColumnSortFlag(ById)`, flags in `ListCtrlSubItemSortFlag` | Declares the comparison mode per column (text/number/date/progress, etc.); clicking that column's header uses it automatically. `kSortByProgress` sorts by progress value, `kSortByTextAsNumber` compares text numerically (`"10" > "9"`), `kSortByTextAsDate` compares text chronologically (`"2024-3-7" < "2024-10-1"`) |
| Batch row deletion | `DeleteDataItems(const std::vector<size_t>&)` | Deletes many rows at once (indices may be unordered and may include out-of-range/duplicate entries, which are ignored); deduplicates by row identity and performs a single-pass compaction of the display-order table, avoiding the O(K×N) shifting of row-by-row deletion; returns the number of rows actually deleted |
| Storage compaction | `CompactStorage()` | An explicit whole-table maintenance operation: renumbers row identities and compacts the storage pages, reclaiming the tombstone slots and empty pages left by repeated insert/delete. Call it only when no fill/paint is in progress; afterwards all outstanding internal cell raw pointers become invalid. During single/batch deletion, a storage page whose slots are all tombstones is released automatically |
| Cell editing | `EnterCellEdit` / `EnterCurrentCellEdit` / `IsCellEditing` / `GetEditingCell` | kCell mode + Report view; enter editing with `F2` or "a single click after the cell is selected" (the click path is unchanged); `Enter` commits and moves down one row (kCell mode only; in kRow row mode committing a candidate neither moves the selected row nor sets a current cell nor scrolls the view), `Esc` cancels, focus loss commits without moving; requires `enable_item_edit="true"` |
| Per-column editor type | `SetColumnEditorType` / `SetColumnEditorTypeById` / `GetColumnEditorType` / `GetColumnEditorTypeById` | Enum `ListCtrlEditorType { kEditText, kEditNumber, kEditDate, kEditCombo }`; column declaration field `ListCtrlColumn::nEditorType` (defaults to `kEditText`); input is validated on commit by the declared type (an empty string clears the cell and is always valid; a number column accepts decimal numbers; a date column accepts `yyyy-mm-dd` / `yyyy/mm/dd`, reusing `Calendar::ParseDateString`); invalid input is discarded (no write-back, and `kEventLeaveEdit` is not fired); `kEditCombo` is the dropdown-candidate editor (configure candidates with `SetColumnCandidates(ById)`, supported in both kCell and row selection modes); **no XML attribute is introduced** (columns are declared in code) |
| Per-column candidates (dropdown editor) | `SetColumnCandidates` / `SetColumnCandidatesById` / `GetColumnCandidates` / `GetColumnCandidatesById` | Stores a plain-text candidate list per column (`std::vector<DString>`, stored by column ID) for `kEditCombo`; passing an empty list clears the candidates; a `kEditCombo` column with no candidates does not enter editing on `F2` / click; the candidate flyout reuses `Flyout` + `VListBox` (theme classes `list_ctrl_combo_flyout` / `list_ctrl_combo_list` / `list_ctrl_combo_item`); commit with a mouse click or `Enter`, move the highlight with `↑`/`↓`, and cancel without write-back with `Esc` or by clicking outside |

Demo: the four groups "Cell Selection & Check", "Copy & Export", "Progress & Editor" and "Event & State" in the left settings panel of `examples/ListCtrl` provide manual test entries for the capabilities above — cell-selection APIs and cell-selection events (also showing the current cell / selected-cell count), row-selection events (`Select` / `SelChanged`, also showing the selected-row count and row indices), the CheckBox checking APIs and `auto_check_select`, copy as CSV, export the whole table as CSV, the copy / CSV-export toggles, runtime switching of `progress_text_pos` / `progress_color` and single-cell progress updates, per-column editor-type switching (text / number / date / combo), custom sorting with `SetSortCompareFunction`, edit-state events, and the column-layout events (shown live by the "recent column event" label). Column references in the on-screen labels and message boxes are shown as the **column index (ColumnIndex)** rather than the column ID for readability.

**Column-level compare mode (sorting):**

The comparison mode is declared per column; clicking a column header sorts by that column's declared mode (determined by the column's semantics, not inferred from the current cell state):

```cpp
// Column 2 (index 1) sorts by progress value; column 4 sorts text numerically; column 5 sorts text chronologically
pListCtrl->SetColumnSortFlag(1, ui::ListCtrlSubItemSortFlag::kSortByProgress);
pListCtrl->SetColumnSortFlag(3, ui::ListCtrlSubItemSortFlag::kSortByTextAsNumber);
pListCtrl->SetColumnSortFlag(4, ui::ListCtrlSubItemSortFlag::kSortByTextAsDate);
uint8_t flag = pListCtrl->GetColumnSortFlag(3); // read the column's compare mode
```

Common flags (`ListCtrlSubItemSortFlag`, bitwise-combinable): `kSortByText` (default, lexicographic text), `kSortByTextAsNumber` (numeric text, `"10" > "9"`), `kSortByTextAsDate` (chronological text, e.g. `"2024-3-7" < "2024-10-1"`; fields may be separated by any non-digit characters and month/day need not be zero-padded; text that cannot be parsed as a date is treated as the minimum value), `kSortByProgress` (by `nProgress`), `kSortByUserDataN` / `kSortByUserDataS` (by user data), `kSortByGroup` (group by `nSortGroup` first), `kSortNoCase` (case-insensitive strings).

**Sort stability:**

Sorting is stable (`std::stable_sort`): elements that compare equal keep their original relative order, in both ascending and descending order. So rows in the same group (e.g. folders/files) or with equal values will not be disordered or interleaved by sorting; descending order is produced by a transposed comparison rather than "sort ascending then reverse the whole list" (the latter would also reverse the relative order of equal rows). Rows with no cell data in the sort column are uniformly treated as the minimum value: first in ascending order, last in descending order. A custom compare function (`SetSortCompareFunction`, or the `pfnCompareFunc` argument of `SortDataItems`) only needs to implement the `(a < b)` semantics; stability is guaranteed by the sorter.

**Frozen columns (freeze first column):**

```cpp
// Fix the first column on the left; it stays fixed when scrolling horizontally
pListCtrl->SetFrozenColumnCount(1);
size_t nCount = pListCtrl->GetFrozenColumnCount();
```

It can also be configured in XML: `<ListCtrl class="list_ctrl" name="list_ctrl" frozen_column_count="1"/>`

**Cell progress bar:**

The progress bar style is controlled by the `progress_color` (foreground) and `progress_bkcolor` (background track) semantic color attributes of `ListCtrlSubItem`; the defaults are configured in the `list_ctrl_sub_item` Class in `global.xml` (`bg_progress` / `bg_progress_track`). The bar is a rounded capsule, vertically centered, and its height automatically fits the text. If text is also set for the cell, the text is drawn on top of the progress bar (useful for showing a percentage); its position is controlled by `progress_text_pos`:

| Attribute | Default | Purpose |
| :--- | :--- | :--- |
| `progress_text_pos` | `center` | Text position: `center` (centered in the cell) / `left` (left-aligned in the cell) / `right` (right-aligned in the cell) / `outside` (to the right of the bar, which shrinks accordingly). C++: `SetProgressTextPos()` / `GetProgressTextPos()` |

```cpp
// Display column 2 (columnIndex=1) as a progress bar with value 60
pListCtrl->SetSubItemProgress(0, 1, 60);
int32_t nProgress = pListCtrl->GetSubItemProgress(0, 1); // returns 60

// Turn off the progress bar for this cell (back to plain text)
pListCtrl->SetSubItemProgress(0, 1, -1);
```

It can also be set at once when filling data:

```cpp
ui::ListCtrlSubItemData subItemData;
subItemData.text = _T("60%");
subItemData.nProgress = 60;      // -1 means no progress bar
pListCtrl->SetSubItemData(itemIndex, columnIndex, subItemData);
```

**Cell editing (C3):**

Only effective in kCell mode + Report view, and requires `enable_item_edit="true"`. Enter editing with `F2` (equivalent to `EnterCurrentCellEdit`, entering editing immediately, bypassing the 600 ms delay; the row is scrolled into view first if it is off-screen) or with "a single click after the cell is selected" (the existing click path is reused); `Enter` commits and moves the current cell down one row (kCell mode only; in kRow row mode committing a candidate neither moves the selected row nor sets a current cell nor scrolls the view), `Esc` cancels without moving, and clicking elsewhere to lose focus commits without moving; pressing `F2` on a pinned row / a non-editable cell / when `enable_item_edit="false"` has no effect.

```cpp
// Declare column 2 as a number column and column 5 as a date column (column index starts at 0)
pListCtrl->SetColumnEditorType(1, ui::ListCtrlEditorType::kEditNumber);
pListCtrl->SetColumnEditorType(4, ui::ListCtrlEditorType::kEditDate);
// Or by column ID (unaffected when the column order changes)
// pListCtrl->SetColumnEditorTypeById(columnId, ui::ListCtrlEditorType::kEditNumber);
// Enter editing on the current cell (equivalent to pressing F2)
pListCtrl->EnterCurrentCellEdit();
```

On commit the input is validated by the column's declared editor type: an empty string is always valid (it clears the cell), `kEditText` is always valid, `kEditNumber` accepts only a valid decimal number, and `kEditDate` accepts only `yyyy-mm-dd` / `yyyy/mm/dd` (reusing `Calendar::ParseDateString`); invalid input is discarded (no write-back, and `kEventLeaveEdit` is not fired). For `kEditNumber` columns the input is also restricted while editing: only digits, a sign and a decimal point can be entered (implemented with the RichEdit character whitelist, effective for both typing and pasting), so non-numeric characters cannot be typed. A `kEditDate` column is edited with the `DateTime` control (edit format `date_calendar`, theme class `list_ctrl_datetime`; the RichEdit is no longer reused): entering editing automatically pops up the month-calendar flyout; picking a date in the calendar commits immediately (in kCell mode the current cell moves down one row on success), clicking the calendar's "Clear" writes back an empty cell, and `Esc` closes the calendar without write-back; before commit the text is still validated with `Calendar::ParseDateString` and invalid dates are discarded. The existing events `kEventEnterEdit` / `kEventLeaveEdit` are reused — no new event is added; `kEditText` / `kEditNumber` share the same RichEdit editor, while `kEditDate` uses the `DateTime` control.

**Cell editor: dropdown candidates (kEditCombo):**

Candidates are configured per column (a plain-text list, stored by column ID). It does not use a RichEdit; instead it pops up a candidate flyout below the cell (internally reusing `Flyout` + `VListBox` without stealing focus: the parent window stays active and keyboard events are still handled by the `ListCtrl`).

```cpp
// Declare column 8 as a dropdown-candidate editor and configure its candidates (column index starts at 0)
std::vector<DString> candidates = { _T("Red"), _T("Green"), _T("Blue") };
pListCtrl->SetColumnCandidates(7, candidates);
pListCtrl->SetColumnEditorType(7, ui::ListCtrlEditorType::kEditCombo);

// By column ID (column IDs are stable while the column order may change)
// pListCtrl->SetColumnCandidatesById(columnId, candidates);
// std::vector<DString> items = pListCtrl->GetColumnCandidatesById(columnId);
// Passing an empty list clears the candidates: pListCtrl->SetColumnCandidates(7, std::vector<DString>());
```

Interaction:
- Enter editing: press `F2` or "a single click after the cell is selected", the same path as the other editors; the initially highlighted candidate is the index of the current cell text in the candidate list (the first item if there is no match).
- Click a candidate or press `Enter`: commits immediately and closes the flyout, writing the text back to the cell (in kCell mode the current cell moves down one row on success, matching the `Enter` semantics of the RichEdit editor; in kRow row mode committing a candidate neither moves the selected row nor sets a current cell nor scrolls the view).
- Press `↑` / `↓`: moves the candidate highlight; press `Esc`, click outside the flyout, or click/scroll elsewhere in the list: closes the flyout **without write-back** (treated as abandoning this edit).
- When the column has no candidates configured, pressing `F2` / clicking does not enter editing (no empty flyout is shown).
- Effective in both kCell and row selection modes + Report view, and still subject to `enable_item_edit="true"` and the cell's editable attribute; in row selection mode editing is entered through the existing "a single click after the cell is selected" path, while the `F2` shortcut is still supported only in kCell mode.

**Cell rich text:**

```cpp
ui::ListCtrlSubItemData subItemData;
subItemData.bRichText = true;
subItemData.text = _T("<b>Title</b> <font color=\"#1E90FF\">desc</font>");
pListCtrl->SetSubItemData(itemIndex, columnIndex, subItemData);

// Or set it separately:
pListCtrl->SetSubItemRichText(0, 2, true);
bool bRichText = pListCtrl->IsSubItemRichText(0, 2); // returns true
```

**Cell word wrap (Report view):**

Cells in the Report view default to `single_line="true"` (single line with a trailing ellipsis). Use the API below to switch a cell to multi-line (word wrap):

```cpp
// Enable word wrap for the cell at row 0, column 1
pListCtrl->SetSubItemSingleLine(0, 1, false);
bool bSingleLine = pListCtrl->IsSubItemSingleLine(0, 1); // returns false

// By column ID
size_t nColumnId = pListCtrl->GetColumnId(1);
pListCtrl->SetSubItemSingleLineById(0, nColumnId, false);
```

Notes:
- Report view only (for List / Icon views, wrapping is controlled by the view's own `SetTextSingleLine`); in non-Report views `Set*` returns `false`.
- Because the view is virtual, the property is stored in the data model and applied when the cell is filled, so scrolling and control reuse never mix up the property.
- Word wrap requires a taller row height, otherwise the overflow is clipped. Call `SetDataItemHeight(itemIndex, height, true)` (per row) or `SetDataItemHeight(height, true)` (default row height) as needed:

```cpp
// Enable wrap and enlarge the default row height to fit multiple lines
pListCtrl->SetSubItemSingleLine(0, 1, false);
pListCtrl->SetDataItemHeight(64, true);
```

**Single-cell refresh:**

When only individual cells change (e.g. progress or status columns), use `RefreshSubItem` to update just that cell instead of the whole row:

```cpp
// Scenario: a progress/status column is advanced by a background task, with the value maintained by the app
// 1) Wrap the "data write" with SetEnableRefresh(false) so the change does not trigger a full-row auto refresh; restore afterwards
bool bOldRefresh = pListCtrl->SetEnableRefresh(false);
pListCtrl->SetSubItemProgress(0, 1, 42);   // write data only, no UI refresh
pListCtrl->SetEnableRefresh(bOldRefresh);

// 2) After refresh is restored, update this one cell only (the fill callback writes the latest value into it)
pListCtrl->RefreshSubItem(0, 1);

// Or refresh by column ID
size_t nColumnId = pListCtrl->GetColumnId(1);
pListCtrl->RefreshSubItemById(0, nColumnId);
```

Note: this API is for Report view only. It returns `false` while `IsEnableRefresh()` is `false` (i.e. still inside the `SetEnableRefresh(false)` "no-refresh window"), so restore refresh before calling it. It also returns `false` if the cell is currently outside the visible range (no UI control exists for it); the data itself is already updated and will be shown when scrolled into view.

> Note: the per-cell filled event `kEventReportViewSubItemFilled` has been removed (it fired too frequently and had a noticeable performance impact); the same functionality can be achieved with `kEventReportViewItemFilled` (`pEventData` is `ListCtrlItem*`; `pExtraData` is the `ListCtrlSubItem*` filled this time — `nullptr` for a whole-row fill, and pointing to the cell when `RefreshSubItem` refreshes a single cell; a cell can also be obtained from the item via `GetSubItem(columnIndex)`).

**Column show/hide:**

```cpp
// Hide column 2
pListCtrl->SetColumnVisible(1, false);
bool bVisible = pListCtrl->IsColumnVisible(1); // returns false

// By column ID
pListCtrl->SetColumnVisibleById(nColumnId, true);
```

**Column layout events (persist column layout):**

Events are fired when column width, order, visibility or sort state changes, so the application can save the layout:

```cpp
// Column width changed: wParam is the column ID, lParam is the related column ID (may be Box::InvalidIndex)
pListCtrl->AttachColumnWidthChanged([](const ui::EventArgs& args) {
    size_t nColumnId = (size_t)args.wParam;
    // Save width: pListCtrl->GetColumnWidthById(nColumnId);
    return true;
});

// Column order changed (after dragging the header)
pListCtrl->AttachColumnOrderChanged([](const ui::EventArgs& args) {
    // Iterate with GetColumnId(columnIndex) to save the order
    return true;
});

// Column visibility changed: wParam is the column ID, lParam is the new state (1 visible, 0 hidden)
pListCtrl->AttachColumnVisibleChanged([](const ui::EventArgs& args) {
    size_t nColumnId = (size_t)args.wParam;
    bool bVisible = (args.lParam != 0);
    return true;
});

// Column sort changed: wParam is the column ID, lParam is the new state (1 ascending, 0 descending)
pListCtrl->AttachColumnSorted([](const ui::EventArgs& args) {
    size_t nColumnId = (size_t)args.wParam;
    bool bSortedUp = (args.lParam != 0);
    return true;
});
```

**Cell selection mode (C1 / C1b):**

The default is `selection_mode="row"` (row selection); the existing row selection, keyboard navigation and event behavior remain completely unchanged. Set it to `cell` to enter cell selection mode:

```cpp
// Switch to cell selection mode
pListCtrl->SetSelectionMode(ui::ListCtrlSelectionMode::kCell);
ui::ListCtrlSelectionMode mode = pListCtrl->GetSelectionMode(); // returns kCell

// Current cell: the cell hit by a click becomes the current cell (and also the area anchor)
ui::ListCtrlCellId cell = pListCtrl->GetCurrentCell(); // cell.nRowIndex / cell.nColumnId; use IsValid() to test validity
pListCtrl->SetCurrentCell(0, nColumnId); // replace semantics: clear other cells and select only this one

// Set / get / clear the selection area: anchorCell is the anchor, focusCell is the focus
ui::ListCtrlCellId anchorCell, focusCell;
pListCtrl->SetCellSelection(anchorCell, focusCell, false); // third parameter: true = append this rectangle (keep existing), false = clear first then set
pListCtrl->GetCellSelection(anchorCell, focusCell);       // get the anchor and focus
pListCtrl->ClearCellSelection(); // repaints and fires kEventCellSelectionChanged only when there is real selection state (selected cells or a current cell); no event when nothing changed

// Cell equivalents of the row APIs SetSelectedDataItems / GetSelectedDataItems
std::vector<ui::ListCtrlCellId> cellList;
pListCtrl->SetSelectedCells(cellList, true); // batch-select cells; true = clear others first
pListCtrl->GetSelectedCells(cellList);       // expand to a list of selected cells

// Queries (for the application's custom paint / edit decisions)
bool bSelected = pListCtrl->IsCellSelected(0, nColumnId); // whether the cell is selected
bool bCurrent  = pListCtrl->IsCellCurrent(0, nColumnId);  // whether the cell is the current cell

// Select-all / clear: dispatched by the current mode (cells in cell mode, rows in row mode)
pListCtrl->SetSelectAll();
pListCtrl->SetSelectNone(); // in cell mode it reuses ClearCellSelection's conditional dispatch: no kEventCellSelectionChanged when nothing is selected

// Events
pListCtrl->AttachCurrentCellChanged([](const ui::EventArgs& args) {
    size_t nRowIndex = (size_t)args.wParam; // row display index
    size_t nColumnId = (size_t)args.lParam; // column ID
    return true;
});
pListCtrl->AttachCellSelectionChanged([](const ui::EventArgs& args) {
    // wParam is the number of selected areas; when there is no selection wParam is 0 and pEventData is nullptr
    auto* pRangeList = (std::vector<ui::ListCtrlCellRange>*)args.pEventData;
    if (pRangeList != nullptr) {
        // the result may contain multiple discontiguous areas, each with nRowBegin / nRowEnd / nColumnIdBegin / nColumnIdEnd
        for (const ui::ListCtrlCellRange& range : *pRangeList) {
            if (range.IsValid()) {
                // use range ...
            }
        }
    }
    return true;
});
```

It can be configured and bound to inline events directly in XML: `<ListCtrl class="list_ctrl" selection_mode="cell"><Event type="current_cell_changed" .../><Event type="cell_selection_changed" .../></ListCtrl>`.

Interaction notes (aligned with Windows Explorer): in `cell` mode,
- Mouse: a plain click replaces the selection with that cell (clearing others); `Ctrl+click` toggles that cell (clicking again deselects, supporting discontiguous selection); `Shift+click` extends from the anchor to that cell (replace); `Ctrl+Shift+click` extends from the anchor to that cell (append); holding the **left or right button** and dragging shows a light-blue translucent rubber-band overlay **identical** to row-selection mode (reusing `ListBoxHelper`: same color, same alpha, same DPI-scaled border, top clipped at the header bottom); on release the cells inside the rectangle are selected (replace), while `Ctrl+drag` appends (without clearing the existing selection); dragging to the viewport edge auto-scrolls the view (same as row-selection mode).
- Keyboard: arrow keys move the current cell (replacing the selection with that cell), `Shift+arrow` extends the area (replace), `Ctrl+arrow` moves the focus only without changing the selection, `Ctrl+Shift+arrow` appends an extension; `Home`/`End` go to the first/last column of the current row, `Ctrl+Home`/`Ctrl+End` go to the first/last cell of the table; `PageUp`/`PageDown` page; `Ctrl+A` selects all cells (multi-select only, ignored in single-select); `Ctrl+Space` toggles the current cell (multi-select only, consistent with row mode, ignored in single-select).
- Mode isolation: in `cell` mode all row-selection entry points (clicking a row, row Hover/Pushed highlight, `Ctrl+A`/arrow keys, row frame selection, `SetSelectedDataItems`, etc.) do not affect row selection (mouse operations never change the row-selection state, so switching back to `row` mode shows no leftover selected rows); clicking a blank area clears the cell selection. Note: cell-mode rubber-band selection reuses the ListBox frame-selection capability and therefore requires `multi_select == true` (ListCtrl defaults to `true`).
- Paint: the selected area is filled translucent, the current cell gets a focus rectangle; frozen-column cells participate in the selection as well.
- Pinned rows: the header and pinned rows (non-selectable rows) do not participate in cell selection or highlight painting; clicking/hovering on them does not "pierce through" to select the cells underneath that they occlude, and the cell highlight is never painted over pinned-row content.
- Single-select semantics: when `multi_select="false"`, the `cell` mode matches the row mode — **at most one cell is selected**: `Shift+click` / `Ctrl+click` behave like a plain click, `Shift+arrow` degrades to plain movement of the current cell, `Ctrl+A`/`Ctrl+Space` change nothing (row mode does not support `Ctrl+Space` either), and no drag-box selection is started; `SetSelectedCells` / `SetCellSelection` keep only the first (or the focus) cell; switching from multi- to single-select trims the selection down to the first cell (symmetric with `OnSwitchToSingleSelect` in row mode). The "current cell" focus box is still kept — only the selection set is limited to one.

See the ListCtrl example program for a full demo (the table freezes the first column by default, column 2 is a progress bar, column 3 is rich text).

## StatusBar (inherits HBox attributes) Attributes
| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| separator_width | 1 | int | Width of the separator line between adjacent status panes (pixels; 0 disables the separator) |
| separator_color | "border_window" | string | Separator line color (semantic color name or color value) |
| size_grip | false | bool | Whether to draw a size grip at the bottom-right corner |
| size_grip_resize | true | bool | Whether the size grip participates in window drag-resize. When true and the status bar sits at the bottom-right corner of the window client area, the window's bottom-right resize area (the right/bottom of `size_box`) is enlarged automatically to cover the grip, so the window can really be resized by dragging. When false the grip is only a visual hint. Note the effect is window-level: after enlarging, a strip along the window bottom edge also becomes resizable. The window must be resizable in the first place: Windows is by default; on SDL and similar platforms declare a non-zero `size_box` on the Window in XML |
| size_grip_color | "border_window" | string | Size grip color (semantic color name or color value) |

The StatusBar control inherits the `HBox` attributes. For more available attributes, please refer to the `HBox` attributes.

A status bar shows status hints such as cursor position, character count, connection state and background progress; it is a common element in editors, IDEs and communication apps.
StatusBar is a horizontal container (it inherits from `HBox`); each child control is a "status pane" (usually a `Label` / `IconControl` / `Progress`):
panes can use a fixed width or stretch (width="stretch") and otherwise keep all the layout capabilities of `HBox`. Vertical separators are drawn automatically
between adjacent panes, and can be disabled or resized via `separator_width` and recolored via `separator_color`. An optional size grip (`size_grip`) can be drawn at the bottom-right corner.

Status panes can also be addressed **by index**: indexes start at 0, follow layout order, and count only visible panes (invisible panes do not occupy an index), so they match the pane order you see.
Application code no longer needs `FindControl` + `dynamic_cast` to reach a child `Label`; pane contents can be read and written by index directly.
Indexes are renumbered whenever the pane set changes (children added/removed, visibility changed).

XML example:
```xml
<StatusBar name="status_bar" width="stretch" height="30" bkcolor="bg_window_card" border_size="0,1,0,0" bordercolor="border_window" size_grip="true">
    <Label name="sb_state" text="Ready" width="110" valign="center" padding="10,0,10,0"/>
    <Label name="sb_conn" text="Connected" width="140" valign="center" padding="10,0,10,0"/>
    <Progress name="sb_progress" width="stretch" min="0" max="100" value="60" valign="center" bkcolor="bg_progress_track" progress_color="bg_progress"/>
</StatusBar>
```

Related C++ APIs:
```cpp
// separators
void SetSeparatorWidth(int32_t nWidth);      // separator width (0 disables it)
void SetSeparatorColor(const DString& strColor);

// size grip
void SetShowSizeGrip(bool bShow);            // bottom-right size grip
void SetSizeGripResize(bool bResize);        // whether the grip drag-resizes the window (default true)
bool  IsSizeGripResize() const;
void SetSizeGripColor(const DString& strColor);

// status pane (cell) index APIs
size_t   GetPanelCount() const;                          // pane count (visible panes only)
Control* GetPanel(size_t nIndex) const;                  // pane by index; nullptr when out of range
int32_t  GetPanelIndex(Control* pPanel) const;           // index of a pane; -1 when not a pane
Control* FindPanel(const DString& strName) const;        // pane by name
bool     SetPanelText(size_t nIndex, const DString& strText); // set pane text (Label/RichEdit-like panes)
DString  GetPanelText(size_t nIndex) const;              // get pane text
```

Using the pane index APIs:
```cpp
ui::StatusBar* pStatusBar = dynamic_cast<ui::StatusBar*>(FindControl(_T("status_bar")));
if (pStatusBar != nullptr) {
    //pane 0 is the "state" pane: set its text by index, no need to FindControl the child Label
    pStatusBar->SetPanelText(0, _T("State: Busy"));

    //read back and reverse lookup
    DString strText = pStatusBar->GetPanelText(0);
    ui::Control* pPanel = pStatusBar->GetPanel(0);
    int32_t nIndex = pStatusBar->GetPanelIndex(pPanel);   // 0
}
```

## ToolBar (inherits HBox attributes) Attributes
| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| overflow | true | bool | Whether to enable overflow collapsing (automatically hide trailing children when there is not enough width) |
| overflow_button_class |  | string | Style Class of the "more" button on the right (a style from global.xml; empty means no style) |
| overflow_button_width | 0 | int | Width of the "more" button in pixels (0 means auto-size to content) |
| overflow_button_text | ">>" | string | Text shown on the "more" button |

The ToolBar control inherits the `HBox` attributes. For more available attributes, please refer to the `HBox` attributes.

A toolbar holds a row of tool buttons and is a common shell element in editors, IDEs and office apps.
ToolBar is a horizontal container (it inherits from `HBox`); its children are laid out horizontally and keep all the layout capabilities of `HBox`.
Separators inside a toolbar can simply use the `Line` control (`vertical="true"`).
When there is not enough width for trailing children, they are hidden automatically and a "more" button (default text `>>`) is shown at the right end.
Clicking that button pops up a flyout listing the collapsed children; clicking an item in the flyout fires the callback registered via `AttachOverflowItemActivated`
(its argument is the `name` of the clicked child), so the application layer can dispatch commands.
Collapsing is recalculated automatically when the window width changes, and previously collapsed items are restored when there is enough width again.

XML example:
```xml
<ToolBar name="main_toolbar" width="stretch" height="36" padding="8,0,8,0" child_margin_x="4"
         overflow_button_class="btn_global" overflow_button_width="28" overflow_button_text="&#187;">
    <Button class="btn_global" name="tb_new" text="New" width="64" height="28"/>
    <Button class="btn_global" name="tb_open" text="Open" width="64" height="28"/>
    <Line vertical="true" line_color="border_window" line_width="1" width="9" height="18" valign="center"/>
    <Button class="btn_global" name="tb_save" text="Save" width="64" height="28"/>
</ToolBar>
```

Related C++ APIs:
```cpp
// Callback invoked when an overflow item is activated (its argument is the name of the clicked child)
void AttachOverflowItemActivated(OverflowItemActivatedEvent callback);
void ClearOverflowItemActivated();

void SetOverflowEnabled(bool bEnabled);                // enable overflow collapsing (enabled by default)
bool IsOverflowEnabled() const;
void SetOverflowButtonClass(const DString& strClass);  // style Class of the "more" button
DString GetOverflowButtonClass() const;
void SetOverflowButtonWidth(int32_t nWidth);           // "more" button width (0 means auto)
int32_t GetOverflowButtonWidth() const;
void SetOverflowButtonText(const DString& strText);    // text of the "more" button
DString GetOverflowButtonText() const;

bool IsOverflowing() const;                            // whether any child is currently collapsed
```

## NavigationView (inherits HBox attributes) Attributes

NavigationView is a sidebar navigation container that combines "a left navigation pane + a right content area (page header + TabBox pages)" internally:
the pane holds a hamburger button, a pane title, a vertically scrollable nav item list and an optional bottom settings item;
clicking a nav item switches the content page by its `page` name and updates the page header;
the hamburger button toggles between expanded and compact states (compact mode shows icons only, hides group headers/separators,
and shows the full item text as a tooltip). It fits settings pages and desktop clients with "multiple pages + sidebar navigation".

| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| pane_width | 220 | int | Pane width in the expanded state (pixels, DPI-aware; only positive values are accepted, <= 0 keeps the default) |
| compact_pane_width | 48 | int | Pane width in the compact state (pixels, DPI-aware; only positive values are accepted, <= 0 keeps the default) |
| pane_title |  | string | Pane title text (hidden automatically in the compact state) |
| pane_title_id |  | string | Pane title text ID (localized; takes precedence over pane_title) |
| toggle_icon |  | string | Image attribute string of the hamburger button icon, e.g. file='nav/hamburger.svg' width='16' height='16' |
| collapsed | false | bool | Whether the initial state is compact (true = icons only) |
| show_header | true | bool | Whether to show the content-area page header (shows the current item text) |
| settings_item | false | bool | Whether to show a settings item at the bottom of the pane (does not scroll with the list, excluded from index counting) |
| settings_page | settings | string | Content page name associated with the settings item |
| settings_text_id |  | string | Settings item text ID (localized; provided by the user) |
| settings_icon |  | string | Image attribute string of the settings item icon (provided by the user) |
| selected_id |  | string | Page name of the initially selected item; when empty or not found, the first selectable item (excluding the settings item) is selected |

The NavigationView control inherits the `HBox` attributes. For more available attributes, please refer to the `HBox` attributes.

### Routing rules for direct children

NavigationView overrides `AddItem`; direct children are routed by type:

- `NavigationViewItem`: goes to the nav item list in the left pane (header/separator also enter the list, only their rendering differs);
- `TabBox`: taken over as the content-area page container (only one is allowed; providing one explicitly lets the application drive page switching directly);
- other controls: enter the content area as pages; a TabBox is created automatically when none is provided explicitly.

Nav items and content pages are linked by `name`: the `page` attribute of a `NavigationViewItem` is the `name` of the content page control;
selecting that nav item switches the content-area TabBox to the matching page and syncs the page header text.
**Index convention**: indices returned by APIs such as `GetSelectedIndex` count only selectable items of the `item` form
(header/separator and the bottom settings item are excluded); `Box::InvalidIndex` is returned when nothing is selected.

### Attributes of NavigationViewItem (inherits Control attributes)

`NavigationViewItem` is the nav item control, used as a direct child of NavigationView. It custom-draws an "icon + text" row and supports three forms:

- `item` (default): a clickable nav item; when selected it shows a gray rounded pill + accent-colored text;
- `header`: a group header (small, muted color, not clickable, hidden in the compact state);
- `separator`: a separator line (hidden in the compact state).

Each form uses a fixed height (DIP, DPI-aware): item 36, header 32, separator 9.
Keyboard navigation is supported: items of the `item` form can receive focus via `Tab`, the Up/Down arrow keys move focus cyclically
among selectable items, and Enter/Space activates the current item; the disabled state (`enabled="false"`) ignores clicks and keys,
dims the text and icon, and disabling the currently selected item clears its selection and the page header (no implicit switch to another item).

| Attribute name | Default value | Parameter type | Purpose |
| :--- | :--- | :--- | :--- |
| text |  | string | Item text |
| text_id |  | string | Item text ID (localized; takes precedence over text) |
| icon |  | string | Image attribute string, e.g. file='nav/home.svg' width='16' height='16' |
| page |  | string | Name of the associated content page control (linked to the content area) |
| item_type | item | string | Form: item (default, clickable nav item) / header (group header; group_header is also accepted) / separator (separator line; sep is also accepted) |
| icon_size | 16 | int | Icon size (DIP, DPI-aware; only positive values are accepted) |
| icon_left | 16 | int | Left margin of the icon in the expanded state (DIP) |
| text_left | 44 | int | Left margin of the text in the expanded state (DIP) |
| text_right | 8 | int | Right margin of the text in the expanded state (DIP) |
| pill_radius | 6 | int | Corner radius of the selected/hovered pill (DIP) |
| pill_inset_x | 6 | int | Horizontal inset of the pill (DIP) |
| pill_inset_y | 3 | int | Vertical inset of the pill (DIP) |
| selected_bg_color | bg_list_item_selected | string | Selected background color name |
| hovered_bg_color | bg_list_item_hovered | string | Hovered background color name |
| selected_text_color | color_accent | string | Selected text color name |
| normal_text_color | text_default | string | Normal text color name |
| disabled_text_color | text_disabled | string | Disabled text color name |
| header_text_color | text_muted | string | Group header text color name |
| separator_color | border_window | string | Separator line color name |

The NavigationViewItem control inherits the `Control` attributes. For more available attributes, please refer to the `Control` attributes.

### XML example

```xml
<NavigationView name="nav" height="stretch"
                pane_width="220" compact_pane_width="48"
                pane_title_id="STRID_NAV_DEMO_TITLE" show_header="true"
                toggle_icon="file='nav/hamburger.svg' width='16' height='16'"
                settings_item="true" settings_page="page_settings"
                settings_text_id="STRID_NAV_SETTINGS"
                settings_icon="file='nav/settings.svg' width='16' height='16'"
                selected_id="page_home">
    <!-- Clickable nav items: page matches the content page name, clicking switches the page -->
    <NavigationViewItem text_id="STRID_NAV_PAGE_HOME"
                        icon="file='nav/home.svg' width='16' height='16'"
                        page="page_home"/>
    <NavigationViewItem text_id="STRID_NAV_PAGE_DASHBOARD"
                        icon="file='nav/dashboard.svg' width='16' height='16'"
                        page="page_dashboard"/>

    <!-- Group header and separator: not clickable, excluded from index counting -->
    <NavigationViewItem item_type="header" text_id="STRID_NAV_GROUP_WORKSPACE"/>
    <NavigationViewItem item_type="separator"/>

    <!-- Content area: an explicit TabBox hosts all pages, so the application can drive the page container directly -->
    <TabBox name="nav_content">
        <VBox name="page_home" bkcolor="bg_window_main">
            <Label text_id="STRID_NAV_PAGE_HOME" font="system_bold_16" text_color="text_default"/>
        </VBox>
        <VBox name="page_dashboard" bkcolor="bg_window_main">
            <Label text_id="STRID_NAV_PAGE_DASHBOARD" font="system_bold_16" text_color="text_default"/>
        </VBox>
        <VBox name="page_settings" bkcolor="bg_window_main">
            <Label text_id="STRID_NAV_SETTINGS" font="system_bold_16" text_color="text_default"/>
        </VBox>
    </TabBox>
</NavigationView>
```

For a full demo, see the "Nav Test" button in the controls sample (`bin/resources/themes/default/controls/navigation.xml`):
11 clickable nav items, 2 group headers, 2 separators and a bottom settings item, an explicit TabBox with multiple pages,
plus demos of dynamically adding/removing nav items and programmatic selection.

### C++ example

```cpp
ui::NavigationView* pNav = dynamic_cast<ui::NavigationView*>(FindControl(_T("nav")));

//Event listeners (all fired with NavigationView itself as the sender)
//Selection changed: wParam = new index, lParam = old index (Box::InvalidIndex when nothing was selected)
pNav->AttachNavSelectionChanged([](const ui::EventArgs& args) {
    size_t nNewIndex = (size_t)args.wParam;
    size_t nOldIndex = (size_t)args.lParam;
    return true;
});
//Also fires when clicking the already-selected item (useful for "refresh/scroll to top"): wParam = selectable item index
pNav->AttachNavItemClick([](const ui::EventArgs& args) {
    size_t nIndex = (size_t)args.wParam;
    return true;
});
//Pane about to collapse/expand: wParam = target state (1=collapsed, 0=expanded); returning false cancels the toggle
pNav->AttachNavPaneToggling([](const ui::EventArgs& args) {
    return true;
});
//Pane collapse/expand completed: wParam = current state (1=collapsed, 0=expanded)
pNav->AttachNavPaneToggled([](const ui::EventArgs& args) {
    return true;
});

//Programmatic selection: by content page name / by selectable index / by control pointer
pNav->SelectItem(_T("page_tools"));
pNav->SelectItemByIndex(0);

//Read back the current selection
size_t nIndex = pNav->GetSelectedIndex();       //Box::InvalidIndex when nothing is selected
DString strPage = pNav->GetSelectedPage();

//Expand/collapse the navigation pane
pNav->SetCollapsed(!pNav->IsCollapsed());

//Dynamically append a nav item: when the 4th argument is a page control, it is attached to the content area
//and linked with the nav item by name
ui::VBox* pPage = new ui::VBox(pNav->GetWindow());
pPage->SetName(_T("page_dynamic"));
ui::NavigationViewItem* pItem = pNav->AddNavItem(DString(),
                                                 _T("file='nav/star.svg' width='16' height='16'"),
                                                 _T("page_dynamic"), pPage);
if (pItem != nullptr) {
    pItem->SetItemText(_T("Dynamic item"));
    pNav->SelectItem(pItem);   //select the new item immediately
}

//Remove a nav item: the 2nd argument true also removes the associated content page.
//Note: containers default to auto_destroy_child=true, so the pointer is invalid after removal
pNav->RemoveNavItem(pItem, true);
```

### Events

| Event | wParam | lParam | Description |
| :--- | :--- | :--- | :--- |
| kEventNavigationItemClick | Selectable item index |  | Fired when a selectable nav item is clicked; also fires when clicking the already-selected item |
| kEventNavigationSelectionChanged | New index | Old index | Fired when the selection changes; Box::InvalidIndex when nothing is selected |
| kEventNavigationPaneToggling | Target state: 1=collapsed, 0=expanded |  | Fired before the pane collapses/expands; returning false from the handler cancels the toggle |
| kEventNavigationPaneToggled | Current state: 1=collapsed, 0=expanded |  | Fired after the pane has collapsed/expanded |

Main C++ APIs:

```cpp
//Selection
bool SelectItem(NavigationViewItem* pItem, bool bFireEvent = true);  //select by control pointer
bool SelectItem(const DString& strPageName);                          //select by content page name
bool SelectItemByIndex(size_t nIndex);                                //select by selectable index
NavigationViewItem* GetSelectedItem() const;                          //current item (nullptr when nothing is selected)
size_t GetSelectedIndex() const;                                      //current index (Box::InvalidIndex when nothing is selected)
DString GetSelectedPage() const;                                      //content page name of the current item

//Pane state
void SetCollapsed(bool bCollapsed);   //expand/collapse the navigation pane
bool IsCollapsed() const;

//Event listeners (equivalent to AttachEvent; event parameters see the table above)
void AttachNavItemClick(const EventCallback& callback, EventCallbackID callbackID = 0);
void AttachNavSelectionChanged(const EventCallback& callback, EventCallbackID callbackID = 0);
void AttachNavPaneToggling(const EventCallback& callback, EventCallbackID callbackID = 0);  //return false to cancel
void AttachNavPaneToggled(const EventCallback& callback, EventCallbackID callbackID = 0);

//Dynamically add/remove nav items
NavigationViewItem* AddNavItem(const DString& strTextId, const DString& strIcon,
                               const DString& strPage, Control* pPage = nullptr);
bool AddPage(Control* pPage);                                            //add a page to the content area alone
bool RemoveNavItem(NavigationViewItem* pItem, bool bRemovePage = false); //also remove the associated content page
void RemoveAllNavItems();                                                //remove all nav items (content pages are kept)
void SetItemEnabled(NavigationViewItem* pItem, bool bEnabled);           //disabled items ignore clicks/keys
```

Common NavigationViewItem APIs:

```cpp
void SetItemText(const DString& strText);        DString GetItemText() const;
void SetItemTextId(const DString& strTextId);    DString GetItemTextId() const;
DString GetDisplayText() const;                  //resolves text_id to the current language text first
void SetIconAttr(const DString& strIcon);        DString GetIconAttr() const;
void SetPageName(const DString& strPage);        DString GetPageName() const;
void SetItemType(ItemType itemType);             ItemType GetItemType() const;  //kItem/kHeader/kSeparator
bool IsSelectable() const;                       //whether it can be clicked/selected (item form only)
void SetSelected(bool bSelected);                bool IsSelected() const;       //selection is managed by NavigationView
void SetCompact(bool bCompact);                  bool IsCompact() const;        //compact state is switched by NavigationView
```

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
