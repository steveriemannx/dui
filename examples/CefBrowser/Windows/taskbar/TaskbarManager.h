#ifndef EXAMPLES_TASKBAR_MANAGER_H_
#define EXAMPLES_TASKBAR_MANAGER_H_

// duilib
#include "duilib/duilib.h"

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)

#include <shobjidl_core.h>

interface ITaskbarList4;

/** @class TaskbarTabItem
  * @brief a taskbar thumbnail Tab attached to a window and shown in the taskbar (only available on Win7 and above)
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2019/3/20
  */
class TaskbarManager;
class TaskbarTabItem final : public ui::Window
{
    typedef ui::Window BaseClass;
public:
    /**
    * @param[in] pBindControl the pointer of the control bound with TaskbarTabItem, which provides the thumbnail and preview images
    * Constructor
    */
    explicit TaskbarTabItem(ui::Control* pBindControl);

    /**
    * Get the control pointer bound with TaskbarTabItem
    * @return ui::Control* the control pointer
    */
    ui::Control* GetBindControl();

    /**
    * Get the unique id of the thumbnail Tab
    * @return std::string& the id
    */
    std::string& GetId();

    /** Initialization function, called immediately after TaskbarTabItem is created with new
    * @param [in] taskbarTitle the title displayed on the taskbar thumbnail
    * @param [in] id the unique id of the thumbnail Tab
    */
    void Init(const DString& taskbarTitle, const std::string& id);

    /** Uninitialization function
    */
    void UnInit();

    /** Set the title of this Tab's thumbnail
    * @param[in] title the title
    */
    void SetTaskbarTitle(const DString& title);

    /** Set the window icon of this Tab's thumbnail
    * @param [in] title the title
    */
    void SetTaskbarIcon(HICON hIcon);

    /** Set the TaskbarManager corresponding to the form this Tab belongs to
    * @param[in] pTaskbarManager the TaskbarManager pointer
    */
    void SetTaskbarManager(TaskbarManager* pTaskbarManager);

    /** Get the TaskbarManager corresponding to the form this Tab belongs to
    * @return TaskbarManager* the TaskbarManager pointer
    */
    TaskbarManager* GetTaskbarManager();

    /**
    * Set the thumbnail of TaskbarTabItem in the taskbar to the invalid state, so that the thumbnail is redrawn
    * @return bool true success, false failure
    */
    bool InvalidateTab();

private:
    /**
    * Intercept and handle the underlying form messages
    * @param[in] uMsg the message type
    * @param[in] wParam additional parameter
    * @param[in] lParam additional parameter
    * @param[in out] bHandled whether the message was handled; if handled, the message is not passed on
    * @return LRESULT the processing result
    */
    virtual LRESULT OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

    /**
    * Respond to the WM_DWMSENDICONICTHUMBNAIL message
    * @param[in] width the required bitmap width
    * @param[in] height the required bitmap height
    * @return void    no return value
    */
    void OnSendThumbnail(int width, int height);

    /**
    * Respond to the WM_DWMSENDICONICLIVEPREVIEWBITMAP message
    * @return void    no return value
    */
    void OnSendPreview();

private:
    bool m_bWin7orGreater;
    ui::Control* m_pBindControl;
    std::string m_id;
    TaskbarManager* m_taskbarManager;
};

/** @class TaskbarManager
  * @brief manages the preview effect of a session window shown in the taskbar (only available on Win7 and above), which contains multiple TaskbarItems
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2019/3/20
  */
class SessionForm;
class TaskbarManager final : public virtual ui::SupportWeakCallback
{
public:
    /** @class ITaskbarDelegate
      * @brief the event delegate class of TaskbarManager; only classes inheriting this class can use the TaskbarManager feature
      * @date 2019/3/20
      */
    class ITaskbarDelegate
    {
    public:
        /**
        * Get the form handle
        * @return HWND    the form handle
        */
        virtual HWND GetHandle() const = 0;

