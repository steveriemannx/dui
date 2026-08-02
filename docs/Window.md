## Window Attributes
| Attribute Name | Attribute Category | Default Value | Parameter Type | Function in [Window.h](../include/duilib/Core/Window.h) | Purpose |
| :---              | :---    | :---    | :---   |:---                     |:--- |
| caption           | Title bar  | 0,0,0,0 | rect   | SetCaptionRect          |The margin of the draggable caption (title bar) area of the window; the last parameter is the distance from the top border, e.g. (0,0,0,36) |
| use_system_caption| Title bar  | false   | bool   | SetUseSystemCaption     |Sets whether to use the system title bar |
| snap_layout_menu  | Title bar  | true    | bool   | SetEnableSnapLayoutMenu |Whether to support showing the Snap Layouts menu (a Windows 11 feature: by hovering over the maximize button of a window or pressing Win + Z, you can easily access the Snap Layouts) |
| sys_menu          | Title bar  | true    | bool   | SetEnableSysMenu        |Whether the system window menu is shown when right-clicking the title bar (allows operations such as adjusting the window state and closing the window) |
| sys_menu_rect     | Title bar  | 0,0,0,0 | rect   | SetSysMenuRect          |The window menu area, at the top-left corner of the window title bar (double-clicking this area exits the window; clicking shows the system window menu); if not set in the XML, this feature is off by default |
| icon              | Title bar  |         | string | SetWindowIcon           |Sets the icon file path of the window; supports the ico format |
| text              | Title bar  |         | string | SetText                 |The window title string |
| textid            | Title bar  |         | string | SetTextId               |The ID of the window title string; the ID is specified in the multilingual file, e.g. (STRID_MIANWINDOW_TITLE) |
| drag_drop         | Drag & drop    | true    | bool   | SetEnableDragDrop       |Sets whether drag-and-drop operations are allowed (dragging in files and dragging in text)<br>When drag_drop="true", the controls in this window support dragging in files and text<br>When drag_drop="false", the controls in this window do not support dragging in files and text |
| shadow_attached   | Window shadow| true    | bool   | SetShadowAttached       |Whether the window has a shadow effect attached, e.g. (true) |
| shadow_type       | Window shadow|         | string | SetShadowType           |Sets the shadow type of the window:<br> "default": default shadow <br> "big": shadow (big), square corners, with border (suitable for normal windows)<br> "big_round": shadow (big), rounded corners, with border (suitable for normal windows)<br> "small": shadow (small), square corners, with border (suitable for normal windows)<br> "small_round": shadow (small), rounded corners, with border (suitable for normal windows)<br> "menu": shadow (small), square corners, with border (suitable for pop-up windows, such as menus)<br> "menu_round": shadow (small), rounded corners, with border (suitable for pop-up windows, such as menus)<br> "none": no shadow, square corners, with border<br> "none_round": no shadow, rounded corners, with border |
| shadow_image      | Window shadow|         | string | SetShadowImage          |Use a custom shadow image to replace the default shadow effect; pay attention to the relative path and the nine-grid (corner) attributes when setting the path, e.g. (file='../public/shadow/shadow_big.svg' corner='64,64,68,70') |
| shadow_corner     | Window shadow| 0,0,0,0 | rect   | SetShadowCorner         |After setting the shadow_image attribute, set this attribute to specify the nine-grid (corner) description of the shadow material |
| shadow_border_round| Window shadow| 0,0       | size  | SetShadowBorderRound  |After setting the shadow_image attribute, set this attribute to specify the corner radius of the shadow |
| shadow_border_color| Window shadow|"#FFA3A3A3"|string | SetShadowBorderColor  |Sets the border color of the window shadow |
| shadow_border_size | Window shadow|2          |int    | SetShadowBorderSize   |Sets the border size of the window shadow; the actually displayed border size is half of this value |
| shadow_snap        | Window shadow| true      | bool  | SetEnableShadowSnap   |Sets whether the shadow supports window edge snapping; if true, when the window is close to the screen edge, the shadow on that side is automatically hidden to increase the effective space in the view |
| size              | Window size| 0,0     | size   | SetInitSize             |The initial size of the window; supported formats: size="1200,800", or size="50%,50%", or size="1200,50%", size="50%,800"; the percentages refer to the percentage of the screen width or height |
| size_contain_shadow| Window size| false  | size   |                         |Whether the initial size of the window (the size attribute) includes the window shadow; by default it does not; the actual size of the window is the configured size value + the shadow size |
| min_size          | Window size| 0,0     | size   | SetWindowMinimumSize    |The minimum size of the window, e.g. (320,240) |
| max_size          | Window size| 0,0     | size   | SetWindowMaximumSize    |The maximum size of the window, e.g. (1600,1200) |
| size_box          | Window size| 0,0,0,0 | rect   | SetSizeBox              |The margin of the window that can be dragged to change the window size, e.g. (4,4,6,6) |
| round_corner      | Window shape| 0,0     | size   | SetRoundCorner          |The corner radius of the window, e.g. (4,4) |
| alpha_fix_corner  | Window shape|14,14,14,14| rect | SetAlphaFixCorner       |The transparent channel repair range of the window's rounded corners |
| layered_window    | Window drawing| false   | bool   | SetLayeredWindow        |Sets whether it is a layered window |
| alpha             | Window drawing| 255     | int    | SetLayeredWindowAlpha   |Sets the transparency value [0, 255]; when alpha is 0, the window is completely transparent; when alpha is 255, the window is opaque.<br>Only valid when layered_window="true";<br>this parameter is used as an argument in the UpdateLayeredWindow function (BLENDFUNCTION.SourceConstantAlpha) |
| opacity           | Window drawing| 255     | int    | SetLayeredWindowOpacity |Sets the opacity value [0, 255]; when opacity is 0, the window is completely transparent; when opacity is 255, the window is opaque.<br>Only valid when IsLayeredWindow() is true; so if the current window is not a layered window, it is automatically set as a layered window internally <br>This parameter is used as an argument in the SetLayeredWindowAttributes function (bAlpha) |
| render_backend_type| Window drawing| "CPU"   | string |SetRenderBackendType     | "CPU": CPU drawing <br> "GL": OpenGL drawing <br> Notes: <br> (1) Within one thread, only one window is allowed to use OpenGL drawing; otherwise the program may crash <br> (2) Windows drawn with OpenGL cannot be layered windows (i.e. windows with the WS_EX_LAYERED attribute)<br> (3) For windows using OpenGL, each draw paints the entire window; partial drawing is not supported, so performance is not necessarily better than drawing with the CPU |

Note: for the parsing function of the window attributes, see: [WindowBuilder::ParseWindowAttributes function](../src/Core/WindowBuilder.cpp)    
Note: the tag name of the window in the XML is "Window"     
Usage example:    
```xml
<Window size="75%,90%" min_size="80,50" use_system_caption="false" caption="0,0,0,36"
        shadow_attached="true" layered_window="true" alpha="255" size_box="4,4,4,4">
</Window>
```
