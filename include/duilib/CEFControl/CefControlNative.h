/**@brief Wrap the Cef browser object as a duilib control
 * @copyright (c) 2016, NetEase Inc. All rights reserved
 * @author Redrain
 * @date 2016/7/19
 */
#ifndef UI_CEF_CONTROL_CEF_NATIVE_CONTROL_H_
#define UI_CEF_CONTROL_CEF_NATIVE_CONTROL_H_

#include "duilib/CEFControl/CefControl.h"

#ifdef DUILIB_BUILD_FOR_CEF

namespace ui {

/** duilib CEF control, window mode
*/
class DUILIB_API CefControlNative : public CefControl
{
    typedef CefControl BaseClass;
public:
    explicit CefControlNative(ui::Window* pWindow);
    virtual ~CefControlNative(void) override;

    virtual void Init() override;
    virtual void SetPos(ui::UiRect rc) override;
    virtual bool OnSetFocus(const EventArgs& msg) override;
    virtual bool OnKillFocus(const EventArgs& msg) override;
    virtual void SetWindow(ui::Window* pWindow) override;

protected:
    /** Recreate the Browser object
    */
    virtual void ReCreateBrowser() override;

    /** Update the position of the CEF control window (child window mode)
    */
    virtual void UpdateCefWindowPos() override;

    /** Close all Browser objects
    */
    virtual void CloseAllBrowsers() override;

    /** Save the web page as an image, the same size as the control
    */
    virtual std::shared_ptr<IBitmap> MakeImageSnapshot() override;

    /** Whether this is a CEF child window mode control
    */
    virtual bool IsCefNative() const override;

    /** The page has gained focus
    */
    virtual void OnGotFocus() override;

    /** Set the visibility state event
    * @param [in] bChanged true means the state changed, false means the state did not change
    */
    virtual void OnSetVisible(bool bChanged) override;

private:
    /** Close all Browser objects
    */
    void DoCloseAllNativeBrowsers(bool bForceClose);

private:
    /** Whether the first paint event is associated
    */
    bool m_bWindowFirstShown;

    /** Whether the parent-child relationship of the CEF window has been detached
    */
    bool m_bSetCefWindowParentNull;

    /** Whether currently inside the OnGotFocus callback
    */
    bool m_bInGotFocusEvent;
};
} //namespace ui

#endif //DUILIB_BUILD_FOR_CEF
#endif //UI_CEF_CONTROL_CEF_NATIVE_CONTROL_H_