        /**
        * Get the render interface
        * @return the render interface
        */
        virtual ui::IRender* GetTaskbarRender() const = 0;

        /**
        * Close a taskbar item
        * @param[in] id the taskbar item id
        * @return void    no return value
        */
        virtual void CloseTaskbarItem(const std::string &id) = 0;

        /**
        * Activate and switch to a taskbar item
        * @param[in] id the taskbar item id
        * @return void no return value
        */
        virtual void SetActiveTaskbarItem(const std::string &id) = 0;
    };

    /**
    * Constructor
    */
    TaskbarManager();

    /**
    * Initialize the ITaskbarList4 interface
    * @param[in] taskbar_delegate used as the main window of the taskbar thumbnails
    * @return void    no return value
    */
    void Init(ITaskbarDelegate *taskbar_delegate);

    /**
    * Register a TaskbarTabItem as a Tab page of the main window
    * @param[in] pTabItem the TaskbarItem object
    * @return bool true success, false failure
    */
    bool RegisterTab(TaskbarTabItem &pTabItem);

    /**
    * Unregister a TaskbarTabItem from the main window
    * @param[in] pTabItem the TaskbarItem object
    * @return bool true success, false failure
    */
    bool UnregisterTab(TaskbarTabItem &pTabItem);

    /**
    * Set the display order of a TaskbarTabItem in the thumbnails of the main window to before another TaskbarTabItem
    * @param[in] pTabItem the TaskbarItem object whose order is being set
    * @param[in] tab_item_insert_before the TaskbarItem object being inserted before
    * @return bool true success, false failure
    */
    bool SetTabOrder(const TaskbarTabItem &pTabItem, const TaskbarTabItem &tab_item_insert_before);

    /**
    * Set a TaskbarTabItem to the active state in the taskbar
    * @param[in] pTabItem the TaskbarItem object
    * @return bool true success, false failure
    */
    bool SetTabActive(const TaskbarTabItem &pTabItem);

    /**
    * Generate the bitmap corresponding to a control (the returned bitmap includes the window, and its size is the window size)
    * @param[in] control the control pointer
    * @return HBITMAP    the generated bitmap
    */
    ui::IBitmap* GenerateBindControlBitmapWithForm(ui::Control *control);

    /**
    * Generate the bitmap corresponding to a control
    * @param[in] control the control pointer
    * @param[in] dest_width the target width
    * @param[in] dest_height the target height
    * @return HBITMAP    the generated bitmap
    */
    ui::IBitmap* GenerateBindControlBitmap(ui::Control *control, const int dest_width, const int dest_height);

    /**
    * Scale an in-memory bitmap
    * @param[in] dest_width the target width
    * @param[in] dest_height the target height
    * @param[in] pSrcRender the source Render interface
    * @param[in] src_x the source x coordinate
    * @param[in] src_y the source y coordinate
    * @param[in] src_width the source width
    * @param[in] src_height the source height
    * @return HBITMAP    the generated bitmap
    */
    ui::IBitmap* ResizeBitmap(int dest_width, int dest_height, ui::IRender* pSrcRender, int src_x, int src_y, int src_width, int src_height);

    /**
    * Handle the WM_CLOSE message of the taskbar Tab's thumbnail
    * @param[in] pTabItem the Tab that triggered the message
    * @return void    no return value
    */
    void OnTabItemClose(TaskbarTabItem &pTabItem);

    /**
    * Handle the WM_ACTIVATE message of the taskbar Tab's thumbnail
    * @param[in] pTabItem the Tab that triggered the message
    * @return void    no return value
    */
    void OnTabItemClicked(TaskbarTabItem &pTabItem);
private:
    ITaskbarDelegate* m_pTaskbarDelegate;
    ITaskbarList4* m_pTaskbarList;
};

#endif //(DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)

#endif //EXAMPLES_TASKBAR_MANAGER_H_
