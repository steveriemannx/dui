## Project documentation links

* [Project Overview](../README.md)
* [Quick Start](Getting-Started.md)
* [Example Programs](Examples.md)
* [Global Styles: Fonts, Colors, Images and Other Resources](Global.md)
* [Window Styles](Window.md)
* [Box Styles](Box.md)
* [Control Styles](Control.md)
* [Menu Styles](Menu.md)
* [Control Events/Messages](Events.md)
* [XML Node Names of Controls](XmlNode.md)
* [Responding to Control Events Directly in XML](XmlEvents.md)
* [CEF Control](CEF.md)
* [Build-related Documents and Scripts](../scripts/build.md)

## Summary table of the main classes in the project
For detailed interface descriptions of each class, please refer to the associated header files, which contain detailed comments.    
* Basic data types

| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| UiSize | [duilib/Core/UiSize.h](../duilib/Core/UiSize.h) | Wrapper of the 32-bit Size type|
| UiSize64 | [duilib/Core/UiSize64.h](../duilib/Core/UiSize64.h) | Wrapper of the 64-bit Size type|
| UiPoint | [duilib/Core/UiPoint.h](../duilib/Core/UiPoint.h) | Wrapper of the integer Point type|
| UiPointF | [duilib/Core/UiPointF.h](../duilib/Core/UiPointF.h) | Wrapper of the floating-point Point type|
| UiFixedInt | [duilib/Core/UiFixedInt.h](../duilib/Core/UiFixedInt.h) | Wrapper of the numeric type for the length (or width) set on a control|
| UiEstInt | [duilib/Core/UiEstInt.h](../duilib/Core/UiEstInt.h) | Wrapper of the numeric type for the estimated length (or width) of a control|
| UiFixedSize | [duilib/Core/UiTypes.h](../duilib/Core/UiTypes.h) | The set control size|
| UiEstSize | [duilib/Core/UiTypes.h](../duilib/Core/UiTypes.h) | The estimated control size (unlike UiFixedSize, it has no Auto type)|
| UiEstResult | [duilib/Core/UiTypes.h](../duilib/Core/UiTypes.h) | The result of estimating the control size|
| UiPadding | [duilib/Core/UiPadding.h](../duilib/Core/UiPadding.h) | Wrapper of the padding type|
| UiMargin | [duilib/Core/UiMargin.h](../duilib/Core/UiMargin.h) | Wrapper of the margin type|
| UiString | [duilib/Core/UiString.h](../duilib/Core/UiString.h) | The string type used by controls, replacing std::wstring to reduce the memory usage of controls. This class is suitable for storing strings with low memory usage; in terms of performance, due to more string copying, its performance is relatively weak|
| UiRect | [duilib/Core/UiRect.h](../duilib/Core/UiRect.h) | Wrapper of the 32-bit Rect type|
| UiFont | [duilib/Core/UiFont.h](../duilib/Core/UiFont.h) | Wrapper of the font type|
| UiColor | [duilib/Core/UiColor.h](../duilib/Core/UiColor.h) | Wrapper of the color type|
| UiColors | [duilib/Core/UiColors.h](../duilib/Core/UiColors.h) | Constants of common color values (ARGB format)|

* Window related

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| Window | [duilib/Core/Window.h](../duilib/Core/Window.h) | Wrapper of the core duilib window|
| WindowBuilder | [duilib/Core/WindowBuilder.h](../duilib/Core/WindowBuilder.h) | Parses XML files and creates windows, container layouts and controls, etc.|
| Shadow | [duilib/Core/Shadow.h](../duilib/Core/Shadow.h) | Window shadow attributes|
| WindowImplBase | [duilib/Utils/WinImplBase.h](../duilib/Utils/WinImplBase.h) | The window base class provided to the application layer. Implements a window with a title bar, with support for maximize, minimize and restore buttons, and provides window shadows with the WS_EX_LAYERED attribute|
| ShadowWnd | [duilib/Utils/ShadowWnd.h](../duilib/Utils/ShadowWnd.h) | A subclass of WinImplBase, a base class implemented with attached shadow windows. Implements creating and attaching a window, and provides window shadows without the WS_EX_LAYERED attribute|
| EventType | [duilib/duilib_defs.h](../duilib/duilib_defs.h) | Defines all message types|
| EventArgs | [duilib/Core/EventArgs.h](../duilib/Core/EventArgs.h) | Parameters of window messages and control event notifications|
| WindowDropTarget | [duilib/Core/WindowDropTarget.h](../duilib/Core/WindowDropTarget.h) | Drag-and-drop support for controls in a window|
| Windows version compatibility | [duilib/duilib_config.h](../duilib/duilib_config.h) | Windows version compatibility definitions; the default configuration supports Win7 and above|

