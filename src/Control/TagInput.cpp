#include "dui/Control/TagInput.h"
#include "dui/Control/RichEdit.h"
#include "dui/Control/Button.h"
#include "dui/Control/Label.h"
#include "dui/Box/HBox.h"
#include "dui/Layout/HFlowLayout.h"
#include "dui/Utils/StringUtil.h"
#include <algorithm>
#include <list>

namespace ui
{
/** Horizontal flow layout dedicated to TagInput: the last child control (the edit control) always fills the
*   remaining width of its row; when the remaining width is smaller than the minimum width of the edit control,
*   the edit control starts a new row and fills the whole row. The other child controls (the tags) are arranged
*   and wrapped by the original HFlowLayout rules.
*/
class TagInputLayout : public HFlowLayout
{
    typedef HFlowLayout BaseClass;
public:
    TagInputLayout() = default;

    /** Sets the edit control (the last visible non-floating child control)
    */
    void SetEditControl(Control* pEdit) { m_pEdit = pEdit; }

    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override
    {
        if (m_pEdit != nullptr) {
            AdjustEditWidth(items, rc);
        }
        return BaseClass::ArrangeChildren(items, rc, bEstimateOnly);
    }

    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override
    {
        if (m_pEdit != nullptr) {
            UiRect rc(0, 0, szAvailable.cx, szAvailable.cy);
            AdjustEditWidth(items, rc);
        }
        return BaseClass::EstimateLayoutSize(items, szAvailable);
    }

private:
    /** Computes the width of the edit control from the arrangement of the tags and sets it as a fixed width
    */
    void AdjustEditWidth(const std::vector<Control*>& items, UiRect rc)
    {
        if (m_pEdit == nullptr) {
            return;
        }
        //Kept consistent with HFlowLayout: remove the container padding first
        DeflatePadding(rc);

        const UiSize szAvailable(rc.Width(), rc.Height());
        int32_t nPosX = rc.left;
        bool bRowHasItem = false;
        for (Control* pControl : items) {
            if ((pControl == nullptr) || (pControl == m_pEdit) || !pControl->IsVisible() || pControl->IsFloat()) {
                continue;
            }
            const UiMargin rcMargin = pControl->GetMargin();
            UiEstSize estSize = pControl->EstimateSize(szAvailable);
            int32_t nWidth = estSize.cx.GetInt32();
            if (estSize.cx.IsStretch()) {
                //Tags should not have a stretch width; clamp it as a fallback
                nWidth = 0;
            }
            nWidth = std::clamp(nWidth, pControl->GetMinWidth(), pControl->GetMaxWidth());
            nWidth = std::max(nWidth, 0);

            if (bRowHasItem && (nPosX + rcMargin.left + nWidth + rcMargin.right > rc.right)) {
                //Wrap to the next row
                nPosX = rc.left;
            }
            nPosX += rcMargin.left + nWidth + rcMargin.right + GetChildMarginX();
            bRowHasItem = true;
        }

        //The edit control comes after the tags; reserve at least its minimum width, otherwise start a new row
        const UiMargin rcEditMargin = m_pEdit->GetMargin();
        const int32_t nMinWidth = std::max(m_pEdit->GetMinWidth(), 0);
        if (bRowHasItem && (nPosX + rcEditMargin.left + nMinWidth + rcEditMargin.right > rc.right)) {
            nPosX = rc.left;
        }
        int32_t nEditWidth = rc.right - nPosX - rcEditMargin.left - rcEditMargin.right;
        nEditWidth = std::max(nEditWidth, nMinWidth);
        //Set the fixed width during layout: do not trigger a re-layout and do not scale by DPI again (the value is already in physical pixels)
        m_pEdit->SetFixedWidth(UiFixedInt(nEditWidth), false, false);
    }

private:
    /** Internal edit control
    */
    Control* m_pEdit = nullptr;
};

/** Edit control dedicated to TagInput: pressing Backspace in an empty text deletes the last tag
*/
class TagInputEdit : public RichEdit
{
    typedef RichEdit BaseClass;
public:
    explicit TagInputEdit(Window* pWindow) : RichEdit(pWindow), m_pOwner(nullptr) {}

    /** Sets the owning TagInput control
    */
    void SetOwner(TagInput* pOwner) { m_pOwner = pOwner; }

