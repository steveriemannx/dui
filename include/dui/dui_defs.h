#ifndef DUI_DEFS_H_
#define DUI_DEFS_H_

#include "dui/dui_config.h"
#include <vector>

#define DUI_NOSET_VALUE        -1

namespace ui
{
    //Defines all control types
    #define  DUI_CTR_CONTROL                         (DUI_T("Control"))
    #define  DUI_CTR_CONTROL_DRAGABLE                (DUI_T("ControlDragable"))
    #define  DUI_CTR_CONTROL_MOVABLE                 (DUI_T("ControlMovable"))
    #define  DUI_CTR_CONTROL_RESIZABLE               (DUI_T("ControlResizable"))
    #define  DUI_CTR_LABEL                           (DUI_T("Label"))
    #define  DUI_CTR_RICHTEXT                        (DUI_T("RichText"))
    #define  DUI_CTR_HYPER_LINK                      (DUI_T("HyperLink"))
    #define  DUI_CTR_BUTTON                          (DUI_T("Button"))
    #define  DUI_CTR_OPTION                          (DUI_T("Option"))
    #define  DUI_CTR_CHECKBOX                        (DUI_T("CheckBox"))
    #define  DUI_CTR_SPLIT                           (DUI_T("Split"))

    #define  DUI_CTR_LABELBOX                        (DUI_T("LabelBox"))
    #define  DUI_CTR_LABELHBOX                       (DUI_T("LabelHBox"))
    #define  DUI_CTR_LABELVBOX                       (DUI_T("LabelVBox"))

    #define  DUI_CTR_RICHTEXT_BOX                    (DUI_T("RichTextBox"))
    #define  DUI_CTR_RICHTEXT_HBOX                   (DUI_T("RichTextHBox"))
    #define  DUI_CTR_RICHTEXT_VBOX                   (DUI_T("RichTextVBox"))

    #define  DUI_CTR_BUTTONBOX                       (DUI_T("ButtonBox"))
    #define  DUI_CTR_BUTTONHBOX                      (DUI_T("ButtonHBox"))
    #define  DUI_CTR_BUTTONVBOX                      (DUI_T("ButtonVBox"))

    #define  DUI_CTR_CHECKBOXBOX                     (DUI_T("CheckBoxBox"))
    #define  DUI_CTR_CHECKBOXHBOX                    (DUI_T("CheckBoxHBox"))
    #define  DUI_CTR_CHECKBOXVBOX                    (DUI_T("CheckBoxVBox"))

    #define  DUI_CTR_OPTIONBOX                       (DUI_T("OptionBox"))
    #define  DUI_CTR_OPTIONHBOX                      (DUI_T("OptionHBox"))
    #define  DUI_CTR_OPTIONVBOX                      (DUI_T("OptionVBox"))

    #define  DUI_CTR_SPLITBOX                        (DUI_T("SplitBox"))

    #define  DUI_CTR_BOX                             (DUI_T("Box"))
    #define  DUI_CTR_HBOX                            (DUI_T("HBox"))
    #define  DUI_CTR_VBOX                            (DUI_T("VBox"))
    #define  DUI_CTR_HFLOWBOX                        (DUI_T("HFlowBox"))
    #define  DUI_CTR_VFLOWBOX                        (DUI_T("VFlowBox"))
    #define  DUI_CTR_XMLBOX                          (DUI_T("XmlBox"))

    #define  DUI_CTR_VTILE_BOX                       (DUI_T("VTileBox"))
    #define  DUI_CTR_HTILE_BOX                       (DUI_T("HTileBox"))

    #define  DUI_CTR_BOX_DRAGABLE                    (DUI_T("BoxDragable"))
    #define  DUI_CTR_HBOX_DRAGABLE                   (DUI_T("HBoxDragable"))
    #define  DUI_CTR_VBOX_DRAGABLE                   (DUI_T("VBoxDragable"))

    #define  DUI_CTR_BOX_MOVABLE                     (DUI_T("BoxMovable"))
    #define  DUI_CTR_HBOX_MOVABLE                    (DUI_T("HBoxMovable"))
    #define  DUI_CTR_VBOX_MOVABLE                    (DUI_T("VBoxMovable"))

    #define  DUI_CTR_BOX_RESIZABLE                   (DUI_T("BoxResizable"))
    #define  DUI_CTR_HBOX_RESIZABLE                  (DUI_T("HBoxResizable"))
    #define  DUI_CTR_VBOX_RESIZABLE                  (DUI_T("VBoxResizable"))

