## Project documentation links

* [Project Overview](../README.md)
* [Quick Start](Getting-Started.md)
* [Example Programs](Examples.md)
* [Three Development Modes](ThreeModes.md)
* [Global Styles: Fonts, Colors, Images and Other Resources](Global.md)
* [Window Styles](Window.md)
* [Box Styles](Box.md)
* [Control Styles](Control.md)
* [Menu Styles](Menu.md)
* [Control Events/Messages](Events.md)
* [XML Node Names of Controls](XmlNode.md)
* [Responding to Control Events Directly in XML](XmlEvents.md)
* [CEF Control](CEF.md)
* [Build Process per Platform](Build.md)
* [Build-related Documents and Scripts](../scripts/build.md)
* [Third-party Libraries Used](ThirdParty.md)
* [History: Modifications Based on NIM_Duilib_Framework](History.md)
* [AI-Assisted Development (Claude Code)](ClaudeCode.md)

## Summary table of the main classes in the project
For detailed interface descriptions of each class, please refer to the associated header files, which contain detailed comments.    
* Basic data types

| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| UiSize | [include/dui/Core/UiSize.h](../include/dui/Core/UiSize.h) | Wrapper of the 32-bit Size type|
| UiSize64 | [include/dui/Core/UiSize64.h](../include/dui/Core/UiSize64.h) | Wrapper of the 64-bit Size type|
| UiPoint | [include/dui/Core/UiPoint.h](../include/dui/Core/UiPoint.h) | Wrapper of the integer Point type|
| UiPointF | [include/dui/Core/UiPointF.h](../include/dui/Core/UiPointF.h) | Wrapper of the floating-point Point type|
| UiFixedInt | [include/dui/Core/UiFixedInt.h](../include/dui/Core/UiFixedInt.h) | Wrapper of the numeric type for the length (or width) set on a control|
| UiEstInt | [include/dui/Core/UiEstInt.h](../include/dui/Core/UiEstInt.h) | Wrapper of the numeric type for the estimated length (or width) of a control|
| UiFixedSize | [include/dui/Core/UiTypes.h](../include/dui/Core/UiTypes.h) | The set control size|
| UiEstSize | [include/dui/Core/UiTypes.h](../include/dui/Core/UiTypes.h) | The estimated control size (unlike UiFixedSize, it has no Auto type)|
| UiEstResult | [include/dui/Core/UiTypes.h](../include/dui/Core/UiTypes.h) | The result of estimating the control size|
| UiPadding | [include/dui/Core/UiPadding.h](../include/dui/Core/UiPadding.h) | Wrapper of the padding type|
| UiMargin | [include/dui/Core/UiMargin.h](../include/dui/Core/UiMargin.h) | Wrapper of the margin type|
| UiString | [include/dui/Core/UiString.h](../include/dui/Core/UiString.h) | A string type for reducing memory usage in controls, replacing DString (the standard string type used by the library) where memory is critical. This class is suitable for storing strings with low memory usage; in terms of performance, due to more string copying, its performance is relatively weak|
| UiRect | [include/dui/Core/UiRect.h](../include/dui/Core/UiRect.h) | Wrapper of the 32-bit Rect type|
| UiFont | [include/dui/Core/UiFont.h](../include/dui/Core/UiFont.h) | Wrapper of the font type|
| UiColor | [include/dui/Core/UiColor.h](../include/dui/Core/UiColor.h) | Wrapper of the color type|
| UiColors | [include/dui/Core/UiColors.h](../include/dui/Core/UiColors.h) | Constants of common color values (ARGB format)|

