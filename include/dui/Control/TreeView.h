#ifndef UI_CONTROL_TREEVIEW_H_
#define UI_CONTROL_TREEVIEW_H_

#include "dui/Box/ListBox.h"
#include <map>

namespace ui
{

/** Node check state
*/
enum class TreeNodeCheck
{
    UnCheck,     //Not checked
    CheckedAll,     //All checked
    CheckedPart  //Partially checked
};

class TreeView;
class DUI_API TreeNode : public ListBoxItem
{
    typedef ListBoxItem BaseClass;
    friend class TreeView;
public:
    explicit TreeNode(Window* pWindow);
    TreeNode(const TreeNode& r) = delete;
    TreeNode& operator=(const TreeNode& r) = delete;
    virtual ~TreeNode() override;

    /// Override the parent class method to provide personalized functionality; please refer to the parent class declaration
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual bool SupportCheckMode() const override;

    /** DPI changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

protected:
    /** Visible state (used by internal subclasses to override the visible state; if true is returned it means visible, false means invisible)
    */
    virtual bool IsVisibleInternal() const override;

private:
    virtual void PaintStateImages(IRender* pRender) override;
    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool OnDoubleClickItem(const EventArgs& msg);
    virtual bool OnReturnKeyDown(const EventArgs& msg);

public:
    /** Set the tree container that the child item belongs to
     * @param[in] pTreeView Tree container pointer
     */
    void SetTreeView(TreeView* pTreeView);

    /** Get the tree container that the child item belongs to
     */
    TreeView* GetTreeView() const;

    /** Get the parent node pointer
     * @return Returns the parent node pointer
     */
    TreeNode* GetParentNode() const;

    /** Set the parent node
     * @param[in] pParentTreeNode Parent node pointer
     */
    void SetParentNode(TreeNode* pParentTreeNode);

    /** Add a new child node at the end
     * @param[in] pTreeNode Child node pointer
     * @return Returns true on success, false on failure
     */
    bool AddChildNode(TreeNode* pTreeNode);

    /** Add a child node at the specified position
     * @param[in] pTreeNode Child node pointer
     * @param[in] iIndex The position after which to insert
     * @return Returns true on success, false on failure
     */
    bool AddChildNodeAt(TreeNode* pTreeNode, const size_t iIndex);

    /** Remove a child node at the specified position
     * @param[in] iIndex The index of the child node to remove
     * @return Returns true on success, false on failure
     */
    bool RemoveChildNodeAt(size_t iIndex);

    /** Remove a child node by its pointer
     * @param[in] pTreeNode Child node pointer
     * @return Returns true on success, false on failure
     */
    bool RemoveChildNode(TreeNode* pTreeNode);

    /** Remove all child nodes
     */
    void RemoveAllChildNodes();

    /** Get the total number of all descendant nodes recursively
     * @return Returns the total number of all descendant nodes
     */
    size_t GetDescendantNodeCount() const;

    /** Get the number of child nodes at the next level
     * @return Returns the number of child nodes
     */
    size_t GetChildNodeCount() const;

    /** Get a child node pointer
     * @param[in] iIndex The index of the child node to get
     * @return Returns the child node pointer
     */
    TreeNode* GetChildNode(size_t iIndex) const;

    /** Get the position index of a child node by its pointer
     * @param[in] pTreeNode Child node pointer
     * @return Returns the position index
     */
    size_t GetChildNodeIndex(TreeNode* pTreeNode) const;

    /** Get the list of child nodes (only first-level child nodes, without recursively getting grandchild nodes)
    * @param [out] childNodes Returns the list of all child nodes of the current tree node
    */
    void GetChildNodes(std::vector<TreeNode*>& childNodes) const;

    /** Find a child node by the control name (Name) of the child node
    * @param [in] name The control name of the child node to find (i.e. the value of Control::GetName())
    * @param [in] bRecursive true means recursive search, false means non-recursive search, only searching the first-level child nodes of the current node
    * @return Returns the pointer of the matching tree node; if names are duplicated, only the first one is returned
    */
    TreeNode* FindChildNodeByName(const DString& name, bool bRecursive) const;

