#ifndef UI_CONTROL_TEXT_DRAWER_H_
#define UI_CONTROL_TEXT_DRAWER_H_

#include "dui/Core/UiTypes.h"
#include "dui/Control/RichText.h"
#include "dui/Render/IRender.h"

namespace ui
{
/** Implementation class for text drawing, supports plain text drawing and RichText text drawing
*/
class DUI_API TextDrawer
{
public:
    TextDrawer();
    ~TextDrawer();

public:
    /** Calculate the width and height of the specified text string
     * @param [in] pRender Rendering engine object
     * @param [in] strText Text content
     * @param [in] measureParam Parameters related to measurement
     * @param [in] fontId Default font ID
     * @param [in] bRichText true means the text content is RichText, false means the text content is plain text
     * @param [in] pOwner Associated control interface
     * @return Returns the width and height of the text string, with the result expressed as a rectangle
     */
    UiRect MeasureString(IRender* pRender,
                         const DString& strText,
                         const MeasureStringParam& measureParam,
                         const DString& fontId,
                         bool bRichText,
                         Control* pOwner);

    /** Draw text
     * @param [in] pRender Rendering engine object
     * @param [in] strText Text content
     * @param [in] drawParam Parameters related to text drawing
     * @param [in] fontId Default font ID
     * @param [in] bRichText true means the text content is RichText, false means the text content is plain text
     * @param [in] pOwner Associated control interface
     */
    void DrawString(IRender* pRender,
                    const DString& strText,
                    const DrawStringParam& drawParam,
                    const DString& fontId,
                    bool bRichText,
                    Control* pOwner);

    /** Set that the original text content has changed (when the text content changes, it needs to be re-parsed)
    */
    void SetTextChanged();

private:
    /** Synchronize text drawing properties
    * @param [in] uFormat Text format, see the enum DrawStringFormat type definition
    * @param [in] fSpacingMul Line spacing multiplier: multiple ratio of the font size (default is usually 1.0, i.e. 100% of the font size)
    * @param [in] fSpacingAdd Line spacing addition: fixed additional pixel value (default is usually 0)
    * @param [in] fontId Default font ID
    */
    void UpdateTextDrawProps(uint32_t uFormat, float fSpacingMul, float fSpacingAdd, const DString& fontId);

private:
    /** RichText implementation interface
    */
    std::unique_ptr<RichText> m_pRichText;

    /** Whether the original text content has changed (when the text content changes, it needs to be re-parsed)
    */
    bool m_bRichTextChanged;
};

}//namespace ui

#endif //UI_CONTROL_TEXT_DRAWER_H_
