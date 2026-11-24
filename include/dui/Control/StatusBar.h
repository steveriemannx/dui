#ifndef UI_CONTROL_STATUS_BAR_H_
#define UI_CONTROL_STATUS_BAR_H_

#include "dui/Box/HBox.h"

namespace ui
{

/** Status bar control
*   It shows status hints such as the caret position, the character count, the connection state and the background
*   progress, and is a common UI element of editors / IDEs / communication software.
*   It is implemented as a horizontal container (derived from HBox) whose child controls are the individual "status
*   panels" (usually Label / IconControl / Progress and so on):
*   - Child controls may use a fixed width, stretch (width="stretch"), padding and so on, using all layout
*     capabilities of HBox;
*   - A vertical separator is painted automatically between adjacent child controls; it can be adjusted or turned off
*     with separator_width / separator_color;
*   - A size grip can optionally be painted in the bottom-right corner (size_grip). When the status bar is docked in
*     the bottom-right corner of the window client area and the window itself is resizable (the default on Windows;
*     on the other platforms a non-zero size_box has to be declared for the Window in the XML), the grip area is
*     merged into the draggable resize area of the window automatically (by enlarging the size_box of the window), so
*     the window can really be resized by dragging with the mouse; set size_grip_resize="false" when that behavior is
*     not wanted.
*
*   Panel (cell) index API: the application code no longer needs FindControl + dynamic_cast to find a child Label, it
*   can operate the individual status panels by index with GetPanel / SetPanelText and so on.
*
*   The XML node name is "StatusBar" and it inherits all attributes of HBox.
*/
class DUI_API StatusBar : public HBox
{
    typedef HBox BaseClass;
public:
    explicit StatusBar(Window* pWindow);

public:
    /// Overrides of the base class interface for custom behavior; see the Control / Box documentation for the details
    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void SetPos(UiRect rc) override;
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;

public:
    /** @name Status panel (cell) index interface
     *
     *   Panel indices follow the layout order, start at 0 and count only the visible panels (invisible panels do not
     *   take an index), which matches the cell order seen on screen. The indices are renumbered after the panel
     *   content changed (child controls added/removed, visibility changed).
     * @{
     */

    /** Gets the number of status panels (in layout order, excluding invisible panels)
     */
    size_t GetPanelCount() const;

    /** Gets the status panel control by index
     * @param [in] nIndex Panel index, starting at 0, in layout order
     * @return Pointer to the panel control; nullptr when the index is out of range
     */
    Control* GetPanel(size_t nIndex) const;

    /** Gets the panel index of the given control in this status bar
     * @param [in] pPanel Pointer to the control to query
     * @return Panel index (starting at 0); -1 when the control is not a visible panel of this status bar
     */
    int32_t GetPanelIndex(Control* pPanel) const;

    /** Finds a status panel by name (equivalent to looking up a name attribute among the child controls of the
     *  StatusBar)
     * @param [in] strName Name of the panel
     * @return Pointer to the panel control; nullptr when it was not found
     */
    Control* FindPanel(const std::string& strName) const;

    /** Sets the text content of the panel with the given index
     * @param [in] nIndex Panel index, starting at 0
     * @param [in] strText Text content to set
     * @return true when the text was set;
     *         false when the index is out of range or the panel is not a text control (a panel that supports text,
     *         such as Label / RichEdit)
     */
    bool SetPanelText(size_t nIndex, const std::string& strText);

    /** Gets the text content of the panel with the given index
     * @param [in] nIndex Panel index, starting at 0
     * @return Text content of the panel; an empty string when the index is out of range or the panel does not
     *         support text
     */
    std::string GetPanelText(size_t nIndex) const;

    /** @} */

public:
    /** Sets the width of the separator between adjacent status panels (pixels; 0 disables the separator)
    */
    void SetSeparatorWidth(int32_t nWidth);
    int32_t GetSeparatorWidth() const;

    /** Sets the separator color (a semantic color name or a color value string, such as "border_window" or
    *  "#FFD5D5D5")
    */
    void SetSeparatorColor(const std::string& strColor);
    std::string GetSeparatorColor() const;

    /** Sets whether the size grip is painted in the bottom-right corner
    *   Whether the grip can be dragged is controlled by IsSizeGripResize (see SetSizeGripResize)
    */
    void SetShowSizeGrip(bool bShow);
    bool IsShowSizeGrip() const;

    /** Sets whether the size grip takes part in the drag resize of the window
    *   When true (the default) and this status bar is docked in the bottom-right corner of the window client area,
    *   the resizable area of the bottom-right corner of the window (the right/bottom of size_box) is enlarged to
    *   cover the grip, so the grip can really be dragged to resize the window (WM_NCHITTEST on Windows, the hit test
    *   callback on the native backends).
    *   The precondition is that the window itself is resizable: on Windows this is the default (WS_THICKFRAME); the
    *   other platforms need a non-zero size_box declared for the Window in the XML, otherwise resizing is not enabled
    *   when the window is created.
    *   Note: the effect is window-wide, so after enlarging, the whole bottom edge of the window also becomes a
    *   resizable area. When false, the grip is only a visual hint (the historical behavior).
    *   This setting takes effect only while IsShowSizeGrip() is true.
    */
    void SetSizeGripResize(bool bResize);
    bool IsSizeGripResize() const;

    /** Sets the color of the size grip
    */
    void SetSizeGripColor(const std::string& strColor);
    std::string GetSizeGripColor() const;

private:
    /** Paints the separators between adjacent panels
    * @param [in] rcContent Rectangle of the content area of the status bar (the padding is already removed)
    */
    void PaintSeparator(IRender* pRender, const UiRect& rcContent);

    /** Paints the size grip in the bottom-right corner
    * @param [in] rcContent Rectangle of the content area of the status bar (the padding is already removed)
    */
    void PaintSizeGrip(IRender* pRender, const UiRect& rcContent);

    /** Gets the painting area of the size grip (client coordinates; the caller handles the values before DPI scaling)
     * @param [in] rcContent Rectangle of the content area of the status bar (the padding is already removed)
     */
    UiRect GetSizeGripRect(const UiRect& rcContent) const;

    /** Gets the minimum hit size required by the size grip (pixels, already DPI scaled)
    *   This size is used to enlarge the resizable area of the bottom-right corner of the window so that it covers
    *   the grip
    */
    int32_t GetSizeGripHitSize() const;

    /** Refreshes the drag hit area of the size grip (called when the layout position or the grip switch changes)
    */
    void UpdateSizeGripHitArea();

    /** Restores the window size_box that this control enlarged temporarily
    */
    void RestoreSizeBox();

private:
    //Separator width (pixels; 0 disables the separator)
    int32_t m_nSeparatorWidth;

    //Separator color
    UiString m_separatorColor;

    //Whether the size grip is painted in the bottom-right corner
    bool m_bShowSizeGrip;

    //Whether the size grip takes part in the drag resize of the window
    bool m_bSizeGripResize;

    //Size grip color
    UiString m_sizeGripColor;

    //Original value of the window size_box that this control enlarged temporarily (valid when m_bSizeBoxBackupValid is true)
    UiRect m_rcSizeBoxBackup;

    //size_box value written into the window by this control (used to check before restoring that nobody else changed it)
    UiRect m_rcSizeBoxApplied;

    //Whether m_rcSizeBoxBackup / m_rcSizeBoxApplied are valid
    bool m_bSizeBoxBackupValid;
};

} // namespace ui

#endif // UI_CONTROL_STATUS_BAR_H_
