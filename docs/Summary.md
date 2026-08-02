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
| UiSize | [include/duilib/Core/UiSize.h](../include/duilib/Core/UiSize.h) | Wrapper of the 32-bit Size type|
| UiSize64 | [include/duilib/Core/UiSize64.h](../include/duilib/Core/UiSize64.h) | Wrapper of the 64-bit Size type|
| UiPoint | [include/duilib/Core/UiPoint.h](../include/duilib/Core/UiPoint.h) | Wrapper of the integer Point type|
| UiPointF | [include/duilib/Core/UiPointF.h](../include/duilib/Core/UiPointF.h) | Wrapper of the floating-point Point type|
| UiFixedInt | [include/duilib/Core/UiFixedInt.h](../include/duilib/Core/UiFixedInt.h) | Wrapper of the numeric type for the length (or width) set on a control|
| UiEstInt | [include/duilib/Core/UiEstInt.h](../include/duilib/Core/UiEstInt.h) | Wrapper of the numeric type for the estimated length (or width) of a control|
| UiFixedSize | [include/duilib/Core/UiTypes.h](../include/duilib/Core/UiTypes.h) | The set control size|
| UiEstSize | [include/duilib/Core/UiTypes.h](../include/duilib/Core/UiTypes.h) | The estimated control size (unlike UiFixedSize, it has no Auto type)|
| UiEstResult | [include/duilib/Core/UiTypes.h](../include/duilib/Core/UiTypes.h) | The result of estimating the control size|
| UiPadding | [include/duilib/Core/UiPadding.h](../include/duilib/Core/UiPadding.h) | Wrapper of the padding type|
| UiMargin | [include/duilib/Core/UiMargin.h](../include/duilib/Core/UiMargin.h) | Wrapper of the margin type|
| UiString | [include/duilib/Core/UiString.h](../include/duilib/Core/UiString.h) | A string type for reducing memory usage in controls, replacing DString (the standard string type used by the library) where memory is critical. This class is suitable for storing strings with low memory usage; in terms of performance, due to more string copying, its performance is relatively weak|
| UiRect | [include/duilib/Core/UiRect.h](../include/duilib/Core/UiRect.h) | Wrapper of the 32-bit Rect type|
| UiFont | [include/duilib/Core/UiFont.h](../include/duilib/Core/UiFont.h) | Wrapper of the font type|
| UiColor | [include/duilib/Core/UiColor.h](../include/duilib/Core/UiColor.h) | Wrapper of the color type|
| UiColors | [include/duilib/Core/UiColors.h](../include/duilib/Core/UiColors.h) | Constants of common color values (ARGB format)|

* Window related

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| Window | [include/duilib/Core/Window.h](../include/duilib/Core/Window.h) | Wrapper of the core duilib window|
| WindowBuilder | [include/duilib/Core/WindowBuilder.h](../include/duilib/Core/WindowBuilder.h) | Parses XML files and creates windows, container layouts and controls, etc.|
| Shadow | [include/duilib/Core/Shadow.h](../include/duilib/Core/Shadow.h) | Window shadow attributes|
| WindowImplBase | [include/duilib/Utils/WinImplBase.h](../include/duilib/Utils/WinImplBase.h) | The window base class provided to the application layer. Implements a window with a title bar, with support for maximize, minimize and restore buttons, and provides window shadows with the WS_EX_LAYERED attribute|
| ShadowWnd | [include/duilib/Utils/ShadowWnd.h](../include/duilib/Utils/ShadowWnd.h) | A subclass of WinImplBase, a base class implemented with attached shadow windows. Implements creating and attaching a window, and provides window shadows without the WS_EX_LAYERED attribute|
| EventType | [include/duilib/duilib_defs.h](../include/duilib/duilib_defs.h) | Defines all message types|
| EventArgs | [include/duilib/Core/EventArgs.h](../include/duilib/Core/EventArgs.h) | Parameters of window messages and control event notifications|
| Windows version compatibility | [include/duilib/duilib_config.h](../include/duilib/duilib_config.h) | Windows version compatibility definitions; the default configuration supports Win7 and above|

