#ifndef UI_CONTROL_PROPERTY_GRID_H_
#define UI_CONTROL_PROPERTY_GRID_H_

#include "duilib/Box/VBox.h"
#include "duilib/Core/FontManager.h"
#include "duilib/Control/Split.h"
#include "duilib/Control/Label.h"
#include "duilib/Control/RichText.h"
#include "duilib/Control/RichEdit.h"
#include "duilib/Control/TreeView.h"
#include "duilib/Control/Combo.h"
#include "duilib/Control/ComboButton.h"
#include "duilib/Control/DateTime.h"
#include "duilib/Control/IPAddress.h"
#include "duilib/Control/HotKey.h"
#include "duilib/Utils/FileDialog.h"

namespace ui
{
/** Supported properties of the property grid control
*/
class PropertyGridGroup;
class PropertyGridProperty;
class PropertyGridTextProperty;         //text and numbers
class PropertyGridComboProperty;        //combo box
class PropertyGridFontProperty;         //font name
class PropertyGridFontSizeProperty;     //font size
class PropertyGridColorProperty;        //color
class PropertyGridDateTimeProperty;     //date and time
class PropertyGridIPAddressProperty;    //IP address
class PropertyGridHotKeyProperty;       //hot key
class PropertyGridFileProperty;         //file path
class PropertyGridDirectoryProperty;    //folder

/** Property grid control
*/
class DUILIB_API PropertyGrid : public VBox
{
    typedef VBox BaseClass;
public:
    explicit PropertyGrid(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** When the DPI changes, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Set whether to display the header
    */
    void SetEnableHeaderCtrl(bool bEnable,
                             const DString& sLeftColumn = _T(""), 
                             const DString& sRightColumn = _T(""));
    /** Determine whether the header is currently displayed
    */
    bool IsEnableHeaderCtrl() const { return m_bHeaderCtrl; }

    /** Get the Header interface
    */
    Control* GetHeaderCtrl() const { return m_pHeaderCtrl; }

    /** Get the left column in the Header
    */
    Label* GetHeaderLeft() const { return m_pHeaderLeft; }

    /** Get the right column in the Header
    */
    Label* GetHeaderRight() const { return m_pHeaderRight; }

    /** Get the split bar in the Header
    */
    Split* GetHeaderSplit() const { return m_pHeaderSplit; }

public:
    /** Add a group
    * @param [in] groupName The name of the group
    * @param [in] description The description of the group
    * @param [in] nGroupData User-defined data
    * @return Returns the interface of the group, which can be used to add properties
    */
    PropertyGridGroup* AddGroup(const DString& groupName, 
                                const DString& description = _T(""),
                                size_t nGroupData = 0);

    /** Get all groups
    * @param [out] groups Returns the list of all current groups
    */
    void GetGroups(std::vector<PropertyGridGroup*>& groups) const;

    /** Delete a group
    * @param [in] pGroup The group to be deleted
    */
    bool RemoveGroup(PropertyGridGroup* pGroup);

    /** Delete all groups
    */
    void RemoveAllGroups();

    /** Add a property (created by the caller)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] pProperty The property interface
    */
    bool AddProperty(PropertyGridGroup* pGroup, PropertyGridProperty* pProperty);

    /** Add a property (text, number type)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridTextProperty* AddTextProperty(PropertyGridGroup* pGroup,
                                              const DString& propertyName, 
                                              const DString& propertyValue,
                                              const DString& description = _T(""),
                                              size_t nPropertyData = 0);

    /** Add a property (combo box)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridComboProperty* AddComboProperty(PropertyGridGroup* pGroup,
                                                const DString& propertyName, 
                                                const DString& propertyValue,
                                                const DString& description = _T(""),
                                                size_t nPropertyData = 0);

    /** Add a property (font name)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (font name)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridFontProperty* AddFontProperty(PropertyGridGroup* pGroup,
                                              const DString& propertyName, 
                                              const DString& propertyValue,
                                              const DString& description = _T(""),
                                              size_t nPropertyData = 0);

    /** Add a property (font size)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (font size)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridFontSizeProperty* AddFontSizeProperty(PropertyGridGroup* pGroup,
                                                      const DString& propertyName, 
                                                      const DString& propertyValue,
                                                      const DString& description = _T(""),
                                                      size_t nPropertyData = 0);

    /** Add a property (color)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (font size)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridColorProperty* AddColorProperty(PropertyGridGroup* pGroup,
                                                const DString& propertyName, 
                                                const DString& propertyValue,
                                                const DString& description = _T(""),
                                                size_t nPropertyData = 0);

    /** Add a property (date and time)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] dateTimeValue The value of the property (date and time value)
    * @param [in] editFormat The edit format of the date
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridDateTimeProperty* AddDateTimeProperty(PropertyGridGroup* pGroup,
                                                      const DString& propertyName, 
                                                      const DString& dateTimeValue,                                                      
                                                      const DString& description = _T(""),
                                                      size_t nPropertyData = 0,
                                                      DateTime::EditFormat editFormat = DateTime::EditFormat::kDateCalendar);
   
    /** Add a property (IP address)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridIPAddressProperty* AddIPAddressProperty(PropertyGridGroup* pGroup,
                                                        const DString& propertyName, 
                                                        const DString& propertyValue,
                                                        const DString& description = _T(""),
                                                        size_t nPropertyData = 0);

    /** Add a property (hot key)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridHotKeyProperty* AddHotKeyProperty(PropertyGridGroup* pGroup,
                                                  const DString& propertyName, 
                                                  const DString& propertyValue,
                                                  const DString& description = _T(""),
                                                  size_t nPropertyData = 0);

    /** Add a property (file path)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (file path)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @param [in] bOpenFileDialog true means open a file, false means save a file
    * @param [in] fileTypes The file types that the dialog can open or save
    * @param [in] nFileTypeIndex The selected file type, valid range: [0, fileTypes.size())
    * @param [in] defaultExt The default file type, for example: "doc;docx"
    * @return Returns the interface of the property
    */
    PropertyGridFileProperty* AddFileProperty(PropertyGridGroup* pGroup,
                                              const DString& propertyName, 
                                              const DString& propertyValue,                                              
                                              const DString& description = _T(""),
                                              size_t nPropertyData = 0,
                                              bool bOpenFileDialog = true,
                                              const std::vector<FileDialog::FileType>& fileTypes = std::vector<FileDialog::FileType>(),
                                              int32_t nFileTypeIndex = -1,
                                              const DString& defaultExt = _T(""));

    /** Add a property (folder)
    * @param [in] pGroup The group to which the property belongs
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @return Returns the interface of the property
    */
    PropertyGridDirectoryProperty* AddDirectoryProperty(PropertyGridGroup* pGroup,
                                                        const DString& propertyName, 
                                                        const DString& propertyValue,
                                                        const DString& description = _T(""),
                                                        size_t nPropertyData = 0);

    /** Set the width of the left column
    * @param [in] nLeftColumnWidth The width of the left column
    * @param [in] bNeedDpiScale Whether the column width value needs DPI adaptation
    */
    void SetLeftColumnWidth(int32_t nLeftColumnWidth, bool bNeedDpiScale);

    /** Get the width value of the left column
    */
    int32_t GetLeftColumnWidth() const;

public:
    /** Set whether to display the description area
    */
    void SetEnableDescriptionArea(bool bEnable);

    /** Determine whether the description area is displayed
    */
    bool IsEnableDescriptionArea() const { return m_bDescriptionArea; }

    /** Set the height value of the description area
    * @param [in] nHeight The height value
    * @param [in] bNeedDpiScale Whether the column width value needs DPI adaptation
    */
    void SetDescriptionAreaHeight(int32_t nHeight, bool bNeedDpiScale);

    /** Get the height value of the current description area
    */
    int32_t GetDescriptionAreaHeight() const;

    /** Get the interface of the description control
    */
    RichText* GetDescriptionArea() const { return m_pDescriptionArea; }

    /** The split bar interface of the description control
    */
    Split* GetDescriptionAreaSplit() const { return m_pDescriptionAreaSplit; }

public:
    /** Get the tree control interface of the property grid (used to manage data)
    */
    TreeView* GetTreeView() const { return m_pTreeView; }

    /** The width of the horizontal grid lines
    * @param [in] nLineWidth The width of the grid lines; if it is 0, the horizontal grid lines are not displayed
    * @param [in] bNeedDpiScale If true, the width needs DPI adaptation
    */
    void SetRowGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetRowGridLineWidth() const;

    /** The color of the horizontal grid lines
    * @param [in] color The color of the horizontal grid lines
    */
    void SetRowGridLineColor(const DString& color);
    DString GetRowGridLineColor() const;

    /** The width of the vertical grid lines
    * @param [in] nLineWidth The width of the grid lines; if it is 0, the vertical grid lines are not displayed
    * @param [in] bNeedDpiScale If true, the width needs DPI adaptation
    */
    void SetColumnGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetColumnGridLineWidth() const;

    /** The color of the vertical grid lines
    * @param [in] color The color of the vertical grid lines
    */
    void SetColumnGridLineColor(const DString& color);
    DString GetColumnGridLineColor() const;

    /** The Class of the header
    */
    void SetHeaderClass(const DString& headerClass);
    DString GetHeaderClass() const;

    /** The Class of the group
    */
    void SetGroupClass(const DString& groupClass);
    DString GetGroupClass() const;

    /** The Class of the property
    */
    void SetPropertyClass(const DString& propertyClass);
    DString GetPropertyClass() const;

protected:
    /** Initialization function
     */
    virtual void OnInit() override;

    /** Draw child controls
    */
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;

    /** Draw grid lines
    */
    void PaintGridLines(IRender* pRender);

    /** Event response function for dragging the list header to change the column width
    */
    void OnHeaderColumnResized();

    /** Adjust the column width of a property node
    */
    void ResizePropertyColumn(TreeNode* pPropertyNode, int32_t nLeftColumnWidth);

private:
    /** Get the value of the left column width
    */
    int32_t GetLeftColumnWidthValue() const;

private:
    /** Configure the XML file
    */
    UiString m_configXml;

private:
    /** Whether to display the Header
    */
    bool m_bHeaderCtrl;

    /** Header interface
    */
    Control* m_pHeaderCtrl;

    /** The left column in the Header
    */
    Label* m_pHeaderLeft;

    /** The right column in the Header
    */
    Label* m_pHeaderRight;

    /** The split bar in the Header
    */
    Split* m_pHeaderSplit;

    /** The width of the left column
    */
    int32_t m_nLeftColumnWidth;

private:
    /** The split bar of the description control
    */
    Split* m_pDescriptionAreaSplit;

    /** Description control
    */
    RichText* m_pDescriptionArea;

    /** Whether to display the description area
    */
    bool m_bDescriptionArea;

private:
    /** The tree control interface of the property grid
    */
    TreeView* m_pTreeView;

    /** The Class of the header
    */
    UiString m_headerClass;

    /** The Class of the group
    */
    UiString m_groupClass;

    /** The Class of the property
    */
    UiString m_propertyClass;

private:
    /** The width of the horizontal grid lines
    */
    int32_t m_nRowGridLineWidth;

    /** The color of the horizontal grid lines
    */
    UiString m_rowGridLineColor;

    /** The width of the vertical grid lines
    */
    int32_t m_nColumnGridLineWidth;

    /** The color of the vertical grid lines
    */
    UiString m_columnGridLineColor;
};

/** The group of the property grid, basic structure
*   <PropertyGridGroup>
*        <HBox>
*            <LabelBox/>
*        </HBox>
*   </PropertyGridGroup>
*/
class DUILIB_API PropertyGridGroup : public TreeNode
{
    typedef TreeNode BaseClass;
public:
    /** Construct a group
    * @param [in] groupName The name of the group
    * @param [in] description The description of the group
    * @param [in] nGroupData User-defined data
    */
    explicit PropertyGridGroup(Window* pWindow,
                               const DString& groupName,
                               const DString& description = _T(""),
                               size_t nGroupData = 0);

public:
    /** Get the property name
    */
    DString GetGroupName() const { return m_groupName.c_str(); }

