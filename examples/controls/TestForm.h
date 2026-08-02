#ifndef EXAMPLES_TEST_FORM_H_
#define EXAMPLES_TEST_FORM_H_

// duilib
#include "duilib/duilib.h"

class TestForm : public ui::WindowImplBase
{
public:
    TestForm();
    virtual ~TestForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path of the window to be drawn
     * The GetSkinFile interface sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Timer function
    */
    void OnTimer();

private:
    /** Progress value
    */
    double m_nProgressValue;
};

#endif //EXAMPLES_TEST_FORM_H_