    #define  DUI_CTR_GROUP_BOX                       (DUI_T("GroupBox"))
    #define  DUI_CTR_GROUP_HBOX                      (DUI_T("GroupHBox"))
    #define  DUI_CTR_GROUP_VBOX                      (DUI_T("GroupVBox"))

    #define  DUI_CTR_SCROLLBOX                       (DUI_T("ScrollBox"))
    #define  DUI_CTR_HSCROLLBOX                      (DUI_T("HScrollBox"))
    #define  DUI_CTR_VSCROLLBOX                      (DUI_T("VScrollBox"))
    #define  DUI_CTR_HFLOW_SCROLLBOX                 (DUI_T("HFlowScrollBox"))
    #define  DUI_CTR_VFLOW_SCROLLBOX                 (DUI_T("VFlowScrollBox"))
    #define  DUI_CTR_HTILE_SCROLLBOX                 (DUI_T("HTileScrollBox"))
    #define  DUI_CTR_VTILE_SCROLLBOX                 (DUI_T("VTileScrollBox"))

    #define  DUI_CTR_LISTBOX_ITEM                    (DUI_T("ListBoxItem"))
    #define  DUI_CTR_HLISTBOX                        (DUI_T("HListBox"))
    #define  DUI_CTR_VLISTBOX                        (DUI_T("VListBox"))
    #define  DUI_CTR_HTILE_LISTBOX                   (DUI_T("HTileListBox"))
    #define  DUI_CTR_VTILE_LISTBOX                   (DUI_T("VTileListBox"))

    #define  DUI_CTR_LISTCTRL                        (DUI_T("ListCtrl"))
    #define  DUI_CTR_PROPERTY_GRID                   (DUI_T("PropertyGrid"))

    #define  DUI_CTR_VIRTUAL_VLISTBOX                (DUI_T("VirtualVListBox"))
    #define  DUI_CTR_VIRTUAL_HLISTBOX                (DUI_T("VirtualHListBox"))
    #define  DUI_CTR_VIRTUAL_HTILE_LISTBOX           (DUI_T("VirtualHTileListBox"))
    #define  DUI_CTR_VIRTUAL_VTILE_LISTBOX           (DUI_T("VirtualVTileListBox"))

    #define  DUI_CTR_TABBOX                          (DUI_T("TabBox"))
    #define  DUI_CTR_GRIDBOX                         (DUI_T("GridBox"))
    #define  DUI_CTR_GRID_SCROLLBOX                  (DUI_T("GridScrollBox"))

    #define  DUI_CTR_TREENODE                        (DUI_T("TreeNode"))
    #define  DUI_CTR_TREEVIEW                        (DUI_T("TreeView"))
    #define  DUI_CTR_DIRECTORY_TREE                  (DUI_T("DirectoryTree"))

    #define  DUI_CTR_RICHEDIT                        (DUI_T("RichEdit"))
    #define  DUI_CTR_COMBO                           (DUI_T("Combo"))
    #define  DUI_CTR_COMBO_BUTTON                    (DUI_T("ComboButton"))
    #define  DUI_CTR_FILTER_COMBO                    (DUI_T("FilterCombo"))
    #define  DUI_CTR_CHECK_COMBO                     (DUI_T("CheckCombo"))
    #define  DUI_CTR_SLIDER                          (DUI_T("Slider"))
    #define  DUI_CTR_PROGRESS                        (DUI_T("Progress"))
    #define  DUI_CTR_CIRCLEPROGRESS                  (DUI_T("CircleProgress"))
    #define  DUI_CTR_SCROLLBAR                       (DUI_T("ScrollBar"))

    #define  DUI_CTR_MENU                            (DUI_T("Menu"))
    #define  DUI_CTR_SUB_MENU                        (DUI_T("SubMenu"))
    #define  DUI_CTR_MENU_ITEM                       (DUI_T("MenuItem"))
    #define  DUI_CTR_MENU_LISTBOX                    (DUI_T("MenuListBox"))
    #define  DUI_CTR_MENU_BAR                        (DUI_T("MenuBar"))

    #define  DUI_CTR_DATETIME                        (DUI_T("DateTime"))
    #define  DUI_CTR_CEF                             (DUI_T("CefControl"))
    #define  DUI_CTR_WEBVIEW2                        (DUI_T("WebView2Control"))

