#ifndef UI_CONTROL_LINE_H_
#define UI_CONTROL_LINE_H_

#include "duilib/Core/Control.h"

namespace ui
{
/** Line drawing control
*/
class DUILIB_API Line: public Control
{
    typedef Control BaseClass;
public:
    explicit Line(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** The DPI has changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Set whether the line is horizontal or vertical (true means vertical)
    */
    void SetLineVertical(bool bVertical);

    /** Get whether the line is horizontal or vertical
    */
    bool IsLineVertical() const;

    /** Set the line width
    * @param [in] fLineWidth The line width
    * @param [in] bNeedDpiScale Whether DPI scaling is supported
    */
    void SetLineWidth(float fLineWidth, bool bNeedDpiScale);

    /** Get the line width
    */
    float GetLineWidth() const;

    /** Set the line color
    */
    void SetLineColor(const DString& lineColor);

    /** Get the line color
    */
    DString GetLineColor() const;

    /** Set the line dash style
    */
    void SetLineDashStyle(const DString& dashStyle);

    /** Get the line dash style
    */
    DString GetLineDashStyle() const;

protected:
    /** Entry function for drawing the control
    * @param [in] pRender The render interface
    * @param [in] rcPaint The drawing area
    */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

private:
    /** Whether the line is horizontal or vertical (true means vertical)
    */
    bool m_bLineVertical;

    /** The line color
    */
    UiString m_lineColor;

    /** The line width
    */
    float m_fLineWidth;

    /** The line dash style
    */
    int8_t m_dashStyle;
};

}//namespace ui

#endif //UI_CONTROL_LINE_H_
