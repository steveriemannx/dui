#ifndef UI_CORE_BOX_SHADOW_H_
#define UI_CORE_BOX_SHADOW_H_

#include "duilib/Core/UiPoint.h"
#include "duilib/Core/UiRect.h"
#include <string>

namespace ui {

class Control;

/** Control shadow property class
*/
class DUILIB_API BoxShadow
{
public:
    /** Constructor
    * @param [in] pControl The associated control interface
    */
    explicit BoxShadow(Control* pControl);

    /** Set the shadow properties
     * @param[in] strBoxShadow The properties to set, such as "color='black' offset='1,1' blur_radius='2' spread_radius='2'"
     */
    void SetBoxShadowString(const DString& strBoxShadow);

    /** Whether there is a box-shadow
     */
    bool HasShadow() const;

    /** Calculate the actual paint rectangle range including the shadow
    * @param [in] controlRect The rectangle range of the control itself
    * @return Returns the rectangle range including the box-shadow (the whole range needs to be painted)
    */
    UiRect GetExpandedRect(const UiRect& controlRect) const;

public:
    // Shadow properties
    // For now, only color, offset and blurSize are supported
    // https://codersblock.com/blog/creating-glow-effects-with-css/

    /** The color value of the shadow
    */
    DString m_strColor;

    /** The shadow offset (offset-x and offset-y)
     *                 <offset-x> The horizontal offset; if negative, the shadow is on the left of the rectangle.
     *                 <offset-y> The vertical offset; if negative, the shadow is above the rectangle.
     */
    UiPoint m_cpOffset;

    /** The blur radius; the larger the value, the larger the blur area, and the bigger and lighter the shadow. If it is 0, the shadow edge is sharp with no blur effect; it cannot be negative.
     */
    int32_t m_nBlurRadius;

    /** The spread radius, i.e. how many pixels the blur area extends from the edge of the rc rectangle. If positive, the shadow expands; if negative, the shadow shrinks.
     */
    int32_t m_nSpreadRadius;

    /** The associated control interface
    */
    Control* m_pControl;
};

} // namespace ui

#endif // UI_CORE_BOX_SHADOW_H_

