#include "duilib/Control/RichTextImpl.h"
#include "duilib/Animation/AnimationManager.h"
#include "duilib/Animation/AnimationPlayer.h"
#include "duilib/Core/WindowBuilder.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/AttributeUtil.h"
#include "duilib/Core/Window.h"

namespace ui 
{

RichTextImpl::RichTextImpl(Control* pOwner) :
    m_pOwner(pOwner),
    m_hAlignType(HorAlignType::kAlignLeft),
    m_vAlignType(VerAlignType::kAlignTop),
    m_fRowSpacingMul(1.0f),
    m_fRowSpacingAdd(0.0f),
    m_bLinkUnderlineFont(true),
    m_nTextDataDisplayScaleFactor(0),
    m_bWordWrap(true),
    m_bReplaceBrace(true),
    m_bEnableRedraw(true)
{
}

RichTextImpl::~RichTextImpl()
{
    m_spDrawRichTextCache.reset();
}

bool RichTextImpl::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("text_align")) {
        //Horizontal alignment
        if (strValue.find(_T("left")) != DString::npos) {
            SetTextHAlignType(HorAlignType::kAlignLeft);
        }
        else if (strValue.find(_T("hcenter")) != DString::npos) {
            SetTextHAlignType(HorAlignType::kAlignCenter);
        }
        else if (strValue.find(_T("right")) != DString::npos) {
            SetTextHAlignType(HorAlignType::kAlignRight);
        }

        //Vertical alignment
        if (strValue.find(_T("top")) != DString::npos) {
            SetTextVAlignType(VerAlignType::kAlignTop);
        }
        else if (strValue.find(_T("vcenter")) != DString::npos) {
            SetTextVAlignType(VerAlignType::kAlignCenter);
        }
        else if (strValue.find(_T("bottom")) != DString::npos) {
            SetTextVAlignType(VerAlignType::kAlignBottom);
        }
        m_textData.clear();
        m_spDrawRichTextCache.reset();
    }    
    else if (strName == _T("font")) {
        SetFontId(strValue);
    }
    else if (strName == _T("text_color")) {
        SetTextColor(strValue);
    }    
    else if ((strName == _T("text_padding")) || (strName == _T("textpadding"))) {
        UiPadding rcTextPadding;
        AttributeUtil::ParsePaddingValue(strValue.c_str(), rcTextPadding);
        SetTextPadding(rcTextPadding);
    }
    else if (strName == _T("row_spacing_mul")) {
        SetRowSpacingMul(StringUtil::StringToFloat(strValue.c_str(), nullptr));
    }
    else if (strName == _T("row_spacing_add")) {
        SetRowSpacingAdd(StringUtil::StringToFloat(strValue.c_str(), nullptr));
    }
    else if (strName == _T("default_link_font_color")) {
        //Hyperlink: normal text color value
        m_linkNormalTextColor = strValue;
    }
    else if (strName == _T("hover_link_font_color")) {
        //Hyperlink: hover state text color value
        m_linkHoverTextColor = strValue;
    }
    else if (strName == _T("mouse_down_link_font_color")) {
        //Hyperlink: mouse pressed state text color value
        m_linkMouseDownTextColor = strValue;
    }
    else if (strName == _T("link_font_underline")) {
        //Hyperlink: whether to use the underlined font
        m_bLinkUnderlineFont = (strValue == _T("true"));
    }
    else if (strName == _T("replace_brace")) {
        //Whether to allow replacing curly braces for the text attribute
        m_bReplaceBrace = strValue == _T("true");
    }
    else if (strName == _T("text")) {
        //Allow using '{' instead of '<', '}' instead of '>' (the m_bReplaceBrace variable is the switch)
        if (m_bReplaceBrace &&
            ((strValue.find(_T('<')) == DString::npos) && (strValue.find(_T('>')) == DString::npos)) &&
            ((strValue.find(_T('{')) != DString::npos) && (strValue.find(_T('}')) != DString::npos))) {
            DString richText(strValue);
            StringUtil::ReplaceAll(_T("{"), _T("<"), richText);
            StringUtil::ReplaceAll(_T("}"), _T(">"), richText);
            SetText(richText);
        }
        else {
            SetText(strValue);
        }        
    }
    else if ((strName == _T("text_id")) || (strName == _T("textid"))) {
        SetTextId(strValue);
    }
    else if (strName == _T("trim_policy")) {
        if (strValue == _T("all")) {
            m_trimPolicy = TrimPolicy::kAll;
        }
        else if (strValue == _T("none")) {
            m_trimPolicy = TrimPolicy::kNone;
        }
        else if (strValue == _T("keep_one")) {
            m_trimPolicy = TrimPolicy::kKeepOne;
        }
        else {
            m_trimPolicy = TrimPolicy::kAll;
        }
    }
    else if (strName == _T("word_wrap")) {
        SetWordWrap(strValue == _T("true"));
    }
    else {
        return false;
    }
    return true;
}