    /** Get the description of the group
    */
    DString GetDescriptiion() const { return m_description.c_str(); }

    /** Get user-defined data
    */
    size_t GetGroupData() const { return m_nGroupData; }

    /** Set user-defined data
    */
    void SetGroupData(size_t nGroupData) { m_nGroupData = nGroupData; }

    /** Get the display control of the property name
    */
    LabelBox* GetLabelBox() const { return m_pLabelBox; }

    /** Get all properties under the group
    * @param [out] properties Returns the list of all current groups
    */
    void GetProperties(std::vector<PropertyGridProperty*>& properties) const;

    /** Delete a property under the group
    * @param [in] pProperty The property interface to be deleted
    */
    bool RemoveProperty(PropertyGridProperty* pProperty);

    /** Delete all properties under the group
    */
    void RemoveAllProperties();

protected:
    /** Initialization function
     */
    virtual void OnInit() override;

private:
    /** The name of the group
    */
    UiString m_groupName;

    /** The description of the group
    */
    UiString m_description;

    /** User-defined data
    */
    size_t m_nGroupData;

    /** The display control of the property name
    */
    LabelBox* m_pLabelBox;
};

/** The type of the property
*/
enum class PropertyGridPropertyType
{
    kNone,        //No specific type, base class
    kText,        //Normal text
    kCombo,        //combo box
    kFont,        //font name
    kFontSize,  //font size
    kColor,        //color
    kDateTime,    //date and time
    kIPAddress,    //IP address
    kHotKey,    //hot key
    kFile,        //file path
    kDirectory,    //folder
    kCustom        //User-defined type, for example, implement a subclass yourself
};

/** The property of the property grid, basic structure
*   <PropertyGridProperty>
*        <HBox>
*            <LabelBox/>
*            <LabelBox/>
*        </HBox>
*   </PropertyGridProperty>
*/
class DUILIB_API PropertyGridProperty: public TreeNode
{
    typedef TreeNode BaseClass;
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    */
    PropertyGridProperty(Window* pWindow, 
                         const DString& propertyName,
                         const DString& propertyValue,
                         const DString& description = _T(""),
                         size_t nPropertyData = 0);

public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const 
    { 
        return PropertyGridPropertyType::kNone; 
    }