* Window related

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| Window | [include/dui/Core/Window.h](../include/dui/Core/Window.h) | Wrapper of the core dui window|
| WindowBuilder | [include/dui/Core/WindowBuilder.h](../include/dui/Core/WindowBuilder.h) | Parses XML files and creates windows, container layouts and controls, etc.|
| Shadow | [include/dui/Core/Shadow.h](../include/dui/Core/Shadow.h) | Window shadow attributes|
| WindowImplBase | [include/dui/Utils/WinImplBase.h](../include/dui/Utils/WinImplBase.h) | The window base class provided to the application layer. Implements a window with a title bar, with support for maximize, minimize and restore buttons, and provides window shadows with the WS_EX_LAYERED attribute|
| ShadowWnd | [include/dui/Utils/ShadowWnd.h](../include/dui/Utils/ShadowWnd.h) | A subclass of WinImplBase, a base class implemented with attached shadow windows. Implements creating and attaching a window, and provides window shadows without the WS_EX_LAYERED attribute|
| EventType | [include/dui/dui_defs.h](../include/dui/dui_defs.h) | Defines all message types|
| EventArgs | [include/dui/Core/EventArgs.h](../include/dui/Core/EventArgs.h) | Parameters of window messages and control event notifications|
| Windows version compatibility | [include/dui/dui_config.h](../include/dui/dui_config.h) | Windows version compatibility definitions; the default configuration supports Win7 and above|

* Base classes of controls and containers

| Class Name | Base Class Name |Associated Header File| Purpose |
| :--- | :--- |:--- | :--- |
| PlaceHolder |ui::SupportWeakCallback| [include/dui/Core/PlaceHolder.h](../include/dui/Core/PlaceHolder.h) | The top-level base class of all controls and containers, encapsulating basic data such as the position, size and appearance of a control|
| Control | PlaceHolder | [include/dui/Core/Control.h](../include/dui/Core/Control.h) | A basic control and the base class of all controls and containers, including basic functionality such as the position, size, state, color, image resources, animation, drawing, cursor, mouse, keyboard, focus and shortcut operations|
| Box | Control| [include/dui/Core/Box.h](../include/dui/Core/Box.h) | The base class of all containers, encapsulating the position, size, appearance and child control management (add, delete, modify, draw, operate, drag-and-drop, layout, etc.) of a container|

* Layout

| Class Name | Base Class Name |Associated Header File| Purpose |
| :--- | :--- |:--- | :--- |
| Layout | |[include/dui/Layout/Layout.h](../include/dui/Layout/Layout.h) | Floating layout|
| HLayout |Layout |[include/dui/Layout/HLayout.h](../include/dui/Layout/HLayout.h) | Horizontal layout|
| VLayout |Layout |[include/dui/Layout/VLayout.h](../include/dui/Layout/VLayout.h) | Vertical layout|
| HFlowLayout |Layout |[include/dui/Layout/HFlowLayout.h](../include/dui/Layout/HFlowLayout.h) | Horizontal flow layout|
| VFlowLayout |Layout |[include/dui/Layout/VFlowLayout.h](../include/dui/Layout/VFlowLayout.h) | Vertical flow layout|
| HTileLayout |Layout| [include/dui/Layout/HTileLayout.h](../include/dui/Layout/HTileLayout.h) | Horizontal tile layout|
| VTileLayout |Layout| [include/dui/Layout/VTileLayout.h](../include/dui/Layout/VTileLayout.h) | Vertical tile layout|
| VirtualHLayout |HLayout| [include/dui/Layout/VirtualHLayout.h](../include/dui/Layout/VirtualHLayout.h) | Virtual list horizontal layout|
| VirtualVLayout |VLayout| [include/dui/Layout/VirtualVLayout.h](../include/dui/Layout/VirtualVLayout.h) | Virtual list vertical layout|
| VirtualHTileLayout |HTileLayout| [include/dui/Layout/VirtualHTileLayout.h](../include/dui/Layout/VirtualHTileLayout.h) | Virtual list horizontal tile layout|
| VirtualVTileLayout |VTileLayout| [include/dui/Layout/VirtualVTileLayout.h](../include/dui/Layout/VirtualVTileLayout.h) | Virtual list vertical tile layout|
| GridLayout |Layout |[include/dui/Layout/GridLayout.h](../include/dui/Layout/GridLayout.h) | Grid layout|

