#ifndef EXAMPLES_BROWSER_BOX_H_
#define EXAMPLES_BROWSER_BOX_H_

// duilib
#include "duilib/duilib.h"

// WebView2
#include "duilib/duilib_webview2.h"

/** Tab box
*/
class BrowserForm;
class BrowserBox : public ui::VBox
{
    typedef ui::VBox BaseClass;
public:
    friend class BrowserForm;

    /** Constructor
    * @param [in] pWindow the associated window
    * @param [in] browserId the unique identifier of the browser box, used to distinguish different tabs
    */
    BrowserBox(ui::Window* pWindow, std::string browserId);
    virtual ~BrowserBox() override {};

    /**
    * Get the id
    * @return string the identifier id
    */
    std::string GetBrowserId() const { return m_browserId; }

    /**
    * Get the browser window pointer that this browser box belongs to
    * @return BrowserForm* the browser window
    */
    BrowserForm* GetBrowserForm() const;

    /** Get the WebView2Control control pointer
    */
    ui::WebView2Control* GetWebView2Control();

    /** Get the web page title
    */
    const DString& GetTitle() const;

    /** Initialize the browser box
    * @param [in] url the initial URL
    */
    virtual void InitBrowserBox(const DString& url);

    /** Uninitialize the browser box
    */
    virtual void UninitBrowserBox();

    //////////////////////////////////////////////////////////////////////////
    //Operations related to the window merge feature
public:
    /** Control type
    */
    virtual DString GetType() const override { return _T("BrowserBox"); }

    /**
    * Override the base class virtual function to specify the form this control belongs to
    * @param[in] pWindow the pointer of the window it belongs to
    */
    virtual void SetWindow(ui::Window* pWindow) override;

protected:
    /** Get the focus
    */
    virtual bool OnSetFocus(const ui::EventArgs& msg) override;
   
private:
    ui::WebView2Control* m_pWebView2Control;
    BrowserForm* m_pBrowserForm;
    std::string m_browserId;
    DString m_url;
    DString m_title;
};

#endif //EXAMPLES_BROWSER_BOX_H_
