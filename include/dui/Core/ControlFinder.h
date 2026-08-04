#ifndef UI_CORE_CONTROL_FINDER_H_
#define UI_CORE_CONTROL_FINDER_H_

#include "dui/Core/UiPoint.h"
#include "dui/Core/ControlPtrT.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ui 
{
// Flags for Control::GetControlFlags()
#define UIFLAG_DEFAULT       0x00000000        // Default state
#define UIFLAG_TABSTOP       0x00000001        // Whether the control is allowed to get focus when TAB switches focus

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_DRAG_DROP     0x00000010
#define UIFIND_TOOLTIP       0x00000020
#define UIFIND_ME_FIRST      0x80000000

class Control;
class Box;

typedef struct tagFINDTABINFO
{
    Control* pFocus;
    Control* pLast;
    bool bForward;
    bool bNextIsIt;
} FINDTABINFO;

/** Wrapper of the Window helper functionality for finding Controls
*/
class DUI_API ControlFinder
{
public:
    ControlFinder();
    ~ControlFinder();
    
public:
    /** Set the Box root node
    */
    void SetRoot(Box* pRoot);

    /** Get the associated Box root node
    */
    Box* GetRoot() const;

    /** Find the specified control by coordinates
     * @param[in] pt The specified coordinates
     * @return Returns the control pointer
     */
    Control* FindControl(const UiPoint& pt) const;

    /** Find the control that supports ToolTip by coordinates (a separate function is needed because when the control's mouse_enabled="false" or the container's mouse_child="false", the FindControl function cannot find this control)
     * @param[in] pt The specified coordinates
     * @return Returns the control pointer
     */
    Control* FindToolTipControl(const UiPoint& pt) const;

    /** Find the control that can respond to WM_CONTEXTMENU by coordinates
     * @param[in] pt The specified coordinates
     * @return Returns the control pointer
     */
    Control* FindContextMenuControl(const UiPoint* pt) const;

    /** Find the Box container that supports drag and drop
    * @param [in] pt The specified coordinates
    * @param [in] nDropInId The drag and drop ID value (each control can set an ID to receive drag and drop; 0 means cannot drop in, non-zero means can drop in)
    */
    Box* FindDroppableBox(const UiPoint& pt, uint8_t nDropInId) const;

    /** Find a control by the control name
     * @param [in] pAncestor A control at the ancestor level
     * @param [in] strName The control name
     * @return Returns the control pointer
     */
    Control* FindControlInCache(Control* pAncestor, const DString& strName) const;

    /** Find a child control by coordinates
     * @param [in] pParent The control to search
     * @param [in] pt The coordinates to search for
     * @return Returns the control pointer
     */
    Control* FindSubControlByPoint(Control* pParent, const UiPoint& pt) const;

    /** Find a child control by name
     * @param [in] pParent The control to search
     * @param [in] strName The name to search for
     * @return Returns the control pointer
     */
    Control* FindSubControlByName(Control* pParent, const DString& strName) const;

    /** Add a control and index it by the control name
    */
    void AddControl(Control* pControl);

    /**@brief Recycle the control (a control added through the AddControl function)
     * @param[in] pControl The control pointer
     */
    void RemoveControl(Control* pControl);

    /** Clear the state
    */
    void Clear();

public:
    static Control* FindControlFromPoint(Control* pThis, void* pData);
    static Control* FindControlFromTab(Control* pThis, void* pData);
    static Control* FindControlFromUpdate(Control* pThis, void* pData);
    static Control* FindControlFromName(Control* pThis, void* pData);
    static Control* FindContextMenuControl(Control* pThis, void* pData);
    static Control* FindControlFromDroppableBox(Control* pThis, void* pData);

private:
    /** The root node
    */
    Box* m_pRoot;

    /** The mapping between the control name and the control, used for fast control lookup
    */
    std::unordered_map<DString, std::vector<ControlPtr>> m_controlNameMap;
};

} // namespace ui

#endif // UI_CORE_CONTROL_FINDER_H_