    /** Get the property name
    */
    DString GetPropertyName() const { return m_propertyName.c_str(); }

    /** Get the property value (original value)
    */
    DString GetPropertyValue() const { return m_propertyValue.c_str(); }

    /** Get the description of the property
    */
    DString GetDescriptiion() const { return m_description.c_str(); }

    /** Get user-defined data
    */
    size_t GetPropertyData() const { return m_nPropertyData; }

    /** Set user-defined data
    */
    void SetPropertyData(size_t nPropertyData) { m_nPropertyData = nPropertyData; }

    /** Get the container control containing the property name and property value, which can be used to set the background color, etc.
    */
    HBox* GetHBox() const {    return m_pHBox;    }

    /** Get the display control of the property name; the parent control is GetHBox()
    */
    LabelBox* GetLabelBoxLeft() const { return m_pLabelBoxLeft; }

    /** Get the display control of the property value; the parent control is GetHBox()
    */
    LabelBox* GetLabelBoxRight() const { return m_pLabelBoxRight; }

    /** Set read-only mode
    */
    void SetReadOnly(bool bReadOnly);

    /** Whether it is read-only mode
    */
    bool IsReadOnly() const { return m_bReadOnly; }

    /** Get the new property value (the modified property value; if unmodified, the original value is returned)
    */
    virtual DString GetPropertyNewValue() const;

protected:
    /** Initialization function
     */
    virtual void OnInit() override;

    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool /*bEnable*/) {}

