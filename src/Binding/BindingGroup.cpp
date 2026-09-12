#include "dui/Binding/BindingGroup.h"
#include "dui/Binding/PropertyRegistry.h"

#include "dui/Core/Control.h"
#include "dui/Core/Box.h"
#include "dui/Core/Window.h"
#include "dui/Core/EventArgs.h"

#include <memory>
#include <vector>

namespace ui
{
namespace binding
{

/*  A note on the event callbacks this file attaches.

    Every callback here is a pure observer and ALWAYS returns true. That is a
    constraint, not a style choice:

      - dui reads a false return as "handled": Control::SendEventMsg stops the
        remaining callbacks and suppresses HandleEvent as well as bubbling. A
        binding that returned false would silently change the event flow of the
        code it merely observes -- exactly the kind of breakage this module is
        required to avoid.
      - UiBind is deliberately NOT used. WeakCallback<T>::operator() yields a
        value-initialized result once its host has died, which for a bool callback
        is false -- so an expired host would swallow the event. The engine tracks
        each control's weak flag itself and simply skips the work instead.

    Consequences worth knowing: a binding can never consume an event, and its
    ordering relative to the application's own handlers follows attach order.
*/

/** Everything a BindingGroup owns. Kept out of the public header so the whole
 implementation can change without touching the API.
*/
class BindingGroupImpl
{
public:
    /** One target-side binding: a control attribute following a source path. */
    struct TargetBinding
    {
        Control* pControl = nullptr;
        std::weak_ptr<WeakFlag> weakControl;
        DString strAttribute;
        DString strPath;
        EventCallbackID nEventCallbackID = 0;
        /** Re-entrancy guard for this binding, covering BOTH directions: while a
         pull is writing the control, the change event that write may raise must
         not push the value straight back, and vice versa. One flag is enough
         because either direction blocks the other. */
        bool bUpdating = false;
    };

    /** One command binding: a control click calling into the source object. */
    struct CommandBinding
    {
        Control* pControl = nullptr;
        std::weak_ptr<WeakFlag> weakControl;
        std::function<void()> command;
        EventCallbackID nEventCallbackID = 0;
    };

    /** Bindings are held by unique_ptr: the event lambdas capture a raw pointer to
     the binding, so its address must survive vector growth. */
    std::vector<std::unique_ptr<TargetBinding> > m_bindingList;
    std::vector<std::unique_ptr<CommandBinding> > m_commandList;

    std::shared_ptr<ObservableObject> m_pContext;
    size_t m_nContextCallbackID = 0;

    ~BindingGroupImpl()
    {
        UnbindAll();
    }

    /** Never 0: AttachEvent's callbackID defaults to 0, so an application's own
     handlers often carry that id. Detaching by 0 would remove theirs. */
    EventCallbackID NextCallbackID()
    {
        return ++m_nNextCallbackID;
    }

    /** Detach one binding's event handler, if it has one and its control is alive. */
    static void DetachOne(TargetBinding& binding)
    {
        if (!binding.weakControl.expired() && (binding.nEventCallbackID != 0)) {
            binding.pControl->DetachEventByID(binding.nEventCallbackID);
        }
        binding.nEventCallbackID = 0;
    }

    void DetachContextCallback()
    {
        if (m_pContext && (m_nContextCallbackID != 0)) {
            m_pContext->DetachPropertyChanged(m_nContextCallbackID);
        }
        m_nContextCallbackID = 0;
    }

    void UnbindAll()
    {
        for (std::unique_ptr<TargetBinding>& pBinding : m_bindingList) {
            DetachOne(*pBinding);
        }
        m_bindingList.clear();

        for (std::unique_ptr<CommandBinding>& pCommand : m_commandList) {
            if (!pCommand->weakControl.expired() && (pCommand->nEventCallbackID != 0)) {
                pCommand->pControl->DetachEventByID(pCommand->nEventCallbackID);
            }
        }
        m_commandList.clear();

        DetachContextCallback();
        m_pContext.reset();
    }