void RichTextImpl::OnWindowChanged()
{
    //When the bound window changes, the DPI may change, so a redraw is needed
    Redraw();
}

void RichTextImpl::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!m_pOwner->Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    UiPadding rcTextPadding = GetTextPadding();
    rcTextPadding = m_pOwner->Dpi().GetScalePadding(rcTextPadding, nOldDpiScale);
    SetTextPadding(rcTextPadding, false);
    //When the DPI changes, a redraw is needed (the font size, etc. need to be scaled)
    Redraw();
}

void RichTextImpl::Redraw()
{
    //Redraw
    m_textData.clear();
    m_spDrawRichTextCache.reset();
    if (IsEnableRedraw()) {
        m_pOwner->Invalidate();
        m_pOwner->RelayoutOrRedraw();
    }    
}

void RichTextImpl::Invalidate()
{
    if (IsEnableRedraw()) {
        m_pOwner->Invalidate();
    }
}

uint16_t RichTextImpl::GetTextStyle() const
{
    uint32_t uTextStyle = 0;
    if (m_hAlignType == HorAlignType::kAlignCenter) {
        uTextStyle |= TEXT_HCENTER;
    }
    else if (m_hAlignType == HorAlignType::kAlignRight) {
        uTextStyle |= TEXT_RIGHT;
    }
    else {
        uTextStyle |= TEXT_LEFT;
    }

    if (m_vAlignType == VerAlignType::kAlignCenter) {
        uTextStyle |= TEXT_VCENTER;
    }
    else if (m_vAlignType == VerAlignType::kAlignBottom) {
        uTextStyle |= TEXT_BOTTOM;
    }
    else {
        uTextStyle |= TEXT_TOP;
    }

    if (IsWordWrap()) {
        uTextStyle |= TEXT_WORD_WRAP;
    }
    else {
        uTextStyle &= ~TEXT_WORD_WRAP;
    }

    //Should not include the single-line attribute
    uTextStyle &= ~TEXT_SINGLELINE;

    return ui::TruncateToUInt16(uTextStyle);
}

void RichTextImpl::CalcDestRect(IRender* pRender, const UiRect& rc, UiRect& rect)
{
    if (pRender == nullptr) {
        return;
    }
    rect.Clear();
    if (!m_textData.empty()) {
        std::vector<RichTextData> richTextData;
        richTextData.reserve(m_textData.size());
        const uint16_t nTextStyle = GetTextStyle();
        for (const RichTextData& textData : m_textData) {
            richTextData.push_back(textData);
            //The drawing text attribute information must be included during calculation
            richTextData[richTextData.size() - 1].m_textStyle = nTextStyle;
        }
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        std::vector<std::vector<UiRect>> richTextRects;
        pRender->MeasureRichText(rc, UiSize(), pRenderFactory, richTextData, &richTextRects);
        ASSERT(richTextRects.size() == m_textData.size());
        if (richTextRects.size() == m_textData.size()) {
            for (size_t index = 0; index < m_textData.size(); ++index) {
                m_textData[index].m_textRects.swap(richTextRects[index]);
            }
        }
    }
    for (const RichTextDataEx& textData : m_textData) {
        for (const UiRect& textRect : textData.m_textRects) {
            rect.Union(textRect);
        }
    }
}

UiSize RichTextImpl::EstimateText(UiSize szAvailable)
{
    UiSize fixedSize;
    IRender* pRender = nullptr;
    if (m_pOwner->GetWindow() != nullptr) {
        pRender = m_pOwner->GetWindow()->GetRender();
    }
    if (pRender == nullptr) {
        return fixedSize;
    }

    int32_t nWidth = szAvailable.cx;
    if (m_pOwner->GetFixedWidth().IsStretch()) {
        //If it is the stretch type, use the external width
        nWidth = CalcStretchValue(m_pOwner->GetFixedWidth(), szAvailable.cx);
    }
    else if (m_pOwner->GetFixedWidth().IsInt32()) {
        nWidth = m_pOwner->GetFixedWidth().GetInt32();
    }
    else if (m_pOwner->GetFixedWidth().IsAuto()) {
        //When the width is auto, do not limit the width
        nWidth = m_pOwner->GetMaxWidth();
    }

    //Maximum height, unlimited
    int32_t nHeight = INT_MAX;

    UiRect rc;
    rc.left = 0;
    rc.right = rc.left + nWidth;
    rc.top = 0;
    rc.bottom = rc.top + nHeight;

    const UiPadding rcTextPadding = GetTextPadding();
    const UiPadding rcPadding = m_pOwner->GetControlPadding();
    if (nWidth != INT_MAX) {
        rc.left += (rcPadding.left + rcTextPadding.left);
        rc.right -= (rcPadding.right + rcTextPadding.right);
    }
    if (nHeight != INT_MAX) {
        rc.top += (rcPadding.top + rcTextPadding.top);
        rc.bottom -= (rcPadding.bottom + rcTextPadding.bottom);
    }
    if (rc.IsEmpty()) {
        return fixedSize;
    }

    //Check and update the text
    CheckParseText();

    //Calculate the size of the area occupied by drawing
    UiRect rect;
    CalcDestRect(pRender, rc, rect);

    fixedSize.cx = rect.Width();
    fixedSize.cx += (rcTextPadding.left + rcTextPadding.right);
    fixedSize.cx += (rcPadding.left + rcPadding.right);

    fixedSize.cy = rect.Height();
    fixedSize.cy += (rcTextPadding.top + rcTextPadding.bottom);
    fixedSize.cy += (rcPadding.top + rcPadding.bottom);

    return fixedSize;
}

