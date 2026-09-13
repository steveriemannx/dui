#ifndef UI_BINDING_OBSERVABLE_OBJECT_H_
#define UI_BINDING_OBSERVABLE_OBJECT_H_

#include "dui/dui_defs.h"
#include "dui/dui_string.h"
#include "dui/Core/Callback.h"

#include <functional>
#include <vector>

namespace ui
{
namespace binding
{

/** Observable object: broadcasts "this property changed".

 This is the shared foundation of MVC and MVVM -- it knows nothing about
 "ViewModels". Any model class may derive from it and be bound to controls;
 a ViewModel is just the name for a model that a view binds to directly.

 It derives from SupportWeakCallback so the binding engine can reuse the
 library's existing weak-callback mechanism (UiBind/WeakCallbackFlag) to tell
 whether the source object is still alive.
*/
class DUI_API ObservableObject : public SupportWeakCallback
{
public:
    /** Invoked with the property name -- the same path string passed to Bind(). */
    typedef std::function<void(const std::string& strPropertyName)> PropertyChangedCallback;

    ObservableObject() = default;
    virtual ~ObservableObject();

    /** Subscribe to property changes. Returns an id for DetachPropertyChanged(). */
    size_t AttachPropertyChanged(const PropertyChangedCallback& callback);
    void DetachPropertyChanged(size_t nCallbackID);

    /** Read a property as a string. The default implementation consults the table
     filled by RegisterProperty(); override it for computed or typed properties.

     This is the source-side counterpart of Control::SetAttribute(). A binding
     needs it because C++ has no reflection: without a name-keyed read, the
     engine would have to know the concrete type of every model it binds.

     @param [out] strValue The current value, when the property is known.
     @return true when the property exists and was read.
    */
    virtual bool GetProperty(const std::string& strPropertyName, std::string& strValue) const;

    /** Write a property from a string; this is what two-way binding uses.
     The default implementation consults the table filled by RegisterProperty();
     override it to parse typed values (StringUtil::StringToInt32, ...).

     @return true when the property exists and was written.
    */
    virtual bool SetProperty(const std::string& strPropertyName, const std::string& strValue);

protected:
    /** Make a string member bindable, and readable/writable by name.

     This is the no-ceremony path: most state a control displays is text anyway.
     Call it from the subclass constructor; refValue must outlive this object,
     which a member of the subclass does.

         MyViewModel::MyViewModel()
         {
             RegisterProperty("name", m_sName);
         }

     Properties that are not std::string (an int, a bool, a computed value) should
     override GetProperty/SetProperty instead -- this module deliberately has no
     type system, so values cross the binding boundary as strings, the same way
     Control::SetAttribute() takes them.
    */
    void RegisterProperty(const std::string& strPropertyName, std::string& refValue);

    /** Subclasses call this after a property actually changed.
     Compare old and new first: every raise re-pulls all bound targets, so
     raising for an unchanged value is pure wasted work.
    */
    void RaisePropertyChanged(const std::string& strPropertyName);

    /** Invalidate everything, for when the whole model was replaced.
     Bound targets are re-pulled without a specific property name, so any
     binding that depends on several properties still refreshes.
    */
    void RaiseAllPropertiesChanged();

private:
    struct CallbackEntry
    {
        size_t nCallbackID = 0;
        PropertyChangedCallback callback;
    };

    /** A registered string property: the name plus a pointer to the subclass's
     member, which is what makes reads and writes by name possible. */
    struct PropertyEntry
    {
        std::string strName;
        std::string* pValue = nullptr;
    };

    std::vector<CallbackEntry> m_callbackList;
    std::vector<PropertyEntry> m_propertyList;
    size_t m_nNextCallbackID = 1;
};

} // namespace binding
} // namespace ui

#endif // UI_BINDING_OBSERVABLE_OBJECT_H_
