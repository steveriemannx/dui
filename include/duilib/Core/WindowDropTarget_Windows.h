#ifndef UI_CORE_WINDOW_DROP_TARGET_WINDOWS_H_
#define UI_CORE_WINDOW_DROP_TARGET_WINDOWS_H_

#include "duilib/Core/ControlDropTarget.h"

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)

#include <oleidl.h>

namespace ui 
{
class NativeWindow_Windows;

/** Drag and drop support for the window
*/
class DUILIB_API WindowDropTarget : public IDropTarget
{
public:
    explicit WindowDropTarget(NativeWindow_Windows* pNativeWindow);
    WindowDropTarget(const WindowDropTarget& r) = delete;
    WindowDropTarget& operator=(const WindowDropTarget& r) = delete;
    virtual ~WindowDropTarget();

public:
    // IUnkown interface
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(/* [in] */ REFIID riid,
                                                     /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override;
    virtual ULONG STDMETHODCALLTYPE AddRef(void) override;

    virtual ULONG STDMETHODCALLTYPE Release(void) override;

    // IDropTarget interface
    virtual HRESULT STDMETHODCALLTYPE DragEnter(/* [unique][in] */ __RPC__in_opt IDataObject* pDataObj,
                                                /* [in] */ DWORD grfKeyState,
                                                /* [in] */ POINTL pt,
                                                /* [out][in] */ __RPC__inout DWORD* pdwEffect) override;

    virtual HRESULT STDMETHODCALLTYPE DragOver(/* [in] */ DWORD grfKeyState,
                                               /* [in] */ POINTL pt,
                                               /* [out][in] */ __RPC__inout DWORD* pdwEffect) override;

    virtual HRESULT STDMETHODCALLTYPE DragLeave(void) override;

    virtual HRESULT STDMETHODCALLTYPE Drop( /* [unique][in] */ __RPC__in_opt IDataObject* pDataObj,
                                            /* [in] */ DWORD grfKeyState,
                                            /* [in] */ POINTL pt,
                                            /* [out][in] */ __RPC__inout DWORD* pdwEffect) override;

public:
    /** Register the drag and drop interface
    */
    bool RegisterDragDrop();

    /** Unregister the drag and drop interface
    */
    bool UnregisterDragDrop();

protected: 
    /** Implementation of the DragOver function
    */
    HRESULT OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

    /** Find the control interface corresponding to a point coordinate
    * @param [in] screenPt Screen coordinate point
    */
    ControlPtrT<ControlDropTarget_Windows> GetControlDropTarget(const UiPoint& screenPt) const;

private:
    /** Reference count
    */
    int32_t m_nRef;

    /** The data object interface passed in by IDropTarget::DragEnter
    */
    IDataObject* m_pDataObj;

    /** The control interface currently in the Hover state
    */
    ControlPtrT<ControlDropTarget_Windows> m_pHoverDropTarget;

    /** The associated Native window implementation
    */
    NativeWindow_Windows* m_pNativeWindow;

    /** Whether the drag and drop interface has been successfully registered
    */
    bool m_bRegisterDragDrop;
};

} // namespace ui

#endif //defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)

#endif // UI_CORE_WINDOW_DROP_TARGET_WINDOWS_H_