    /** Find a child node by the display text (Text) of the child node
    * @param [in] name The display text of the child node to find (i.e. the value of LabelTemplate::GetText())
    * @return Returns the pointer of the matching tree node; if the display text is duplicated, only the first one is returned
    * @param [in] bRecursive true means recursive search, false means non-recursive search, only searching the first-level child nodes of the current node
    */
    TreeNode* FindChildNodeByText(const DString& text, bool bRecursive) const;

    /** Determine whether it is in the expanded state
     * @return Returns true if expanded, otherwise false
     */
    bool IsExpand() const; 

    /** Set whether to expand to show child nodes
     * @param[in] bExpand true to expand, false not to expand
     */
    void SetExpand(bool bExpand, bool bTriggerEvent = false);

    /** Get the level of the child item
     * @return Returns the current level. The level of the root node is 0; the root node is a virtual node, and first-level nodes are real nodes with level 1
     *         The indentation of a tree node is:
     *         int32_t indent = 0;
     *         if(GetDepth() > 0) {
     *                indent = (GetDepth() - 1) * TreeView::GetIndent();
     *           }
     */
    uint16_t GetDepth() const;

#ifdef DUI_BUILD_FOR_WIN

    /** Set the background image (HICON handle)
     * @param [in] hIcon The icon handle to set. The HICON handle is not stored internally; its lifetime is managed externally
     *             If hIcon is nullptr, the icon of the node is deleted
     * @param [in] nIconSize The size of the icon (width and height values);
                   If set to 0, the actual icon size is used as the icon size, but this setting makes DPI awareness impossible, so a reasonable value is recommended.
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetBkIcon(HICON hIcon, uint32_t nIconSize, bool bNeedDpiScale);

#endif

    /** Set the background image (icon ID)
     * @param [in] nIconID The icon ID to set, returned by the ui::GlobalManager::Instance().Icon().AddIcon function
     *              If nIconID is 0, the icon of the node is deleted
     * @param [in] nIconSize The size of the icon (width and height values);
                   If set to 0, the actual icon size is used as the icon size, but this setting makes DPI awareness impossible, so a reasonable value is recommended.
     * @param [in] bNeedDpiScale Whether DPI adaptation is needed
     */
    void SetBkIconID(uint32_t nIconID, uint32_t nIconSize, bool bNeedDpiScale);

    /** Clear the icon of the node, do not display the node icon
    */
    void ClearBkIcon();

    /** Set whether to display the icon
    */
    void SetEnableIcon(bool bEnable);

    /** Listen for the child item expand event
     * @param [in] callback The callback function triggered when the child item is expanded
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachExpand(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventExpand, callback, callbackID); }

    /** Listen for the child item collapse event
     * @param [in] callback The callback function triggered when the child item is collapsed
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachCollapse(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventCollapse, callback, callbackID); }

private:
    /** Set the Class associated with the [collapsed/expanded] flag image. If not empty, the expand flag feature is enabled; if empty, it is disabled
    *   Scope of application: the node itself
    * @param [in] expandClass The Class attribute of the expand flag image
    */
    void SetExpandImageClass(const DString& expandClass);

    /** Set the Class associated with the CheckBox. If not empty, the CheckBox feature is enabled; if empty, it is disabled
    *   Scope of application: the node itself
    * @param [in] checkBoxClass The Class attribute of ui::CheckBox. The main attributes commonly set are:
                  normal_image: the image of the normal state, a required attribute (i.e. the image when not checked)
                  selected_normal_image: the image of the normal state when selected, a required attribute (i.e. the image when checked)
    */
    bool SetCheckBoxClass(const DString& checkBoxClass);

    /** Change the check status of all child nodes, but do not trigger the selection change event
    * @param [in] bChecked The check status (checked or unchecked)
    */
    void SetChildrenCheckStatus(bool bChecked);

    /** Update the check status of itself and all parent nodes (checked or unchecked), but do not trigger the selection change event
    *   When the check status of a node changes / child nodes are added or removed, this function must be called to update the check status of the node
    * @param [in] bUpdateSelf Whether the check status of itself needs to be updated
    */
    void UpdateParentCheckStatus(bool bUpdateSelf);

