## I. Basic Concepts of Controls (Control), Containers (Box) and Layouts (Layout)
### 1. Control    
   Control is the most basic component in the UI library; its class name is Control and its base class is PlaceHolder.    
   The control (Control) includes the following basic attributes:
* Visibility: visible
* Enabled state: enabled
* Position: x,y
* Size: width, height
* Corner radius: border_round
* Border size: border_size
* Border style: border_dash_style
* Border color: border_color
* Alignment: halign/valign
* Margin: margin
* Padding: padding
* Background color: bkcolor
* Background image: bkimage
* Tooltip: tooltip_text
* Cursor style: cursor_type
* Transparency: alpha
* Drag-and-drop operations: enable_drag_drop/enable_drop_file/drop_file_types
* Image animation operations: start_image_animation/stop_image_animation/set_image_animation_frame
* For other attributes, see the related documentation or source code
   
### 2. Box    
   Box is the class responsible for managing attributes such as the position and size of the child controls contained in the container. It includes a series of subclasses (such as HBox/VBox, etc.), and its base class is the control (Control), so it has all the attributes of a control.    
   Each container has a different layout scheme, whose layout functionality is implemented by the Layout class and its subclasses. Every Box aggregates a Layout object, which is responsible for carrying out the concrete layout work.    
   Depending on the layout scheme, the Box classes are categorized as follows:    
