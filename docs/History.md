# History

## Main Modifications Based on NIM_Duilib_Framework Source
<table>
    <tr>
        <th>Category</th>
        <th>Modification content</th>
    </tr>
    <tr>
        <td rowspan="11">Overall improvements</td>
        <td align="left">1. Reorganized the code structure, dividing it by functional module; large files were split into multiple small files by class, making the library's architecture easier to understand</td>
    </tr>
    <tr><td align="left">2. Reorganized the interface files and added comments and feature documentation for each interface, making the code easier to read and understand</td></tr>
    <tr><td align="left">3. Optimized the configuration XML files, adjusted the attribute naming rules; control width and height can now be set as percentages; image attributes were extended; the image loading flow was optimized</td></tr>
    <tr><td align="left">4. Extended image resource support: added APNG/WEBP animations, Lottie JSON animations, PAG animations, and ICO support; optimized the image loading engine and its code implementation logic</td></tr>
    <tr><td align="left">5. Reimplemented the layout code and added common layout schemes, grouped as: float layout, horizontal layout, vertical layout, horizontal flow layout, vertical flow layout, grid layout, horizontal tile layout, vertical tile layout, virtual horizontal list layout, virtual vertical list layout, virtual horizontal tile list layout, virtual vertical tile list layout, making the layout concept easier to understand and extend; see the documentation for details (docs/Box.md)</td></tr>
    <tr><td align="left">6. Replaced the XML parsing engine with the pugixml parser for better performance</td></tr>
    <tr><td align="left">7. Removed the dependency on the base library; the message loop and thread communication functionality are now implemented in-house</td></tr>
    <tr><td align="left">8. Integrated the Skia engine as the default rendering engine</td></tr>
    <tr><td align="left">9. SDL3 support for cross-platform use (Windows, Linux, macOS, and FreeBSD platforms adapted)</td></tr>
    <tr><td align="left">10. Moved the CEF component into the duilib project and upgraded the CEF version (supports libcef 109 for Win7 compatibility; supports libcef 142 for Win10 and above)</td></tr>
    <tr><td align="left">11. Redesigned the image management interfaces and loading flow (Image directory), supporting multi-threaded image loading for better extensibility of other image formats</td></tr>
    <tr>
        <td rowspan="22">Feature improvements</td>
        <td align="left">1. Improved the window class (Window): enhanced DPI adaptation and code tolerance for window messages; the code structure was adjusted</td>
    </tr>
    <tr><td align="left">2. Added new properties to the window class (Window): use_system_caption, snap_layout_menu, sys_menu, sys_menu_rect, icon, providing the option to use the operating system's default title bar; the custom-drawn title bar has functionality similar to the system title bar</td></tr>
    <tr><td align="left">3. Introduced the WindowDropTarget helper class for the window (Window), providing window-based drag-and-drop support</td></tr>
    <tr><td align="left">4. Reorganized resource-management related code so that managing resources such as fonts, colors, and images is easier to understand</td></tr>
    <tr><td align="left">5. Optimized the ListBox control: subdivided into ListBox, VListBox, HListBox, VTileListBox, HTileListBox, VirtualListBox, VirtualVListBox, VirtualHListBox, VirtualVTileListBox, VirtualHTileListBox with more complete functionality; the usability of virtual-list-based ListBox was improved</td></tr>
    <tr><td align="left">6. Optimized the controls associated with Combo (CheckCombo, FilterCombo) for better usability</td></tr>
    <tr><td align="left">7. Improved the editing functionality of the date-time (DateTime) control</td></tr>
    <tr><td align="left">8. Optimized the menu (CMenuWnd) class code and reimplemented menus with a new structure, making controls inside menus fully compatible with the existing container/control system, easier to understand and maintain</td></tr>
    <tr><td align="left">9. Optimized and extended the text editing control (RichEdit), enriching many common features</td></tr>
    <tr><td align="left">10. Optimized and extended the tree control (TreeView), enriching many common features and improving usability</td></tr>
    <tr><td align="left">11. Optimized the global resource (GlobalManager) interfaces so that all resources are managed through this interface, easier to understand and maintain</td></tr>
    <tr><td align="left">12. Between different containers in the same window, controls can support being dragged out of and into containers via property settings</td></tr>
    <tr><td align="left">13. Control background colors support gradients; foreground color functionality was added</td></tr>
    <tr><td align="left">14. Improved multi-language functionality for better dynamic language switching, with the example program examples/MultiLang</td></tr>
    <tr><td align="left">15. Improved DPI awareness: supports four modes — Unaware, SystemAware, PerMonitorAware, PerMonitorAware_V2 — with independent DPI settings and high-DPI adaptation, with the example program examples/DpiAware</td></tr>
    <tr><td align="left">16. Removed the ui_components project; the CEF component code was reorganized and merged into the duilib project; other content was deleted</td></tr>
    <tr><td align="left">17. Optimized the window shadow feature: shadows use SVG images, with a new shadow type property (shadow_type) whose values are: <br> "default", default shadow <br> "big", large shadow, right-angle corners (suitable for normal windows) <br> "big_round", large shadow, rounded corners (suitable for normal windows) <br> "small", small shadow, right-angle corners (suitable for normal windows) <br> "small_round", small shadow, rounded corners (suitable for normal windows) <br> "menu", small shadow, right-angle corners (suitable for popup windows such as menus) <br> "menu_round", small shadow, rounded corners (suitable for popup windows such as menus) <br> "none", no shadow</td></tr>
    <tr><td align="left">18. Added support for APNG/SVG/WEBP/ICO/LOTTIE/PAG image formats</td></tr>
    <tr><td align="left">19. Redesigned the control loading functionality: Box containers display the loading UI, configured through XML files (including animated images), with interaction with the animated images supported</td></tr>
    <tr><td align="left">20. Enhanced the Label text display control: added "justified" text alignment, vertical text support (drawing direction from top to bottom, right to left), and support for setting line spacing and letter spacing</td></tr>
    <tr><td align="left">21. Control supports fullscreen display (implemented via the new Window::SetFullscreenControl function); CEF and WebView2 controls support F11 page fullscreen toggling</td></tr>
    <tr><td align="left">22. Improved control animation details and introduced easing functions; supports setting control animation properties, such as easing function type, total animation duration, and playback interval</td></tr>
    <tr>
        <td rowspan="25">New controls/new containers</td>
        <td align="left">1. GroupBox: group container</td>
    </tr>
    <tr><td align="left">2. HotKey: hotkey control</td></tr>
    <tr><td align="left">3. HyperLink: text with hyperlinks</td></tr>
    <tr><td align="left">4. IPAddress: IP address control</td></tr>
    <tr><td align="left">5. Line: line drawing control</td></tr>
    <tr><td align="left">6. RichText: formatted text (HTML-like format)</td></tr>
    <tr><td align="left">7. Split: split bar control/container</td></tr>
    <tr><td align="left">8. TabCtrl: multi-tab control (like browser tabs)</td></tr>
    <tr><td align="left">9. ListCtrl: list control (Report/Icon/List views)</td></tr>
    <tr><td align="left">10. PropertyGrid: property grid control, supporting text, number, checkbox, font, color, date, IP address, hotkey, file path, folder, and other properties</td></tr>
    <tr><td align="left">11. ColorPicker: color picker in an independent window; its child controls can be used individually as color controls</td></tr>
    <tr><td align="left">12. ComboButton: a button with a dropdown combo box</td></tr>
    <tr><td align="left">13. ShadowWnd: a subclass of WinImplBase, a base class implemented with an attached shadow window; it creates and attaches windows, providing window shadows without the kWS_EX_LAYERED property</td></tr>
    <tr><td align="left">14. DirectoryTree: directory tree control for displaying directories in the file system</td></tr>
    <tr><td align="left">15. AddressBar: address bar control for displaying paths in the local file system</td></tr>
    <tr><td align="left">16. WebView2Control: wraps the basic functionality of the WebView2 control</td></tr>
    <tr><td align="left">17. GridBox/GridScrollBox: controls based on grid layout</td></tr>
    <tr><td align="left">18. HFlowBox/VFlowBox/HFlowScrollBox/VFlowScrollBox: controls based on horizontal and vertical flow layouts</td></tr>
    <tr><td align="left">19. MenuBar: menu bar control</td></tr>
    <tr><td align="left">20. IconControl/BitmapControl: display in-memory small icons and bitmap data</td></tr>
    <tr><td align="left">21. ChildWindow: child window control; the Windows implementation is a native system child window (with the WS_CHILD property); other platforms use SDL popup windows, which are not native child windows because SDL does not support native child windows</td></tr>
    <tr><td align="left">22. ControlDragableT (template class, including the four standard controls ControlDragable/BoxDragable/HBoxDragable/VBoxDragable): supports reordering child controls within the same Box by dragging, and adjusting a control's container by dragging between different Boxes</td></tr>
    <tr><td align="left">23. ControlMovableT (template class, including the four standard controls ControlMovable/BoxMovable/HBoxMovable/VBoxMovable): supports adjusting a control's position by mouse dragging, and adjusting the parent container's position by dragging</td></tr>
    <tr><td align="left">24. ControlResizableT (template class, including the four standard controls ControlResizable/BoxResizable/HBoxResizable/VBoxResizable): supports resizing a control by mouse dragging, similar to resizing a window</td></tr>
    <tr><td align="left">25. XmlBox: a container that can load and preview XML files of the UI library, used to preview the display effects of controls defined in XML files</td></tr>
    <tr>
        <td rowspan="3">Performance optimizations</td>
        <td align="left">1. Optimized the memory usage of Control and its child controls; memory footprint is greatly reduced when there are many UI elements</td>
    </tr>
    <tr><td align="left">2. Optimized the animation drawing flow and merged timer-triggered events, avoiding severe UI lag while playing control animations or animated images</td></tr>
    <tr><td align="left">3. Virtual-list-based ListBox controls and related controls: usability and performance were greatly improved through implementation mechanism optimization</td></tr>
    <tr>
        <td rowspan="14">Example program improvements</td>
        <td align="left">1. examples/ColorPicker: added a color picker example program</td>
    </tr>
    <tr><td align="left">2. examples/ListCtrl: added a list example program demonstrating special list features</td></tr>
    <tr><td align="left">3. examples/render: added a rendering engine example program demonstrating most container, control, and resource management features</td></tr>
    <tr><td align="left">4. examples/TreeView: added a tree control example program demonstrating various tree control features</td></tr>
    <tr><td align="left">5. examples/RichEdit: added a rich text editing control example program demonstrating various rich text editing features</td></tr>
    <tr><td align="left">6. examples/MultiLang: demonstrates dynamic multi-language switching</td></tr>
    <tr><td align="left">7. examples/DpiAware: demonstrates DPI awareness</td></tr>
    <tr><td align="left">8. examples/threads: demonstrates multi-threading</td></tr>
    <tr><td align="left">9. examples/WebView2: demonstrates the WebView2 control</td></tr>
    <tr><td align="left">10. examples/WebView2Browser: demonstrates the WebView2 control (multi-tab)</td></tr>
    <tr><td align="left">11. examples/layout: demonstrates all layouts and containers</td></tr>
    <tr><td align="left">12. examples/ChildWindow: demonstrates the child window control</td></tr>
    <tr><td align="left">13. examples/XmlPreview: tests the preview of XML file UI effects (tests the XmlBox container)</td></tr>
    <tr><td align="left">14. Other example programs: most received code compatibility fixes and optimizations so they can also be used as test programs</td></tr>
    <tr>
        <td rowspan="4">Documentation improvements</td>
        <td align="left">1. Reorganized the README.md and docs subdirectory documentation so readers can more easily understand the UI library's features and usage and get started more easily</td>
    </tr>
    <tr><td align="left">2. The control interfaces are not documented separately, because reading the comments in the interface files serves the purpose; the interface comments are currently fairly complete</td></tr>
    <tr><td align="left">3. Build documentation and dependency build scripts for each platform</td></tr>
    <tr><td align="left">4. Reorganized the license files of the main project and third-party source dependencies, centrally managed in the licenses directory</td></tr>
</table>

## Development Plans
 - Continue enriching the UI library's controls and improving its features
 - Continuous testing and refinement of the cross-platform (Windows/Linux/macOS/FreeBSD) window engine based on [SDL3.0](https://www.libsdl.org/) (currently stable under X11/XWayland desktop environments, but with more problems under pure Wayland)
 - Test the UI library, find and fix defects, and continuously improve the code
