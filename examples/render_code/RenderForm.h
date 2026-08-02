#ifndef EXAMPLES_RENDER_FORM_H_
#define EXAMPLES_RENDER_FORM_H_

// duilib
#include "duilib/duilib.h"

class RenderForm : public ui::WindowImplBase
{
public:
    RenderForm();
    virtual ~RenderForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path for the window to be drawn
     * The GetSkinFile interface sets the xml description file for the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Called after the window has been closed, for subclasses to do some cleanup work
    */
    virtual void OnCloseWindow() override;

    /**@brief Called when the control to be created does not have a standard control name
     * @param[in] strClass control name
     * @return Returns a pointer to the custom control; normally the custom control is created according to the strClass parameter
     */
    virtual ui::Control* CreateControl(const DString& strClass) override;

private:
    //Test the PropertyGrid control
    void TestPropertyGrid();

    //Get the result of the PropertyGrid control
    void CheckPropertyGridResult();
};

#endif //EXAMPLES_RENDER_FORM_H_