* Base classes of controls and containers

| Class Name | Base Class Name |Associated Header File| Purpose |
| :--- | :--- |:--- | :--- |
| PlaceHolder |ui::SupportWeakCallback| [include/duilib/Core/PlaceHolder.h](../include/duilib/Core/PlaceHolder.h) | The top-level base class of all controls and containers, encapsulating basic data such as the position, size and appearance of a control|
| Control | PlaceHolder | [include/duilib/Core/Control.h](../include/duilib/Core/Control.h) | A basic control and the base class of all controls and containers, including basic functionality such as the position, size, state, color, image resources, animation, drawing, cursor, mouse, keyboard, focus and shortcut operations|
| Box | Control| [include/duilib/Core/Box.h](../include/duilib/Core/Box.h) | The base class of all containers, encapsulating the position, size, appearance and child control management (add, delete, modify, draw, operate, drag-and-drop, layout, etc.) of a container|

* Layout

| Class Name | Base Class Name |Associated Header File| Purpose |
| :--- | :--- |:--- | :--- |
| Layout | |[include/duilib/Layout/Layout.h](../include/duilib/Layout/Layout.h) | Floating layout|
| HLayout |Layout |[include/duilib/Layout/HLayout.h](../include/duilib/Layout/HLayout.h) | Horizontal layout|
| VLayout |Layout |[include/duilib/Layout/VLayout.h](../include/duilib/Layout/VLayout.h) | Vertical layout|
| HFlowLayout |Layout |[include/duilib/Layout/HFlowLayout.h](../include/duilib/Layout/HFlowLayout.h) | Horizontal flow layout|
| VFlowLayout |Layout |[include/duilib/Layout/VFlowLayout.h](../include/duilib/Layout/VFlowLayout.h) | Vertical flow layout|
| HTileLayout |Layout| [include/duilib/Layout/HTileLayout.h](../include/duilib/Layout/HTileLayout.h) | Horizontal tile layout|
| VTileLayout |Layout| [include/duilib/Layout/VTileLayout.h](../include/duilib/Layout/VTileLayout.h) | Vertical tile layout|
| VirtualHLayout |HLayout| [include/duilib/Layout/VirtualHLayout.h](../include/duilib/Layout/VirtualHLayout.h) | Virtual list horizontal layout|
| VirtualVLayout |VLayout| [include/duilib/Layout/VirtualVLayout.h](../include/duilib/Layout/VirtualVLayout.h) | Virtual list vertical layout|
| VirtualHTileLayout |HTileLayout| [include/duilib/Layout/VirtualHTileLayout.h](../include/duilib/Layout/VirtualHTileLayout.h) | Virtual list horizontal tile layout|
| VirtualVTileLayout |VTileLayout| [include/duilib/Layout/VirtualVTileLayout.h](../include/duilib/Layout/VirtualVTileLayout.h) | Virtual list vertical tile layout|
| GridLayout |Layout |[include/duilib/Layout/GridLayout.h](../include/duilib/Layout/GridLayout.h) | Grid layout|

* Containers