    /** Source -> control: read the path and write the attribute. */
    void PullToTarget(TargetBinding& binding)
    {
        if (binding.bUpdating) {
            return;
        }
        if (binding.weakControl.expired()) {
            return;
        }
        if (!m_pContext) {
            return;
        }

        DString strValue;
        if (!m_pContext->GetProperty(binding.strPath, strValue)) {
            // The source does not expose this path. Silence is right here: a
            // recycled item is briefly bound to no context at all, and reads
            // failing before a context arrives is the normal state.
            return;
        }

        binding.bUpdating = true;
        binding.pControl->SetAttribute(binding.strAttribute, strValue);
        binding.bUpdating = false;
    }

    /** Control -> source: read the attribute and write the path. */
    void PushToContext(TargetBinding& binding)
    {
        if (binding.bUpdating) {
            return;
        }
        if (binding.weakControl.expired()) {
            return;
        }
        if (!m_pContext) {
            return;
        }

        const PropertyAccessor* pAccessor = FindPropertyAccessor(binding.strAttribute);
        if ((pAccessor == nullptr) || (pAccessor->get == nullptr)) {
            return;
        }

        DString strValue;
        if (!pAccessor->get(binding.pControl, strValue)) {
            return;
        }

        binding.bUpdating = true;
        m_pContext->SetProperty(binding.strPath, strValue);
        binding.bUpdating = false;
    }