* Containers

| Class Name | Base Class Name |Layout Type|Associated Header File| Name / Purpose |
| :--- | :--- |:--- |:--- | :--- |
| Box | Control|[Layout](../include/dui/Layout/Layout.h)|[include/dui/Core/Box.h](../include/dui/Core/Box.h) | Free-layout container, the base class of all containers, encapsulating the position, size, appearance and child control management (add, delete, modify, draw, operate, drag-and-drop, layout, etc.) of a container|
| VBox | Box|[VLayout](../include/dui/Layout/VLayout.h)| [include/dui/Box/VBox.h](../include/dui/Box/VBox.h) | Vertical layout container |
| HBox | Box|[HLayout](../include/dui/Layout/HLayout.h)|[include/dui/Box/HBox.h](../include/dui/Box/HBox.h) | Horizontal layout container |
| VFlowBox | Box|[VFlowLayout](../include/dui/Layout/VFlowLayout.h)| [include/dui/Box/VBox.h](../include/dui/Box/VBox.h) | Vertical flow layout container |
| HFlowBox | Box|[HFlowLayout](../include/dui/Layout/HFlowLayout.h)|[include/dui/Box/HBox.h](../include/dui/Box/HBox.h) | Horizontal flow layout container |
| TabBox | Box|[Layout](../include/dui/Layout/Layout.h)|[include/dui/Box/TabBox.h](../include/dui/Box/TabBox.h) | Multi-tab layout container |
| VTileBox | Box|[VTileLayout](../include/dui/Layout/VTileLayout.h)|[include/dui/Box/TileBox.h](../include/dui/Box/TileBox.h) | Vertical tile layout container|
| HTileBox | Box|[HTileLayout](../include/dui/Layout/HTileLayout.h)|[include/dui/Box/TileBox.h](../include/dui/Box/TileBox.h) | Horizontal tile layout container|
| ScrollBox | Box|[Layout](../include/dui/Layout/Layout.h)|[include/dui/Box/ScrollBox.h](../include/dui/Box/ScrollBox.h) | Scrollable free-layout container with a vertical or horizontal scrollbar|
| VScrollBox | ScrollBox|[VLayout](../include/dui/Layout/VLayout.h)|[include/dui/Box/ScrollBox.h](../include/dui/Box/ScrollBox.h) | Scrollable vertical-layout container with a vertical or horizontal scrollbar|
| HScrollBox | ScrollBox|[HLayout](../include/dui/Layout/HLayout.h)|[include/dui/Box/ScrollBox.h](../include/dui/Box/ScrollBox.h) | Scrollable horizontal-layout container with a vertical or horizontal scrollbar|
| VTileScrollBox |ScrollBox|[VTileLayout](../include/dui/Layout/VTileLayout.h)|[include/dui/Box/ScrollBox.h](../include/dui/Box/ScrollBox.h) | Scrollable vertical tile layout container with a vertical or horizontal scrollbar|
| HTileScrollBox |ScrollBox|[HTileLayout](../include/dui/Layout/HTileLayout.h)|[include/dui/Box/ScrollBox.h](../include/dui/Box/ScrollBox.h) | Scrollable horizontal tile layout container with a vertical or horizontal scrollbar|
| ListBox | ScrollBox|[Layout](../include/dui/Layout/Layout.h)|[include/dui/Box/ListBox.h](../include/dui/Box/ListBox.h) | List container with free layout|
| VListBox | ListBox|[VLayout](../include/dui/Layout/VLayout.h)|[include/dui/Box/ListBox.h](../include/dui/Box/ListBox.h) | List container with vertical layout|
| HListBox | ListBox|[HLayout](../include/dui/Layout/HLayout.h)|[include/dui/Box/ListBox.h](../include/dui/Box/ListBox.h) | List container with horizontal layout|
| VTileListBox | ListBox|[VTileLayout](../include/dui/Layout/VTileLayout.h)|[include/dui/Box/ListBox.h](../include/dui/Box/ListBox.h) | List container with vertical tile layout|
| HTileListBox | ListBox|[HTileLayout](../include/dui/Layout/HTileLayout.h)|[include/dui/Box/ListBox.h](../include/dui/Box/ListBox.h) | List container with horizontal layout|
| VirtualListBox | ListBox|[Layout](../include/dui/Layout/Layout.h)|[include/dui/Box/VirtualListBox.h](../include/dui/Box/VirtualListBox.h) | Free-layout list container implemented with a virtual list|
| VirtualVListBox | VirtualListBox|[VirtualVLayout](../include/dui/Layout/VirtualVLayout.h)|[include/dui/Box/VirtualListBox.h](../include/dui/Box/VirtualListBox.h) | Vertical-layout list container implemented with a virtual list|
| VirtualHListBox | VirtualListBox|[VirtualHLayout](../include/dui/Layout/VirtualHLayout.h)|[include/dui/Box/VirtualListBox.h](../include/dui/Box/VirtualListBox.h) | Horizontal-layout list container implemented with a virtual list|
| VirtualVTileListBox | VirtualListBox|[VirtualVTileLayout](../include/dui/Layout/VirtualVTileLayout.h)|[include/dui/Box/VirtualListBox.h](../include/dui/Box/VirtualListBox.h) | Vertical tile layout list container implemented with a virtual list|
| VirtualHTileListBox | VirtualListBox|[VirtualHTileLayout](../include/dui/Layout/VirtualHTileLayout.h)|[include/dui/Box/VirtualListBox.h](../include/dui/Box/VirtualListBox.h) | Horizontal tile layout list container implemented with a virtual list|
| GridBox | Box|[GridLayout](../include/dui/Layout/GridLayout.h)|[include/dui/Box/GridBox.h](../include/dui/Box/GridBox.h) | Grid layout container |
| GridScrollBox | ScrollBox|[GridLayout](../include/dui/Layout/GridLayout.h)|[include/dui/Box/GridBox.h](../include/dui/Box/GridBox.h) | Grid layout container (with scrollbar support) |