| Class Name | Base Class Name |Layout Type|Associated Header File| Name / Purpose |
| :--- | :--- |:--- |:--- | :--- |
| Box | Control|[Layout](../include/duilib/Layout/Layout.h)|[include/duilib/Core/Box.h](../include/duilib/Core/Box.h) | Free-layout container, the base class of all containers, encapsulating the position, size, appearance and child control management (add, delete, modify, draw, operate, drag-and-drop, layout, etc.) of a container|
| VBox | Box|[VLayout](../include/duilib/Layout/VLayout.h)| [include/duilib/Box/VBox.h](../include/duilib/Box/VBox.h) | Vertical layout container |
| HBox | Box|[HLayout](../include/duilib/Layout/HLayout.h)|[include/duilib/Box/HBox.h](../include/duilib/Box/HBox.h) | Horizontal layout container |
| VFlowBox | Box|[VFlowLayout](../include/duilib/Layout/VFlowLayout.h)| [include/duilib/Box/VBox.h](../include/duilib/Box/VBox.h) | Vertical flow layout container |
| HFlowBox | Box|[HFlowLayout](../include/duilib/Layout/HFlowLayout.h)|[include/duilib/Box/HBox.h](../include/duilib/Box/HBox.h) | Horizontal flow layout container |
| TabBox | Box|[Layout](../include/duilib/Layout/Layout.h)|[include/duilib/Box/TabBox.h](../include/duilib/Box/TabBox.h) | Multi-tab layout container |
| VTileBox | Box|[VTileLayout](../include/duilib/Layout/VTileLayout.h)|[include/duilib/Box/TileBox.h](../include/duilib/Box/TileBox.h) | Vertical tile layout container|
| HTileBox | Box|[HTileLayout](../include/duilib/Layout/HTileLayout.h)|[include/duilib/Box/TileBox.h](../include/duilib/Box/TileBox.h) | Horizontal tile layout container|
| ScrollBox | Box|[Layout](../include/duilib/Layout/Layout.h)|[include/duilib/Box/ScrollBox.h](../include/duilib/Box/ScrollBox.h) | Scrollable free-layout container with a vertical or horizontal scrollbar|
| VScrollBox | ScrollBox|[VLayout](../include/duilib/Layout/VLayout.h)|[include/duilib/Box/ScrollBox.h](../include/duilib/Box/ScrollBox.h) | Scrollable vertical-layout container with a vertical or horizontal scrollbar|
| HScrollBox | ScrollBox|[HLayout](../include/duilib/Layout/HLayout.h)|[include/duilib/Box/ScrollBox.h](../include/duilib/Box/ScrollBox.h) | Scrollable horizontal-layout container with a vertical or horizontal scrollbar|
| VTileScrollBox |ScrollBox|[VTileLayout](../include/duilib/Layout/VTileLayout.h)|[include/duilib/Box/ScrollBox.h](../include/duilib/Box/ScrollBox.h) | Scrollable vertical tile layout container with a vertical or horizontal scrollbar|
| HTileScrollBox |ScrollBox|[HTileLayout](../include/duilib/Layout/HTileLayout.h)|[include/duilib/Box/ScrollBox.h](../include/duilib/Box/ScrollBox.h) | Scrollable horizontal tile layout container with a vertical or horizontal scrollbar|
| ListBox | ScrollBox|[Layout](../include/duilib/Layout/Layout.h)|[include/duilib/Box/ListBox.h](../include/duilib/Box/ListBox.h) | List container with free layout|
| VListBox | ListBox|[VLayout](../include/duilib/Layout/VLayout.h)|[include/duilib/Box/ListBox.h](../include/duilib/Box/ListBox.h) | List container with vertical layout|
| HListBox | ListBox|[HLayout](../include/duilib/Layout/HLayout.h)|[include/duilib/Box/ListBox.h](../include/duilib/Box/ListBox.h) | List container with horizontal layout|
| VTileListBox | ListBox|[VTileLayout](../include/duilib/Layout/VTileLayout.h)|[include/duilib/Box/ListBox.h](../include/duilib/Box/ListBox.h) | List container with vertical tile layout|
| HTileListBox | ListBox|[HTileLayout](../include/duilib/Layout/HTileLayout.h)|[include/duilib/Box/ListBox.h](../include/duilib/Box/ListBox.h) | List container with horizontal layout|
| VirtualListBox | ListBox|[Layout](../include/duilib/Layout/Layout.h)|[include/duilib/Box/VirtualListBox.h](../include/duilib/Box/VirtualListBox.h) | Free-layout list container implemented with a virtual list|
| VirtualVListBox | VirtualListBox|[VirtualVLayout](../include/duilib/Layout/VirtualVLayout.h)|[include/duilib/Box/VirtualListBox.h](../include/duilib/Box/VirtualListBox.h) | Vertical-layout list container implemented with a virtual list|
| VirtualHListBox | VirtualListBox|[VirtualHLayout](../include/duilib/Layout/VirtualHLayout.h)|[include/duilib/Box/VirtualListBox.h](../include/duilib/Box/VirtualListBox.h) | Horizontal-layout list container implemented with a virtual list|
| VirtualVTileListBox | VirtualListBox|[VirtualVTileLayout](../include/duilib/Layout/VirtualVTileLayout.h)|[include/duilib/Box/VirtualListBox.h](../include/duilib/Box/VirtualListBox.h) | Vertical tile layout list container implemented with a virtual list|
| VirtualHTileListBox | VirtualListBox|[VirtualHTileLayout](../include/duilib/Layout/VirtualHTileLayout.h)|[include/duilib/Box/VirtualListBox.h](../include/duilib/Box/VirtualListBox.h) | Horizontal tile layout list container implemented with a virtual list|
| GridBox | Box|[GridLayout](../include/duilib/Layout/GridLayout.h)|[include/duilib/Box/GridBox.h](../include/duilib/Box/GridBox.h) | Grid layout container |
| GridScrollBox | ScrollBox|[GridLayout](../include/duilib/Layout/GridLayout.h)|[include/duilib/Box/GridBox.h](../include/duilib/Box/GridBox.h) | Grid layout container (with scrollbar support) |