    //Color-related controls
    #define  DUI_CTR_COLOR_CONTROL                   (DUI_T("ColorControl"))
    #define  DUI_CTR_COLOR_SLIDER                    (DUI_T("ColorSlider"))
    #define  DUI_CTR_COLOR_PICKER_REGULAR            (DUI_T("ColorPickerRegular"))
    #define  DUI_CTR_COLOR_PICKER_STANDARD           (DUI_T("ColorPickerStatard"))
    #define  DUI_CTR_COLOR_PICKER_STANDARD_GRAY      (DUI_T("ColorPickerStatardGray"))
    #define  DUI_CTR_COLOR_PICKER_CUSTOM             (DUI_T("ColorPickerCustom"))

    #define  DUI_CTR_LINE                            (DUI_T("Line"))
    #define  DUI_CTR_IPADDRESS                       (DUI_T("IPAddress"))
    #define  DUI_CTR_HOTKEY                          (DUI_T("HotKey"))
    #define  DUI_CTR_TAB_CTRL                        (DUI_T("TabCtrl"))
    #define  DUI_CTR_TAB_CTRL_ITEM                   (DUI_T("TabCtrlItem"))    
    #define  DUI_CTR_ADDRESS_BAR                     (DUI_T("AddressBar"))
    #define  DUI_CTR_ICON_CONTROL                    (DUI_T("IconControl"))
    #define  DUI_CTR_BITMAP_CONTROL                  (DUI_T("BitmapControl"))
    #define  DUI_CTR_CHILD_WINDOW                    (DUI_T("ChildWindow"))

    //Window title bar buttons: names of maximize, minimize, close, restore and fullscreen buttons, hard-coded
    #define  DUI_CTR_CAPTION_BAR                     (DUI_T("window_caption_bar"))
    #define  DUI_CTR_CAPTION_TITLE                   (DUI_T("window_caption_title"))
    #define  DUI_CTR_BUTTON_CLOSE                    (DUI_T("closebtn"))
    #define  DUI_CTR_BUTTON_MIN                      (DUI_T("minbtn"))
    #define  DUI_CTR_BUTTON_MAX                      (DUI_T("maxbtn"))
    #define  DUI_CTR_BUTTON_RESTORE                  (DUI_T("restorebtn"))
    #define  DUI_CTR_BUTTON_FULLSCREEN               (DUI_T("fullscreenbtn"))

    class Control;
    class Image;

    //Horizontal alignment
    enum class HorAlignType: int8_t
    {
        kAlignLeft   = 0,    // Left aligned
        kAlignCenter = 1,    // Horizontally centered
        kAlignRight  = 2     // Right aligned
    };

    //Vertical alignment
    enum class VerAlignType: int8_t
    {
        kAlignTop    = 0,    // Top aligned
        kAlignCenter = 1,    // Vertically centered
        kAlignBottom = 2     // Bottom aligned
    };

    //Control image type
    enum StateImageType: int8_t
    {
        kStateImageBk               = 0,    //Background image (used by Control)
        kStateImageFore             = 1,    //Foreground image (used by Control)
        kStateImageSelectedBk       = 2,    //Background image for the selected state (used by CheckBox)
        kStateImageSelectedFore     = 3,    //Foreground image for the selected state (used by CheckBox)
        kStateImagePartSelectedBk   = 4,    //Background image for the partial selection state (used by CheckBox)
        kStateImagePartSelectedFore = 5,    //Foreground image for the partial selection state (used by CheckBox)
    };

    //Control state
    enum ControlStateType: int8_t
    {
        kControlStateNormal     = 0,    // Normal state
        kControlStateHot        = 1,    // Hover state
        kControlStatePushed     = 2,    // Pushed state
        kControlStateDisabled   = 3,    // Disabled state

        kControlStateCount      = 4     // The number of states
    };

    //Control animation type
    enum class AnimationType: int8_t
    {
        kAnimationNone              = 0,    //No animation
        kAnimationHot               = 1,    //Mouse hover state animation
        kAnimationAlpha             = 2,    //Opacity gradient animation
        kAnimationHeight            = 3,    //Control height change animation (height must not be a stretch type)
        kAnimationWidth             = 4,    //Control width change animation (width must not be a stretch type)
        kAnimationSize              = 5,    //Control size change animation (width and height must not be stretch types)
        kAnimationInoutXFromLeft    = 6,    //Control X coordinate change animation, from the left
        kAnimationInoutXFromRight   = 7,    //Control X coordinate change animation, from the right
        kAnimationInoutYFromTop     = 8,    //Control Y coordinate change animation, from the top
        kAnimationInoutYFromBottom  = 9,    //Control Y coordinate change animation, from the bottom
    };