* Images

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| Image | [include/dui/Image/Image.h](../include/dui/Image/Image.h) | Image-related wrapper. Supported file formats: SVG/PNG/GIF/JPG/BMP/APNG/WEBP/ICO/Lottie-JSON/PAG|
| ImageAttribute | [include/dui/Image/ImageAttribute.h](../include/dui/Image/ImageAttribute.h) | Image attributes|
| ImageLoadParam | [include/dui/Image/ImageLoadParam.h](../include/dui/Image/ImageLoadParam.h) | Image loading parameters, used to load an image|
| ImageInfo | [include/dui/Image/ImageInfo.h](../include/dui/Image/ImageInfo.h) | Image information|
| ImageDecoder | [include/dui/Image/ImageDecoder.h](../include/dui/Image/ImageDecoder.h) | Interface supporting multi-threaded decoding (used for low-level decoding; supports deferred decoding and decoding on multiple threads, avoiding UI freezes caused by decoding images on the UI thread)|
| ImagePlayer | [include/dui/Image/ImagePlayer.h](../include/dui/Image/ImagePlayer.h) | Logic wrapper for playing image animations on controls (supports GIF/WebP/APNG/Lottie-JSON/PAG animations)|
| StateImage | [include/dui/Image/StateImage.h](../include/dui/Image/StateImage.h) | Mapping of control states to images|
| StateImageMap | [include/dui/Image/StateImageMap.h](../include/dui/Image/StateImageMap.h) | Mapping of control image types to state images|
| ImageManager | [include/dui/Core/ImageManager.h](../include/dui/Core/ImageManager.h) | Image resource manager|
| IconManager | [include/dui/Core/IconManager.h](../include/dui/Core/IconManager.h) | Icon resource manager (thread-safe, suitable for small icon-type image resources), supports HICON handles on Windows|
| ImageList | [include/dui/Core/ImageList.h](../include/dui/Core/ImageList.h) | Image list|

