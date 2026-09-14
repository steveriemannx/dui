#include "dui/Binding/ObservableObject.h"

namespace ui
{
namespace binding
{

ObservableObject::~ObservableObject() = default;

size_t ObservableObject::AttachPropertyChanged(const PropertyChangedCallback& callback)
{
    if (!callback) {
        return 0;
    }
    const size_t nCallbackID = m_nNextCallbackID++;
    CallbackEntry entry;
    entry.nCallbackID = nCallbackID;
    entry.callback = callback;
    m_callbackList.push_back(entry);
    return nCallbackID;
}

void ObservableObject::DetachPropertyChanged(size_t nCallbackID)
{
    if (nCallbackID == 0) {
        return;
    }
    for (auto it = m_callbackList.begin(); it != m_callbackList.end(); ++it) {
        if (it->nCallbackID == nCallbackID) {
            m_callbackList.erase(it);
            return;
        }
    }
}

bool ObservableObject::GetProperty(const std::string& strPropertyName, std::string& strValue) const
{
    for (const PropertyEntry& entry : m_propertyList) {
        if ((entry.pValue != nullptr) && (entry.strName == strPropertyName)) {
            strValue = *entry.pValue;
            return true;
        }
    }
    return false;
}

bool ObservableObject::SetProperty(const std::string& strPropertyName, const std::string& strValue)
{
    for (PropertyEntry& entry : m_propertyList) {
        if ((entry.pValue != nullptr) && (entry.strName == strPropertyName)) {
            *entry.pValue = strValue;
            return true;
        }
    }
    return false;
}

void ObservableObject::RegisterProperty(const std::string& strPropertyName, std::string& refValue)
{
    for (PropertyEntry& entry : m_propertyList) {
        if (entry.strName == strPropertyName) {
            // Re-registering a name rebinds it, which makes a subclass constructor
            // idempotent if it ever runs twice on the same object.
            entry.pValue = &refValue;
            return;
        }
    }
    PropertyEntry entry;
    entry.strName = strPropertyName;
    entry.pValue = &refValue;
    m_propertyList.push_back(entry);
}

void ObservableObject::RaisePropertyChanged(const std::string& strPropertyName)
{
    // Copy first: a callback is allowed to detach itself (or another binding)
    // while we are iterating, which would otherwise invalidate the iterator.
    std::vector<CallbackEntry> callbackList = m_callbackList;
    for (const CallbackEntry& entry : callbackList) {
        if (entry.callback) {
            entry.callback(strPropertyName);
        }
    }
}

void ObservableObject::RaiseAllPropertiesChanged()
{
    // An empty name means "everything": bindings re-pull regardless of which
    // property they follow, so a whole-model reload refreshes every target.
    RaisePropertyChanged(std::string());
}

} // namespace binding
} // namespace ui