    void OnContextPropertyChanged(const DString& strPropertyName)
    {
        // An empty name means "everything changed" (RaiseAllPropertiesChanged).
        const bool bAll = strPropertyName.empty();

        // Index-based with a size snapshot: a pull runs SetAttribute, which runs
        // application code, which is free to add bindings to this very group.
        // Elements are unique_ptr, so addresses survive that growth.
        const size_t nCount = m_bindingList.size();
        for (size_t nIndex = 0; (nIndex < nCount) && (nIndex < m_bindingList.size()); ++nIndex) {
            TargetBinding& binding = *m_bindingList[nIndex];
            if (bAll || (binding.strPath == strPropertyName)) {
                PullToTarget(binding);
            }
        }
    }

private:
    EventCallbackID m_nNextCallbackID = 0;
};

BindingGroup::BindingGroup()
    : m_impl(new BindingGroupImpl)
{
}

BindingGroup::~BindingGroup() = default;

void BindingGroup::SetContext(const std::shared_ptr<ObservableObject>& pContext)
{
    m_impl->DetachContextCallback();
    m_impl->m_pContext = pContext;

    if (m_impl->m_pContext) {
        m_impl->m_nContextCallbackID = m_impl->m_pContext->AttachPropertyChanged(
            [this](const DString& strPropertyName) {
                m_impl->OnContextPropertyChanged(strPropertyName);
            });
    }

    // Bindings declared before the context existed are applied now -- this is what
    // lets a recycled list item declare its bindings once and only swap contexts.
    RefreshAll();
}

std::shared_ptr<ObservableObject> BindingGroup::GetContext() const
{
    return m_impl->m_pContext;
}

bool BindingGroup::Bind(Control* pControl, const DString& strAttribute, const DString& strPath)
{
    if (pControl == nullptr) {
        return false;
    }
    if (FindPropertyAccessor(strAttribute) == nullptr) {
        // Not a bindable attribute. Rejecting here turns a typo into an immediate
        // false instead of a silent no-op: Control::SetAttribute() ignores names
        // it does not recognize (asserting only in debug builds).
        return false;
    }

    std::unique_ptr<BindingGroupImpl::TargetBinding> pBinding(new BindingGroupImpl::TargetBinding);
    pBinding->pControl = pControl;
    pBinding->weakControl = pControl->GetWeakFlag();
    pBinding->strAttribute = strAttribute;
    pBinding->strPath = strPath;

    BindingGroupImpl::TargetBinding* pRaw = pBinding.get();
    m_impl->m_bindingList.push_back(std::move(pBinding));

    m_impl->PullToTarget(*pRaw);
    return true;
}

bool BindingGroup::BindTwoWay(Control* pControl, const DString& strAttribute, const DString& strPath)
{
    if (pControl == nullptr) {
        return false;
    }
    const PropertyAccessor* pAccessor = FindPropertyAccessor(strAttribute);
    if (pAccessor == nullptr) {
        return false;
    }
    if (pAccessor->changedEvent == EventType::kEventNone) {
        // The control never reports changes for this attribute, so there is no
        // event to write back from -- one-way is all that is on offer. This is
        // per control type: "value" works two-way on a Slider but not on the
        // Progress it derives from.
        return false;
    }

    std::unique_ptr<BindingGroupImpl::TargetBinding> pBinding(new BindingGroupImpl::TargetBinding);
    pBinding->pControl = pControl;
    pBinding->weakControl = pControl->GetWeakFlag();
    pBinding->strAttribute = strAttribute;
    pBinding->strPath = strPath;

    BindingGroupImpl::TargetBinding* pRaw = pBinding.get();
    pRaw->nEventCallbackID = m_impl->NextCallbackID();
    const EventCallbackID nCallbackID = pRaw->nEventCallbackID;
    m_impl->m_bindingList.push_back(std::move(pBinding));

    pControl->AttachEvent(pAccessor->changedEvent,
        [this, pRaw](const EventArgs&) -> bool {
            m_impl->PushToContext(*pRaw);
            return true; // observer: never swallow the event (see the note at the top)
        },
        nCallbackID);

    m_impl->PullToTarget(*pRaw);
    return true;
}

bool BindingGroup::Bind(Box* pRoot, const DString& strTargetName,
                        const DString& strAttribute, const DString& strPath)
{
    return Bind(FindTarget(pRoot, strTargetName), strAttribute, strPath);
}

bool BindingGroup::Bind(Window* pWindow, const DString& strTargetName,
                        const DString& strAttribute, const DString& strPath)
{
    return Bind(FindTarget(pWindow, strTargetName), strAttribute, strPath);
}

bool BindingGroup::BindTwoWay(Box* pRoot, const DString& strTargetName,
                              const DString& strAttribute, const DString& strPath)
{
    return BindTwoWay(FindTarget(pRoot, strTargetName), strAttribute, strPath);
}

bool BindingGroup::BindTwoWay(Window* pWindow, const DString& strTargetName,
                              const DString& strAttribute, const DString& strPath)
{
    return BindTwoWay(FindTarget(pWindow, strTargetName), strAttribute, strPath);
}

bool BindingGroup::AttachCommand(Control* pControl, const std::function<void()>& command)
{
    if ((pControl == nullptr) || !command) {
        return false;
    }

    std::unique_ptr<BindingGroupImpl::CommandBinding> pBinding(new BindingGroupImpl::CommandBinding);
    pBinding->pControl = pControl;
    pBinding->weakControl = pControl->GetWeakFlag();
    pBinding->command = command;
    pBinding->nEventCallbackID = m_impl->NextCallbackID();

    BindingGroupImpl::CommandBinding* pRaw = pBinding.get();
    const EventCallbackID nCallbackID = pRaw->nEventCallbackID;
    m_impl->m_commandList.push_back(std::move(pBinding));

    pControl->AttachEvent(EventType::kEventClick,
        [pRaw](const EventArgs&) -> bool {
            // The context is held weakly by the caller's wrapper, so a dead
            // context makes this a no-op rather than a crash.
            if (!pRaw->weakControl.expired() && pRaw->command) {
                pRaw->command();
            }
            return true; // observer: never swallow the event (see the note at the top)
        },
        nCallbackID);
    return true;
}

Control* BindingGroup::FindTarget(Box* pRoot, const DString& strTargetName)
{
    if (pRoot == nullptr) {
        return nullptr;
    }
    return pRoot->FindSubControl(strTargetName);
}

Control* BindingGroup::FindTarget(Window* pWindow, const DString& strTargetName)
{
    if (pWindow == nullptr) {
        return nullptr;
    }
    return pWindow->FindControl(strTargetName);
}

void BindingGroup::UnbindAll()
{
    m_impl->UnbindAll();
}

void BindingGroup::RefreshAll()
{
    for (std::unique_ptr<BindingGroupImpl::TargetBinding>& pBinding : m_impl->m_bindingList) {
        m_impl->PullToTarget(*pBinding);
    }
}

} // namespace binding
} // namespace ui