    /** Update the check status of the current node (tri-state check status)
    */
    void UpdateSelfCheckStatus();

    /** Get the check status of the current node (itself and child nodes)
    */
    TreeNodeCheck GetCheckStatus(void) const;

    /** Get the check status of the child nodes of the current node (excluding itself, only the child nodes)
    */
    TreeNodeCheck GetChildrenCheckStatus(void) const;

    /** Get the image of the expanded state
     * @param [in] stateType The state of the image to get; refer to the ControlStateType enumeration
     * @return Returns the image path and attributes
     */
    DString GetExpandStateImage(ControlStateType stateType);

    /** Set the image of the expanded state
     * @param [in] stateType The state of the image to set
     * @param [in] strImage The image path and attributes
     */
    void SetExpandStateImage(ControlStateType stateType, const DString& strImage);

    /** Get the image of the collapsed state
     * @param [in] stateType The state of the image to get; refer to the ControlStateType enumeration
     * @return Returns the image path and attributes
     */
    DString GetCollapseStateImage(ControlStateType stateType);

    /** Set the image of the collapsed state
     * @param [in] stateType The state of the image to set
     * @param [in] strImage The image path and attributes
     */
    void SetCollapseStateImage(ControlStateType stateType, const DString& strImage);

private:
    /** Delete itself
     * @return Returns true on success, false on failure
     */
    bool RemoveSelf();

    /** Remove a child node at the specified position
     * @param [in] iIndex The index of the child node to remove
     * @param [in] bUpdateCheckStatus Whether to update the check status
     * @return Returns true on success, false on failure
     */
    bool RemoveChildNodeAt(size_t iIndex, bool bUpdateCheckStatus);

    /** Adjust the padding associated with the expand flag according to the current configuration (reentrant function, repeated calls have no side effects)
    */
    void AdjustExpandImagePadding();

    /** Adjust the padding associated with the CheckBox according to the current configuration (reentrant function, repeated calls have no side effects)
    */
    void AdjustCheckBoxPadding();

    /** Adjust the padding associated with the icon according to the current configuration (reentrant function, repeated calls have no side effects)
    */
    void AdjustIconPadding();

    /** Triggered when the check status of a child item changes
     * @param[in] args Message body
     * @return Always returns true
     */
    bool OnNodeCheckStatusChanged(const EventArgs& args);

    /** Get the padding width occupied by the expand state icon
    */
    int32_t GetExpandImagePadding(void) const;

    /** Get the maximum ListBox index among itself and its descendant nodes, used to calculate the insertion position of a newly added node
     * @param [in] nInsertIndex The insertion position of the new node; if it is Box::InvalidIndex, it means inserting at the end
     *   If there are no valid elements, Box::InvalidIndex is returned
     */
    size_t GetDescendantNodeMaxListBoxIndex(size_t nInsertIndex) const;

    /** Set the spacing after the [expand/collapse] button
    */
    void SetExpandIndent(int32_t nExpandIndent, bool bNeedDpiScale);

    /** Get the spacing after the [expand/collapse] button
    */
    uint16_t GetExpandIndent() const;

    /** Set the spacing after the CheckBox
    */
    void SetCheckBoxIndent(int32_t nIndent, bool bNeedDpiScale);

    /** Get the spacing after the CheckBox
    */
    uint16_t GetCheckBoxIndent() const;

    /** Set the spacing after the icon
    */
    void SetIconIndent(int32_t nIndent, bool bNeedDpiScale);

    /** Get the spacing after the icon
    */
    uint16_t GetIconIndent() const;
    
private:
    //The level of the child item
    uint16_t m_uDepth;

    //Whether to expand to show child nodes
    bool m_bExpand;

    //The tree container that the child item belongs to
    TreeView* m_pTreeView;

    //Parent node
    TreeNode* m_pParentTreeNode;

    //List of child nodes
    std::vector<TreeNode*> m_aTreeNodes;