| Class Name (Container) | Base Class | Related Header File | Default Layout Scheme | Description |
| :---     | :---   |:--- | :--- | :--- |
| Box      | Control|[duilib/Core/Box.h](../duilib/Core/Box.h) | Floating Layout (Layout) | Container base class; child controls are arranged with the floating layout |
| HBox     | Box|[duilib/Box/HBox.h](../duilib/Box/HBox.h) | Horizontal Layout (HLayout) | Child controls are arranged sequentially in the horizontal direction without wrapping |
| VBox     | Box|[duilib/Box/VBox.h](../duilib/Box/VBox.h) | Vertical Layout (VLayout) | Child controls are arranged sequentially in the vertical direction without wrapping |
| HFlowBox | Box|[duilib/Box/HBox.h](../duilib/Box/HBox.h) | Horizontal Flow Layout (HFlowLayout) | Child controls are arranged sequentially in the horizontal direction, wrapping automatically |
| VFlowBox | Box|[duilib/Box/VBox.h](../duilib/Box/VBox.h) | Vertical Flow Layout (VFlowLayout) | Child controls are arranged sequentially in the vertical direction, wrapping automatically |
| HTileBox | Box|[duilib/Box/TileBox.h](../duilib/Box/TileBox.h) | Horizontal Tile Layout (HTileLayout) | Container for the horizontal tile layout; supports setting the number of rows |
| VTileBox | Box|[duilib/Box/TileBox.h](../duilib/Box/TileBox.h) | Vertical Tile Layout (VTileLayout) | Container for the vertical tile layout; supports setting the number of columns |
| ScrollBox | Box|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Floating Layout (Layout)     | Box container with vertical or horizontal scroll bars |
| HScrollBox | Box|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Horizontal Layout (HLayout)   | HBox container with vertical or horizontal scroll bars |
| VScrollBox | Box|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Vertical Layout (VLayout)   | VBox container with vertical or horizontal scroll bars |
| HFlowScrollBox | Box|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Horizontal Flow Layout (HFlowLayout)| HFlowBox container with vertical or horizontal scroll bars |
| VFlowScrollBox | Box|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Vertical Flow Layout (VFlowLayout)| VFlowBox container with vertical or horizontal scroll bars |
| HTileScrollBox | Box|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Horizontal Tile Layout (HTileLayout)| HTileBox container with vertical or horizontal scroll bars |
| VTileScrollBox | Box|[duilib/Box/ScrollBox.h](../duilib/Box/ScrollBox.h) | Vertical Tile Layout (VTileLayout)| VTileBox container with vertical or horizontal scroll bars |
| ListBox | ScrollBox|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | Floating Layout (Layout)     | List container with vertical or horizontal scroll bars |
| HListBox | ScrollBox|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | Horizontal Layout (HLayout)   | List container with vertical or horizontal scroll bars |
| VListBox | ScrollBox|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | Vertical Layout (VLayout)   | List container with vertical or horizontal scroll bars |
| HTileListBox | ScrollBox|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | Horizontal Tile Layout (HTileLayout)| List container with vertical or horizontal scroll bars |
| VTileListBox | ScrollBox|[duilib/Box/ListBox.h](../duilib/Box/ListBox.h) | Vertical Tile Layout (VTileLayout)| List container with vertical or horizontal scroll bars |
| VirtualListBox | ListBox|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Floating Layout (Layout)     | ListBox implemented with a virtual table; supports large data volumes and scroll bars |
| VirtualHListBox | ListBox|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Horizontal Layout (HLayout)   | ListBox implemented with a virtual table; supports large data volumes and scroll bars |
| VirtualVListBox | ListBox|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Vertical Layout (VLayout)   | ListBox implemented with a virtual table; supports large data volumes and scroll bars |
| VirtualHTileListBox | ListBox|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Horizontal Tile Layout (HTileLayout)| ListBox implemented with a virtual table; supports large data volumes and scroll bars |
| VirtualVTileListBox | ListBox|[duilib/Box/VirtualListBox.h](../duilib/Box/VirtualListBox.h) | Vertical Tile Layout (VTileLayout)| ListBox implemented with a virtual table; supports large data volumes and scroll bars |
| TabBox | Box|[duilib/Box/TabBox.h](../duilib/Box/TabBox.h) | Floating Layout (Layout) | Page management container; among the multiple child controls inside, only one is visible and the others are hidden; they can be switched dynamically |
| GridBox | Box|[duilib/Box/GridBox.h](../duilib/Box/GridBox.h) | Grid Layout (GridLayout) | Grid layout container; supports cell merging |
| GridScrollBox | ScrollBox|[duilib/Box/GridBox.h](../duilib/Box/GridBox.h) | Grid Layout (GridLayout) | Grid layout container; supports cell merging and scroll bars |
| BoxDragable | Box|[duilib/Core/ControlDragable.h](../duilib/Core/ControlDragable.h) | Floating Layout (Layout)    | Container that supports dragging child controls in/out |
| HBoxDragable | HBox|[duilib/Core/ControlDragable.h](../duilib/Core/ControlDragable.h) | Horizontal Layout (HLayout) | Container that supports dragging child controls in/out |
| VBoxDragable | VBox|[duilib/Core/ControlDragable.h](../duilib/Core/ControlDragable.h) | Vertical Layout (VLayout) | Container that supports dragging child controls in/out |

Box includes the following basic attributes:    
* The spacing between child controls (X and Y directions): child_margin_x/child_margin_y
* The alignment of child controls (horizontal and vertical): child_halign/child_valign
* Margin: margin
* Padding: padding
* Whether child controls can be operated with the mouse: mouse_child
* Whether dragging out of this container is supported: drag_out_id
* Whether dragging into this container is supported: drop_in_id
* Other attributes: different container types have different attributes

### 3. Layout    
   Layout is the concrete implementation code for the container's layout scheme; it is aggregated into and used by the Box object, and the application layer generally does not use it directly.    
   Layouts can inherit from the base class and implement customized layout schemes in subclasses; use the `Layout* Box::ResetLayout(Layout* pNewLayout)` function to replace the original layout implementation of the container.    
   Depending on the layout scheme, the Layout classes are categorized as follows:    