    /** Show or hide the edit control
    * @param [in] bShow true means show the edit control, false means hide the edit control
    * @return Returns the interface of the edit control
    */
    virtual Control* ShowEditControl(bool /*bShow*/) { return nullptr; }

    /** The scroll bar has scrolled (used to handle the position of the pop-up sub-window)
    */
    virtual void OnScrollPosChanged() {}

    /** Get the Margin.right of the edit control (to avoid the scroll bar covering the edit control)
    */
    int32_t GetEditControlMarginRight() const;

protected:
    /** Set the text of the property value (display control)
    * @param [in] text The text content
    * @param [in] bChanged Whether to mark as changed
    */
    void SetPropertyText(const DString& text, bool bChanged);

    /** Get the text of the property value (display control)
    */
    DString GetPropertyText() const;

    /** Set the text color of the property value (display control)
    * @param [in] text The text content
    */
    void SetPropertyTextColor(const DString& textColor);

    /** Set the focus to the property value text display control
    */
    void SetPropertyFocus();

    /** Add a control to the LabelBox of the property value
    */
    bool AddPropertySubItem(Control* pControl);

    /** Remove a control from the LabelBox of the property value
    */
    bool RemovePropertySubItem(Control* pControl);

    /** Determine whether the LabelBox of the property value contains a control
    */
    bool HasPropertySubItem(Control* pControl) const;

private:
    /** The name of the property
    */
    UiString m_propertyName;