void RichTextImpl::PaintText(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }
    UiRect rc = m_pOwner->GetRect();
    rc.Deflate(m_pOwner->GetControlPadding());
    rc.Deflate(GetTextPadding());

    //Check and update the text
    CheckParseText();

    //If an alignment is set, the drawing position needs to be evaluated
    const uint32_t uTextStyle = GetTextStyle();
    if ((uTextStyle & (TEXT_HCENTER | TEXT_RIGHT | TEXT_VCENTER | TEXT_BOTTOM))) {
        //Calculate the size of the area occupied by drawing
        UiRect rect;
        CalcDestRect(pRender, rc, rect);
        if ((rect.Width() < rc.Width()) && (uTextStyle & (TEXT_HCENTER | TEXT_RIGHT))) {
            //Horizontal direction
            int32_t diff = rc.Width() - rect.Width();
            if (uTextStyle & TEXT_HCENTER) {
                rc.Offset(diff / 2, 0);
            }
            else if (uTextStyle & TEXT_RIGHT) {
                rc.Offset(diff, 0);
            }
        }
        if ((rect.Height() < rc.Height()) && (uTextStyle & (TEXT_VCENTER | TEXT_BOTTOM))) {
            //Vertical direction
            int32_t diff = rc.Height() - rect.Height();
            if (uTextStyle & TEXT_VCENTER) {
                rc.Offset(0, diff / 2);
            }
            else if (uTextStyle & TEXT_BOTTOM) {
                rc.Offset(0, diff);
            }
        }
    }

    if (!m_textData.empty()) {
        UiColor normalLinkTextColor;
        if (!m_linkNormalTextColor.empty()) {
            normalLinkTextColor = m_pOwner->GetUiColor(m_linkNormalTextColor.c_str());
        }
        UiColor mouseDownLinkTextColor;
        if (!m_linkMouseDownTextColor.empty()) {
            mouseDownLinkTextColor = m_pOwner->GetUiColor(m_linkMouseDownTextColor.c_str());
        }
        UiColor linkHoverTextColor;
        if (!m_linkHoverTextColor.empty()) {
            linkHoverTextColor = m_pOwner->GetUiColor(m_linkHoverTextColor.c_str());
        }

        std::vector<RichTextData> richTextData;
        richTextData.reserve(m_textData.size());
        for (const RichTextDataEx& textDataEx : m_textData) {
            if (!textDataEx.m_linkUrl.empty()) {
                //For hyperlinks, set the default text format
                RichTextData textData = textDataEx;
                textData.m_pFontInfo.reset(new UiFontEx);
                ASSERT(textDataEx.m_pFontInfo != nullptr);
                if (textDataEx.m_pFontInfo != nullptr) {
                    ASSERT(!textDataEx.m_pFontInfo->m_fontName.empty());
                    textData.m_pFontInfo->CopyFrom(*textDataEx.m_pFontInfo);
                }                
                if (textDataEx.m_bMouseDown || textDataEx.m_bMouseHover) {
                    textData.m_pFontInfo->m_bUnderline = m_bLinkUnderlineFont;//whether to show the underline font
                }
                if (!m_linkNormalTextColor.empty()) {                    
                    if (!normalLinkTextColor.IsEmpty()) {
                        textData.m_textColor = normalLinkTextColor;//font color of the normal state
                    }
                }
                if (textDataEx.m_bMouseDown && !m_linkMouseDownTextColor.empty()) {                    
                    if (!mouseDownLinkTextColor.IsEmpty()) {
                        textData.m_textColor = mouseDownLinkTextColor;//font color when the mouse is pressed
                    }
                }
                else if (textDataEx.m_bMouseHover && !m_linkHoverTextColor.empty()) {                    
                    if (!linkHoverTextColor.IsEmpty()) {
                        textData.m_textColor = linkHoverTextColor;//font color when the mouse hovers
                    }
                }
                richTextData.push_back(textData);
            }
            else {
                richTextData.push_back(textDataEx);
            }
        }
        std::vector<std::vector<UiRect>> richTextRects;
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        const UiRect& textRect = rc;
        UiSize szScrollOffset;
        if (m_spDrawRichTextCache != nullptr) {
            //Validate the cache; regenerate it if it is invalid
            if (!pRender->IsValidDrawRichTextCache(textRect, richTextData, m_spDrawRichTextCache)) {
                m_spDrawRichTextCache.reset();
            }
        }
        if (m_spDrawRichTextCache == nullptr) {
            bool bRet = pRender->CreateDrawRichTextCache(textRect, szScrollOffset, pRenderFactory, richTextData, m_spDrawRichTextCache);
            if (!bRet) {
                m_spDrawRichTextCache.reset();
            }
        }
        if (m_spDrawRichTextCache != nullptr) {
            //Use the drawing cache
            std::vector<int32_t> rowXOffset;//used when the alignment is centered or right-aligned
            pRender->DrawRichTextCacheData(m_spDrawRichTextCache, textRect, szScrollOffset, rowXOffset, m_pOwner->GetAlpha(), &richTextRects);

#if 0
            //End of the test code (compares whether the results are the same when drawing with and without the cache)
            std::vector<std::vector<UiRect>> richTextRectsOld;
            pRender->DrawRichText(textRect, szScrollOffset, pRenderFactory, richTextData, GetAlpha(), &richTextRectsOld);
            ASSERT(richTextRects.size() == m_textData.size());
            ASSERT(richTextRectsOld.size() == m_textData.size());
            if (richTextRectsOld.size() == richTextRects.size()) {
                for (size_t i = 0; i < richTextRectsOld.size(); ++i) {
                    const std::vector<UiRect>& rectOld = richTextRectsOld[i];
                    const std::vector<UiRect>& rectNew = richTextRects[i];
                    ASSERT(rectOld.size() == rectNew.size());
                    for (size_t j = 0; j < rectOld.size(); ++j) {
                        const UiRect& oldRect = rectOld[j];
                        const UiRect& newRect = rectNew[j];
                        ASSERT(oldRect == newRect);
                    }
                }

            }
#endif      //End of the test code
        }
        else {
            //Do not use the drawing cache            
            pRender->DrawRichText(textRect, szScrollOffset, pRenderFactory, richTextData, m_pOwner->GetAlpha(), &richTextRects);
        }
        ASSERT(richTextRects.size() == m_textData.size());
        if (richTextRects.size() == m_textData.size()) {
            for (size_t index = 0; index < m_textData.size(); ++index) {
                m_textData[index].m_textRects.swap(richTextRects[index]);
            }
        }
    }
}