* Images

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| Image | [include/duilib/Image/Image.h](../include/duilib/Image/Image.h) | Image-related wrapper. Supported file formats: SVG/PNG/GIF/JPG/BMP/APNG/WEBP/ICO/Lottie-JSON/PAG|
| ImageAttribute | [include/duilib/Image/ImageAttribute.h](../include/duilib/Image/ImageAttribute.h) | Image attributes|
| ImageLoadParam | [include/duilib/Image/ImageLoadParam.h](../include/duilib/Image/ImageLoadParam.h) | Image loading parameters, used to load an image|
| ImageInfo | [include/duilib/Image/ImageInfo.h](../include/duilib/Image/ImageInfo.h) | Image information|
| ImageDecoder | [include/duilib/Image/ImageDecoder.h](../include/duilib/Image/ImageDecoder.h) | Interface supporting multi-threaded decoding (used for low-level decoding; supports deferred decoding and decoding on multiple threads, avoiding UI freezes caused by decoding images on the UI thread)|
| ImagePlayer | [include/duilib/Image/ImagePlayer.h](../include/duilib/Image/ImagePlayer.h) | Logic wrapper for playing image animations on controls (supports GIF/WebP/APNG/Lottie-JSON/PAG animations)|
| StateImage | [include/duilib/Image/StateImage.h](../include/duilib/Image/StateImage.h) | Mapping of control states to images|
| StateImageMap | [include/duilib/Image/StateImageMap.h](../include/duilib/Image/StateImageMap.h) | Mapping of control image types to state images|
| ImageManager | [include/duilib/Core/ImageManager.h](../include/duilib/Core/ImageManager.h) | Image resource manager|
| IconManager | [include/duilib/Core/IconManager.h](../include/duilib/Core/IconManager.h) | Icon resource manager (thread-safe, suitable for small icon-type image resources), supports HICON handles on Windows|
| ImageList | [include/duilib/Core/ImageList.h](../include/duilib/Core/ImageList.h) | Image list|

* Animation

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| AnimationManager | [include/duilib/Animation/AnimationManager.h](../include/duilib/Animation/AnimationManager.h) | Image animation manager|
| AnimationPlayer | [include/duilib/Animation/AnimationPlayer.h](../include/duilib/Animation/AnimationPlayer.h) | Image animation playback state management|

