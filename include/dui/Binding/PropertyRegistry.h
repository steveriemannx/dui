#ifndef UI_BINDING_PROPERTY_REGISTRY_H_
#define UI_BINDING_PROPERTY_REGISTRY_H_

#include "dui/dui_defs.h"
#include "dui/dui_string.h"

namespace ui
{
    class Control;

namespace binding
{

/** How one control attribute is read back, and whether the control reports changes.

 Despite living in the public include tree (this library keeps all headers under
 include/dui), this is module-internal plumbing, in the same spirit as
 Core/ControlFinder.h. Applications should use BindingGroup instead.

 The attribute names are the same strings the XML and SetAttribute() use, so a
 binding target reads exactly like the layout does.

 This registry doubles as the whitelist of bindable attributes. That matters:
 Control::SetAttribute() silently ignores names it does not recognize (it only
 asserts in debug), so without a whitelist a typo in a binding would be a no-op
 that looks like it works. Bind() rejects unknown names up front instead.
*/
struct PropertyAccessor
{
    /** Event the control fires when this attribute changes.
     kEventNone means the attribute is readable but the control never reports
     changes, so a two-way binding on it would never write back.

     This is per control TYPE, not per attribute, which is why it sits here next
     to the getter rather than in a table keyed by name alone:
       - "value": Slider fires kEventValueChanged (Slider derives from Progress),
         but plain Progress does not -- so a Progress binding is read-only.
       - "text": only RichEdit fires kEventTextChanged; a Label's text changes
         only programmatically and reports nothing.
    */
    EventType changedEvent = EventType::kEventNone;

    /** Read the current value as a string. Returns false when this control type
     does not have this attribute. */
    bool (*get)(Control* pControl, std::string& strValue) = nullptr;
};

/** Look up an attribute by name. Returns nullptr when it is not bindable.
 Case-sensitive, matching Control::SetAttribute()'s comparison.
*/
const PropertyAccessor* FindPropertyAccessor(const std::string& strAttribute);

/** Number of registered attributes; for diagnostics and tests. */
size_t GetPropertyCount();

/** Name of the i-th registered attribute; for diagnostics and tests. */
const std::string& GetPropertyName(size_t nIndex);

} // namespace binding
} // namespace ui

#endif // UI_BINDING_PROPERTY_REGISTRY_H_