| Class Name (Layout) | Base Class | Related Header File | Layout Scheme Description |
| :--- | :--- |:--- | :--- |
| Layout | |[duilib/Layout/Layout.h](../duilib/Layout/Layout.h) | Floating layout: child controls are not related to each other; each uses its own attributes to set its position and size |
| HLayout |Layout |[duilib/Layout/HLayout.h](../duilib/Layout/HLayout.h) | Horizontal layout: child controls are arranged sequentially in the horizontal direction without wrapping |
| VLayout |Layout |[duilib/Layout/VLayout.h](../duilib/Layout/VLayout.h) | Vertical layout: child controls are arranged sequentially in the vertical direction without wrapping |
| HFlowLayout |Layout |[duilib/Layout/HFlowLayout.h](../duilib/Layout/HFlowLayout.h) | Horizontal flow layout: child controls are arranged sequentially in the horizontal direction, wrapping automatically |
| VFlowLayout |Layout |[duilib/Layout/VFlowLayout.h](../duilib/Layout/VFlowLayout.h) | Vertical flow layout: child controls are arranged sequentially in the vertical direction, wrapping automatically |
| HTileLayout |Layout| [duilib/Layout/HTileLayout.h](../duilib/Layout/HTileLayout.h) | Horizontal tile layout; supports setting the number of rows |
| VTileLayout |Layout| [duilib/Layout/VTileLayout.h](../duilib/Layout/VTileLayout.h) | Vertical tile layout; supports setting the number of columns |
| GridLayout  |Layout |[duilib/Layout/GridLayout.h](../duilib/Layout/GridLayout.h) | Grid layout |
| VirtualHLayout |HLayout| [duilib/Layout/VirtualHLayout.h](../duilib/Layout/VirtualHLayout.h) | Virtual horizontal layout, single row |
| VirtualVLayout |VLayout| [duilib/Layout/VirtualVLayout.h](../duilib/Layout/VirtualVLayout.h) | Virtual vertical layout, single column |
| VirtualHTileLayout |HTileLayout| [duilib/Layout/VirtualHTileLayout.h](../duilib/Layout/VirtualHTileLayout.h) | Virtual horizontal tile layout; supports setting the number of rows |
| VirtualVTileLayout |VTileLayout| [duilib/Layout/VirtualVTileLayout.h](../duilib/Layout/VirtualVTileLayout.h) | Virtual vertical tile layout; supports setting the number of columns |

When a container lays out its child controls, the related attributes of the child controls (which can be a Control, a Box, or their subclasses) mainly include:    
* The container's padding: padding
* The attributes of the container's associated layout: the spacing between child controls (horizontal: child_margin_x, vertical: child_margin_y)
* The attributes of the container's associated layout: content alignment (horizontal: child_halign, vertical: child_valign)
* The child control's own alignment: horizontal (halign), vertical (valign)
* The child control's own margin: margin
* The child control's own cell merge attribute: row_span, the cell merge attribute (how many rows it spans); only takes effect in the GridLayout layout
* The child control's own cell merge attribute: col_span, the cell merge attribute (how many columns it spans); only takes effect in the GridLayout layout

## II. Layout Attributes
### 1. Floating Layout (Layout)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| child_margin   | 0 | int | The spacing between child controls: sets the X and Y directions to the same value at the same time |
| child_margin_x | 0 | int | The spacing between child controls: X direction |
| child_margin_y | 0 | int | The spacing between child controls: Y direction |
| child_valign   |   | string | The vertical alignment of child controls; valid values: "top", "center", "bottom" |
| child_halign   |   | string | The horizontal alignment of child controls; valid values: "left", "center", "right" |
| child_align    |   | string | Sets both the horizontal and vertical alignment of child controls at the same time; same function as child_valign and child_halign.<br>Valid values: left, right, hcenter, top, vcenter, bottom, separated by commas, e.g. "hcenter,vcenter" |

### 2. Horizontal Layout (HLayout)
Available attributes are inherited from the `Floating Layout (Layout)` attributes