    /** The value of the property
    */
    UiString m_propertyValue;

    /** The description of the property
    */
    UiString m_description;

    /** User-defined data
    */
    size_t m_nPropertyData;

    /** The container control containing the property name and property value
    */
    HBox* m_pHBox;

    /** The display control of the property name
    */
    LabelBox* m_pLabelBoxLeft;

    /** The display control of the property value
    */
    LabelBox* m_pLabelBoxRight;

    /** Read-only mode
    */
    bool m_bReadOnly;
};

/** Text type property: edited with RichEdit
*/
class DUILIB_API PropertyGridTextProperty : public PropertyGridProperty
{
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    */
    PropertyGridTextProperty(Window* pWindow,
                             const DString& propertyName,
                             const DString& propertyValue,
                             const DString& description = _T(""),
                             size_t nPropertyData = 0);

public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kText;
    }

    /** Get the new property value (the modified property value; if unmodified, the original value is returned)
    */
    virtual DString GetPropertyNewValue() const override;

    /** Get the edit control
    */
    RichEdit* GetRichEdit() const { return m_pRichEdit; }

    /** Set new text
    * @param [in] newText The new text content
    */
    void SetNewTextValue(const DString& newText);

    /** Set password mode (display ***)
     * @param[in] bPasswordMode Set to true to make the control display content as ***, false to display normal content
     */
    void SetPasswordMode(bool bPasswordMode);

    /** Whether it is password mode
    */
    bool IsPasswordMode() const { return m_bPasswordMode; }

    /** Set whether the Spin control is supported
    * @param [in] bEnable true means the Spin control is supported, false means the Spin control is not supported
    * @param [in] nMin Sets the minimum value of the number
    * @param [in] nMax Sets the maximum value of the number; if both nMin and nMax are 0, the minimum and maximum values of the number are not set
    */
    void SetEnableSpin(bool bEnable, int32_t nMin = 0, int32_t nMax = 0);

protected:
    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool bEnable) override;

    /** Show or hide the edit control
    * @param [in] bShow true means show the edit control, false means hide the edit control
    */
    virtual Control* ShowEditControl(bool bShow) override;

private:
    /** Edit control (used to modify the property)
    */
    RichEdit* m_pRichEdit;

    /** Password mode
    */
    bool m_bPasswordMode;
};