void RichTextImpl::CheckParseText()
{
    if (!m_richTextId.empty() && (m_langFileName != GlobalManager::Instance().GetLanguageFileName())) {
        //Multilingual version: when the language changes, update the text content
        DoSetText(GlobalManager::Instance().Lang().GetStringViaID(m_richTextId.c_str()));
        m_langFileName = GlobalManager::Instance().GetLanguageFileName();
    }

    //When the DPI changes, the text needs to be re-parsed and the font size updated
    if (m_nTextDataDisplayScaleFactor != m_pOwner->Dpi().GetDisplayScaleFactor()) {
        m_textData.clear();
        m_spDrawRichTextCache.reset();
    }

    if (m_textData.empty()) {
        ParseText(m_textData);
        m_nTextDataDisplayScaleFactor = m_pOwner->Dpi().GetDisplayScaleFactor();
        m_spDrawRichTextCache.reset();
    }
}

bool RichTextImpl::ParseText(std::vector<RichTextDataEx>& outTextData) const
{
    //Default font
    DString sFontId = GetFontId();
    IFont* pFont = GlobalManager::Instance().Font().GetIFont(sFontId, m_pOwner->Dpi());
    ASSERT(pFont != nullptr);
    if (pFont == nullptr) {
        return false;
    }

    RichTextDataEx parentTextData;
    //Default text color
    parentTextData.m_textColor = m_pOwner->GetUiColor(GetTextColor());
    if (parentTextData.m_textColor.IsEmpty()) {
        parentTextData.m_textColor = UiColor(UiColors::Black);
    }
    parentTextData.m_pFontInfo.reset(new UiFontEx);
    parentTextData.m_pFontInfo->m_fontName = pFont->FontName();
    parentTextData.m_pFontInfo->m_fontSize = pFont->FontSize();
    parentTextData.m_pFontInfo->m_bBold = pFont->IsBold();
    parentTextData.m_pFontInfo->m_bUnderline = pFont->IsUnderline();
    parentTextData.m_pFontInfo->m_bItalic = pFont->IsItalic();
    parentTextData.m_pFontInfo->m_bStrikeOut = pFont->IsStrikeOut();
    parentTextData.m_fRowSpacingMul = m_fRowSpacingMul;
    parentTextData.m_fRowSpacingAdd = m_fRowSpacingAdd;
    parentTextData.m_textStyle = GetTextStyle();

    std::vector<RichTextDataEx> textData;

    for (const RichTextSlice& textSlice : m_textSlice) {
        if (!ParseTextSlice(textSlice, parentTextData, textData)) {
            return false;
        }
    }
    outTextData.swap(textData);
    return true;
}