### 3. Vertical Layout (VLayout)
Available attributes are inherited from the `Floating Layout (Layout)` attributes

### 4. Horizontal Flow Layout (HFlowLayout)
Available attributes are inherited from the `Floating Layout (Layout)` attributes

### 5. Vertical Flow Layout (VFlowLayout)
Available attributes are inherited from the `Floating Layout (Layout)` attributes

### 6. Horizontal Tile Layout (HTileLayout)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| rows | 0 | int | If set to "auto", the number of rows is calculated automatically; if set to a number, the number of rows is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |
| auto_calc_item_size | false | bool | Takes effect when a fixed number of rows is set; calculates the tile height automatically based on the container's total height |

In addition, available attributes are inherited from the `Floating Layout (Layout)` attributes

### 7. Vertical Tile Layout (VTileLayout)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| columns | 0 | int | If set to "auto", the number of columns is calculated automatically; if set to a number, the number of columns is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |
| auto_calc_item_size | false | bool | Takes effect when a fixed number of columns is set; calculates the tile width automatically based on the container's total width |

In addition, available attributes are inherited from the `Floating Layout (Layout)` attributes

### 8. Grid Layout (GridLayout)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| rows | 0 | int | The number of grid rows (0 means automatic calculation) |
| columns | 0 | int | The number of grid columns (0 means automatic calculation) |
| grid_width | 0 | int | The grid cell width (0 means automatic calculation) |
| grid_height | 0 | int | The grid cell height (0 means automatic calculation) |
| scale_down | false | bool | When the control's content exceeds the boundary, whether to scale it down proportionally<br>true  use the child control's size; if it exceeds the grid size, scale it down proportionally so that the content is fully displayed within the grid<br>false ignore the child control's own size; the child control's size matches the grid size |

In addition, available attributes are inherited from the `Floating Layout (Layout)` attributes

### 9. Virtual Horizontal Layout (VirtualHLayout)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| auto_calc_item_size | false | bool | Calculates the tile height automatically based on the container's total height |

In addition, available attributes are inherited from the `Horizontal Layout (HLayout)` attributes

### 10. Virtual Vertical Layout (VirtualVLayout)
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| auto_calc_item_size | false | bool | Calculates the tile width automatically based on the container's total width |

In addition, available attributes are inherited from the `Vertical Layout (VLayout)` attributes

### 11. Virtual Horizontal Tile Layout (VirtualHTileLayout)
Available attributes are inherited from the `Horizontal Tile Layout (HTileLayout)` attributes

### 12. Virtual Vertical Tile Layout (VirtualVTileLayout)
Available attributes are inherited from the `Vertical Tile Layout (VTileLayout)` attributes

## III. Attributes of Various Containers
### 1. Box Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| child_margin   | 0 | int | Layout attribute; the spacing between child controls: sets the X and Y directions to the same value at the same time |
| child_margin_x | 0 | int | Layout attribute; the spacing between child controls: X direction |
| child_margin_y | 0 | int | Layout attribute; the spacing between child controls: Y direction |
| child_valign   |   | string | Layout attribute; the vertical alignment of child controls; valid values: "top", "center", "bottom" |
| child_halign   |   | string | Layout attribute; the horizontal alignment of child controls; valid values: "left", "center", "right" |
| child_align    |   | string | Layout attribute; sets both the horizontal and vertical alignment of child controls at the same time; same function as child_valign and child_halign.<br>Valid values: left, right, hcenter, top, vcenter, bottom, separated by commas, e.g. "hcenter,vcenter" |
| margin | 0,0,0,0 | rect | Margin, e.g. (2,2,2,2) |
| padding | 0,0,0,0 | rect | Padding, e.g. (2,2,2,2) |
| mouse_child | true | bool | Whether child controls support mouse operations; true or false |
| drag_out_id | 0 | int | Sets whether dragging out of this container is supported: if not 0, dragging out is supported; otherwise it is not supported (dragged out to the container where drop_in_id==drag_out_id) |
| drop_in_id | 0 | int | Sets whether dragging into this container is supported: if not 0, dragging in is supported; otherwise it is not supported (dragged into this container from the container where drag_out_id==drop_in_id) |

