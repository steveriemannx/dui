## Control Events (EventArgs) Documentation
The event-related content of controls is defined in the files [`include/dui/Core/EventArgs.h`](../include/dui/Core/EventArgs.h) and [`include/dui/dui_defs.h`](../include/dui/dui_defs.h).

| Event (eventType)           | Parameter (wParam)  | Parameter (lParam) | Parameter (ptMouse) | Parameter (vkCode) | Parameter (modifierKey) |Parameter (eventData) | Remarks     |
| :---                      | :---          | :---         |:---           |:---          |:---               |:---            |:---      |
|kEventKeyDown              |Raw value|Raw value|     |   Related key  |  Key flags  |     |     |
|kEventKeyUp                |Raw value|Raw value |     |   Related key  |  Key flags  |     |     |
|kEventChar                 |Raw value|Raw value|     |   Related key  |  Key flags  |     |     |
|kEventMouseEnter           |     |     |Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseLeave           |     |     |Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseMove            |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseHover           |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseWheel           |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |  wheelDelta data |     |
|kEventMouseButtonDown      |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseButtonUp        |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseDoubleClick     |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseRButtonDown     |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseRButtonUp       |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseRDoubleClick    |Raw value|Raw value|Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventContextMenu          |     |Control*<br> the control at the mouse position| Client area coordinates<br>of the mouse position    |     |       |     | If the user presses SHIFT+F10,<br> ptMouse is (-1,-1) and lParam is 0 |
|kEventClick                |     |     |Client area coordinates<br>of the mouse position   | Parameter (vkCode)   | Key flags   | Old event type  | There are many possible parameters; they need to be checked  |
|kEventRClick               |     |     |Client area coordinates<br>of the mouse position   |     | Key flags   |     |     |
|kEventMouseClickChanged    |     |     |     |     |       |     | No parameters |
|kEventMouseClickEsc        |     |     |     |     |       |     | No parameters |

| Event (eventType)           | Parameter (wParam)  | Parameter (lParam) | Parameter (ptMouse) | Parameter (vkCode) | Parameter (modifierKey) |Parameter (eventData) | Remarks     |
| :---                      | :---          | :---         |:---           |:---          |:---               |:---            |:---      |
|kEventSetFocus             |     |     |     |     |       |     | No parameters |
|kEventKillFocus            |     |Control*<br> the new focused control<br>or nullptr |     |     |       |     |     |
|kEventSetCursor            |     |     | The mouse's<br>client area coordinates   |     |       |     |   |
|kEventImeStartComposition  |     |     |     |     |       |     |No parameters|
|kEventImeEndComposition    |     |     |     |     |       |     |No parameters|
|kEventWindowKillFocus      |     |     |     |     |       |     |No parameters |
|kEventWindowSize           |     |     |     |     |       |Window size change type:<br>WindowSizeType|     |
|kEventWindowMove           |     |     | The coordinates of the<br>window's top-left corner    |     |       |     |     |
|kEventWindowClose          |0: Normal close <br> 1: Cancel close|     |     |     |       |     |     |
|kEventSelect               |ListBox/Combo: <br>Newly selected index | ListBox/Combo: <br>Previously selected index|     |     |       |     | No parameters for other classes |
|kEventUnSelect             |ListBox: <br>Newly selected index | ListBox: <br>Previously selected index|     |     |       |     |  No parameters for other classes   |
|kEventCheck              |     |     |     |     |       |     | No parameters |
|kEventUnCheck              |     |     |     |     |       |     | No parameters |
|kEventTabSelect            |Newly selected index | Previously selected index|     |     |    |     |    |
|kEventExpand               |     |     |     |     |       |     | No parameters |
|kEventCollapse             |     |     |     |     |       |     | No parameters |


| Event (eventType)           | Parameter (wParam)  | Parameter (lParam) | Parameter (ptMouse) | Parameter (vkCode) | Parameter (modifierKey) |Parameter (eventData) | Remarks     |
| :---                      | :---          | :---         |:---           |:---          |:---               |:---            |:---      |
|kEventZoom                 | Zoom ratio numerator [0,64] | Zoom ratio denominator (0,64] |     |     |       |     | RichEdit: Ctrl + mouse wheel: zoom feature |
|kEventTextChanged           |     |     |     |     |       |     | No parameters |
|kEventSelChanged            |     |     |     |     |       |     | No parameters |
|kEventReturn               |     |     |     |     |       |     | No parameters    |
|kEventTab                  |     |     |     |     |       |     | No parameters   |
|kEventLinkClick            | DString.c_str()<br> URL string    |     |     |     |       |     |     |
|kEventScrollPosChanged         | 0: cy unchanged<br> 1: cy changed   | 0: cx unchanged<br> 1: cx changed    |     |     |       |     |     |
|kEventValueChanged          |     |     |     |     |       |     | No parameters |
|kEventSizeChanged               |     |     |     |     |       |     | No parameters |
|kEventVisibleChanged        |     |     |     |     |       |     | No parameters |
|kEventStateChanged          | New state | Old state   |     |     |       |     | ControlStateType |
|kEventSelectColor          | Selected color |     |     |     |       |     | newColor.GetARGB() |
|kEventSplitDraged          | Control*: <br>the first control interface| Control*:<br>the second control interface|     |     |       |     |  May be nullptr  |
|kEventEnterEdit            | ListCtrlEditParam*:<br>the data entering the editing state  |     |     |     |       |     |     |
|kEventLeaveEdit            | ListCtrlEditParam*:<br>the data leaving the editing state    |     |     |     |       |     |     |