bool RichTextImpl::ParseTextSlice(const RichTextSlice& textSlice, 
                                  const RichTextDataEx& parentTextData, 
                                  std::vector<RichTextDataEx>& textData) const
{
    //Current node
    RichTextDataEx currentTextData;
    currentTextData.m_fRowSpacingMul = parentTextData.m_fRowSpacingMul;
    currentTextData.m_fRowSpacingAdd = parentTextData.m_fRowSpacingAdd;
    currentTextData.m_textStyle = parentTextData.m_textStyle;
    currentTextData.m_textView = std::wstring_view(textSlice.m_text.c_str(), textSlice.m_text.size());

    //Should not contain carriage return and line feed (handled by the external caller)
    ASSERT(currentTextData.m_textView.find(L"\r") == DStringW::npos);    
    ASSERT(currentTextData.m_textView.find(L"\t") == DStringW::npos);
    if (currentTextData.m_textView.size() != 1) {
        ASSERT(currentTextData.m_textView.find(L"\n") == DStringW::npos);
    }

    currentTextData.m_linkUrl = textSlice.m_linkUrl;
    if (!textSlice.m_textColor.empty()) {
        currentTextData.m_textColor = m_pOwner->GetUiColor(textSlice.m_textColor.c_str());
    }
    if (textSlice.m_textColor.empty()) {
        currentTextData.m_textColor = parentTextData.m_textColor;
    }

    if (!textSlice.m_bgColor.empty()) {
        currentTextData.m_bgColor = m_pOwner->GetUiColor(textSlice.m_bgColor.c_str());
    }
    else {
        currentTextData.m_bgColor = parentTextData.m_bgColor;
    }

    currentTextData.m_pFontInfo.reset(new UiFontEx);
    if (parentTextData.m_pFontInfo != nullptr) {
        currentTextData.m_pFontInfo->CopyFrom(*parentTextData.m_pFontInfo);
    }

    if (!textSlice.m_fontInfo.m_fontName.empty()) {
        currentTextData.m_pFontInfo->m_fontName = textSlice.m_fontInfo.m_fontName;
    }
    if (textSlice.m_fontInfo.m_fontSize > 0) {
        currentTextData.m_pFontInfo->m_fontSize = m_pOwner->Dpi().GetScaleInt(textSlice.m_fontInfo.m_fontSize); //font size, needs DPI scaling
    }
    if (textSlice.m_fontInfo.m_bBold) {
        currentTextData.m_pFontInfo->m_bBold = textSlice.m_fontInfo.m_bBold;
    }
    if (textSlice.m_fontInfo.m_bUnderline) {
        currentTextData.m_pFontInfo->m_bUnderline = textSlice.m_fontInfo.m_bUnderline;
    }
    if (textSlice.m_fontInfo.m_bItalic) {
        currentTextData.m_pFontInfo->m_bItalic = textSlice.m_fontInfo.m_bItalic;
    }
    if (textSlice.m_fontInfo.m_bStrikeOut) {
        currentTextData.m_pFontInfo->m_bStrikeOut = textSlice.m_fontInfo.m_bStrikeOut;
    }
    if (!currentTextData.m_textView.empty() || !currentTextData.m_linkUrl.empty()) {
        textData.push_back(currentTextData);
    }
        
    if (!textSlice.m_children.empty()) {
        //Child nodes
        for (const RichTextSlice& childSlice : textSlice.m_children) {
            if (!ParseTextSlice(childSlice, currentTextData, textData)) {
                return false;
            }
        }
    }
    return true;
}

UiPadding RichTextImpl::GetTextPadding() const
{
    return UiPadding(m_rcTextPadding.left, m_rcTextPadding.top, m_rcTextPadding.right, m_rcTextPadding.bottom);
}

void RichTextImpl::SetTextPadding(UiPadding padding, bool bNeedDpiScale)
{
    ASSERT((padding.left >= 0) && (padding.top >= 0) && (padding.right >= 0) && (padding.bottom >= 0));
    if ((padding.left < 0) || (padding.top < 0) ||
        (padding.right < 0) || (padding.bottom < 0)) {
        return;
    }
    if (bNeedDpiScale) {
        m_pOwner->Dpi().ScalePadding(padding);
    }
    if (!GetTextPadding().Equals(padding)) {
        m_rcTextPadding.left = TruncateToUInt16(padding.left);
        m_rcTextPadding.top = TruncateToUInt16(padding.top);
        m_rcTextPadding.right = TruncateToUInt16(padding.right);
        m_rcTextPadding.bottom = TruncateToUInt16(padding.bottom);
        if (IsEnableRedraw()) {
            m_pOwner->RelayoutOrRedraw();
        }        
    }
}