    //Fixed spacing between image/text elements (DPI-related)
    uint16_t m_expandIndent;    //Spacing after the [expand/collapse] button
    uint16_t m_checkBoxIndent;    //Spacing after the CheckBox
    uint16_t m_iconIndent;        //Spacing after the icon

    //Icon/text padding associated with the Expand icon: 3 items (DPI-related)
    uint16_t m_expandCheckBoxPadding;
    uint16_t m_expandIconPadding;
    uint16_t m_expandTextPadding;

    //Icon/text padding associated with the CheckBox: 2 items (DPI-related)
    uint16_t m_checkBoxIconPadding;
    uint16_t m_checkBoxTextPadding;

    //Text padding associated with the icon: 1 item (DPI-related)
    uint16_t m_iconTextPadding;

    /** The MAP of image types and state images for the expanded state of the control, the target rectangle for drawing
    */
    std::unique_ptr<StateImage> m_expandImage;
    UiRect* m_pExpandImageRect;//DPI-independent; this value is updated after each drawing

    /** The MAP of image types and state images for the collapsed state of the control, the target rectangle for drawing
    */
    std::unique_ptr<StateImage> m_collapseImage;
    UiRect* m_pCollapseImageRect;//DPI-independent; this value is updated after each drawing
};

class DUI_API TreeView : public ListBox
{
    typedef ListBox BaseClass;
    friend class TreeNode;
public:
    explicit TreeView(Window* pWindow);
    virtual ~TreeView() override;

    /// Override the parent class method to provide personalized functionality; please refer to the parent class declaration
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void SetParent(Box* pParent) override;
    virtual void SetWindow(Window* pWindow) override;

    /** DPI changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Get the root node
     * @return Returns the root node pointer
     */
    TreeNode* GetRootNode() const{ return m_rootNode.get(); }

    /** Get the child node indent value
     * @return Returns the child node indent value
     */
    int32_t GetIndent() const { return m_iIndent;    }

    /** Set the child node indent value
     * @param [in] indent The indent value to set, in pixels
     * @param [in] bNeedDpiScale Whether DPI scaling is needed
     */
    void SetIndent(int32_t indent, bool bNeedDpiScale);

    /** Set the Class associated with the [collapsed/expanded] flag image. If not empty, the expand flag feature is enabled; if empty, it is disabled
    *   Scope of application: all nodes of the tree
    * @param [in] className The Class attribute of the expand flag image
    */
    void SetExpandImageClass(const DString& className);

    /** Get the Class associated with the [collapsed/expanded] flag image
    */
    DString GetExpandImageClass() const;

    /** Set the Class associated with the CheckBox. If not empty, the CheckBox feature is enabled; if empty, it is disabled
    *   Scope of application: all nodes of the tree
    * @param [in] className The Class attribute of ui::CheckBox. The main attributes commonly set are:
                  normal_image: the image of the normal state, a required attribute (i.e. the image when not checked)
                  selected_normal_image: the image of the normal state when selected, a required attribute (i.e. the image when checked)
    */
    void SetCheckBoxClass(const DString& className);

    /** Get the Class associated with the CheckBox
    */
    DString GetCheckBoxClass() const;

    /** Set whether to display the icon
    */
    void SetEnableIcon(bool bEnable);

    /** Determine whether the icon is displayed
    */
    bool IsEnableIcon() const;

    /** Add an ordinary control before a tree node to achieve certain effects, such as separators between different types of nodes, etc.
    * @param [in] pTreeNode The tree node interface, must not be null
    * @param [in] pControl The ordinary control interface to add, must not be null
    */
    bool InsertControlBeforeNode(TreeNode* pTreeNode, Control* pControl);

    /** Remove an ordinary control from a tree node
    * @param [in] pControl The ordinary control interface to remove, must not be null
    */
    bool RemoveControl(Control* pControl);

    /** Remove a node from the tree
    * @param [in] pTreeNode The tree node interface, must not be null
    */
    bool RemoveTreeNode(TreeNode* pTreeNode);

    /** Remove all nodes
     */
    void RemoveAllNodes();