| Event (eventType)           | Parameter (wParam)  | Parameter (lParam) | Parameter (ptMouse) | Parameter (vkCode) | Parameter (modifierKey) |Parameter (eventData) | Remarks     |
| :---                      | :---          | :---         |:---           |:---          |:---               |:---            |:---      |
|kEventPathChanged          |     |     |     |     |       |     |  No parameters   |
|kEventPathClick            |     |     |     |     |       |     |  No parameters   |
|kEventDropEnter            | ControlDropType|When wParam is kControlDropTypeWindows,<br>lParam is a pointer to ControlDropData_Windows|     |     |       |     |  No parameters   |
|kEventDropOver             | ControlDropType|When wParam is kControlDropTypeWindows,<br>lParam is a pointer to ControlDropData_Windows|     |     |       |     |  No parameters   |
|kEventDropLeave            |     |     |     |     |       |     |  No parameters   |
|kEventDropData             | ControlDropType | When wParam is kControlDropTypeWindows,<br>lParam is a pointer to ControlDropData_Windows; <br>when wParam is kControlDropTypeWayland,<br>lParam is a pointer to ControlDropData_Wayland|     |     |       |     |  No parameters   |
|kEventImageAnimationStart  | wParam is a data pointer: ui::ImageAnimationStatus*|     |     |     |       |     |  No parameters   |
|kEventImageAnimationPlayFrame  | wParam is a data pointer: ui::ImageAnimationStatus*|     |     |     |       |     |  No parameters   |
|kEventImageAnimationStop   | wParam is a data pointer: ui::ImageAnimationStatus*|     |     |     |       |     |  No parameters   |
|kEventLoadingStart         | wParam is a data pointer: ui::ControlLoadingStatus*|     |     |     |       |     |  No parameters   |
|kEventLoading              | wParam is a data pointer: ui::ControlLoadingStatus*|     |     |     |       |     |  No parameters   |
|kEventLoadingStop          | wParam is a data pointer: ui::ControlLoadingStatus*|     |     |     |       |     |  No parameters   |
|kEventImageLoad            | wParam is a data pointer: ui::ImageDecodeResult*   |     |     |     |       |     |  No parameters   |
|kEventImageDecode          | wParam is a data pointer: ui::ImageDecodeResult*   |     |     |     |       |     |  No parameters   |
|kEventLast                 |     |     |     |     |       |     |  No parameters   |

## Events added by the upstream update

| Event (eventType) | Emitted by | Parameter (wParam) | Parameter (lParam) | Remarks |
| :--- | :--- | :--- | :--- | :--- |
|kEventPanelCollapsing      |Panel| | |Fired before a panel collapses; an event callback returning false cancels it|
|kEventPanelExpanding       |Panel| | |Fired before a panel expands; an event callback returning false cancels it|
|kEventNavigationItemClick  |NavigationView|Index of the clicked item| |Fired when a selectable navigation item is clicked (clicking the selected item also fires)|
|kEventNavigationSelectionChanged|NavigationView|New index|Old index (`Box::InvalidIndex` when nothing is selected)|Fired when the selected navigation item changes|
|kEventNavigationPaneToggling|NavigationView|Target state (1 = collapsed, 0 = expanded)| |Fired before the pane collapses/expands; an event callback returning false cancels the toggle|
|kEventNavigationPaneToggled|NavigationView|Current state (1 = collapsed, 0 = expanded)| |Fired when the pane has finished collapsing/expanding|
|kEventTimelineItemClick    |Timeline|Node index (from 0)| |Fired when a timeline node is clicked|
|kEventTagAdded             |TagInput|Index of the new tag| |Fired after a tag is added|
|kEventTagRemoved           |TagInput|Index of the removed tag| |Fired before a tag is removed; the tag text can still be read in the callback|
|kEventTagClick             |TagInput|Index of the tag| |Fired when the body of a tag is clicked|
|kEventAutoCompleteSelect   |AutoComplete / SearchBox|Index of the selected candidate in the current candidate list| |Fired after a suggestion is selected|
|kEventProgressCompleted    |Progress| | |Fired when the progress value reaches the completion end point (max when increasing, min when decreasing)|
|kEventColumnWidthChanged   |ListCtrl|Column ID (`size_t`)|Associated other column ID (`size_t`, may be `Box::InvalidIndex`)|Fired when the width of a column changes (used to persist column layout)|
|kEventColumnOrderChanged   |ListCtrl| | |Fired when the display order of the columns changes|
|kEventColumnVisibleChanged |ListCtrl|Column ID (`size_t`)|New state (1 = visible, 0 = hidden)|Fired when a column is shown/hidden|
|kEventColumnSorted         |ListCtrl|Column ID (`size_t`)|New state (1 = ascending, 0 = descending)|Fired when the sort state of a column changes|
|kEventCurrentCellChanged   |ListCtrl|Row display index (`size_t`)|Column ID (`size_t`)|Fired when the current cell changes (cell selection mode)|
|kEventCellSelectionChanged |ListCtrl|Number of selected areas (`size_t`)|0|pEventData points to `std::vector<ui::ListCtrlCellRange>` (the merged selected-area list)|
|kEventViewModeChanged      |Calendar|New view mode (0 = month, 1 = year, 2 = decade)|Old view mode|Fired when the calendar view mode changes|
|kEventDisplayDateChanged   |Calendar| | |Fired when the displayed period (month/year/decade) changes|