    //Image animation frame type
    enum class AnimationImagePos: int8_t
    {
        kFrameFirst   = 0,   // The first frame    
        kFrameCurrent = 1,   // The current frame        
        kFrameLast    = 2    // The last frame
    };

    //Image animation playback state
    struct ImageAnimationStatus
    {
        //Image name: the value of the name field in the image attribute in the XML, used to identify the image resource
        DString m_name;

        //Whether the current animated image is a background image
        bool m_bBkImage;

        //The total number of image frames
        int32_t m_nFrameCount;

        //The current frame index of the playing image (starting from 0)
        int32_t m_nFrameIndex;

        //The duration of the current frame, in milliseconds
        int32_t m_nFrameDelayMs;

        //The number of loop plays (a positive integer means that many loops; -1 means loop forever)
        int32_t m_nLoopCount;
    };

    //Cursor: on Windows, see https://learn.microsoft.com/windows/win32/menurc/about-cursors
    enum class CursorType : uint8_t
    {
        kCursorArrow    = 0,    // Arrow, normal selection, XML name: "arrow"
        kCursorIBeam    = 1,    // "I" shape, text selection, XML name: "ibeam"
        kCursorHand     = 2,    // Hand, link selection, XML name: "hand"
        kCursorWait     = 3,    // Busy, XML name: "wait"
        kCursorCross    = 4,    // Crosshair, XML name: "cross"
        kCursorSizeWE   = 5,    // Horizontal resize, XML name: "size_we"
        kCursorSizeNS   = 6,    // Vertical resize, XML name: "size_ns"
        kCursorSizeNWSE = 7,    // Diagonal resize, northwest-southeast direction 1, XML name: "size_nwse"
        kCursorSizeNESW = 8,    // Diagonal resize, northeast-southwest direction 2, XML name: "size_nesw"
        kCursorSizeAll  = 9,    // Move, four-direction resize, XML name: "size_all"
        kCursorNo       = 10,   // Forbidden cursor, XML name: "no"
        kCursorProgress = 11    // Progress, application start cursor, XML name: "progress"
    };

    //The window exit parameter
    enum WindowCloseParam
    {
        kWindowCloseNormal  = 0,    //Indicates clicking the "Close" button closes this window (default)
        kWindowCloseOK      = 1,    //Indicates clicking the "OK" button closes this window
        kWindowCloseCancel  = 2     //Indicates clicking the "Cancel" button closes this window
    };

    //The drag-and-drop type
    enum ControlDropType
    {
        kControlDropTypeWindows = 0, //Indicates an event from the ControlDropTarget_Windows interface
        kControlDropTypeSDL     = 1, //Indicates an event from the ControlDropTarget_SDL interface
    };

    //Windows platform drop data: for the related values, see the IDropTarget interface declaration
    struct ControlDropData_Windows
    {
        void* m_pDataObj;       //IDataObject*
        uint32_t m_grfKeyState; //The keyboard state
        int32_t m_ptClientX;    //The X coordinate of the mouse position, in client coordinates
        int32_t m_ptClientY;    //The Y coordinate of the mouse position, in client coordinates
        uint32_t m_dwEffect;    //The parameter return value
        int32_t m_hResult;      //The function return value
        bool m_bHandled;        //Whether the event has been fully handled; returning true means it was handled and is not dispatched to UI controls
        std::vector<DString> m_textList;    //The text content contained in m_pDataObj; each element represents one line
        std::vector<DString> m_fileList;    //The text content contained in m_pDataObj; each element represents a file path
    };

    //SDL drag-and-drop data
    struct ControlDropData_SDL
    {
        bool m_bHandled;                    //Whether the event has been fully handled; returning true means it was handled and is not dispatched to UI controls
        bool m_bTextData;                   // true means m_textList holds the valid data; false means m_fileList does
        int32_t m_ptClientX;                // The X coordinate of the mouse position, in client coordinates
        int32_t m_ptClientY;                // The Y coordinate of the mouse position, in client coordinates

        std::vector<DString> m_textList;    //The text content included in the drag-and-drop operation; each element represents one line

