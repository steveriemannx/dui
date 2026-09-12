#include "dui/Binding/PropertyRegistry.h"


#include "dui/Core/Control.h"
#include "dui/Core/PlaceHolder.h"
#include "dui/Control/LabelImpl.h"
#include "dui/Control/Progress.h"
#include "dui/Utils/StringUtil.h"

#include <vector>

namespace ui
{
namespace binding
{

namespace
{

/** Read "text" from anything that owns text. Label and its Box/HBox/VBox variants,
 Button, CheckBox, GroupBox and Option all implement LabelOwner, so one cast
 covers the whole family.
*/
bool GetText(Control* pControl, DString& strValue)
{
    LabelOwner* pLabelOwner = dynamic_cast<LabelOwner*>(pControl);
    if (pLabelOwner == nullptr) {
        return false;
    }
    strValue = pLabelOwner->GetText();
    return true;
}

/** Read "value". Progress::GetValue() also covers Slider, which derives from it. */
bool GetValue(Control* pControl, DString& strValue)
{
    Progress* pProgress = dynamic_cast<Progress*>(pControl);
    if (pProgress == nullptr) {
        return false;
    }
    strValue = StringUtil::Printf(DUI_T("%g"), pProgress->GetValue());
    return true;
}

bool GetVisible(Control* pControl, DString& strValue)
{
    strValue = pControl->IsVisible() ? DUI_T("true") : DUI_T("false");
    return true;
}

bool GetEnabled(Control* pControl, DString& strValue)
{
    strValue = pControl->IsEnabled() ? DUI_T("true") : DUI_T("false");
    return true;
}

bool GetBkImage(Control* pControl, DString& strValue)
{
    strValue = pControl->GetBkImage();
    return true;
}

struct PropertyEntry
{
    DString strName;
    PropertyAccessor accessor;
};

/** Built once. Adding a bindable attribute means adding one row here -- nothing
 else in the engine is attribute-aware.
*/
const std::vector<PropertyEntry>& GetPropertyTable()
{
    static const std::vector<PropertyEntry> kTable = {
        // Readable on any LabelOwner. Only RichEdit reports changes, so a two-way
        // binding on a Label attaches a handler that simply never fires.
        { DUI_T("text"),    { EventType::kEventTextChanged,    &GetText } },
        // Readable on Progress and Slider; Slider reports changes, Progress does not.
        { DUI_T("value"),   { EventType::kEventValueChanged,   &GetValue } },
        // Readable on every Control, and every Control reports visibility changes.
        { DUI_T("visible"), { EventType::kEventVisibleChanged, &GetVisible } },
        // Readable but unreported: these are one-way only in practice.
        { DUI_T("enabled"), { EventType::kEventNone,           &GetEnabled } },
        { DUI_T("bkimage"), { EventType::kEventNone,           &GetBkImage } },
    };
    return kTable;
}

} // namespace

const PropertyAccessor* FindPropertyAccessor(const DString& strAttribute)
{
    for (const PropertyEntry& entry : GetPropertyTable()) {
        if (entry.strName == strAttribute) {
            return &entry.accessor;
        }
    }
    return nullptr;
}

size_t GetPropertyCount()
{
    return GetPropertyTable().size();
}

const DString& GetPropertyName(size_t nIndex)
{
    static const DString kEmptyName;
    const std::vector<PropertyEntry>& table = GetPropertyTable();
    if (nIndex >= table.size()) {
        return kEmptyName;
    }
    return table[nIndex].strName;
}

} // namespace binding
} // namespace ui