* Colors

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| UiColor | [include/duilib/Core/UiColor.h](../include/duilib/Core/UiColor.h) | Wrapper of the color type|
| UiColors | [include/duilib/Core/UiColors.h](../include/duilib/Core/UiColors.h) | Constants of common color values (ARGB format)|
| StateColorMap | [include/duilib/Core/StateColorMap.h](../include/duilib/Core/StateColorMap.h) | Mapping of control states to color values|

* Fonts

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| UiFont | [include/duilib/Core/UiFont.h](../include/duilib/Core/UiFont.h) | Wrapper of the font type|
| FontManager | [include/duilib/Core/FontManager.h](../include/duilib/Core/FontManager.h) | Font manager|

* Rendering engine interfaces

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| IRenderFactory | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Rendering factory interface, used to create rendering implementation objects such as Font, Pen, Brush, Path, Matrix, Bitmap and Render |
| IFont | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Font interface |
| IBitmap | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Bitmap interface |
| IPen | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Pen interface |
| IBrush | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Brush interface |
| IPath | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Path interface |
| IMatrix | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Matrix interface |
| IRender | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Rendering interface, used for drawing images, text, etc. |

* Skia rendering engine

| Class Name / Description | Associated Header File| Purpose |
| :--- | :--- | :--- |
| RenderFactory_Skia | [include/duilib/RenderSkia/RenderFactory_Skia.h](../include/duilib/RenderSkia/RenderFactory_Skia.h) | Implementation of the rendering factory interface |
| Font_Skia | [include/duilib/RenderSkia/Font_Skia.h](../include/duilib/RenderSkia/Font_Skia.h) | Implementation of the font interface |
| Bitmap_Skia | [include/duilib/RenderSkia/Bitmap_Skia.h](../include/duilib/RenderSkia/Bitmap_Skia.h) | Implementation of the bitmap interface |
| Pen_Skia | [include/duilib/RenderSkia/Pen_Skia.h](../include/duilib/RenderSkia/Pen_Skia.h) | Implementation of the pen interface |
| Brush_Skia | [include/duilib/RenderSkia/Brush_Skia.h](../include/duilib/RenderSkia/Brush_Skia.h) | Implementation of the brush interface |
| Path_Skia | [include/duilib/RenderSkia/Path_Skia.h](../include/duilib/RenderSkia/Path_Skia.h) | Implementation of the path interface |
| Matrix_Skia | [include/duilib/RenderSkia/Matrix_Skia.h](../include/duilib/RenderSkia/Matrix_Skia.h) | Implementation of the matrix interface |
| FontMgr_Skia | [include/duilib/RenderSkia/FontMgr_Skia.h](../include/duilib/RenderSkia/FontMgr_Skia.h) | Interface implementation of the font manager |
| Render_Skia | [include/duilib/RenderSkia/Render_Skia.h](../include/duilib/RenderSkia/Render_Skia.h) | Implementation of the rendering interface, used for drawing images, text, etc. |
| Render_Skia_Windows | [include/duilib/RenderSkia/Render_Skia_Windows.h](../include/duilib/RenderSkia/Render_Skia_Windows.h) | Implementation of the Windows-related functionality of the rendering interface |
| Render_Skia_SDL | [include/duilib/RenderSkia/Render_Skia_SDL.h](../include/duilib/RenderSkia/Render_Skia_SDL.h) | Implementation of the SDL-related functionality of the rendering interface, mainly used on Linux; Windows is also supported |

* Controls / functional components

