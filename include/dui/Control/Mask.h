#ifndef UI_CONTROL_MASK_H_
#define UI_CONTROL_MASK_H_

#include "dui/Core/Box.h"

namespace ui {

/** Mask/overlay control (derived from Box)
 *  Used for the following scenarios, on every platform (Windows/Linux/macOS/FreeBSD):
 *  - Background mask of a modal dialog/popup: a translucent overlay covers the whole area and intercepts mouse input;
 *  - Product tour overlay: "cuts out" a highlighted control from the overlay while the rest stays translucent, and
 *    clicks inside the cut-out area can pass through to the target control;
 *  - Loading mask: the overlay covers the content area and intercepts interaction, and child controls such as a
 *    loading animation or a hint text can be placed on top of it.
 *
 *  Mask is essentially a container (Box): arbitrary child controls (progress animation, tour text, buttons, ...)
 *  can be placed on top of the overlay.
 *  When used as a floating overlay, it is recommended to set float="true" width="stretch" height="stretch" and to
 *  add it as the last child of the parent container (or use paint_order) so it is painted on top and hit-tested first.
 *
 *  XML usage examples (the default mask color is the semantic color bg_overlay, and mouse input is intercepted by default):
 *  <Mask class="mask"/>                                        // fills the parent container, intercepts the mouse
 *  <Mask class="mask" mask_color="bg_overlay" mouse_enabled="false"/>  // only obscures visually, does not intercept the mouse
 *  <Mask class="mask" highlight_control="btn_guide" highlight_padding="6" highlight_corner="6"
 *        click_through="true"/>                                // cuts out and highlights btn_guide, clicks pass through
 */
class DUI_API Mask : public Box
{
    typedef Box BaseClass;
public:
    explicit Mask(Window* pWindow);
    Mask(const Mask& r) = delete;
    Mask& operator=(const Mask& r) = delete;
    ~Mask() override;

    /// Overrides of the base class interface for custom behavior; see the Control documentation for the details of each method
    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void PaintBkColor(IRender* pRender) override;
    virtual Control* FindControl(FINDCONTROLPROC Proc, void* pProcData, uint32_t uFlags,
                                 const UiPoint& ptMouse = UiPoint(),
                                 const UiPoint& scrollPos = UiPoint()) override;
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Sets/gets the mask color (a semantic color name, default bg_overlay; empty means no mask color is painted)
    */
    void SetMaskColor(const std::string& strColor);
    const std::string& GetMaskColor() const { return m_strMaskColor; }

    /** Sets/gets the name of the control to cut out (highlight); empty means no cut-out and the whole area is masked
    */
    void SetHighlightControl(const std::string& strControlName);
    const std::string& GetHighlightControl() const { return m_strHighlightControl; }

    /** Sets/gets the padding of the cut-out area around the highlighted control (DPI aware); default 4
    */
    void SetHighlightPadding(int32_t nPadding, bool bNeedDpiScale = true);
    int32_t GetHighlightPadding() const { return m_nHighlightPadding; }

    /** Sets/gets the corner radius of the cut-out area (DPI aware); default 0 (square corners)
    */
    void SetHighlightCorner(int32_t nCorner, bool bNeedDpiScale = true);
    int32_t GetHighlightCorner() const { return m_nHighlightCorner; }

    /** Sets/gets whether mouse clicks inside the cut-out area pass through to the controls below; default false
    *   When true (usually used together with highlight_control), a click inside the cut-out area hits the target
    *   control below, which supports product tours of the form "let the user click the highlighted button".
    */
    void SetClickThrough(bool bThrough);
    bool IsClickThrough() const { return m_bClickThrough; }

public:
    /** Convenience interface: covers the window with a mask layer (default mask color bg_overlay, fills the whole window, intercepts the mouse)
    * @param [in] pWindow Target window
    * @return Returns the created mask control (nullptr on failure); the caller may then set the highlighted control,
    *         append child controls, etc.; call Remove() when it is no longer needed.
    */
    static Mask* ShowOverWindow(Window* pWindow);

    /** Removes this mask from its parent container
    *   Note: when the parent container destroys its children automatically (IsAutoDestroyChild is true), this object
    *        is destroyed by this call and the pointer must not be used after it returns.
    */
    void Remove();

private:
    /** Gets the cut-out (highlight) area in window client coordinates; returns false when there is no valid cut-out area
    */
    bool GetHighlightRect(UiRect& rcHighlight);

    /** Appends a rounded rectangle to the path (clockwise)
    */
    static void AddRoundRectPath(IPath* pPath, const UiRectF& rc, float rx, float ry);

private:
    /** Mask color (semantic color name)
    */
    std::string m_strMaskColor;

    /** Name of the cut-out (highlighted) control; empty means no cut-out
    */
    std::string m_strHighlightControl;

    /** Padding of the cut-out area (physical pixels after DPI scaling)
    */
    int32_t m_nHighlightPadding;

    /** Corner radius of the cut-out area (physical pixels after DPI scaling)
    */
    int32_t m_nHighlightCorner;

    /** Whether clicks inside the cut-out area pass through to the controls below
    */
    bool m_bClickThrough;
};

} // namespace ui

#endif // UI_CONTROL_MASK_H_
