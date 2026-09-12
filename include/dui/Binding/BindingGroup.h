#ifndef UI_BINDING_GROUP_H_
#define UI_BINDING_GROUP_H_

#include "dui/dui_defs.h"
#include "dui/dui_string.h"
#include "dui/Binding/ObservableObject.h"

#include <functional>
#include <memory>

namespace ui
{
    class Control;
    class Box;
    class Window;

namespace binding
{

class BindingGroupImpl;

/** Owner of a set of bindings. The view holds one as a member, so unbinding is
 automatic on destruction (RAII) -- there is no global registry and no per-control
 state, which is what keeps this module from touching Control at all.

 Lifetime: the group must not outlive the controls it binds. A member of the
 window class satisfies this naturally -- the window's members are destroyed
 before the framework frees the control tree. Each binding also remembers the
 target control's weak flag, so touches are skipped if a control dies first.

 Typical use:

     class MyForm : public ui::WindowImplBase
     {
         ui::binding::BindingGroup m_bindings;   // member: unbinds automatically
         std::shared_ptr<MyViewModel> m_vm;
     };

     void MyForm::OnInitWindow()
     {
         m_vm = std::make_shared<MyViewModel>();
         m_bindings.SetContext(m_vm);
         m_bindings.Bind(this, DUI_T("label_title"), DUI_T("text"), DUI_T("title"));
         m_bindings.BindTwoWay(this, DUI_T("edit_name"), DUI_T("text"), DUI_T("title"));
     }

 Target lookup: the name-based overloads resolve a control by name the same way
 FindControl() does -- case-sensitive, first match in traversal order. The
 application must keep those names unique per binding scope; list item templates
 in particular repeat inner names (label_title, ...) across hundreds of instances,
 so bind an item's controls against the item itself (a Box), never against the window.
*/
class DUI_API BindingGroup
{
public:
    BindingGroup();
    ~BindingGroup();

    BindingGroup(const BindingGroup&) = delete;
    BindingGroup& operator=(const BindingGroup&) = delete;

    /** Set the source object (a ViewModel, or any ObservableObject).
     Paths are resolved against it. Bindings declared before this call are
     remembered and applied as soon as a context arrives, which is what lets a
     recycled list item declare its bindings once in its constructor and only
     swap contexts on each fill.
    */
    void SetContext(const std::shared_ptr<ObservableObject>& pContext);
    std::shared_ptr<ObservableObject> GetContext() const;

    /** One-way: pControl's strAttribute follows strPath on the context object.
     strAttribute is the same name the XML uses ("text", "value", "visible",
     "enabled", "checked", "selected", "bkimage"). Returns false if the control
     is null or the attribute is not bindable.
    */
    bool Bind(Control* pControl, const DString& strAttribute, const DString& strPath);

    /** Same, but the target control is found by name within pRoot's subtree. */
    bool Bind(Box* pRoot, const DString& strTargetName,
              const DString& strAttribute, const DString& strPath);

    /** Same, but the target control is found by name in the window. */
    bool Bind(Window* pWindow, const DString& strTargetName,
              const DString& strAttribute, const DString& strPath);

    /** Two-way: control -> context as well, driven by the control's own change
     event. Returns false when the control does not report changes for this
     attribute (see PropertyAccessor::changedEvent) -- a "value" binding on a
     Progress, for instance, is one-way only, because Progress fires nothing.

     Writes are re-entrancy guarded per binding, so a control -> context ->
     control round trip settles in one pass instead of looping.
    */
    bool BindTwoWay(Control* pControl, const DString& strAttribute, const DString& strPath);

    /** Same, but the target control is found by name within pRoot's subtree. */
    bool BindTwoWay(Box* pRoot, const DString& strTargetName,
                    const DString& strAttribute, const DString& strPath);

    /** Same, but the target control is found by name in the window. */
    bool BindTwoWay(Window* pWindow, const DString& strTargetName,
                    const DString& strAttribute, const DString& strPath);

    /** Command: the control's click calls pMethod on the context object.
     Takes a member pointer rather than a string, so the method is checked at
     compile time. The context is held weakly; a dead context is a no-op.
    */
    template <typename T>
    bool BindCommand(Control* pControl, const std::shared_ptr<T>& pContext, void (T::*pMethod)())
    {
        static_assert(std::is_base_of<ObservableObject, T>::value,
                      "ui::binding::BindCommand: T must derive from ui::binding::ObservableObject");
        std::weak_ptr<T> weakContext = pContext;
        return AttachCommand(pControl, [weakContext, pMethod]() {
            if (std::shared_ptr<T> pContextLocked = weakContext.lock()) {
                (pContextLocked.get()->*pMethod)();
            }
        });
    }

    /** Same, but the target control is found by name within pRoot's subtree. */
    template <typename T>
    bool BindCommand(Box* pRoot, const DString& strTargetName,
                     const std::shared_ptr<T>& pContext, void (T::*pMethod)())
    {
        return BindCommand(FindTarget(pRoot, strTargetName), pContext, pMethod);
    }

    /** Same, but the target control is found by name in the window. */
    template <typename T>
    bool BindCommand(Window* pWindow, const DString& strTargetName,
                     const std::shared_ptr<T>& pContext, void (T::*pMethod)())
    {
        return BindCommand(FindTarget(pWindow, strTargetName), pContext, pMethod);
    }

    /** Detach every binding. Called by the destructor; safe to call earlier,
     after which the group can be reused for new bindings.
    */
    void UnbindAll();

    /** Re-pull every bound target from the context immediately. */
    void RefreshAll();

private:
    /** Attach a click handler; the public template wraps this to stay type-safe. */
    bool AttachCommand(Control* pControl, const std::function<void()>& command);

    /** Resolve a control by name within pRoot's subtree; null when not found. */
    static Control* FindTarget(Box* pRoot, const DString& strTargetName);

    /** Resolve a control by name in the window; null when not found. */
    static Control* FindTarget(Window* pWindow, const DString& strTargetName);

    std::unique_ptr<BindingGroupImpl> m_impl;
};

} // namespace binding
} // namespace ui

#endif // UI_BINDING_GROUP_H_