        DString m_source;                   // Valid when m_bTextData is false
        std::vector<DString> m_fileList;    //The text content included in the drag-and-drop operation; each element represents a file path
    };

    //The DPI of the screen containing the window changed; related data
    struct WindowDisplayScaleData
    {
        float m_fNewDisplayScale;   //The new window UI display scale; 1.0f means no scaling
        float m_fNewPixelDensity;   //The new window pixel density (only used in the SDL implementation)
    };

    //The resolution of the screen containing the window changed; related data
    struct WindowDisplayResolutionData
    {
        int32_t m_nColorDepth;      //The new display color depth, in bits per pixel
        int32_t m_nScreenWidth;     //The horizontal resolution of the screen
        int32_t m_nScreenHeight;    //The vertical resolution of the screen
    };

    //The image loading or decoding result
    struct ImageDecodeResult
    {
        Control* m_pControl;    //the control associated with the image
        Image* m_pImage;        //the image resource interface
        DString m_imageFilePath;//The image path
        DString m_imageName;    //Image name, unique ID
        bool m_bBkImage;        //Whether the image is a background image
        bool m_bLoadError;      //Whether the image has a loading error
        bool m_bDecodeError;    //Whether the image has a data decoding error
    };

    //Control/window related message types (note: when the type definitions change, besides the window messages, the InitEventStringMap function in EventArgs.cpp must be updated to keep the message type/name mapping in sync)
    enum EventType: uint8_t
    {
        /** Control-related messages
        */
        kEventNone,                 //Empty message, the starting value of control messages
        kEventAll,                  //Represents all control messages (no parameter associated data)
        kEventDestroy,              //Control destruction (the last message in the control lifecycle)

        //Keyboard message
        kEventKeyBegin,
        kEventKeyDown,              //Window: triggered when the WM_KEYDOWN message is received, sent to the focus control
        kEventKeyUp,                //Window: triggered when the WM_KEYUP message is received, sent to the focus control of the WM_KEYDOWN event
        kEventChar,                 //Window: triggered when the WM_CHAR message is received, sent to the focus control of the WM_KEYDOWN event
        kEventKeyEnd,

        //Mouse message
        kEventMouseBegin,        
        kEventMouseEnter,           //Window: triggered when the WM_MOUSEMOVE message is received, sent to the control the mouse entered on MouseMove, indicating the mouse entered this control
        kEventMouseLeave,           //Window: triggered when the WM_MOUSEMOVE message is received, sent to the control the mouse entered on MouseMove, indicating the mouse has left this control
        kEventMouseMove,            //Window: triggered when the WM_MOUSEMOVE message is received, sent to the control recorded on left/right click or double-click
        kEventMouseHover,           //Window: triggered when the WM_MOUSEHOVER message is received, sent to the control the mouse entered on MouseMove, indicating the mouse is hovering over this control
        kEventMouseWheel,           //Window: triggered when the WM_MOUSEWHEEL message is received, sent to the control at the current mouse position
        kEventMouseButtonDown,      //Window: triggered when the WM_LBUTTONDOWN message is received, sent to the control where the left button was pressed
        kEventMouseButtonUp,        //Window: triggered when the WM_LBUTTONUP message is received, sent to the control where the left button was pressed
        kEventMouseDoubleClick,     //Window: triggered when the WM_LBUTTONDBLCLK message is received, sent to the control at the current mouse position        
        kEventMouseRButtonDown,     //Window: triggered when the WM_RBUTTONDOWN message is received, sent to the control where the right button was pressed
        kEventMouseRButtonUp,       //Window: triggered when the WM_RBUTTONUP message is received, sent to the control where the right button was pressed
        kEventMouseRDoubleClick,    //Window: triggered when the WM_RBUTTONDBLCLK message is received, sent to the control at the current mouse position
        kEventMouseMButtonDown,     //Window: triggered when the WM_MBUTTONDOWN message is received, sent to the control where the middle button was pressed
        kEventMouseMButtonUp,       //Window: triggered when the WM_MBUTTONUP message is received, sent to the control where the middle button was pressed
        kEventMouseMDoubleClick,    //Window: triggered when the WM_MBUTTONDBLCLK message is received, sent to the control at the current mouse position
        kEventContextMenu,          //Window: triggered when the WM_CONTEXTMENU message is received, sent to all controls with registered callbacks
        kEventMouseEnd,