| Class Name / Component | Base Class | Associated Header File| Purpose |
| :--- | :--- | :---| :--- |
| ScrollBar | Control| [include/duilib/Core/ScrollBar.h](../include/duilib/Core/ScrollBar.h) | Scrollbar control |
| Label | Control| [include/duilib/Control/Label.h](../include/duilib/Control/Label.h) | Label control (template), used to display text |
| LabelBox | Box| [include/duilib/Control/Label.h](../include/duilib/Control/Label.h) | Label container (template), used to display text |
| Button | Control| [include/duilib/Control/Button.h](../include/duilib/Control/Button.h) | Button control (template implementation) |
| ButtonBox | Box| [include/duilib/Control/Button.h](../include/duilib/Control/Button.h) | Button container control (template implementation) |
| CheckBox | Control| [include/duilib/Control/CheckBox.h](../include/duilib/Control/CheckBox.h) | Checkbox control (template implementation) |
| CheckBoxBox | Box| [include/duilib/Control/CheckBox.h](../include/duilib/Control/CheckBox.h) | Checkbox container (template implementation) |
| Option | Control| [include/duilib/Control/Option.h](../include/duilib/Control/Option.h) | Radio button control|
| OptionBox | Box| [include/duilib/Control/Option.h](../include/duilib/Control/Option.h) | Radio button container|
| GroupBox | Box| [include/duilib/Control/GroupBox.h](../include/duilib/Control/GroupBox.h) | Group container (template) |
| GroupVBox | VBox| [include/duilib/Control/GroupBox.h](../include/duilib/Control/GroupBox.h) | Vertical group container (template) |
| GroupHBox | HBox| [include/duilib/Control/GroupBox.h](../include/duilib/Control/GroupBox.h) | Horizontal group container (template) |
| Combo | Box| [include/duilib/Control/Combo.h](../include/duilib/Control/Combo.h) | Combo box |
| ComboButton | Box| [include/duilib/Control/ComboButton.h](../include/duilib/Control/ComboButton.h) | Button with a drop-down combo box |
| CheckCombo | Control| [include/duilib/Control/CheckCombo.h](../include/duilib/Control/CheckCombo.h) | Combo box with checkboxes |
| FilterCombo | Combo| [include/duilib/Control/FilterCombo.h](../include/duilib/Control/FilterCombo.h) | Combo box with filtering |
| DateTime | Label| [include/duilib/Control/DateTime.h](../include/duilib/Control/DateTime.h) | Date-time picker control |
| HotKey | HBox| [include/duilib/Control/HotKey.h](../include/duilib/Control/HotKey.h) | Hotkey control |
| HyperLink | Label| [include/duilib/Control/HyperLink.h](../include/duilib/Control/HyperLink.h) | Text with a hyperlink; if the URL is empty, it can be used as an ordinary text button |
| IPAddress | HBox| [include/duilib/Control/IPAddress.h](../include/duilib/Control/IPAddress.h) | IP address control |
| Line | Control| [include/duilib/Control/Line.h](../include/duilib/Control/Line.h) | Line drawing control |
| Menu | WindowImplBase| [include/duilib/Control/Menu.h](../include/duilib/Control/Menu.h) | Menu, an independent window |
| Progress | Label| [include/duilib/Control/Progress.h](../include/duilib/Control/Progress.h) | Progress bar control |
| Slider | Progress| [include/duilib/Control/Slider.h](../include/duilib/Control/Slider.h) | Slider control |
| CircleProgress | Control| [include/duilib/Control/CircleProgress.h](../include/duilib/Control/CircleProgress.h) | Circular progress bar |
| RichEdit | ScrollBox| [include/duilib/Control/RichEdit.h](../include/duilib/Control/RichEdit.h) | Rich text edit control |
| RichEdit implementation classes | | [include/duilib/Control/RichEditCtrl_Windows.h](../include/duilib/Control/RichEditCtrl_Windows.h) | Core functionality wrapper of the rich text edit control (Windows) |
| RichEdit implementation classes | | [include/duilib/Control/RichEditHost_Windows.h](../include/duilib/Control/RichEditHost_Windows.h) | Core functionality implementation of the rich text edit control (Windows) |
| RichEdit implementation classes | | [include/duilib/Control/RichEdit_SDL.h](../include/duilib/Control/RichEdit_SDL.h) | Core functionality wrapper of the rich text edit control (SDL) |
| RichText | Control| [include/duilib/Control/RichText.h](../include/duilib/Control/RichText.h) | Formatted text (HTML-like format) |
| Split | Control| [include/duilib/Control/Split.h](../include/duilib/Control/Split.h) | Splitter control |
| SplitBox | Box| [include/duilib/Control/Split.h](../include/duilib/Control/Split.h) | Splitter container |
| TabCtrl | ListBox| [include/duilib/Control/TabCtrl.h](../include/duilib/Control/TabCtrl.h) | Multi-tab control (similar to browser tabs) |
| TreeView | ListBox| [include/duilib/Control/TreeView.h](../include/duilib/Control/TreeView.h) | Tree control |
| TreeNode | ListBoxItem| [include/duilib/Control/TreeView.h](../include/duilib/Control/TreeView.h) | Node of the tree control |
| DirectoryTree | TreeView| [include/duilib/Control/DirectoryTree.h](../include/duilib/Control/DirectoryTree.h) | Directory tree control, used to display the directory structure of the file system |
| ListCtrl | VBox| [include/duilib/Control/ListCtrl.h](../include/duilib/Control/ListCtrl.h) | List control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlDefs.h](../include/duilib/Control/ListCtrlDefs.h) | Basic type definitions of the list control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlHeader.h](../include/duilib/Control/ListCtrlHeader.h) | Header of the list control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlHeaderItem.h](../include/duilib/Control/ListCtrlHeaderItem.h) | Header items of the list control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlItem.h](../include/duilib/Control/ListCtrlItem.h) | Data items of the list control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlSubItem.h](../include/duilib/Control/ListCtrlSubItem.h) | Sub-items of the data items of the list control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlView.h](../include/duilib/Control/ListCtrlView.h) | View base class of the list control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlReportView.h](../include/duilib/Control/ListCtrlReportView.h) | Report view of the list control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlIconView.h](../include/duilib/Control/ListCtrlIconView.h) | Icon/List view of the list control |
| ListCtrl implementation classes | | [include/duilib/Control/ListCtrlData.h](../include/duilib/Control/ListCtrlData.h) | Data manager of the list control |
| PropertyGrid | VBox| [include/duilib/Control/PropertyGrid.h](../include/duilib/Control/PropertyGrid.h) | Property table control, supporting properties such as text, number, checkbox, font, color, date, IP address, hotkey, file path and folder |
| ColorPicker | WindowImplBase| [include/duilib/Control/ColorPicker.h](../include/duilib/Control/ColorPicker.h) | Color picker, an independent window |
| Color picker implementation classes | | [include/duilib/Control/ColorControl.h](../include/duilib/Control/ColorControl.h) | Implementation class of ColorPicker, a custom color control |
| Color picker implementation classes | | [include/duilib/Control/ColorConvert.h](../include/duilib/Control/ColorConvert.h) | Implementation class of ColorPicker, a color type (RGB/HSV/HSL) conversion class |
| Color picker implementation classes | | [include/duilib/Control/ColorPickerCustom.h](../include/duilib/Control/ColorPickerCustom.h) | Implementation class of ColorPicker, custom colors |
| Color picker implementation classes | | [include/duilib/Control/ColorPickerRegular.h](../include/duilib/Control/ColorPickerRegular.h) | Implementation class of ColorPicker, common colors |
| Color picker implementation classes | | [include/duilib/Control/ColorPickerStatard.h](../include/duilib/Control/ColorPickerStatard.h) | Implementation class of ColorPicker, standard colors |
| Color picker implementation classes | | [include/duilib/Control/ColorPickerStatardGray.h](../include/duilib/Control/ColorPickerStatardGray.h) | Implementation class of ColorPicker, standard colors, gray |
| Color picker implementation classes | | [include/duilib/Control/ColorSlider.h](../include/duilib/Control/ColorSlider.h) | Implementation class of ColorPicker |
| ControlDragable | Control | [include/duilib/Core/ControlDragable.h](../include/duilib/Core/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| BoxDragable | Box | [include/duilib/Core/ControlDragable.h](../include/duilib/Core/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| HBoxDragable | HBox | [include/duilib/Core/ControlDragable.h](../include/duilib/Core/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| VBoxDragable | VBoxDragable | [include/duilib/Core/ControlDragable.h](../include/duilib/Core/ControlDragable.h) | Supports reordering child controls within the same Box by dragging |
| IconControl | Control | [include/duilib/Control/IconControl.h](../include/duilib/Control/IconControl.h) | Control used to display icons; if no icon data is set, it is compatible with all the functionality of the base class Control|
| AddressBar | HBox | [include/duilib/Control/AddressBar.h](../include/duilib/Control/AddressBar.h) | Address bar control, used to display paths of the local file system |

* Global resources

| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| GlobalManager | [include/duilib/Core/GlobalManager.h](../include/duilib/Core/GlobalManager.h) | Global attribute management utility class, used to manage various global attributes, including global styles (global.xml) and language settings |
| IRenderFactory | [include/duilib/Render/IRender.h](../include/duilib/Render/IRender.h) | Management class of the rendering interface, used to create rendering implementation objects such as Font, Pen, Brush, Path, Matrix, Bitmap and Render |
| FontManager | [include/duilib/Core/FontManager.h](../include/duilib/Core/FontManager.h) | Management class of fonts |
| ImageManager | [include/duilib/Core/ImageManager.h](../include/duilib/Core/ImageManager.h) | Management class of images |
| IconManager | [include/duilib/Core/IconManager.h](../include/duilib/Core/IconManager.h) | Icon resource manager (thread-safe, suitable for small icon-type image resources), supports HICON handles on Windows |
| ZipManager | [include/duilib/Core/ZipManager.h](../include/duilib/Core/ZipManager.h) | ZIP archive manager |
| DpiManager | [include/duilib/Core/DpiManager.h](../include/duilib/Core/DpiManager.h) | DPI manager, used to support features such as DPI adaptation |
| TimerManager | [include/duilib/Core/TimerManager.h](../include/duilib/Core/TimerManager.h) | Timer manager |
| LangManager | [include/duilib/Core/LangManager.h](../include/duilib/Core/LangManager.h) | Multi-language support manager |
| CursorManager | [include/duilib/Core/CursorManager.h](../include/duilib/Core/CursorManager.h) | Cursor manager |
| ThreadManager | [include/duilib/Core/ThreadManager.h](../include/duilib/Core/ThreadManager.h) | Thread manager |
| ColorManager | [include/duilib/Core/ColorManager.h](../include/duilib/Core/ColorManager.h) | Color manager |
| WindowManager | [include/duilib/Core/WindowManager.h](../include/duilib/Core/WindowManager.h) | Window manager |
| ImageDecoderFactory | [include/duilib/Image/ImageDecoderFactory.h](../include/duilib/Image/ImageDecoderFactory.h) | Management of image decoders |

* libcef control wrapper related

| Class Name | Associated Header File| Purpose |
| :--- | :--- | :--- |
| CefManager | [include/duilib/CEFControl/CefManager.h](../include/duilib/CEFControl/CefManager.h) | CEF control manager, responsible for the initialization and uninitialization of the CEF module |
| CefControl | [include/duilib/CEFControl/CefControl.h](../include/duilib/CEFControl/CefControl.h) | CEF control interface, providing basic web browsing functionality and event acceptance |
| CefControlEvent | [include/duilib/CEFControl/CefControlEvent.h](../include/duilib/CEFControl/CefControlEvent.h) | Event reception interface for web browsing of the CEF control |
| CefControlNative | [include/duilib/CEFControl/CefControlNative.h](../include/duilib/CEFControl/CefControlNative.h) | Wrapper of the windowed mode of the CEF control |
| CefControlOffScreen | [include/duilib/CEFControl/CefControlOffScreen.h](../include/duilib/CEFControl/CefControlOffScreen.h) | Wrapper of the off-screen rendering mode of the CEF control |