    /** Select a tree node (if the parent node is not expanded, it is expanded cascadingly)
    * @param [in] pTreeNode The tree node
    */
    virtual bool SelectTreeNode(TreeNode* pTreeNode);

    /** Expand a tree node (if the parent node is not expanded, it is expanded cascadingly)
    * @param [in] pTreeNode The tree node
    */
    bool ExpandTreeNode(TreeNode* pTreeNode);

    /** Ensure that the tree node is visible (if the parent node is not expanded, it is expanded cascadingly)
    * @param [in] pTreeNode The tree node
    */
    bool EnsureTreeNodeVisible(TreeNode* pTreeNode);

    /** Determine whether a tree node is valid
    */
    bool IsValidTreeNode(TreeNode* pTreeNode) const;

public:
    /** Whether multi-select is allowed
    */
    virtual bool IsMultiSelect() const override;

    /** Set whether multi-select is allowed
    */
    virtual void SetMultiSelect(bool bMultiSelect) override;

    /** Whether it is the multi-select check mode. When this function returns true, the business logic is as follows:
    *   (1) For the ListBox of the tree itself: it behaves as single-select logic;
    *   (2) For the CheckBox on the tree node: the selection logic (i.e. the logic related to IsSelected()) is not used;
            the check logic (i.e. the logic related to IsChecked()) is used, and the check logic allows multiple items to be checked.
    */
    bool IsMultiCheckMode() const;

protected:
    /** Calculate how many child items are displayed on the current page
    * @param [in] bIsHorizontal Whether the current layout is a horizontal layout
    * @param [out] nColumns Returns the number of columns
    * @param [out] nRows Returns the number of rows
    * @return Returns the number of records displayed in the visible area
    */
    virtual size_t GetDisplayItemCount(bool bIsHorizontal, size_t& nColumns, size_t& nRows) const override;

private:
    /** The check status of a tree node changes
     * @param [in] pTreeNode Tree node interface
     */
    void OnNodeCheckStatusChanged(TreeNode* pTreeNode);

    /** Whether to draw the background color of the selected state; a virtual function provided as an option
       (for example, when ListBox/TreeView nodes are in multi-select mode, since there are checked items, there is no need to draw the background color of the selected state)
       @param [in] bHasStateImages Whether the current list item has CheckBox check items
    */
    virtual bool CanPaintSelectedColors(bool bHasStateImages) const override;

private:
    //The following functions are intentionally made private to indicate that they are forbidden; the related functions in TreeNode should be used instead
    bool AddItem(Control* pControl) override;
    bool AddItemAt(Control* pControl, size_t iIndex) override;
    bool RemoveItem(Control* pControl) override;
    bool RemoveItemAt(size_t iIndex) override;
    void RemoveAllItems() override;

    /** When switching from multi-select to single-select mode, it is necessary to ensure that there is only one selected item in the list
    * @return Returns true if there are changes, otherwise returns false
    */
    virtual bool OnSwitchToSingleSelect() override;

    /** Synchronize the check status of the currently selected item
    * @return Returns true if there are changes, otherwise returns false
    */
    bool UpdateCurSelItemCheckStatus();

    /** When the CheckBox switches from shown to hidden, synchronize Check and Select
        (1) Change checked items to selected;
        (2) Set all Checked flags to false
       @return Returns true if redraw is needed, otherwise no redraw is needed
    */
    bool OnCheckBoxHided();

    /** When the CheckBox switches from hidden to shown, synchronize Select and Check
        (1) Change selected items to checked;
        (2) The Selected state does not change
      @return Returns true if redraw is needed, otherwise no redraw is needed
    */
    bool OnCheckBoxShown();

private:
    //The indent value of the child nodes, in pixels
    int32_t m_iIndent;

    //The Class of the expand flag image
    UiString m_expandImageClass;

    //The Class of the CheckBox
    UiString m_checkBoxClass;

    //Whether to display the icon
    bool m_bEnableIcon;

    //The root node of the tree
    std::unique_ptr<TreeNode> m_rootNode;

    //Whether multi-select is allowed (check mode)
    bool m_bMultiCheckMode;
};

}

#endif // UI_CONTROL_TREEVIEW_H_