/** Combo box type property: edited with Combo
*/
class DUILIB_API PropertyGridComboProperty : public PropertyGridProperty
{
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    */
    PropertyGridComboProperty(Window* pWindow, 
                              const DString& propertyName,
                              const DString& propertyValue,
                              const DString& description = _T(""),
                              size_t nPropertyData = 0);

public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kCombo;
    }

    /** Get the new property value (the modified property value; if unmodified, the original value is returned)
    */
    virtual DString GetPropertyNewValue() const override;

    /** Add a combo box option
    * @param [in] optionText The content of the combo box list item
    * @return Returns the index value of the item
    */
    size_t AddOption(const DString& optionText);

    /** Get the format of the combo box options
    */
    size_t GetOptionCount() const;

    /** Get the text of the dropdown list item
    * @param [in] nIndex The index value of the item, valid range: [0, GetOptionCount())
    */
    DString GetOption(size_t nIndex) const;

    /** Set the data associated with the item
    * @param [in] nIndex The index value of the item, valid range: [0, GetOptionCount())
    * @param [in] nOptionData The associated data
    */
    void SetOptionData(size_t nIndex, size_t nOptionData);

    /** Get the data associated with the item
    * @param [in] nIndex The index value of the item, valid range: [0, GetOptionCount())
    */
    size_t GetOptionData(size_t nIndex) const;

    /** Delete the specified item
    * @param [in] nIndex The index value of the item, valid range: [0, GetOptionCount())
    */
    bool RemoveOption(size_t nIndex);

    /** Delete all items
    */
    void RemoveAllOptions();

    /** Get the index of the currently selected item
     * @return Returns the index of the currently selected item (if there is no valid index, Box::InvalidIndex is returned)
     */
    size_t GetCurSel() const;

    /** Select an item without triggering the selection event
     * @param[in] nIndex The index of the item to select, valid range: [0, GetOptionCount())
     * @return Returns true on success, otherwise false
     */
    bool SetCurSel(size_t nIndex);

    /** Set to list mode
    * @param [in] bListMode true means editing text is not supported, can only select from the dropdown list; false means editing and selection are allowed
    */
    void SetComboListMode(bool bListMode);

    /** Get the Combo interface
    */
    Combo* GetCombo() const { return m_pCombo; }

protected:
    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool bEnable) override;

    /** Show or hide the edit control
    * @param [in] bShow true means show the edit control, false means hide the edit control
    */
    virtual Control* ShowEditControl(bool bShow) override;

    /** The scroll bar has scrolled (used to handle the position of the pop-up sub-window)
    */
    virtual void OnScrollPosChanged() override;

private:
    /** Combo interface
    */
    Combo* m_pCombo;
};

/** Property for setting the font name
*/
class DUILIB_API PropertyGridFontProperty : public PropertyGridComboProperty
{
    typedef PropertyGridComboProperty BaseClass;
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (original font name)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    */
    PropertyGridFontProperty(Window* pWindow,
                             const DString& propertyName,
                             const DString& propertyValue,
                             const DString& description = _T(""),
                             size_t nPropertyData = 0);

public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kFont;
    }

    /** Get the new font value (the modified property value; if unmodified, the original value is returned)
    */
    virtual DString GetPropertyNewValue() const override;

protected:

    /** Initialization function
     */
    virtual void OnInit() override;
};