* Base classes of controls and containers

| Class Name | Base Class Name |Associated Header File| Purpose |
| :--- | :--- |:--- | :--- |
| PlaceHolder |nbase::SupportWeakCallback| [duilib/Core/PlaceHolder.h](../duilib/Core/PlaceHolder.h) | The top-level base class of all controls and containers, encapsulating basic data such as the position, size and appearance of a control|
| Control | PlaceHolder | [duilib/Core/Control.h](../duilib/Core/Control.h) | A basic control and the base class of all controls and containers, including basic functionality such as the position, size, state, color, image resources, animation, drawing, cursor, mouse, keyboard, focus and shortcut operations|
| Box | Control| [duilib/Core/Box.h](../duilib/Core/Box.h) | The base class of all containers, encapsulating the position, size, appearance and child control management (add, delete, modify, draw, operate, drag-and-drop, layout, etc.) of a container|

* Layout

| Class Name | Base Class Name |Associated Header File| Purpose |
| :--- | :--- |:--- | :--- |
| Layout | |[duilib/Layout/Layout.h](../duilib/Layout/Layout.h) | Floating layout|
| HLayout |Layout |[duilib/Layout/HLayout.h](../duilib/Layout/HLayout.h) | Horizontal layout|
| VLayout |Layout |[duilib/Layout/VLayout.h](../duilib/Layout/VLayout.h) | Vertical layout|
| HFlowLayout |Layout |[duilib/Layout/HFlowLayout.h](../duilib/Layout/HFlowLayout.h) | Horizontal flow layout|
| VFlowLayout |Layout |[duilib/Layout/VFlowLayout.h](../duilib/Layout/VFlowLayout.h) | Vertical flow layout|
| HTileLayout |Layout| [duilib/Layout/HTileLayout.h](../duilib/Layout/HTileLayout.h) | Horizontal tile layout|
| VTileLayout |Layout| [duilib/Layout/VTileLayout.h](../duilib/Layout/VTileLayout.h) | Vertical tile layout|
| VirtualHLayout |HLayout| [duilib/Layout/VirtualHLayout.h](../duilib/Layout/VirtualHLayout.h) | Virtual list horizontal layout|
| VirtualVLayout |VLayout| [duilib/Layout/VirtualVLayout.h](../duilib/Layout/VirtualVLayout.h) | Virtual list vertical layout|
| VirtualHTileLayout |HTileLayout| [duilib/Layout/VirtualHTileLayout.h](../duilib/Layout/VirtualHTileLayout.h) | Virtual list horizontal tile layout|
| VirtualVTileLayout |VTileLayout| [duilib/Layout/VirtualVTileLayout.h](../duilib/Layout/VirtualVTileLayout.h) | Virtual list vertical tile layout|
| GridLayout |Layout |[duilib/Layout/GridLayout.h](../duilib/Layout/GridLayout.h) | Grid layout|

* Containers

