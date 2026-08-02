#ifndef EXAMPLES_RENDER_TEST1_H_
#define EXAMPLES_RENDER_TEST1_H_

// duilib
#include "duilib/duilib.h"

namespace ui {

class RenderTest1 : public ui::Control
{
    typedef ui::Control BaseClass;
public:
    explicit RenderTest1(ui::Window* pWindow);
    virtual ~RenderTest1() override;

public:
    /** Draw the control itself and its child controls
     * @param[in] pRender render interface
     * @param[in] rcPaint the specified drawing area
     */
    virtual void AlphaPaint(IRender* pRender, const UiRect& rcPaint) override;

    /** Draw the control itself
    * @param[in] pRender render interface
    * @param[in] rcPaint the specified drawing area
    */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

    /** Draw the child controls of the control
    * @param[in] pRender render interface
    * @param[in] rcPaint the specified drawing area
    */
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;

private:
    /** The image to draw
    */
    std::unique_ptr<Image> m_pImage;
};

} //end of namespace ui
#endif //EXAMPLES_RENDER_TEST1_H_