The Box control inherits the `Control` attributes; for more available attributes, refer to: the base class [Control (Basic Control) Attributes](./Control.md)

### 2. VBox Attributes
The VBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 3. HBox Attributes
The HBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 4. VFlowBox Attributes
The VFlowBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 5. HFlowBox Attributes
The HFlowBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 6. VTileBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| columns | 0 | int | If set to "auto", the number of columns is calculated automatically; if set to a number, the number of columns is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |

The VTileBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 7. HTileBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| rows | 0 | int | If set to "auto", the number of rows is calculated automatically; if set to a number, the number of rows is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |

The HTileBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 8. ScrollBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| vscrollbar | false | bool | Whether to use a vertical scroll bar, e.g. (true) |
| hscrollbar | false | bool | Whether to use a horizontal scroll bar, e.g. (true) |
| vscrollbar_style |  | string | Sets the style of the vertical scroll bar of this container |
| hscrollbar_style |  | string | Sets the style of the horizontal scroll bar of this container |
| vscrollbar_class |  | string | Sets the Class of the vertical scroll bar of this container |
| hscrollbar_class |  | string | Sets the Class of the horizontal scroll bar of this container |
| scrollbar_padding | 0,0,0,0 | rect | The margin of the scroll bar, which allows the scroll bar not to fill the whole container, e.g. (2,2,2,2) |
| vscroll_unit | 30 | int | The scroll step of the container's vertical scroll bar; 0 means the default step is used |
| hscroll_unit | 30 | int | The scroll step of the container's horizontal scroll bar; 0 means the default step is used |
| scrollbar_float | true | bool | Whether the container's scroll bar floats above the child controls, e.g. (true) |
| vscrollbar_left | false | bool | Whether the container's scroll bar is displayed on the left |
| hold_end | false | bool | Whether to keep showing the end position, e.g. (true) |

The ScrollBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 9. VScrollBox Attributes
The VScrollBox control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

### 10. HScrollBox Attributes
The HScrollBox control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

### 11. VFlowScrollBox Attributes
The VFlowScrollBox control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

### 12. HFlowScrollBox Attributes
The HFlowScrollBox control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

### 13. VTileScrollBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| columns | 0 | int | If set to "auto", the number of columns is calculated automatically; if set to a number, the number of columns is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |

The VTileScrollBox control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

### 14. HTileScrollBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| rows | 0 | int | If set to "auto", the number of rows is calculated automatically; if set to a number, the number of rows is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |

The HTileScrollBox control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

### 15. ListBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| multi_select | false | bool | Whether multiple selection is supported |
| paint_selected_colors | default rule | bool | Whether to show the selection background color when multi-selecting<br>Default rule: if there is a CheckBox, the selection background color is not shown by default when multi-selecting; otherwise the background color is shown |
| scroll_select | false | bool | Whether the selected item changes with mouse wheel scrolling (this option is only valid for single selection) |
| select_next_when_active_removed | | bool | After removing a sub-item, if the removed item was the selected item, whether to automatically select the next item (this option is only valid for single selection) |
| frame_selection | false | bool | Whether the mouse frame (rubber band) selection feature is supported; only takes effect in multi-select mode |
| frame_selection_color |"#FFAACCEE"| string | The fill color of the mouse frame selection |
| frame_selection_border_size | 1 | int | The border size of the mouse frame selection |
| frame_selection_border_color | "#FF0078D7" | string | The border color of the mouse frame selection |
| frame_selection_alpha | 128 | int | The Alpha value of the mouse frame selection fill color |
| select_none_when_click_blank | true | bool | Sets whether the selection is cancelled when the mouse clicks on a blank area (only valid when the mouse frame selection feature is enabled) |
| select_like_list_ctrl | false | bool | Sets the selection mode: similar to ListCtrl (i.e. a way similar to operating on files in Windows Explorer); only valid in multi-select mode |