| Class Name | Base Class Name |Layout Type|Associated Header File| Name / Purpose |
| :--- | :--- |:--- |:--- | :--- |
| Box | Control|[Layout](../duilib/Layout/Layout.h)|[duilib/Core/Box.h](../duilib/Core/Box.h) | Free-layout container, the base class of all containers, encapsulating the position, size, appearance and child control management (add, delete, modify, draw, operate, drag-and-drop, layout, etc.) of a container|
| VBox | Box|[VLayout](../duilib/Layout/VLayout.h)| [duilib/Box/VBox.h](../duilib/Box/VBox.h) | Vertical layout container |
| HBox | Box|[HLayout](../duilib/Layout/HLayout.h)|[duilib/Box/HBox.h](../duilib/Box/HBox.h) | Horizontal layout container |
| VFlowBox | Box|[VFlowLayout](../duilib/Layout/VFlowLayout.h)| [duilib/Box/VBox.h](../duilib/Box/VBox.h) | Vertical flow layout container |
| HFlowBox | Box|[HFlowLayout](../duilib/Layout/HFlowLayout.h)|[duilib/Box/HBox.h](../duilib/Box/HBox.h) | Horizontal flow layout container |
| TabBox | Box|[Layout](../duilib/Layout/Layout.h)|[duilib/Box/TabBox.h](../duilib/Box/TabBox.h) | Multi-tab layout container |
| VTileBox | Box|[VTileLayout](../duilib/Layout/VTileLayout.h)|[duilib/Box/TileBox.h](../duilib/Box/TileBox.h) | Vertical tile layout container|
| HTileBox | Box|[HTileLayout](../duilib/Layout/HTileLayout.h)|[duilib/Box/TileBox.h](../duilib/Box/TileBox.h) | Horizontal tile layout container|
| ScrollBox | Box|[Layout](../duilib/Layout/Layout.h)|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Scrollable free-layout container with a vertical or horizontal scrollbar|
| VScrollBox | ScrollBox|[VLayout](../duilib/Layout/VLayout.h)|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Scrollable vertical-layout container with a vertical or horizontal scrollbar|
| HScrollBox | ScrollBox|[HLayout](../duilib/Layout/HLayout.h)|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Scrollable horizontal-layout container with a vertical or horizontal scrollbar|
| VTileScrollBox |ScrollBox|[VTileLayout](../duilib/Layout/VTileLayout.h)|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Scrollable vertical tile layout container with a vertical or horizontal scrollbar|
| HTileScrollBox |ScrollBox|[HTileLayout](../duilib/Layout/HTileLayout.h)|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Scrollable horizontal tile layout container with a vertical or horizontal scrollbar|
| ListBox | ScrollBox|[Layout](../duilib/Layout/Layout.h)|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | List container with free layout|
| VListBox | ListBox|[VLayout](../duilib/Layout/VLayout.h)|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | List container with vertical layout|
| HListBox | ListBox|[HLayout](../duilib/Layout/HLayout.h)|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | List container with horizontal layout|
| VTileListBox | ListBox|[VTileLayout](../duilib/Layout/VTileLayout.h)|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | List container with vertical tile layout|
| HTileListBox | ListBox|[HTileLayout](../duilib/Layout/HTileLayout.h)|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | List container with horizontal layout|
| VirtualListBox | ListBox|[Layout](../duilib/Layout/Layout.h)|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Free-layout list container implemented with a virtual list|
| VirtualVListBox | VirtualListBox|[VirtualVLayout](../duilib/Layout/VirtualVLayout.h)|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Vertical-layout list container implemented with a virtual list|
| VirtualHListBox | VirtualListBox|[VirtualHLayout](../duilib/Layout/VirtualHLayout.h)|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Horizontal-layout list container implemented with a virtual list|
| VirtualVTileListBox | VirtualListBox|[VirtualVTileLayout](../duilib/Layout/VirtualVTileLayout.h)|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Vertical tile layout list container implemented with a virtual list|
| VirtualHTileListBox | VirtualListBox|[VirtualHTileLayout](../duilib/Layout/VirtualHTileLayout.h)|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Horizontal tile layout list container implemented with a virtual list|
| GridBox | Box|[GridLayout](../duilib/Layout/GridLayout.h)|[duilib/Box/GridBox.h](../duilib/Box/GridBox.h) | Grid layout container |
| GridScrollBox | ScrollBox|[GridLayout](../duilib/Layout/GridLayout.h)|[duilib/Box/GridBox.h](../duilib/Box/GridBox.h) | Grid layout container (with scrollbar support) |