const DString& RichTextImpl::TrimText(DString& text)
{
    //Carriage return / line feed / TAB key: replace with 1 space
    DString tempText;
    tempText.swap(text);
    size_t nCount = tempText.size();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        if ((tempText[nIndex] == _T('\r')) || (tempText[nIndex] == _T('\n')) || (tempText[nIndex] == _T('\t'))) {
            tempText[nIndex] = _T(' ');
        }
    }

    if (m_trimPolicy == TrimPolicy::kNone) {
        //do nothing
    }
    else if (m_trimPolicy == TrimPolicy::kKeepOne) {
        //Keep only one space
        if (!tempText.empty()) {
            bool bFirst = (tempText.front() == _T(' '));
            bool bLast = tempText[tempText.size() - 1] == _T(' ');
            StringUtil::Trim(tempText);
            if (tempText.empty()) {
                tempText = DString(_T(" "));
            }
            else {
                if (bFirst) {
                    tempText = DString(_T(" ")) + tempText;
                }
                else if (bLast) {
                    tempText += DString(_T(" "));
                }
            }
        }
    }
    else {
        //Remove spaces from both the left and right sides
        StringUtil::Trim(tempText);
    }
    tempText.swap(text);
    return text;
}

DString RichTextImpl::TrimText(const DString::value_type* text)
{
    DString retText;
    if (text != nullptr) {
        retText = text;
    }
    return TrimText(retText);
}

void RichTextImpl::SetEnableRedraw(bool bEnable)
{
    m_bEnableRedraw = bEnable;
}

bool RichTextImpl::IsEnableRedraw() const
{
    return m_bEnableRedraw;
}

bool RichTextImpl::DoSetText(const DString& richText)
{
    Clear();
    //The XML parsing is all encapsulated in the WindowBuilder class to avoid using XML parsers everywhere, thus reducing code maintenance complexity
    bool bResult = true;
    if (!richText.empty()) {
        if (richText.find(_T("<RichText")) == DString::npos) {
            DString formatedText = _T("<RichText>") + richText + _T("</RichText>");
            bResult = WindowBuilder::ParseRichTextXmlText(formatedText, m_pOwner);
        }
        else {
            bResult = WindowBuilder::ParseRichTextXmlText(richText, m_pOwner);
        }
    }
    return bResult;
}

bool RichTextImpl::SetText(const DString& richText, bool bRedraw)
{
    bool bResult = DoSetText(richText);
    if (bResult && bRedraw) {
        if (IsEnableRedraw()) {
            m_pOwner->RelayoutOrRedraw();
        }
    }
    return bResult;
}

bool RichTextImpl::SetTextId(const DString& richTextId, bool bRedraw)
{
    bool bRet = SetText(GlobalManager::Instance().Lang().GetStringViaID(richTextId), bRedraw);
    m_richTextId = richTextId;
    if (!m_richTextId.empty()) {
        m_langFileName = GlobalManager::Instance().GetLanguageFileName();
    }
    else {
        m_langFileName.clear();
    }
    return bRet;
}

void RichTextImpl::Clear()
{
    m_textData.clear();
    m_spDrawRichTextCache.reset();
    if (!m_textSlice.empty()) {
        m_textSlice.clear();
        Invalidate();
    }
}

DString RichTextImpl::GetFontId() const
{
    return m_sFontId.c_str();
}

void RichTextImpl::SetFontId(const DString& strFontId)
{
    if (m_sFontId != strFontId) {
        m_sFontId = strFontId;
        Redraw();
    }
}

DString RichTextImpl::GetTextColor() const
{
    return m_sTextColor.c_str();
}

void RichTextImpl::SetTextColor(const DString& sTextColor)
{
    if (m_sTextColor != sTextColor) {
        m_sTextColor = sTextColor;
        Redraw();
    }
}

float RichTextImpl::GetRowSpacingMul() const
{
    return m_fRowSpacingMul;
}

void RichTextImpl::SetRowSpacingMul(float fRowSpacingMul)
{
    if (m_fRowSpacingMul != fRowSpacingMul) {
        m_fRowSpacingMul = fRowSpacingMul;
        if (m_fRowSpacingMul <= 0.01f) {
            m_fRowSpacingMul = 1.0f;
        }
        Redraw();
    }
}

float RichTextImpl::GetRowSpacingAdd() const
{
    return m_fRowSpacingAdd;
}

void RichTextImpl::SetRowSpacingAdd(float fRowSpacingAdd)
{
    if (m_fRowSpacingAdd != fRowSpacingAdd) {
        m_fRowSpacingAdd = fRowSpacingAdd;
        if (m_fRowSpacingAdd <= 0.0001f) {
            m_fRowSpacingAdd = 0.0f;
        }
        Redraw();
    }
}