/** Property for setting the font size
*/
class DUILIB_API PropertyGridFontSizeProperty : public PropertyGridComboProperty
{
    typedef PropertyGridComboProperty BaseClass;
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (original font name)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    */
    PropertyGridFontSizeProperty(Window* pWindow, 
                                 const DString& propertyName,
                                 const DString& propertyValue,
                                 const DString& description = _T(""),
                                 size_t nPropertyData = 0);

public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kFontSize;
    }

    /** Get the new font size value, the display value (the modified property value; if unmodified, the original value is returned)
    */
    virtual DString GetPropertyNewValue() const override;

    /** Get the font size value, a floating point number, without DPI adaptation
    * @return If selected from the list, the return value is non-empty; if not selected from the list, an empty value is returned
    */
    DString GetFontSize() const;

    /** Get the font size value, a floating point number, with DPI adaptation
    * @return If selected from the list, the return value is non-empty; if not selected from the list, an empty value is returned
    */
    DString GetDpiFontSize() const;

    /** Get the font size value corresponding to the font size display name, a floating point number, without DPI adaptation
    * @param [in] fontSizeName For example: "No.5"
    */
    DString GetFontSize(const DString& fontSizeName) const;

    /** Get the font size value corresponding to the font size display name, a floating point number, with DPI adaptation
    * @param [in] fontSizeName For example: "No.5"
    */
    DString GetDpiFontSize(const DString& fontSizeName) const;

protected:

    /** Initialization function
     */
    virtual void OnInit() override;

private:
    /** Font size
    */
    std::vector<FontSizeInfo> m_fontSizeList;
};

/** Property for setting the color
*/
class DUILIB_API PropertyGridColorProperty : public PropertyGridProperty
{
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (original font name)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    */
    PropertyGridColorProperty(Window* pWindow, 
                              const DString& propertyName,
                              const DString& propertyValue,
                              const DString& description = _T(""),
                              size_t nPropertyData = 0);


public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kColor;
    }

    /** Get the color selection control interface
    */
    ComboButton* GetComboButton() const { return m_pComboButton; }

protected:
    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool bEnable) override;

    /** Show or hide the edit control
    * @param [in] bShow true means show the edit control, false means hide the edit control
    */
    virtual Control* ShowEditControl(bool bShow) override;

    /** The scroll bar has scrolled (used to handle the position of the pop-up sub-window)
    */
    virtual void OnScrollPosChanged() override;

private:
    /** Initialize the Combo button for setting the color
    */
    void InitColorCombo();

    /** Show the color picker window
    */
    void ShowColorPicker();

    /** Set the selected color
    */
    void OnSelectColor(const DString& color);

private:
    /** Color selection control
    */
    ComboButton* m_pComboButton;
};

/** Property for setting the date and time (only available on the Windows platform)
*/
class DUILIB_API PropertyGridDateTimeProperty : public PropertyGridProperty
{
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] dateTimeValue The date and time value
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @param [in] editFormat The format of the date and time
    */
    PropertyGridDateTimeProperty(Window* pWindow, 
                                 const DString& propertyName,
                                 const DString& dateTimeValue,
                                 const DString& description = _T(""),
                                 size_t nPropertyData = 0,
                                 DateTime::EditFormat editFormat = DateTime::EditFormat::kDateCalendar);


public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kDateTime;
    }

    /** Get the date and time control interface
    */
    DateTime* GetDateTime() const { return m_pDateTime; }

protected:
    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool bEnable) override;

    /** Show or hide the edit control
    * @param [in] bShow true means show the edit control, false means hide the edit control
    */
    virtual Control* ShowEditControl(bool bShow) override;

    /** The scroll bar has scrolled (used to handle the position of the pop-up sub-window)
    */
    virtual void OnScrollPosChanged() override;

private:
    /** Date and time control interface
    */
    DateTime* m_pDateTime;

    /** The format of the date and time
    */
    DateTime::EditFormat m_editFormat;
};

/** Property for setting the IP address
*/
class DUILIB_API PropertyGridIPAddressProperty : public PropertyGridProperty
{
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    */
    PropertyGridIPAddressProperty(Window* pWindow, 
                                  const DString& propertyName,
                                  const DString& propertyValue,
                                  const DString& description = _T(""),
                                  size_t nPropertyData = 0);


public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kIPAddress;
    }

    /** Get the IP address control interface
    */
    IPAddress* GetIPAddress() const { return m_pIPAddress; }

    /** Set a new IP address
    * @param [in] newIP The new IP address
    */
    void SetNewIPAddressValue(const DString& newIP);