* Animation

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| AnimationManager | [include/dui/Animation/AnimationManager.h](../include/dui/Animation/AnimationManager.h) | Image animation manager|
| AnimationPlayer | [include/dui/Animation/AnimationPlayer.h](../include/dui/Animation/AnimationPlayer.h) | Image animation playback state management|

* Colors

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| UiColor | [include/dui/Core/UiColor.h](../include/dui/Core/UiColor.h) | Wrapper of the color type|
| UiColors | [include/dui/Core/UiColors.h](../include/dui/Core/UiColors.h) | Constants of common color values (ARGB format)|
| StateColorMap | [include/dui/Core/StateColorMap.h](../include/dui/Core/StateColorMap.h) | Mapping of control states to color values|

* Fonts

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| UiFont | [include/dui/Core/UiFont.h](../include/dui/Core/UiFont.h) | Wrapper of the font type|
| FontManager | [include/dui/Core/FontManager.h](../include/dui/Core/FontManager.h) | Font manager|

* Rendering engine interfaces

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| IRenderFactory | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Rendering factory interface, used to create rendering implementation objects such as Font, Pen, Brush, Path, Matrix, Bitmap and Render |
| IFont | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Font interface |
| IBitmap | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Bitmap interface |
| IPen | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Pen interface |
| IBrush | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Brush interface |
| IPath | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Path interface |
| IMatrix | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Matrix interface |
| IRender | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Rendering interface, used for drawing images, text, etc. |

* Skia rendering engine

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| RenderFactory_Skia | [include/dui/RenderSkia/RenderFactory_Skia.h](../include/dui/RenderSkia/RenderFactory_Skia.h) | Implementation of the rendering factory interface |
| Font_Skia | [include/dui/RenderSkia/Font_Skia.h](../include/dui/RenderSkia/Font_Skia.h) | Implementation of the font interface |
| Bitmap_Skia | [include/dui/RenderSkia/Bitmap_Skia.h](../include/dui/RenderSkia/Bitmap_Skia.h) | Implementation of the bitmap interface |
| Pen_Skia | [include/dui/RenderSkia/Pen_Skia.h](../include/dui/RenderSkia/Pen_Skia.h) | Implementation of the pen interface |
| Brush_Skia | [include/dui/RenderSkia/Brush_Skia.h](../include/dui/RenderSkia/Brush_Skia.h) | Implementation of the brush interface |
| Path_Skia | [include/dui/RenderSkia/Path_Skia.h](../include/dui/RenderSkia/Path_Skia.h) | Implementation of the path interface |
| Matrix_Skia | [include/dui/RenderSkia/Matrix_Skia.h](../include/dui/RenderSkia/Matrix_Skia.h) | Implementation of the matrix interface |
| FontMgr_Skia | [include/dui/RenderSkia/FontMgr_Skia.h](../include/dui/RenderSkia/FontMgr_Skia.h) | Interface implementation of the font manager |
| Render_Skia | [include/dui/RenderSkia/Render_Skia.h](../include/dui/RenderSkia/Render_Skia.h) | Implementation of the rendering interface, used for drawing images, text, etc. |
| Render_Skia_Windows | [include/dui/RenderSkia/Render_Skia_Windows.h](../include/dui/RenderSkia/Render_Skia_Windows.h) | Implementation of the Windows-related functionality of the rendering interface |
| Render_Skia_SDL | [include/dui/RenderSkia/Render_Skia_SDL.h](../include/dui/RenderSkia/Render_Skia_SDL.h) | Implementation of the SDL-related functionality of the rendering interface, mainly used on Linux; Windows is also supported |

* Controls / functional components