bool RichTextImpl::IsWordWrap() const
{
    return m_bWordWrap;
}

void RichTextImpl::SetWordWrap(bool bWordWrap)
{
    if (m_bWordWrap != bWordWrap) {
        m_bWordWrap = bWordWrap;
        Redraw();
    }
}

void RichTextImpl::SetTextHAlignType(HorAlignType alignType)
{
    if (m_hAlignType != alignType) {
        m_hAlignType = alignType;
        Redraw();
    }
}

HorAlignType RichTextImpl::GetHAlignType() const
{
    return m_hAlignType;
}

void RichTextImpl::SetTextVAlignType(VerAlignType alignType)
{
    if (m_vAlignType != alignType) {
        m_vAlignType = alignType;
        Redraw();
    }
}

VerAlignType RichTextImpl::GetVAlignType() const
{
    return m_vAlignType;
}


void RichTextImpl::AppendTextSlice(const RichTextSlice&& textSlice)
{
    m_textSlice.emplace_back(textSlice);
    m_textData.clear();
    m_spDrawRichTextCache.reset();
}

void RichTextImpl::AppendTextSlice(const RichTextSlice& textSlice)
{
    m_textSlice.emplace_back(textSlice);
    m_textData.clear();
    m_spDrawRichTextCache.reset();
}

DString RichTextImpl::ToString() const
{
    const DString indentValue = _T("    ");
    const DString lineBreak = _T("\r\n");
    DString richText = _T("<RichText>");
    richText += lineBreak;
    for (const RichTextSlice& textSlice : m_textSlice) {
        richText += ToString(textSlice, indentValue);
    }
    richText += _T("</RichText>");
    return richText;
}

DString RichTextImpl::ToString(const RichTextSlice& textSlice, const DString& indent) const
{
    // List of supported tags (HTML-compatible tags):
    // 
    // Hyperlink:   <a href="URL">text</a>
    // Bold:      <b> </b>
    // Italic:      <i> </i>
    // Strikethrough:      <s> </s> or <del> </del> or <strike> </strike>
    // Underline:    <u> </u>
    // Set background color:  <bgcolor color="#000000"> </bgcolor>
    // Set font:    <font face="SimSun" size="12" color="#000000">
    // Line break tag:   <br/>
    const DString indentValue = _T("    ");
    const DString lineBreak = _T("\r\n");
    DString richText;
    if (textSlice.m_nodeName.empty()) {
        if (!textSlice.m_text.empty()) {
            richText += indent;
            richText += StringConvert::WStringToT(textSlice.m_text);
            richText += lineBreak;
        }
        //When there is no node name, there are no attributes or child nodes, so return directly
        return richText;
    }

    //Generate the attribute list
    DString attrList;    
    if (!textSlice.m_linkUrl.empty()) {
        attrList += _T("href=\"");
        DString url = textSlice.m_linkUrl.c_str();
        attrList += textSlice.m_linkUrl.c_str();
        attrList += _T("\"");
    }
    if (!textSlice.m_bgColor.empty()) {
        attrList += _T("color=\"");
        attrList += textSlice.m_bgColor.c_str();
        attrList += _T("\"");
    }
    if (!textSlice.m_textColor.empty()) {
        attrList += _T("color=\"");
        attrList += textSlice.m_textColor.c_str();
        attrList += _T("\"");
    }
    if (!textSlice.m_fontInfo.m_fontName.empty()) {
        attrList += _T("face=\"");
        attrList += textSlice.m_fontInfo.m_fontName.c_str();
        attrList += _T("\"");
    }
    if (textSlice.m_fontInfo.m_fontSize != 0) {
        attrList += _T("size=\"");
        attrList += StringUtil::Printf(_T("%d"), textSlice.m_fontInfo.m_fontSize);
        attrList += _T("\"");
    }

    if(!textSlice.m_children.empty()) {
        //Has child nodes: node start
        richText += indent;
        richText += _T("<");
        richText += textSlice.m_nodeName.c_str();
        if (!attrList.empty()) {
            richText += _T(" ");
            richText += attrList;
        }
        richText += _T(">");
        richText += lineBreak;

        //Add child nodes
        for (const RichTextSlice& childSlice : textSlice.m_children) {
            richText += ToString(childSlice, indent + indentValue);
        }

        //Node end
        richText += indent;
        richText += _T("</");
        richText += textSlice.m_nodeName.c_str();
        richText += _T(">");
        richText += lineBreak;
    }
    else if (!textSlice.m_linkUrl.empty()) {
        //Hyperlink node: needs special handling
        richText += indent;
        richText += _T("<");
        richText += textSlice.m_nodeName.c_str();
        if (!attrList.empty()) {
            richText += _T(" ");
            richText += attrList;
        }
        richText += _T(">");

        //Add the hyperlink text
        richText += StringConvert::WStringToT(textSlice.m_text);

        //Node end
        richText += _T("</");
        richText += textSlice.m_nodeName.c_str();
        richText += _T(">");
        richText += lineBreak;
    }
    else {
        //No child nodes: put it on one line
        richText += indent;
        richText += _T("<");
        richText += textSlice.m_nodeName.c_str();
        richText += _T(" ");
        if (!attrList.empty()) {            
            richText += attrList;
        }
        richText += _T("/>");
        richText += lineBreak;
    }
    return richText;
}