* Images

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| Image | [duilib/Image/Image.h](../duilib/Image/Image.h) | Image-related wrapper. Supported file formats: SVG/PNG/GIF/JPG/BMP/APNG/WEBP/ICO/Lottie-JSON/PAG|
| ImageAttribute | [duilib/Image/ImageAttribute.h](../duilib/Image/ImageAttribute.h) | Image attributes|
| ImageLoadParam | [duilib/Image/ImageLoadParam.h](../duilib/Image/ImageLoadParam.h) | Image loading parameters, used to load an image|
| ImageInfo | [duilib/Image/ImageInfo.h](../duilib/Image/ImageInfo.h) | Image information|
| ImageDecoder | [duilib/Image/ImageDecoder.h](../duilib/Image/ImageDecoder.h) | Interface supporting multi-threaded decoding (used for low-level decoding; supports deferred decoding and decoding on multiple threads, avoiding UI freezes caused by decoding images on the UI thread)|
| ImagePlayer | [duilib/Image/ImagePlayer.h](../duilib/Image/ImagePlayer.h) | Logic wrapper for playing image animations on controls (supports GIF/WebP/APNG/Lottie-JSON/PAG animations)|
| StateImage | [duilib/Image/StateImage.h](../duilib/Image/StateImage.h) | Mapping of control states to images|
| StateImageMap | [duilib/Image/StateImageMap.h](../duilib/Image/StateImageMap.h) | Mapping of control image types to state images|
| ImageManager | [duilib/Core/ImageManager.h](../duilib/Core/ImageManager.h) | Image resource manager|
| IconManager | [duilib/Core/IconManager.h](../duilib/Core/IconManager.h) | Icon resource manager (thread-safe, suitable for small icon-type image resources), supports HICON handles on Windows|
| ImageList | [duilib/Core/ImageList.h](../duilib/Core/ImageList.h) | Image list|

* Animation

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| AnimationManager | [duilib/Animation/AnimationManager.h](../duilib/Animation/AnimationManager.h) | Image animation manager|
| AnimationPlayer | [duilib/Animation/AnimationPlayer.h](../duilib/Animation/AnimationPlayer.h) | Image animation playback state management|

* Colors

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| UiColor | [duilib/Core/UiColor.h](../duilib/Core/UiColor.h) | Wrapper of the color type|
| UiColors | [duilib/Core/UiColors.h](../duilib/Core/UiColors.h) | Constants of common color values (ARGB format)|
| StateColorMap | [duilib/Core/StateColorMap.h](../duilib/Core/UiColors.h) | Mapping of control states to color values|

* Fonts

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| UiFont | [duilib/Core/UiFont.h](../duilib/Core/UiFont.h) | Wrapper of the font type|
| FontManager | [duilib/Core/FontManager.h](../duilib/Core/FontManager.h) | Font manager|

* Rendering engine interfaces

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| IRenderFactory | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Rendering factory interface, used to create rendering implementation objects such as Font, Pen, Brush, Path, Matrix, Bitmap and Render |
| IFont | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Font interface |
| IBitmap | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Bitmap interface |
| IPen | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Pen interface |
| IBrush | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Brush interface |
| IPath | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Path interface |
| IMatrix | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Matrix interface |
| IRender | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Rendering interface, used for drawing images, text, etc. |

* Skia rendering engine

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| RenderFactory_Skia | [duilib/RenderSkia/RenderFactory_Skia.h](../duilib/RenderSkia/RenderFactory_Skia.h) | Implementation of the rendering factory interface |
| Font_Skia | [duilib/RenderSkia/Font_Skia.h](../duilib/RenderSkia/Font_Skia.h) | Implementation of the font interface |
| Bitmap_Skia | [duilib/RenderSkia/Bitmap_Skia.h](../duilib/RenderSkia/Bitmap_Skia.h) | Implementation of the bitmap interface |
| Pen_Skia | [duilib/RenderSkia/Pen_Skia.h](../duilib/RenderSkia/Pen_Skia.h) | Implementation of the pen interface |
| Brush_Skia | [duilib/RenderSkia/Brush_Skia.h](../duilib/RenderSkia/Brush_Skia.h) | Implementation of the brush interface |
| Path_Skia | [duilib/RenderSkia/Path_Skia.h](../duilib/RenderSkia/Path_Skia.h) | Implementation of the path interface |
| Matrix_Skia | [duilib/RenderSkia/Matrix_Skia.h](../duilib/RenderSkia/Matrix_Skia.h) | Implementation of the matrix interface |
| FontMgr_Skia | [duilib/RenderSkia/FontMgr_Skia.h](../duilib/RenderSkia/FontMgr_Skia.h) | Interface implementation of the font manager |
| Render_Skia | [duilib/RenderSkia/Render_Skia.h](../duilib/RenderSkia/Render_Skia.h) | Implementation of the rendering interface, used for drawing images, text, etc. |
| Render_Skia_Windows | [duilib/RenderSkia/Render_Skia_Windows.h](../duilib/RenderSkia/Render_Skia_Windows.h) | Implementation of the Windows-related functionality of the rendering interface |
| Render_Skia_SDL | [duilib/RenderSkia/Render_Skia_SDL.h](../duilib/RenderSkia/Render_Skia_SDL.h) | Implementation of the SDL-related functionality of the rendering interface, mainly used on Linux; Windows is also supported |