protected:
    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool bEnable) override;

    /** Show or hide the edit control
    * @param [in] bShow true means show the edit control, false means hide the edit control
    */
    virtual Control* ShowEditControl(bool bShow) override;

private:
    /** IP address control
    */
    IPAddress* m_pIPAddress;
};

/** Property for setting the hot key
*/
class DUILIB_API PropertyGridHotKeyProperty : public PropertyGridProperty
{
public:
    /** Construct a property
    @param [in] propertyName The name of the property
    @param [in] propertyValue The value of the property
    @param [in] description The description of the property
    @param [in] nPropertyData User-defined data
    */
    PropertyGridHotKeyProperty(Window* pWindow, 
                               const DString& propertyName,
                               const DString& propertyValue,
                               const DString& description = _T(""),
                               size_t nPropertyData = 0);


public:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kHotKey;
    }

    /** Get the hot key control interface
    */
    HotKey* GetHotKey() const { return m_pHotKey; }

protected:
    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool bEnable) override;

    /** Show or hide the edit control
    * @param [in] bShow true means show the edit control, false means hide the edit control
    */
    virtual Control* ShowEditControl(bool bShow) override;

private:
    /** Hot key control
    */
    HotKey* m_pHotKey;
};

/** Property for setting the file path
*/
class DUILIB_API PropertyGridFileProperty : public PropertyGridTextProperty
{
    typedef PropertyGridTextProperty BaseClass;
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (the file path)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    * @param [in] bOpenFileDialog true means open a file, false means save a file
    * @param [in] fileTypes The file types that the dialog can open or save
    * @param [in] nFileTypeIndex The selected file type, valid range: [0, fileTypes.size())
    * @param [in] defaultExt The default file type, for example: "doc;docx"
    */
    PropertyGridFileProperty(Window* pWindow, 
                             const DString& propertyName,
                             const DString& propertyValue,
                             const DString& description = _T(""),
                             size_t nPropertyData = 0,
                             bool bOpenFileDialog = true,
                             const std::vector<FileDialog::FileType>& fileTypes = std::vector<FileDialog::FileType>(),
                             int32_t nFileTypeIndex = -1,
                             const DString& defaultExt = _T(""));


protected:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kFile;
    }

    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool bEnable) override;

    /** The browse button was clicked
    */
    virtual void OnBrowseButtonClicked();

private:
    /** Browse button
    */
    Button* m_pBrowseBtn;

    /** true means open a file, false means save a file
    */
    bool m_bOpenFileDialog;

    /** File type filter
    */
    std::vector<FileDialog::FileType> m_fileTypes;

    /** The index value of the selected file type
    */
    int32_t m_nFileTypeIndex;

    /** The default file type
    */
    DString m_defaultExt;
};

/** Property for setting the folder
*/
class DUILIB_API PropertyGridDirectoryProperty : public PropertyGridTextProperty
{
    typedef PropertyGridTextProperty BaseClass;
public:
    /** Construct a property
    * @param [in] propertyName The name of the property
    * @param [in] propertyValue The value of the property (the folder path)
    * @param [in] description The description of the property
    * @param [in] nPropertyData User-defined data
    */
    PropertyGridDirectoryProperty(Window* pWindow, 
                                  const DString& propertyName,
                                  const DString& propertyValue,
                                  const DString& description = _T(""),
                                  size_t nPropertyData = 0);


protected:
    /** Get the property type
    */
    virtual PropertyGridPropertyType GetPropertyType() const override
    {
        return PropertyGridPropertyType::kDirectory;
    }

    /** Set whether an edit control is allowed to exist
    * @param [in] bEnable true means the edit control is allowed to exist, false means the edit control is not allowed to exist
    */
    virtual void EnableEditControl(bool bEnable) override;

    /** The browse button was clicked
    */
    virtual void OnBrowseButtonClicked();

private:
    /** Browse button
    */
    Button* m_pBrowseBtn;
};

}//namespace ui

#endif //UI_CONTROL_PROPERTY_GRID_H_
