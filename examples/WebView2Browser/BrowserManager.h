#ifndef EXAMPLES_BROWSER_MANAGER_H_
#define EXAMPLES_BROWSER_MANAGER_H_

#include "BrowserBox.h"

/** Multi-tab browser manager class
 */
class BrowserManager : public virtual ui::SupportWeakCallback
{
public:
    BrowserManager();
    BrowserManager(const BrowserManager&) = delete;
    BrowserManager& operator=(const BrowserManager&) = delete;

    /** Singleton object
    */
    static BrowserManager* GetInstance();

private:
    ~BrowserManager();

public:
    /** Create a Browser window
    */
    BrowserForm* CreateBrowserForm();

    /** Create a browser box
    * @param[in] pBrowserForm the browser window; if it is nullptr, a new window is created, otherwise a new browser box is created in this window
    * @param[in] browserId the unique identifier of a browser box (can be empty)
    * @param[in] url the initial URL
    * @return BorwserBox*  the browser box pointer
    */
    BrowserBox* CreateBorwserBox(BrowserForm* pBrowserForm, std::string browserId, const DString& url);

    /** Determine whether a browser box is active
    * @param[in] browserId the unique identifier of a browser box
    * @return bool true active, false inactive
    */
    bool IsBorwserBoxActive(const std::string& browserId);

    /** Find the browser box by the session id
    * @param [in] browserId the unique identifier of a browser box
    * @return BorwserBox* the pointer of the browser box
    */
    BrowserBox* FindBorwserBox(const std::string& browserId);

    /** Remove the browser box information in this class by the session id
    * @param [in] browserId the unique identifier of a browser box
    * @param [in] box the pointer of the browser box corresponding to the id
    */
    void RemoveBorwserBox(const std::string& browserId, const BrowserBox* box = nullptr);

    /** Find an active window
    */
    BrowserForm* GetLastActiveBrowserForm() const;

    /** Create a Browser ID
    */
    std::string CreateBrowserID() const;

private:
    /** All browser boxes are kept in this container by ID
    */
    std::map<std::string, BrowserBox*> m_boxMap;
};
#endif //EXAMPLES_BROWSER_MANAGER_H_