        //Focus/cursor/input method related messages
        kEventSetFocus,             //Window: sent to the focus control when the control gains focus (control focus is not window focus; they are completely different)
        kEventKillFocus,            //Window: sent to the focus control when the control loses focus (control focus is not window focus; they are completely different)        
        kEventSetCursor,            //Window: sent to the control under the mouse to set the cursor
        kEventCaptureChanged,       //Window: sent to the focus control when the WM_CAPTURECHANGED message is received
        kEventImeSetContext,        //Window: sent to the focus control when the system WM_IME_SETCONTEXT message is received
        kEventImeStartComposition,  //Window: sent to the focus control when the system WM_IME_STARTCOMPOSITION message is received        
        kEventImeComposition,       //Window: sent to the focus control when the system WM_IME_COMPOSITION message is received
        kEventImeEndComposition,    //Window: sent to the focus control when the system WM_IME_ENDCOMPOSITION message is received

        kEventWindowSetFocus,       //Window: sent to the focus control when the window gains focus; the callback sent to the receiver (wParam: the handle of the window that lost keyboard focus)
        kEventWindowKillFocus,      //Window: sent to the controls recorded on left/right button down and to the focus control when the window loses focus (mainly to restore internal state); the callback sent to the receiver (wParam: the handle of the window receiving keyboard focus)
        kEventWindowPosChanged,     //Window: sent to the focus control when the window position/size changes
        kEventWindowSize,           //Window: sent to the focus control when the window size changes
        kEventWindowMove,           //Window: sent to the focus control when the window position changes        
        kEventWindowCreate,         //Window: triggered when the window creation completes; wParam 1 means a DoModal dialog, 0 means a normal window
        kEventWindowClose,          //Window, Combo control: triggered when the window closes (or the Combo drop-down window closes); wParam is the window close parameter, see enum WindowCloseParam

        //Left click / right click events
        kEventClick,                //Button, ListBoxItem, Option, CheckBox, etc.: triggered when the button is clicked (or Enter is pressed)
        kEventRClick,               //Control: triggered when the right mouse button is released

        //The mouse-click control changed; sent to the old mouse-click event
        kEventMouseClickChanged,    //Window: triggered when a mouse button-down message is received, sent to the control of the last mouse press
        kEventMouseClickEsc,        //Window: triggered when the ESC key message is received, sent to the control of the mouse press

        //Select/deselect
        kEventSelect,               //CheckBox, Option, ListBox, Combo, ListCtrl: triggered when they become selected
        kEventUnSelect,             //CheckBox, Option, ListBox, Combo: triggered when they become unselected

        //Check/uncheck
        kEventCheck,                //CheckBox: triggered when it becomes Checked
        kEventUnCheck,              //CheckBox: triggered when it becomes UnChecked

        //TAB page selection
        kEventTabSelect,            //TabBox: a TAB page was selected; WPARAM is the new page ID, LPARAM is the old page ID

        //Tree node expand/collapse
        kEventExpand,               //TreeNode: triggered when a tree node expands
        kEventCollapse,             //TreeNode: triggered when a tree node collapses

        kEventZoom,                 //RichEdit: triggered when the zoom level changes; wParam is the zoom percentage, e.g. 200 means 200%
        kEventTextChanged,          //RichEdit: triggered when the text content changes
        kEventSelChanged,           //ListCtrl: the selection changed; RichEdit: triggered when the text selection changes
        kEventReturn,               //ListCtrl, VirtualListBox, ListBoxItem, RichEdit, AddressBar: triggered when the Enter key is received
        kEventEsc,                  //RichEdit, AddressBar: triggered when the ESC key is received
        kEventTab,                  //RichEdit: triggered when the TAB key is received while WantTab is false
        kEventLinkClick,            //RichEdit, RichText: triggered when clicking on hyperlink data; the clicked URL can be obtained from WPARAM, of type const DStringW::value_type*

        kEventScrollPosChanged,     //ScrollBox: triggered when the scrollbar position changes
        kEventValueChanged,         //DateTime and Slider: triggered when the value changes. For Slider, WPARAM is the new value and LPARAM is the old value
        kEventPosChanged,           //Control: triggered when the control position changes
        kEventSizeChanged,          //Control: triggered when the control size changes
        kEventVisibleChanged,       //Control: triggered when the Visible property changes. WPARAM is the new state (1 visible, 0 hidden)
        kEventStateChanged,         //Control: triggered when the State property changes. WPARAM is the new state, LPARAM is the old state
        kEventSelectColor,          //ColorPicker: triggered when a new color is selected. WPARAM is the new color value, LPARAM is the old color value