The ListBox control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

### 16. VListBox Attributes
The VListBox control inherits the `ListBox` attributes; for more available attributes, refer to the `ListBox` attributes

### 17. HListBox Attributes
The HListBox control inherits the `ListBox` attributes; for more available attributes, refer to the `ListBox` attributes

### 18. VTileListBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| columns | 0 | int | If set to "auto", the number of columns is calculated automatically; if set to a number, the number of columns is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |

The VTileListBox control inherits the `ListBox` attributes; for more available attributes, refer to the `ListBox` attributes

### 19. HTileListBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| rows | 0 | int | If set to "auto", the number of rows is calculated automatically; if set to a number, the number of rows is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |

The HTileListBox control inherits the `ListBox` attributes; for more available attributes, refer to the `ListBox` attributes

### 20. VirtualListBox Attributes
The VirtualListBox control inherits the `ListBox` attributes; for more available attributes, refer to the `ListBox` attributes

### 21. VirtualVListBox Attributes
The VirtualVListBox control inherits the `VirtualListBox` attributes; for more available attributes, refer to the `VirtualListBox` attributes

### 22. VirtualHListBox Attributes
The VirtualHListBox control inherits the `VirtualListBox` attributes; for more available attributes, refer to the `VirtualListBox` attributes

### 23. VirtualVTileListBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| columns | 0 | int | If set to "auto", the number of columns is calculated automatically; if set to a number, the number of columns is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |

The VirtualVTileListBox control inherits the `VirtualListBox` attributes; for more available attributes, refer to the `VirtualListBox` attributes

### 24. VirtualHTileListBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| item_size | 0,0 | size | The sub-item size; this width and height include the control's margin and padding, e.g. "100,40" |
| rows | 0 | int | If set to "auto", the number of rows is calculated automatically; if set to a number, the number of rows is fixed |
| scale_down | true | bool | When the control's content exceeds the boundary, scale it down proportionally so that the content is fully displayed within the tile area |

The VirtualHTileListBox control inherits the `VirtualListBox` attributes; for more available attributes, refer to the `VirtualListBox` attributes

### 25. TabBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| selected_id | 0 | int | The ID of the page selected by default |
| fade_switch | true | bool | Whether to use an animation effect when switching pages; values: "false" or "true" |
| fade_switch_type | "FadeInOut" | string | The animation type of page switching; values: "FadeInOut" means fade in/out, "FadeInOutX" means the content area slides horizontally |
| fade_switch_frame_interval_ms | 16 | int | The timer interval (in milliseconds) for playing the switching animation |
| fade_switch_total_ms | 200 | int | The total duration (in milliseconds) of the switching animation |
| fade_switch_easing_function | "EaseOutCubic" | string | The easing function type of the switching animation |

The TabBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 26. GridBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| rows | 0 | int | The number of grid rows (0 means automatic calculation) |
| columns | 0 | int | The number of grid columns (0 means automatic calculation) |
| grid_width | 0 | int | The grid cell width (0 means automatic calculation) |
| grid_height | 0 | int | The grid cell height (0 means automatic calculation) |
| scale_down | false | bool | When the control's content exceeds the boundary, whether to scale it down proportionally<br>true  use the child control's size; if it exceeds the grid size, scale it down proportionally so that the content is fully displayed within the grid<br>false ignore the child control's own size; the child control's size matches the grid size |

The GridBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes

### 27. GridScrollBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| rows | 0 | int | The number of grid rows (0 means automatic calculation) |
| columns | 0 | int | The number of grid columns (0 means automatic calculation) |
| grid_width | 0 | int | The grid cell width (0 means automatic calculation) |
| grid_height | 0 | int | The grid cell height (0 means automatic calculation) |
| scale_down | false | bool | When the control's content exceeds the boundary, whether to scale it down proportionally<br>true  use the child control's size; if it exceeds the grid size, scale it down proportionally so that the content is fully displayed within the grid<br>false ignore the child control's own size; the child control's size matches the grid size |

The GridScrollBox control inherits the `ScrollBox` attributes; for more available attributes, refer to the `ScrollBox` attributes

### 28. BoxDragable Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :--- | :--- | :--- | :--- |
| drag_order | true | bool | Whether dragging to adjust the order is supported (within the same container) |
| drag_alpha | 216 | int | Sets the transparency of the control when dragging to reorder (0 - 255) |
| drag_out | true | bool | Whether dragging out is supported (between different containers in the same window) |

The BoxDragable control inherits the `ControlDragableT` and `Box` attributes; for more available attributes, refer to the `ControlDragableT` and `Box` attributes

### 29. HBoxDragable Attributes
HBoxDragable and BoxDragable are implemented by one template class (ControlDragableT); refer to the `BoxDragable` attributes    
The HBoxDragable control inherits the `HBox` attributes; for more available attributes, refer to the `HBox` attributes

### 30. VBoxDragable Attributes
VBoxDragable and BoxDragable are implemented by one template class (ControlDragableT); refer to the `BoxDragable` attributes    
The VBoxDragable control inherits the `VBox` attributes; for more available attributes, refer to the `VBox` attributes

### 31. BoxMovable Attributes
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

The ControlMovable control inherits the `ControlMovableT` and `Control` attributes; for more available attributes, refer to the `ControlMovableT` and `Control` attributes

### 32. HBoxMovable Attributes
The HBoxMovable control inherits the `ControlMovableT` and `HBox` attributes; for more available attributes, refer to the `ControlMovableT` and `HBox` attributes

### 33. VBoxMovable Attributes
The VBoxMovable control inherits the `ControlMovableT` and `VBox` attributes; for more available attributes, refer to the `ControlMovableT` and `VBox` attributes

### 34. BoxResizable Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :---     | :---   | :---     | :--- |
| enable_resize   | true  | bool | Whether the mouse can be dragged to change the control's size |
| enable_move_pos  | false | bool    | Whether dragging to adjust the control's position is supported; disabled by default; if enabled, see the `ControlMovableT` attributes for the related attributes |
| resize_size_box   | | UiRect | Sets the size of the stretchable range when resizing the four edges of the control |
| resize_reserve_width   | 10| int | Sets the minimum width reserved when resizing (not DPI-scaled) |
| resize_reserve_height   | 10| int | Sets the minimum height reserved when resizing (not DPI-scaled) |
| resize_keep_within_parent| false | bool | When resizing the control, whether to ensure the child control stays within the parent container without overflow |

The BoxResizable control inherits the `ControlResizableT`, `ControlMovableT` and `Box` attributes; for more available attributes, refer to the `ControlResizableT`, `ControlMovableT` and `Box` attributes

### 35. HBoxResizable Attributes
The HBoxResizable control inherits the `ControlResizableT` and `HBox` attributes; for more available attributes, refer to the `ControlResizableT` and `HBox` attributes

### 36. VBoxResizable Attributes
The VBoxResizable control inherits the `ControlResizableT` and `VBox` attributes; for more available attributes, refer to the `ControlResizableT` and `VBox` attributes

### 37. XmlBox Attributes
| Attribute Name | Default Value | Parameter Type | Purpose |
| :---     | :---   | :---     | :--- |
| xml_file_path | | string | Sets the path of the XML file |
| res_path      | | string | Sets the path where image resources are located (the resource root directory corresponding to the XML file) |

The XmlBox control inherits the `Box` attributes; for more available attributes, refer to the `Box` attributes