| Class Name / Component | Base Class | Associated Header File| Purpose |
| :--- | :--- | :---| :--- |
| ScrollBar | Control| [include/dui/Core/ScrollBar.h](../include/dui/Core/ScrollBar.h) | Scrollbar control |
| Label | Control| [include/dui/Control/Label.h](../include/dui/Control/Label.h) | Label control (template), used to display text |
| LabelBox | Box| [include/dui/Control/Label.h](../include/dui/Control/Label.h) | Label container (template), used to display text |
| Button | Control| [include/dui/Control/Button.h](../include/dui/Control/Button.h) | Button control (template implementation) |
| ButtonBox | Box| [include/dui/Control/Button.h](../include/dui/Control/Button.h) | Button container control (template implementation) |
| CheckBox | Control| [include/dui/Control/CheckBox.h](../include/dui/Control/CheckBox.h) | Checkbox control (template implementation) |
| CheckBoxBox | Box| [include/dui/Control/CheckBox.h](../include/dui/Control/CheckBox.h) | Checkbox container (template implementation) |
| Option | Control| [include/dui/Control/Option.h](../include/dui/Control/Option.h) | Radio button control|
| OptionBox | Box| [include/dui/Control/Option.h](../include/dui/Control/Option.h) | Radio button container|
| GroupBox | Box| [include/dui/Control/GroupBox.h](../include/dui/Control/GroupBox.h) | Group container (template) |
| GroupVBox | VBox| [include/dui/Control/GroupBox.h](../include/dui/Control/GroupBox.h) | Vertical group container (template) |
| GroupHBox | HBox| [include/dui/Control/GroupBox.h](../include/dui/Control/GroupBox.h) | Horizontal group container (template) |
| Combo | Box| [include/dui/Control/Combo.h](../include/dui/Control/Combo.h) | Combo box |
| ComboButton | Box| [include/dui/Control/ComboButton.h](../include/dui/Control/ComboButton.h) | Button with a drop-down combo box |
| CheckCombo | Control| [include/dui/Control/CheckCombo.h](../include/dui/Control/CheckCombo.h) | Combo box with checkboxes |
| FilterCombo | Combo| [include/dui/Control/FilterCombo.h](../include/dui/Control/FilterCombo.h) | Combo box with filtering |
| DateTime | Label| [include/dui/Control/DateTime.h](../include/dui/Control/DateTime.h) | Date-time picker control |
| HotKey | HBox| [include/dui/Control/HotKey.h](../include/dui/Control/HotKey.h) | Hotkey control |
| HyperLink | Label| [include/dui/Control/HyperLink.h](../include/dui/Control/HyperLink.h) | Text with a hyperlink; if the URL is empty, it can be used as an ordinary text button |
| IPAddress | HBox| [include/dui/Control/IPAddress.h](../include/dui/Control/IPAddress.h) | IP address control |
| Line | Control| [include/dui/Control/Line.h](../include/dui/Control/Line.h) | Line drawing control |
| Menu | WindowImplBase| [include/dui/Control/Menu.h](../include/dui/Control/Menu.h) | Menu, an independent window |
| Progress | Label| [include/dui/Control/Progress.h](../include/dui/Control/Progress.h) | Progress bar control |
| Slider | Progress| [include/dui/Control/Slider.h](../include/dui/Control/Slider.h) | Slider control |
| CircleProgress | Control| [include/dui/Control/CircleProgress.h](../include/dui/Control/CircleProgress.h) | Circular progress bar |
| RichEdit | ScrollBox| [include/dui/Control/RichEdit.h](../include/dui/Control/RichEdit.h) | Rich text edit control |
| RichEdit implementation classes | | [include/dui/Control/RichEditCtrl_Windows.h](../include/dui/Control/RichEditCtrl_Windows.h) | Core functionality wrapper of the rich text edit control (Windows) |
| RichEdit implementation classes | | [include/dui/Control/RichEditHost_Windows.h](../include/dui/Control/RichEditHost_Windows.h) | Core functionality implementation of the rich text edit control (Windows) |
| RichEdit implementation classes | | [include/dui/Control/RichEdit_SDL.h](../include/dui/Control/RichEdit_SDL.h) | Core functionality wrapper of the rich text edit control (SDL) |
| RichText | Control| [include/dui/Control/RichText.h](../include/dui/Control/RichText.h) | Formatted text (HTML-like format) |
| Split | Control| [include/dui/Control/Split.h](../include/dui/Control/Split.h) | Splitter control |
| SplitBox | Box| [include/dui/Control/Split.h](../include/dui/Control/Split.h) | Splitter container |
| TabCtrl | ListBox| [include/dui/Control/TabCtrl.h](../include/dui/Control/TabCtrl.h) | Multi-tab control (similar to browser tabs) |
| TreeView | ListBox| [include/dui/Control/TreeView.h](../include/dui/Control/TreeView.h) | Tree control |
| TreeNode | ListBoxItem| [include/dui/Control/TreeView.h](../include/dui/Control/TreeView.h) | Node of the tree control |
| DirectoryTree | TreeView| [include/dui/Control/DirectoryTree.h](../include/dui/Control/DirectoryTree.h) | Directory tree control, used to display the directory structure of the file system |
| ListCtrl | VBox| [include/dui/Control/ListCtrl.h](../include/dui/Control/ListCtrl.h) | List control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlDefs.h](../include/dui/Control/ListCtrlDefs.h) | Basic type definitions of the list control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlHeader.h](../include/dui/Control/ListCtrlHeader.h) | Header of the list control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlHeaderItem.h](../include/dui/Control/ListCtrlHeaderItem.h) | Header items of the list control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlItem.h](../include/dui/Control/ListCtrlItem.h) | Data items of the list control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlSubItem.h](../include/dui/Control/ListCtrlSubItem.h) | Sub-items of the data items of the list control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlView.h](../include/dui/Control/ListCtrlView.h) | View base class of the list control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlReportView.h](../include/dui/Control/ListCtrlReportView.h) | Report view of the list control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlIconView.h](../include/dui/Control/ListCtrlIconView.h) | Icon/List view of the list control |
| ListCtrl implementation classes | | [include/dui/Control/ListCtrlData.h](../include/dui/Control/ListCtrlData.h) | Data manager of the list control |
| PropertyGrid | VBox| [include/dui/Control/PropertyGrid.h](../include/dui/Control/PropertyGrid.h) | Property table control, supporting properties such as text, number, checkbox, font, color, date, IP address, hotkey, file path and folder |
| ColorPicker | WindowImplBase| [include/dui/Control/ColorPicker.h](../include/dui/Control/ColorPicker.h) | Color picker, an independent window |
| Color picker implementation classes | | [include/dui/Control/ColorControl.h](../include/dui/Control/ColorControl.h) | Implementation class of ColorPicker, a custom color control |
| Color picker implementation classes | | [include/dui/Control/ColorConvert.h](../include/dui/Control/ColorConvert.h) | Implementation class of ColorPicker, a color type (RGB/HSV/HSL) conversion class |
| Color picker implementation classes | | [include/dui/Control/ColorPickerCustom.h](../include/dui/Control/ColorPickerCustom.h) | Implementation class of ColorPicker, custom colors |
| Color picker implementation classes | | [include/dui/Control/ColorPickerRegular.h](../include/dui/Control/ColorPickerRegular.h) | Implementation class of ColorPicker, common colors |
| Color picker implementation classes | | [include/dui/Control/ColorPickerStatard.h](../include/dui/Control/ColorPickerStatard.h) | Implementation class of ColorPicker, standard colors |
| Color picker implementation classes | | [include/dui/Control/ColorPickerStatardGray.h](../include/dui/Control/ColorPickerStatardGray.h) | Implementation class of ColorPicker, standard colors, gray |
| Color picker implementation classes | | [include/dui/Control/ColorSlider.h](../include/dui/Control/ColorSlider.h) | Implementation class of ColorPicker |
| ControlDragable | Control | [include/dui/Core/ControlDragable.h](../include/dui/Core/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| BoxDragable | Box | [include/dui/Core/ControlDragable.h](../include/dui/Core/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| HBoxDragable | HBox | [include/dui/Core/ControlDragable.h](../include/dui/Core/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| VBoxDragable | VBoxDragable | [include/dui/Core/ControlDragable.h](../include/dui/Core/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| IconControl | Control | [include/dui/Control/IconControl.h](../include/dui/Control/IconControl.h) | Control used to display icons; if no icon data is set, it is compatible with all the functionality of the base class Control|
| AddressBar | HBox | [include/dui/Control/AddressBar.h](../include/dui/Control/AddressBar.h) | Address bar control, used to display paths of the local file system |

* Global resources

| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| GlobalManager | [include/dui/Core/GlobalManager.h](../include/dui/Core/GlobalManager.h) | Global attribute management utility class, used to manage various global attributes, including global styles (global.xml) and language settings |
| IRenderFactory | [include/dui/Render/IRender.h](../include/dui/Render/IRender.h) | Management class of the rendering interface, used to create rendering implementation objects such as Font, Pen, Brush, Path, Matrix, Bitmap and Render |
| FontManager | [include/dui/Core/FontManager.h](../include/dui/Core/FontManager.h) | Management class of fonts |
| ImageManager | [include/dui/Core/ImageManager.h](../include/dui/Core/ImageManager.h) | Management class of images |
| IconManager | [include/dui/Core/IconManager.h](../include/dui/Core/IconManager.h) | Icon resource manager (thread-safe, suitable for small icon-type image resources), supports HICON handles on Windows |
| ZipManager | [include/dui/Core/ZipManager.h](../include/dui/Core/ZipManager.h) | ZIP archive manager |
| DpiManager | [include/dui/Core/DpiManager.h](../include/dui/Core/DpiManager.h) | DPI manager, used to support features such as DPI adaptation |
| TimerManager | [include/dui/Core/TimerManager.h](../include/dui/Core/TimerManager.h) | Timer manager |
| LangManager | [include/dui/Core/LangManager.h](../include/dui/Core/LangManager.h) | Multi-language support manager |
| CursorManager | [include/dui/Core/CursorManager.h](../include/dui/Core/CursorManager.h) | Cursor manager |
| ThreadManager | [include/dui/Core/ThreadManager.h](../include/dui/Core/ThreadManager.h) | Thread manager |
| ColorManager | [include/dui/Core/ColorManager.h](../include/dui/Core/ColorManager.h) | Color manager |
| WindowManager | [include/dui/Core/WindowManager.h](../include/dui/Core/WindowManager.h) | Window manager |
| ImageDecoderFactory | [include/dui/Image/ImageDecoderFactory.h](../include/dui/Image/ImageDecoderFactory.h) | Management of image decoders |

* libcef control wrapper related

| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| CefManager | [include/dui/CEFControl/CefManager.h](../include/dui/CEFControl/CefManager.h) | CEF control manager, responsible for the initialization and uninitialization of the CEF module |
| CefControl | [include/dui/CEFControl/CefControl.h](../include/dui/CEFControl/CefControl.h) | CEF control interface, providing basic web browsing functionality and event acceptance |
| CefControlEvent | [include/dui/CEFControl/CefControlEvent.h](../include/dui/CEFControl/CefControlEvent.h) | Event reception interface for web browsing of the CEF control |
| CefControlNative | [include/dui/CEFControl/CefControlNative.h](../include/dui/CEFControl/CefControlNative.h) | Wrapper of the windowed mode of the CEF control |
| CefControlOffScreen | [include/dui/CEFControl/CefControlOffScreen.h](../include/dui/CEFControl/CefControlOffScreen.h) | Wrapper of the off-screen rendering mode of the CEF control |