        kEventSplitDraged,          //Split: the control size was adjusted by dragging; WPARAM is the first control interface, LPARAM is the second

        kEventElementFilled,            //VirtualListBox: UI element fill event for view data items (virtual list); wParam is a Control* pointer, lParam is the data element index

        kEventEnterEdit,                //ListCtrl: entering edit state; wParam is an interface: ListCtrlEditParam*
        kEventLeaveEdit,                //ListCtrl: leaving edit state; wParam is an interface: ListCtrlEditParam*
        kEventDataItemCountChanged,     //ListCtrl: the number of data items changed; wParam is the new count (size_t), lParam is the old count (size_t)

        kEventItemMouseEnter,           //ListCtrl/VirtualListBox: mouse enter event on a ListCtrlItem/ListCtrlListViewItem/ListCtrlIconViewItem/ListBoxItem control
        kEventItemMouseLeave,           //ListCtrl/VirtualListBox: mouse leave event from a ListCtrlItem/ListCtrlListViewItem/ListCtrlIconViewItem/ListBoxItem control
        kEventSubItemMouseEnter,        //ListCtrl: Report view, mouse enter on a ListCtrlSubItem control
        kEventSubItemMouseLeave,        //ListCtrl: Report view, mouse leave from a ListCtrlSubItem control
        kEventReportViewItemFilled,     //ListCtrl: UI element fill event for view data items (virtual list), data of a specific row
        kEventReportViewSubItemFilled,  //ListCtrl: UI element fill event for view data items (virtual list), data of a specific row and column
        kEventListViewItemFilled,       //ListCtrl: UI element fill event for view data items (virtual list)
        kEventIconViewItemFilled,       //ListCtrl: UI element fill event for view data items (virtual list)
        kEventViewTypeChanged,          //ListCtrl: triggered when the view type changes; wParam is the current view type, lParam is the previous view type (see listCtrlType enum), pEventData is the view pointer
        kEventViewPosChanged,           //ListCtrl: triggered when the position of the view control changes; see listCtrlType for the view type
        kEventViewSizeChanged,          //ListCtrl: triggered when the size of the view control changes; see listCtrlType for the view type

        kEventPathChanged,          //AddressBar: triggered when the path mode changes
        kEventPathClick,            //AddressBar: triggered when the user clicks a path button

        kEventDropEnter,            //wParam is ControlDropType, representing the source type, 
                                    //lParam represents the associated data: when wParam is kControlDropTypeWindows, lParam is a pointer to ControlDropData_Windows
        kEventDropOver,             //wParam is ControlDropType, representing the source type, 
                                    //lParam represents the associated data: when wParam is kControlDropTypeWindows, lParam is a pointer to ControlDropData_Windows
        kEventDropLeave,            //No additional description
        kEventDropData,             //wParam is ControlDropType, representing the source type, 
                                    //lParam represents the associated data: when wParam is kControlDropTypeWindows, lParam is a pointer to ControlDropData_Windows
                                    //                   When wParam is kControlDropTypeSDL, lParam is a pointer to ControlDropData_SDL

        kEventImageAnimationStart,      // Starts playing the image animation (background image): wParam is a data pointer: ui::ImageAnimationStatus*
        kEventImageAnimationPlayFrame,  // A frame of the image animation is played (background image): wParam is a data pointer: ui::ImageAnimationStatus*
        kEventImageAnimationStop,       // Stops playing the image animation (background image): wParam is a data pointer: ui::ImageAnimationStatus*

                                        //ui::ControlLoadingStatus is defined in "dui/Core/ControlLoading.h"
        kEventLoadingStart,             //The control enters the loading state and becomes Disabled: wParam is a data pointer: ui::ControlLoadingStatus*
        kEventLoading,                  //The control is in the loading state, periodic callback:             wParam is a data pointer: ui::ControlLoadingStatus*
        kEventLoadingStop,              //The control leaves the loading state and becomes Enabled: wParam is a data pointer: ui::ControlLoadingStatus*

        kEventImageLoad,                //Image load complete event; wParam is a data pointer: ui::ImageDecodeResult*
        kEventImageDecode,              //Image decode complete event; wParam is a data pointer: ui::ImageDecodeResult*

        kEventLast,                     //The last message of the control

