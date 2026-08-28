//MainForm.h
#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"

/** Main window implementation of the application
*/
class MainForm : public ui::WindowImplBase
{
public:
    MainForm();
    virtual ~MainForm() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin directory
    * @return The subclass must implement and return the window skin directory
    */
    virtual DString GetSkinFolder() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin XML description file
    * @return The subclass must implement and return the window skin XML description file
    *         The returned content can be the XML file content (a string starting with the character '<'),
    *         or a file path (a string not starting with the character '<'); the file must be found under the GetSkinFolder() path
    */
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** The window's DPI scale factor changed; update control sizes and layout (for subclasses to use)
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with the value of Dpi().GetDisplayScaleFactor(); it may be the same as nOldScaleFactor
    */
    virtual void OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) override;

protected:
    /** The window size has changed (WM_SIZE)
    * @param [in] sizeType The type that triggered the window size change
    * @param [in] newWindowSize The new window size (width and height)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message was handled successfully and need not be passed to the window procedure; returning false means the message is passed on to the window procedure
    * @return The message handling result; if the application handled this message, zero should be returned
    */
    virtual LRESULT OnSizeMsg(ui::WindowSizeType sizeType, const ui::UiSize& newWindowSize, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

private:
    /** Refresh the UI
    */
    void UpdateUI();
};

#endif //EXAMPLES_MAIN_FORM_H_