* Controls / functional components

| Class Name / Component | Base Class | Associated Header File| Purpose |
| :--- | :--- | :---| :--- |
| ScrollBar | Control| [duilib/Core/ScrollBar.h](../duilib/Core/ScrollBar.h) | Scrollbar control |
| Label | Control| [duilib/Control/Label.h](../duilib/Control/Label.h) | Label control (template), used to display text |
| LabelBox | Box| [duilib/Control/Label.h](../duilib/Control/Label.h) | Label container (template), used to display text |
| Button | Control| [duilib/Control/Button.h](../duilib/Control/Button.h) | Button control (template implementation) |
| ButtonBox | Box| [duilib/Control/Button.h](../duilib/Control/Button.h) | Button container control (template implementation) |
| CheckBox | Control| [duilib/Control/CheckBox.h](../duilib/Control/CheckBox.h) | Checkbox control (template implementation) |
| CheckBoxBox | Box| [duilib/Control/CheckBox.h](../duilib/Control/CheckBox.h) | Checkbox container (template implementation) |
| Option | Control| [duilib/Control/Option.h](../duilib/Control/Option.h) | Radio button control|
| OptionBox | Box| [duilib/Control/Option.h](../duilib/Control/Option.h) | Radio button container|
| GroupBox | Box| [duilib/Control/GroupBox.h](../duilib/Control/GroupBox.h) | Group container (template) |
| GroupVBox | VBox| [duilib/Control/GroupBox.h](../duilib/Control/GroupBox.h) | Vertical group container (template) |
| GroupHBox | HBox| [duilib/Control/GroupBox.h](../duilib/Control/GroupBox.h) | Horizontal group container (template) |
| Combo | Box| [duilib/Control/Combo.h](../duilib/Control/Combo.h) | Combo box |
| ComboButton | Box| [duilib/Control/ComboButton.h](../duilib/Control/ComboButton.h) | Button with a drop-down combo box |
| CheckCombo | Control| [duilib/Control/CheckCombo.h](../duilib/Control/CheckCombo.h) | Combo box with checkboxes |
| FilterCombo | Combo| [duilib/Control/FilterCombo.h](../duilib/Control/FilterCombo.h) | Combo box with filtering |
| DateTime | Label| [duilib/Control/DateTime.h](../duilib/Control/DateTime.h) | Date-time picker control |
| HotKey | HBox| [duilib/Control/HotKey.h](../duilib/Control/HotKey.h) | Hotkey control |
| HyperLink | Label| [duilib/Control/HyperLink.h](../duilib/Control/HyperLink.h) | Text with a hyperlink; if the URL is empty, it can be used as an ordinary text button |
| IPAddress | HBox| [duilib/Control/IPAddress.h](../duilib/Control/IPAddress.h) | IP address control |
| Line | Control| [duilib/Control/Line.h](../duilib/Control/Line.h) | Line drawing control |
| Menu | WindowImplBase| [duilib/Control/Menu.h](../duilib/Control/Menu.h) | Menu, an independent window |
| Progress | Label| [duilib/Control/Progress.h](../duilib/Control/Progress.h) | Progress bar control |
| Slider | Progress| [duilib/Control/Slider.h](../duilib/Control/Slider.h) | Slider control |
| CircleProgress | Control| [duilib/Control/CircleProgress.h](../duilib/Control/CircleProgress.h) | Circular progress bar |
| RichEdit | ScrollBox| [duilib/Control/RichEdit.h](../duilib/Control/RichEdit.h) | Rich text edit control |
| RichEdit implementation classes | | [duilib/Control/RichEditCtrl_Windows.h](../duilib/Control/RichEditCtrl_Windows.h) | Core functionality wrapper of the rich text edit control (Windows) |
| RichEdit implementation classes | | [duilib/Control/RichEditHost_Windows.h](../duilib/Control/RichEditHost_Windows.h) | Core functionality implementation of the rich text edit control (Windows) |
| RichEdit implementation classes | | [duilib/Control/RichEdit_SDL.h](../duilib/Control/RichEdit_SDL.h) | Core functionality wrapper of the rich text edit control (SDL) |
| RichText | Control| [duilib/Control/RichText.h](../duilib/Control/RichText.h) | Formatted text (HTML-like format) |
| Split | Control| [duilib/Control/Split.h](../duilib/Control/Split.h) | Splitter control |
| SplitBox | Box| [duilib/Control/Split.h](../duilib/Control/Split.h) | Splitter container |
| TabCtrl | ListBox| [duilib/Control/TabCtrl.h](../duilib/Control/TabCtrl.h) | Multi-tab control (similar to browser tabs) |
| TreeView | ListBox| [duilib/Control/TreeView.h](../duilib/Control/TreeView.h) | Tree control |
| TreeNode | ListBoxItem| [duilib/Control/TreeView.h](../duilib/Control/TreeView.h) | Node of the tree control |
| DirectoryTree | TreeView| [duilib/Control/DirectoryTree.h](../duilib/Control/DirectoryTree.h) | Directory tree control, used to display the directory structure of the file system |
| ListCtrl | VBox| [duilib/Control/ListCtrl.h](../duilib/Control/ListCtrl.h) | List control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlDefs.h](../duilib/Control/ListCtrlDefs.h) | Basic type definitions of the list control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlHeader.h](../duilib/Control/ListCtrlHeader.h) | Header of the list control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlHeaderItem.h](../duilib/Control/ListCtrlHeaderItem.h) | Header items of the list control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlItem.h](../duilib/Control/ListCtrlItem.h) | Data items of the list control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlSubItem.h](../duilib/Control/ListCtrlSubItem.h) | Sub-items of the data items of the list control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlView.h](../duilib/Control/ListCtrlView.h) | View base class of the list control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlReportView.h](../duilib/Control/ListCtrlReportView.h) | Report view of the list control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlIconView.h](../duilib/Control/ListCtrlIconView.h) | Icon/List view of the list control |
| ListCtrl implementation classes | | [duilib/Control/ListCtrlData.h](../duilib/Control/ListCtrlData.h) | Data manager of the list control |
| PropertyGrid | VBox| [duilib/Control/PropertyGrid.h](../duilib/Control/PropertyGrid.h) | Property table control, supporting properties such as text, number, checkbox, font, color, date, IP address, hotkey, file path and folder |
| ColorPicker | WindowImplBase| [duilib/Control/ColorPicker.h](../duilib/Control/ColorPicker.h) | Color picker, an independent window |
| Color picker implementation classes | | [duilib/Control/ColorControl.h](../duilib/Control/ColorControl.h) | Implementation class of ColorPicker, a custom color control |
| Color picker implementation classes | | [duilib/Control/ColorConvert.h](../duilib/Control/ColorConvert.h) | Implementation class of ColorPicker, a color type (RGB/HSV/HSL) conversion class |
| Color picker implementation classes | | [duilib/Control/ColorPickerCustom.h](../duilib/Control/ColorPickerCustom.h) | Implementation class of ColorPicker, custom colors |
| Color picker implementation classes | | [duilib/Control/ColorPickerRegular.h](../duilib/Control/ColorPickerRegular.h) | Implementation class of ColorPicker, common colors |
| Color picker implementation classes | | [duilib/Control/ColorPickerStatard.h](../duilib/Control/ColorPickerStatard.h) | Implementation class of ColorPicker, standard colors |
| Color picker implementation classes | | [duilib/Control/ColorPickerStatardGray.h](../duilib/Control/ColorPickerStatardGray.h) | Implementation class of ColorPicker, standard colors, gray |
| Color picker implementation classes | | [duilib/Control/ColorSlider.h](../duilib/Control/ColorSlider.h) | Implementation class of ColorPicker |
| ControlDragable | Control | [duilib/Control/ControlDragable.h](../duilib/Control/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| BoxDragable | Box | [duilib/Control/ControlDragable.h](../duilib/Control/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| HBoxDragable | HBox | [duilib/Control/ControlDragable.h](../duilib/Control/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| VBoxDragable | VBoxDragable | [duilib/Control/ControlDragable.h](../duilib/Control/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| IconControl | Control | [duilib/Control/IconControl.h](../duilib/Control/IconControl.h) | Control used to display icons; if no icon data is set, it is compatible with all the functionality of the base class Control|
| AddressBar | HBox | [duilib/Control/AddressBar.h](../duilib/Control/AddressBar.h) | Address bar control, used to display paths of the local file system |

* Global resources

| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| GlobalManager | [duilib/Core/GlobalManager.h](../duilib/Core/GlobalManager.h) | Global attribute management utility class, used to manage various global attributes, including global styles (global.xml) and language settings |
| IRenderFactory | [duilib/Render/IRender.h](../duilib/Render/IRender.h) | Management class of the rendering interface, used to create rendering implementation objects such as Font, Pen, Brush, Path, Matrix, Bitmap and Render |
| FontManager | [duilib/Core/FontManager.h](../duilib/Core/FontManager.h) | Management class of fonts |
| ImageManager | [duilib/Core/ImageManager.h](../duilib/Core/ImageManager.h) | Management class of images |
| IconManager | [duilib/Core/IconManager.h](../duilib/Core/IconManager.h) | Icon resource manager (thread-safe, suitable for small icon-type image resources), supports HICON handles on Windows |
| ZipManager | [duilib/Core/ZipManager.h](../duilib/Core/ZipManager.h) | ZIP archive manager |
| DpiManager | [duilib/Core/DpiManager.h](../duilib/Core/DpiManager.h) | DPI manager, used to support features such as DPI adaptation |
| TimerManager | [duilib/Core/TimerManager.h](../duilib/Core/TimerManager.h) | Timer manager |
| LangManager | [duilib/Core/LangManager.h](../duilib/Core/LangManager.h) | Multi-language support manager |
| CursorManager | [duilib/Core/CursorManager.h](../duilib/Core/CursorManager.h) | Cursor manager |
| ThreadManager | [duilib/Core/ThreadManager.h](../duilib/Core/ThreadManager.h) | Thread manager |
| ColorManager | [duilib/Core/ColorManager.h](../duilib/Core/ColorManager.h) | Color manager |
| WindowManager | [duilib/Core/WindowManager.h](../duilib/Core/WindowManager.h) | Window manager |
| ImageDecoderFactory | [duilib/Image/ImageDecoderFactory.h](../duilib/Image/ImageDecoderFactory.h) | Management of image decoders |

* libcef control wrapper related

| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| CefManager | [duilib/CEFControl/CefManager.h](../duilib/CEFControl/CefManager.h) | CEF control manager, responsible for the initialization and uninitialization of the CEF module |
| CefControl | [duilib/CEFControl/CefControl.h](../duilib/CEFControl/CefControl.h) | CEF control interface, providing basic web browsing functionality and event acceptance |
| CefControlEvent | [duilib/CEFControl/CefControlEvent.h](../duilib/CEFControl/CefControlEvent.h) | Event reception interface for web browsing of the CEF control |
| CefControlNative | [duilib/CEFControl/CefControlNative.h](../duilib/CEFControl/CefControlNative.h) | Wrapper of the windowed mode of the CEF control |
| CefControlOffScreen | [duilib/CEFControl/CefControlOffScreen.h](../duilib/CEFControl/CefControlOffScreen.h) | Wrapper of the off-screen rendering mode of the CEF control |