        /** Window-related events; these events are not sent to any control, but directly to the application layer
        */
        kWindowMsgBegin,            //The start of window messages
        kWindowCreateMsg,           //Window create message
        kWindowCloseMsg,            //Window close message
        kWindowShowWindowMsg,       //Window show or hide
        kWindowFirstShown,          //Calls back this event the first time the window is shown (the callback must be set before the UI is displayed, i.e. while IsWindowFirstShown() returns false, otherwise there is no chance to set it later)
        kWindowPaintMsg,            //Window drawing
        kWindowEnterFullscreenMsg,  //The window entered fullscreen
        kWindowExitFullscreenMsg,   //The window exited fullscreen
        kWindowMaximizedMsg,        //The window entered the maximized state
        kWindowMinimizedMsg,        //The window entered the minimized state
        kWindowRestoredMsg,         //The window entered the restored state
        kWindowPosChangedMsg,       //The window position and size changed
        kWindowSizeMsg,             //The window size changed
        kWindowMoveMsg,             //The window position changed
        kWindowDisplayScaleChangedMsg,      //The DPI attribute of the window changed
        kWindowDisplayResolutionChangedMsg, //The resolution of the screen containing the window changed
        kWindowSetFocusMsg,         //The window gained focus
        kWindowKillFocusMsg,        //The window lost focus
        kWindowSetCursorMsg,        //The window sets the cursor
        kWindowKeyDownMsg,          //Window key down
        kWindowKeyUpMsg,            //Window key up
        kWindowMouseWheelMsg,       //Window mouse wheel message
        kWindowMouseMoveMsg,        //Window mouse move message
        kWindowMouseHoverMsg,       //Window mouse hover message
        kWindowMouseLeaveMsg,       //Window mouse leave message
        kWindowLButtonDownMsg,      //Window mouse left button down message
        kWindowLButtonUpMsg,        //Window mouse left button up message
        kWindowLButtonDbClickMsg,   //Window mouse left button double-click message
        kWindowMButtonDownMsg,      //Window mouse middle button down message
        kWindowMButtonUpMsg,        //Window mouse middle button up message
        kWindowMButtonDbClickMsg,   //Window mouse middle button double-click message
        kWindowRButtonDownMsg,      //Window mouse right button down message
        kWindowRButtonUpMsg,        //Window mouse right button up message
        kWindowRButtonDbClickMsg,   //Window mouse right button double-click message
        kWindowCaptureChangedMsg,   //The window lost mouse capture
        kWindowDropEnterMsg,        //Window drag-and-drop: drag in; wParam is ControlDropType, representing the source type
                                    //                  When wParam is kControlDropTypeWindows, lParam is a pointer to ControlDropData_Windows
                                    //                  When wParam is kControlDropTypeSDL, lParam is a pointer to ControlDropData_SDL
        kWindowDropOverMsg,         //Window drag-and-drop: drag over; wParam is ControlDropType, representing the source type
                                    //                  When wParam is kControlDropTypeWindows, lParam is a pointer to ControlDropData_Windows
                                    //                  When wParam is kControlDropTypeSDL, lParam is a pointer to ControlDropData_SDL
        kWindowDropMsg,             //Window drag-and-drop: drop operation; wParam is ControlDropType, representing the source type
                                    //                  When wParam is kControlDropTypeWindows, lParam is a pointer to ControlDropData_Windows
                                    //                  When wParam is kControlDropTypeSDL, lParam is a pointer to ControlDropData_SDL
        kWindowDropLeaveMsg,        //Window drag-and-drop: leave, no parameters
        kWindowMsgEnd               //The end of window messages
    };

    /** The hotkey modifier flags
    */
    enum HotKeyModifiers
    {
        kHotKey_Shift   = 0x01, //Shift key
        kHotKey_Contrl  = 0x02, //Control key
        kHotKey_Alt     = 0x04, //Alt key
        kHotKey_Ext     = 0x08  //The extended key
    };

    /** Mouse operation flags
    */
    enum VKFlag
    {
        kVkLButton  = 0x0001, //MK_LBUTTON,     //mouse left button is pressed
        kVkRButton  = 0x0002, //MK_RBUTTON,     //mouse right button is pressed
        kVkShift    = 0x0004, //MK_SHIFT,       //SHIFT key is pressed
        kVkControl  = 0x0008  //MK_CONTROL,     //CTRL key is pressed
    };
}// namespace ui

#endif //DUI_DEFS_H_