void RichTextImpl::ButtonDown(const EventArgs& msg)
{
    if (msg.IsSenderExpired()) {
        return;
    }
    for (size_t index = 0; index < m_textData.size(); ++index) {
        RichTextDataEx& textData = m_textData[index];
        textData.m_bMouseDown = false;
        if (textData.m_linkUrl.empty()) {
            continue;
        }        
        for (const UiRect& textRect : textData.m_textRects) {
            if (textRect.ContainsPt(msg.ptMouse)) {
                //On the hyperlink
                textData.m_bMouseDown = true;
                Invalidate();
            }
        }
    }
}

void RichTextImpl::ButtonUp(const EventArgs& msg)
{
    if (msg.IsSenderExpired()) {
        return;
    }
    for (size_t index = 0; index < m_textData.size(); ++index) {
        RichTextDataEx& textData = m_textData[index];
        if (textData.m_linkUrl.empty()) {
            continue;
        }
        for (const UiRect& textRect : textData.m_textRects) {
            if (textRect.ContainsPt(msg.ptMouse)) {
                //On the hyperlink, and the same as when the mouse was pressed down, trigger the click event
                if (textData.m_bMouseDown) {
                    textData.m_bMouseDown = false;
                    Invalidate();
                    DString url = textData.m_linkUrl.c_str();
                    m_pOwner->SendEvent(kEventLinkClick, (WPARAM)url.c_str());
                    return;
                }                
            }
        }
    }
    bool bInvalidate = false;
    for (RichTextDataEx& textData : m_textData) {
        if (textData.m_linkUrl.empty()) {
            continue;
        }
        if (textData.m_bMouseDown) {
            textData.m_bMouseDown = false;
            bInvalidate = true;
        }
    }
    if (bInvalidate) {
        Invalidate();
    }
}

void RichTextImpl::MouseMove(const EventArgs& msg)
{
    bool bOnLinkUrl = false;
    for (RichTextDataEx& textData : m_textData) {
        textData.m_bMouseHover = false;
        if (textData.m_linkUrl.empty()) {
            continue;
        }
        for (const UiRect& textRect : textData.m_textRects) {
            if (textRect.ContainsPt(msg.ptMouse)) {
                //On the hyperlink
                textData.m_bMouseHover = true;
                Invalidate();
                if (textData.m_linkUrl == m_pOwner->GetToolTipText()) {
                    bOnLinkUrl = true;
                }
            }
        }
    }
    if (!bOnLinkUrl) {
        m_pOwner->SetToolTipText(_T(""));
    }
}

void RichTextImpl::MouseHover(const EventArgs& msg)
{
    bool hasHover = false;
    for (const RichTextDataEx& textData : m_textData) {
        if (textData.m_linkUrl.empty()) {
            continue;
        }
        for (const UiRect& textRect : textData.m_textRects) {
            if (textRect.ContainsPt(msg.ptMouse)) {
                //Hyperlink, show the ToolTip
                m_pOwner->SetToolTipText(textData.m_linkUrl.c_str());
                hasHover = true;
            }
        }        
    }
    if (!hasHover) {
        m_pOwner->SetToolTipText(_T(""));
    }
}

void RichTextImpl::MouseLeave(const EventArgs& /*msg*/)
{
    bool bInvalidate = false;
    for (RichTextDataEx& textData : m_textData) {
        if (textData.m_linkUrl.empty()) {
            continue;
        }
        if (textData.m_bMouseHover) {
            textData.m_bMouseHover = false;
            bInvalidate = true;
        }
        if (textData.m_bMouseDown) {
            textData.m_bMouseDown = false;
            bInvalidate = true;
        }
    }
    if (bInvalidate) {
        Invalidate();
    }
}

bool RichTextImpl::OnSetCursor(const EventArgs& msg)
{
    if (m_pOwner->IsEnabled() && m_pOwner->IsVisible()) {
        for (const RichTextDataEx& textData : m_textData) {
            if (textData.m_linkUrl.empty()) {
                continue;
            }
            for (const UiRect& textRect : textData.m_textRects) {
                if (textRect.ContainsPt(msg.ptMouse)) {
                    //Hyperlink, the cursor becomes a hand
                    m_pOwner->SetCursor(CursorType::kCursorHand);
                    return true;
                }
            }
        }
    }
    return false;
}

} // namespace ui