    virtual bool OnKeyDown(const EventArgs& msg) override
    {
        if ((msg.vkCode == kVK_BACK) && (m_pOwner != nullptr)) {
            std::string text = GetText();
            if (StringUtil::Trim(text).empty()) {
                //When the edit control is empty, Backspace is handled by TagInput to delete the last tag
                if (m_pOwner->HandleEditBackspace()) {
                    return true;
                }
            }
        }
        return BaseClass::OnKeyDown(msg);
    }

private:
    TagInput* m_pOwner;
};

TagInput::TagInput(Window* pWindow) :
    BaseClass(pWindow, new TagInputLayout()),
    m_pEdit(nullptr),
    m_bAllowDuplicate(false),
    m_nMaxCount(0)
{
}

std::string TagInput::GetType() const
{
    return DUI_CTR_TAG_INPUT;
}

void TagInput::SetAttribute(const std::string& strName, const std::string& strValue2)
{
    //dui does not support variable expansion in attribute values
    const std::string& strValue = strValue2;
    if ((strName == "prompt_text") || (strName == "prompttext")) {
        m_promptText = strValue;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute(strName, strValue);
        }
    }
    else if ((strName == "prompt_text_id") || (strName == "prompt_textid") || (strName == "prompttextid")) {
        m_promptTextId = strValue;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute(strName, strValue);
        }
    }
    else if ((strName == "prompt_color") || (strName == "promptcolor")) {
        m_promptColor = strValue;
        if (m_pEdit != nullptr) {
            m_pEdit->SetAttribute(strName, strValue);
        }
    }
    else if ((strName == "allow_duplicate") || (strName == "allowduplicate")) {
        SetAllowDuplicate(strValue == "true" || strValue == "1");
    }
    else if ((strName == "max_count") || (strName == "maxcount")) {
        SetMaxCount(StringUtil::StringToInt32(strValue.c_str()));
    }
    else if (strName == "delimiter") {
        m_delimiter = strValue;
    }
    else if (strName == "tags") {
        if (m_pEdit != nullptr) {
            RemoveAllTags();
            ParseInitTags(strValue);
        }
        else {
            m_initTags = strValue;
        }
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void TagInput::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    //Create the internal edit control first so it always is the last child control; new tags are inserted before it
    m_pEdit = new TagInputEdit(GetWindow());
    static_cast<TagInputEdit*>(m_pEdit)->SetOwner(this);
    m_pEdit->SetClass("tag_input_edit");
    m_pEdit->SetAttribute("single_line", "true");
    m_pEdit->SetAttribute("want_tab", "false");
    //Do not handle Enter as a new line; TagInput receives the Enter key and creates a tag
    m_pEdit->SetAttribute("want_return", "false");
    m_pEdit->SetAttribute("prompt_mode", "true");
    if (!m_promptText.empty()) {
        m_pEdit->SetAttribute("prompt_text", std::string(m_promptText.c_str()));
    }
    if (!m_promptTextId.empty()) {
        m_pEdit->SetAttribute("prompt_text_id", std::string(m_promptTextId.c_str()));
    }
    if (!m_promptColor.empty()) {
        m_pEdit->SetAttribute("prompt_color", std::string(m_promptColor.c_str()));
    }
    AddItem(m_pEdit);
    static_cast<TagInputLayout*>(GetLayout())->SetEditControl(m_pEdit);

    //Enter: commit the tag
    m_pEdit->AttachReturn([this](const EventArgs& /*args*/) {
        CommitInput();
        return true;
        });
    //Text changed: a delimiter creates a tag and the text-changed event is forwarded
    m_pEdit->AttachTextChanged([this](const EventArgs& /*args*/) {
        HandleEditTextChanged();
        return true;
        });

    //A click on an empty area of the container moves the focus to the edit control
    AttachButtonDown([this](const EventArgs& /*args*/) {
        if (m_pEdit != nullptr) {
            m_pEdit->SetFocus();
        }
        return true;
        });

    //Create the initial tags (the edit control is in place now, so the tags are inserted before it)
    if (!m_initTags.empty()) {
        ParseInitTags(m_initTags.c_str());
    }
}

void TagInput::SetFocus()
{
    if (m_pEdit != nullptr) {
        m_pEdit->SetFocus();
    }
    else {
        BaseClass::SetFocus();
    }
}

bool TagInput::AddTag(const std::string& text)
{
    if (m_pEdit == nullptr) {
        //The control is not initialized yet, so cache the tag in the initial tags
        std::string strText = StringUtil::Trim(text.c_str());
        if (strText.empty()) {
            return false;
        }
        if (m_initTags.empty()) {
            m_initTags = strText;
        }
        else {
            std::string init = m_initTags.c_str();
            init += ";";
            init += strText;
            m_initTags = init;
        }
        return true;
    }

    std::string strText = StringUtil::Trim(text.c_str());
    if (strText.empty()) {
        return false;
    }
    if (!m_bAllowDuplicate && IsTagExists(strText)) {
        return false;
    }
    if ((m_nMaxCount > 0) && (static_cast<int32_t>(m_tags.size()) >= m_nMaxCount)) {
        return false;
    }

    Control* pTag = CreateTagItem(strText);
    const size_t nIndex = m_tags.size();
    AddItemAt(pTag, nIndex);
    m_tags.push_back(strText);
    SendEvent(kEventTagAdded, static_cast<WPARAM>(nIndex));
    return true;
}

bool TagInput::RemoveTagAt(size_t index)
{
    if (index >= m_tags.size()) {
        return false;
    }
    //Fire the event first, so the callback can still read the text of the removed tag
    SendEvent(kEventTagRemoved, static_cast<WPARAM>(index));
    RemoveItemAt(index);
    m_tags.erase(m_tags.begin() + index);
    return true;
}

void TagInput::RemoveAllTags()
{
    while (!m_tags.empty()) {
        RemoveTagAt(m_tags.size() - 1);
    }
}

size_t TagInput::GetTagCount() const
{
    return m_tags.size();
}

std::string TagInput::GetTagText(size_t index) const
{
    if (index >= m_tags.size()) {
        return std::string();
    }
    return std::string(m_tags[index].c_str());
}

void TagInput::SetAllowDuplicate(bool bAllowDuplicate)
{
    m_bAllowDuplicate = bAllowDuplicate;
}

bool TagInput::IsAllowDuplicate() const
{
    return m_bAllowDuplicate;
}

void TagInput::SetMaxCount(int32_t nMaxCount)
{
    m_nMaxCount = (nMaxCount > 0) ? nMaxCount : 0;
}

int32_t TagInput::GetMaxCount() const
{
    return m_nMaxCount;
}

RichEdit* TagInput::GetEditControl() const
{
    return m_pEdit;
}

Control* TagInput::CreateTagItem(const std::string& text)
{
    HBox* pTag = new HBox(GetWindow());
    pTag->SetClass("tag_input_tag");

    Label* pText = new Label(GetWindow());
    pText->SetClass("tag_input_tag_text");
    //The text does not intercept the mouse; clicks pass through to the tag container
    pText->SetMouseEnabled(false);
    pText->SetText(text);
    pTag->AddItem(pText);

    Button* pClose = new Button(GetWindow());
    pClose->SetClass("tag_input_tag_close");
    //The close button does not take part in Tab key navigation
    pClose->SetTabStop(false);
    pTag->AddItem(pClose);

    //Click on the body of the tag (not on the close button)
    pTag->AttachButtonUp([this, pTag](const EventArgs& /*args*/) {
        size_t nIndex = GetItemIndex(pTag);
        if (nIndex != Box::InvalidIndex) {
            SendEvent(kEventTagClick, static_cast<WPARAM>(nIndex));
        }
        return true;
        });

    //Click on the close button: remove by the current actual index (the index may have changed during removal)
    pClose->AttachClick([this, pTag](const EventArgs& /*args*/) {
        size_t nIndex = GetItemIndex(pTag);
        if (nIndex != Box::InvalidIndex) {
            RemoveTagAt(nIndex);
        }
        return true;
        });
    return pTag;
}

void TagInput::CommitInput()
{
    if (m_pEdit == nullptr) {
        return;
    }
    std::string text = m_pEdit->GetText();
    text = StringUtil::Trim(text.c_str());
    if (text.empty()) {
        return;
    }
    if (AddTag(text)) {
        m_pEdit->SetText("");
        m_pEdit->SetFocus();
    }
}

bool TagInput::HandleEditBackspace()
{
    if (m_tags.empty()) {
        return false;
    }
    return RemoveTagAt(m_tags.size() - 1);
}

void TagInput::HandleEditTextChanged()
{
    //Forward the text-changed event
    SendEvent(kEventTextChanged);

    if ((m_pEdit == nullptr) || m_delimiter.empty()) {
        return;
    }
    std::string text = m_pEdit->GetText();
    const std::string delimiter = m_delimiter.c_str();
    size_t nPos = text.find(delimiter);
    if (nPos == std::string::npos) {
        return;
    }
    std::string prefix = StringUtil::Trim(text.substr(0, nPos).c_str());
    std::string suffix = text.substr(nPos + delimiter.size());
    if (!prefix.empty()) {
        AddTag(prefix);
    }
    m_pEdit->SetText(suffix);
    m_pEdit->SetFocus();
}

bool TagInput::IsTagExists(const std::string& text) const
{
    for (const UiString& tag : m_tags) {
        if (text == tag.c_str()) {
            return true;
        }
    }
    return false;
}

void TagInput::ParseInitTags(const std::string& tags)
{
    if (tags.empty()) {
        return;
    }
    std::list<std::string> tagList = StringUtil::Split(std::string(tags), std::string(";"));
    for (const std::string& tag : tagList) {
        std::string strTag = StringUtil::Trim(tag.c_str());
        if (!strTag.empty()) {
            AddTag(strTag);
        }
    }
}

} // namespace ui
