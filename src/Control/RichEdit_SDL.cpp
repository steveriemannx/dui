#include "dui/Control/RichEdit_SDL.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Core/WindowMessage.h"
#include "dui/Core/ScrollBar.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/AttributeUtil.h"
#include "dui/Utils/PerformanceUtil.h"
#include "dui/Utils/Clipboard.h"
#include "dui/Render/IRender.h"
#include "dui/Render/AutoClip.h"
#include "dui/Animation/AnimationManager.h"
#include "dui/Animation/AnimationPlayer.h"
#include "dui/Control/Menu.h"
#include "dui/Control/Button.h"
#include "dui/Box/VBox.h"

#if defined(DUI_BUILD_FOR_SDL) || defined(DUI_BUILD_FOR_WAYLAND)
#if defined(DUI_BUILD_FOR_SDL)
#include <SDL3/SDL.h>
#endif

//The maximum value of the zoom percentage
#define MAX_ZOOM_PERCENT 800

namespace ui {

RichEdit::RichEdit(Window* pWindow) :
    ScrollBox(pWindow, new Layout),
    m_bWantTab(false),
    m_bWantReturn(false),
    m_bWantCtrlReturn(false),
    m_bAllowPrompt(false),
    m_bSelAllEver(false),         
    m_bNoSelOnKillFocus(true), 
    m_bSelAllOnFocus(false),    
    m_bNoCaretReadonly(false),
    m_bIsCaretVisiable(false),
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    m_bIsComposition(false),
#endif
    m_iCaretPosX(0),
    m_iCaretPosY(0),
    m_iCaretWidth(0),
    m_iCaretHeight(0),
    m_nRowHeight(0),
    m_sFontId(),
    m_sTextColor(),
    m_sDisabledTextColor(),
    m_sPromptColor(),
    m_sPromptText(),
    m_drawCaretFlag(),
    m_pFocusedImage(nullptr),
    m_bUseControlCursor(false),
    m_nZoomPercent(100),
    m_bEnableWheelZoom(false),
    m_bEnableDefaultContextMenu(false),
    m_bDisableTextChangeEvent(false),
    m_maxNumber(INT_MAX),
    m_minNumber(INT_MIN),
    m_pSpinBox(nullptr),
    m_pClearButton(nullptr),
    m_pShowPasswordButton(nullptr),
    m_bReadOnly(false),
    m_bPasswordMode(false),
    m_bShowPassword(false),
    m_chPasswordChar(L'*'),
    m_bFlashPasswordChar(false),
    m_bInputPasswordChar(false),
    m_bNumberOnly(false),
    m_bWordWrap(false),
    m_nLimitText(0),
    m_bModified(false),
    m_nSelStartIndex(0),
    m_nSelEndCharIndex(0),
    m_nShiftStartIndex(-1),
    m_nCtrlStartIndex(-1),
    m_bSelForward(true),
    m_nSelXPos(-1),
    m_bHideSelection(true),
    m_bActive(false),
    m_bTextInputMode(false),
    m_bMouseDownInView(false),
    m_bMouseDown(false),
    m_bRMouseDown(false),
    m_bInMouseMove(false),
    m_pMouseSender(nullptr),
    m_pTextData(nullptr),
    m_sSelectionBkColor(_T("CornflowerBlue")),
    m_sInactiveSelectionBkColor(_T("DarkGray")),
    m_sCurrentRowBkColor(_T("")),
    m_sInactiveCurrentRowBkColor(_T("")),
    m_nFocusBottomBorderSize(0),
    m_fRowSpacingMul(1.0f),
    m_fRowSpacingAdd(0.0f)
{
    m_pTextData = new RichEditData(this);
}

RichEdit::~RichEdit()
{
    if (m_pFocusedImage != nullptr) {
        delete m_pFocusedImage;
        m_pFocusedImage = nullptr;
    }
    m_pLimitChars.reset();
    if (m_pTextData != nullptr) {
        delete m_pTextData;
        m_pTextData = nullptr;
    }
    DString internalFontId = GetInternalFontId();
    if (GlobalManager::Instance().Font().HasFontId(internalFontId)) {
        GlobalManager::Instance().Font().RemoveFontId(internalFontId);
    }
}

DString RichEdit::GetType() const { return DUI_CTR_RICHEDIT; }

void RichEdit::SetAttribute(const DString& strName, const DString& strValue)
{
    if ((strName == _T("single_line")) || (strName == _T("singleline"))) {
        SetMultiLine(strValue != _T("true"));
    }
    else if ((strName == _T("multi_line")) || (strName == _T("multiline"))) {
        SetMultiLine(strValue == _T("true"));
    }
    else if (strName == _T("readonly")) {
        SetReadOnly(strValue == _T("true"));
    }
    else if (strName == _T("password")) {
        SetPasswordMode(strValue == _T("true"));
    }
    else if (strName == _T("show_password")) {
        SetShowPassword(strValue == _T("true"));
    }
    else if (strName == _T("password_char")) {
        if (!strValue.empty()) {
            SetPasswordChar(strValue.front());
        }
    }
    else if (strName == _T("flash_password_char")) {
        SetFlashPasswordChar(strValue == _T("true"));
    }
    else if ((strName == _T("number_only")) || (strName == _T("number"))) {
        SetNumberOnly(strValue == _T("true"));
    }
    else if (strName == _T("max_number")) {
        SetMaxNumber(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("min_number")) {
        SetMinNumber(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("number_format")) {
        SetNumberFormat64(strValue);
    }
    else if (strName == _T("text_align")) {
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
    }
    else if ((strName == _T("text_padding")) || (strName == _T("textpadding"))) {
        UiPadding rcTextPadding;
        AttributeUtil::ParsePaddingValue(strValue.c_str(), rcTextPadding);
        SetTextPadding(rcTextPadding, true);
    }
    else if ((strName == _T("text_color")) || (strName == _T("normal_text_color")) || (strName == _T("normaltextcolor"))) {
        SetTextColor(strValue);
    }
    else if ((strName == _T("disabled_text_color")) || (strName == _T("disabledtextcolor"))) {
        SetDisabledTextColor(strValue);
    }
    else if ((strName == _T("caret_color")) || (strName == _T("caretcolor"))) {
        //Set the caret color
        SetCaretColor(strValue);
    }
    else if ((strName == _T("prompt_mode")) || (strName == _T("promptmode"))) {
        //Prompt mode
        SetPromptMode(strValue == _T("true"));
    }
    else if ((strName == _T("prompt_color")) || (strName == _T("promptcolor"))) {
        //The color of the prompt text
        SetPromptTextColor(strValue);
    }
    else if ((strName == _T("prompt_text")) || (strName == _T("prompttext"))) {
        //Prompt text
        SetPromptText(strValue);
    }
    else if ((strName == _T("prompt_text_id")) || (strName == _T("prompt_textid")) || (strName == _T("prompttextid"))) {
        //Prompt text ID
        SetPromptTextId(strValue);
    }
    else if ((strName == _T("focused_image")) || (strName == _T("focusedimage"))) {
        SetFocusedImage(strValue);
    }
    else if (strName == _T("font")) {
        SetFontId(strValue);
    }
    else if (strName == _T("text")) {
        SetText(strValue);
    }
    else if ((strName == _T("text_id")) || (strName == _T("textid"))) {
        SetTextId(strValue);
    }
    else if ((strName == _T("want_tab")) || (strName == _T("wanttab"))) {
        SetWantTab(strValue == _T("true"));
    }
    else if ((strName == _T("want_return")) || (strName == _T("want_return_msg")) || (strName == _T("wantreturnmsg"))) {
        SetWantReturn(strValue == _T("true"));
    }
    else if ((strName == _T("want_ctrl_return")) || (strName == _T("return_msg_want_ctrl")) || (strName == _T("returnmsgwantctrl"))) {
        SetWantCtrlReturn(strValue == _T("true"));
    }
    else if ((strName == _T("limit_text")) || (strName == _T("max_char")) || (strName == _T("maxchar"))) {
        //Limit the maximum number of characters
        SetLimitText(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("limit_chars")) {
        //Restrict which characters are allowed to be entered
        SetLimitChars(strValue);
    }
    else if (strName == _T("word_wrap")) {
        //Whether to automatically wrap
        SetWordWrap(strValue == _T("true"));
    }
    else if (strName == _T("no_caret_readonly")) {
        //Do not show the caret in read-only mode
        SetNoCaretReadonly();
    }
    else if (strName == _T("default_context_menu")) {
        //Whether to use the default context menu
        SetEnableDefaultContextMenu(strValue == _T("true"));
    }
    else if (strName == _T("spin_class")) {
        SetSpinClass(strValue);
    }
    else if (strName == _T("clear_btn_class")) {
        SetClearBtnClass(strValue);
    }
    else if (strName == _T("show_passowrd_btn_class")) {
        SetShowPasswordBtnClass(strValue);
    }
    else if (strName == _T("wheel_zoom")) {
        //Set whether to allow adjusting the zoom ratio via Ctrl + mouse wheel
        SetEnableWheelZoom(strValue == _T("true"));
    }
    else if (strName == _T("hide_selection")) {
        //Whether to hide the selection when the control is inactive
        SetHideSelection(strValue == _T("true"));
    }
    else if (strName == _T("focus_bottom_border_size")) {
        //The size of the bottom border in the focused state
        SetFocusBottomBorderSize(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("focus_bottom_border_color")) {
        //The color of the bottom border in the focused state
        SetFocusBottomBorderColor(strValue);
    }
    else if (strName == _T("zoom")) {
        //Zoom ratio: there are two formats, one like "2,1" to zoom in to 200%; the other like "200%", which means zooming in to 200%.
        // "2,1" format sets the zoom ratio (compatible with Microsoft's RichEdit control format): wParam is the numerator of the zoom ratio, lParam is the denominator of the zoom ratio,
        //                         "wParam,lParam" means the zoom is displayed as numerator/denominator of the zoom ratio, value range: 1/64 < (wParam / lParam) < 64.
        //                         Examples: "0,0" means the zoom feature is disabled, "2,1" means zooming in to 200%, "1,2" means zooming out to 50%
        float fZoomRatio = 1.0f;
        if (strValue.find(L',') != DString::npos) {
            UiSize zoomValue;
            AttributeUtil::ParseSizeValue(strValue.c_str(), zoomValue);
            if ((zoomValue.cx > 0) && (zoomValue.cx <= 64) &&
                (zoomValue.cy > 0) && (zoomValue.cy <= 64)) {
                fZoomRatio = (float)zoomValue.cx / (float)zoomValue.cy;
            }
        }
        else if (strValue.find(L'%') != DString::npos) {
            DString zoomValue = strValue.substr(0, strValue.find(L'%'));
            int32_t nZoomValue = StringUtil::StringToInt32(zoomValue.c_str());
            ASSERT(nZoomValue > 0);
            if (nZoomValue > 0) {
                fZoomRatio = (float)nZoomValue / 100.0f;
            }
        }
        else {
            ASSERT(0);
        }
        uint32_t nZoomPercent = (uint32_t)(fZoomRatio * 100.0f);
        SetZoomPercent(nZoomPercent);
    }

    //These attributes are not supported
    else if ((strName == _T("auto_vscroll")) || (strName == _T("autovscroll"))) {
        //When the user presses ENTER on the last line, automatically scroll the text up one page.
    }
    else if ((strName == _T("auto_hscroll")) || (strName == _T("autohscroll"))) {
        //When the user types a character at the end of a line, automatically scroll the text right by 10 characters.
        //When the user presses Enter, the control scrolls all text back to the zero position.
    }
    else if ((strName == _T("rich_text")) || (strName == _T("rich"))) {
        //Whether it is the rich text attribute
    }
    else if (strName == _T("auto_detect_url")) {
        //Whether to automatically detect URLs; if it is a URL, it is displayed as a hyperlink
    }
    else if (strName == _T("allow_beep")) {
        //Whether a Beep sound is allowed
    }
    else if (strName == _T("save_selection")) {
        //If TRUE, the boundary of the selected content should be saved when the control is inactive.
        //If FALSE, the selection boundary can be reset to start = 0, length = 0 when the control becomes active again.
    }
    else if (strName == _T("select_all_on_focus")) {
        //Whether to select all when gaining focus
        SetSelAllOnFocus(strValue == _T("true"));
    }
    else if (strName == _T("selection_bkcolor")) {
        //The background color of the selected text (focused state); if set to empty, it is not shown
        SetSelectionBkColor(strValue);
    }
    else if (strName == _T("inactive_selection_bkcolor")) {
        //The background color of the selected text (non-focused state); if set to empty, it is not shown
        SetInactiveSelectionBkColor(strValue);
    }
    else if (strName == _T("current_row_bkcolor")) {
        //The background color of the current row (focused state); if set to empty, the current row background is not shown in the focused state
        SetCurrentRowBkColor(strValue);
    }
    else if (strName == _T("inactive_current_row_bkcolor")) {
        //The background color of the current row (non-focused state); if set to empty, the current row background is not shown in the non-focused state
        SetInactiveCurrentRowBkColor(strValue);
    }
    else if (strName == _T("row_spacing_mul")) {
        SetRowSpacingMul(StringUtil::StringToFloat(strValue.c_str(), nullptr));
    }
    else if (strName == _T("row_spacing_add")) {
        SetRowSpacingAdd(StringUtil::StringToFloat(strValue.c_str(), nullptr));
    }
    else {
        ScrollBox::SetAttribute(strName, strValue);
    }
}

void RichEdit::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    //Set the font and font color
    DString fontId = GetCurrentFontId();
    if (fontId.empty()) {
        fontId = GlobalManager::Instance().Font().GetDefaultFontId();
        SetFontId(fontId);
    }

    ScrollBar* pHScrollBar = GetHScrollBar();
    if (pHScrollBar != nullptr) {
        pHScrollBar->SetScrollRange(0);
    }
    ScrollBar* pVScrollBar = GetVScrollBar();
    if (pVScrollBar != nullptr) {
        pVScrollBar->SetScrollRange(0);
    }
}

void RichEdit::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    UiPadding rcTextPadding = GetTextPadding();
    rcTextPadding = Dpi().GetScalePadding(rcTextPadding, nOldDpiScale);
    SetTextPadding(rcTextPadding, false);

    //Update the font size
    SetFontIdInternal(GetCurrentFontId());

    //Clear the drawing cache and redraw
    ClearCacheAndRedraw();

    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

bool RichEdit::IsWantTab() const
{
    return m_bWantTab;
}

void RichEdit::SetWantTab(bool bWantTab)
{
    m_bWantTab = bWantTab;
}

bool RichEdit::CanPlaceCaptionBar() const
{
    return true;
}

bool RichEdit::IsWantReturn() const
{
    return m_bWantReturn;
}

void RichEdit::SetWantReturn(bool bWantReturn)
{
    m_bWantReturn = bWantReturn;
}

bool RichEdit::IsWantCtrlReturn() const
{
    return m_bWantCtrlReturn;
}

void RichEdit::SetWantCtrlReturn(bool bWantCtrlReturn)
{
    m_bWantCtrlReturn = bWantCtrlReturn;
}

bool RichEdit::IsReadOnly() const
{
    return m_bReadOnly;
}

void RichEdit::SetReadOnly(bool bReadOnly)
{
    if (m_bReadOnly != bReadOnly) {
        m_bReadOnly = bReadOnly;
        Redraw();
    }    
}

bool RichEdit::RemoveInvalidPasswordChar(DStringA& text)
{
    bool bRet = false;
    if (!text.empty()) {
        DStringA oldText = text;
        StringUtil::ReplaceAll("\r", "", text);
        StringUtil::ReplaceAll("\n", "", text);
        StringUtil::ReplaceAll("\t", "", text);
        if (oldText != text) {
            bRet = true;
        }
    }
    return bRet;
}

bool RichEdit::RemoveInvalidPasswordChar(DStringW& text)
{
    bool bRet = false;
    if (!text.empty()) {
        DStringW oldText = text;
        StringUtil::ReplaceAll(L"\r", L"", text);
        StringUtil::ReplaceAll(L"\n", L"", text);
        StringUtil::ReplaceAll(L"\t", L"", text);
        if (oldText != text) {
            bRet = true;
        }
    }
    return bRet;
}

void RichEdit::ReplacePasswordChar(DStringW& text) const
{
    const size_t nTextLen = text.size();
    if (!IsShowPassword() && (nTextLen > 0)) {
        DStringW oldText = text;
        text.clear();
        text.resize(nTextLen, m_chPasswordChar);
        if (IsFlashPasswordChar() && m_bInputPasswordChar) {
            //The last character: flash it
            text[nTextLen - 1] = oldText[nTextLen - 1];
        }
    }
}

int32_t RichEdit::GetTextLimitLength() const
{
    return GetLimitText();
}

void RichEdit::StopFlashPasswordChar()
{
    if (m_bInputPasswordChar) {
        m_bInputPasswordChar = false;
        Invalidate();
    }
}

bool RichEdit::IsPasswordMode() const
{
    return m_bPasswordMode;
}

void RichEdit::SetPasswordMode(bool bPasswordMode)
{
    if (m_bPasswordMode != bPasswordMode) {
        m_bPasswordMode = bPasswordMode;
        bool bUpdated = false;
        if (bPasswordMode) {
            //In password mode, remove the carriage return, newline, and TAB keys from the text
            DString text = GetText();
            if (RemoveInvalidPasswordChar(text)) {
                SetText(text);
                bUpdated = true;
            }
        }
        if (!bUpdated) {
            m_pTextData->SetCacheDirty(true);
            Redraw();
        }
    }
}

void RichEdit::SetShowPassword(bool bShow)
{
    if (m_bShowPassword != bShow) {
        m_bShowPassword = bShow;
        m_pTextData->SetCacheDirty(true);
        Redraw();
    }
}

bool RichEdit::IsShowPassword() const
{
    return m_bShowPassword;
}

void RichEdit::SetPasswordChar(DStringW::value_type ch)
{
    if (m_chPasswordChar != ch) {
        m_chPasswordChar = ch;
        if (IsPasswordMode()) {
            m_pTextData->SetCacheDirty(true);
            Redraw();
        }
    }
}

DStringW::value_type RichEdit::GetPasswordChar() const
{
    return m_chPasswordChar;
}

void RichEdit::SetFlashPasswordChar(bool bFlash)
{
    m_bFlashPasswordChar = bFlash;
}

bool RichEdit::IsFlashPasswordChar() const
{
    return m_bFlashPasswordChar;
}

bool RichEdit::IsNumberOnly() const
{
    return m_bNumberOnly;
}

void RichEdit::SetNumberOnly(bool bNumberOnly)
{
    //Only affects input, does not affect other ways of changing the text (e.g. via SetText)
    m_bNumberOnly = bNumberOnly;
}

void RichEdit::SetMaxNumber(int32_t maxNumber)
{
    m_maxNumber = maxNumber;
}

int32_t RichEdit::GetMaxNumber() const
{
    return m_maxNumber;
}

void RichEdit::SetMinNumber(int32_t minNumber)
{
    m_minNumber = minNumber;
}

int32_t RichEdit::GetMinNumber() const
{
    return m_minNumber;
}

void RichEdit::SetNumberFormat64(const DString& numberFormat)
{
    DString format = numberFormat;
#if defined (DUI_BUILD_FOR_WIN)
    StringUtil::ReplaceAll(_T("lld"), _T("I64d"), format);
#else
    StringUtil::ReplaceAll(_T("I64d"), _T("lld"), format);
#endif
    m_numberFormat = format;
}

DString RichEdit::GetNumberFormat64() const
{
    return m_numberFormat.c_str();
}

bool RichEdit::IsWordWrap() const
{
    return m_bWordWrap;
}

void RichEdit::SetWordWrap(bool bWordWrap)
{
    if (m_bWordWrap != bWordWrap) {
        m_bWordWrap = bWordWrap;
        //Clear the drawing cache and redraw
        ClearCacheAndRedraw();
    }
}

bool RichEdit::IsMultiLine() const
{
    return !m_pTextData->IsSingleLineMode();
}

void RichEdit::SetMultiLine(bool bMultiLine)
{
    bool bSingleLineMode = !bMultiLine;
    if (m_pTextData->IsSingleLineMode() != bSingleLineMode) {
        m_pTextData->SetSingleLineMode(bSingleLineMode);
        //Clear the drawing cache and redraw
        ClearCacheAndRedraw();
    }
}

void RichEdit::SetFontId(const DString& strFontId)
{
    if (m_sFontId != strFontId) {
        m_sFontId = strFontId;
        //If there is externally set font information, remove it (the font information set via SetFontInfo)
        DString internalFontId = GetInternalFontId();
        if (GlobalManager::Instance().Font().HasFontId(internalFontId)) {
            GlobalManager::Instance().Font().RemoveFontId(internalFontId);
        }
        OnFontChanged(strFontId);
    }
}

DString RichEdit::GetFontId() const
{
    return m_sFontId.c_str();
}

DString RichEdit::GetInternalFontId() const
{
    return StringUtil::Printf(_T("RichEdit_SDL(0x%p)"), this);
}

DString RichEdit::GetCurrentFontId() const
{
    DString fontId = GetInternalFontId();
    if (!GlobalManager::Instance().Font().HasFontId(fontId)) {
        fontId = GetFontId();
    }
    return fontId;
}

void RichEdit::SetFontIdInternal(const DString& fontId)
{
    //Create the caret
    IFont* pFont = GetIFontInternal(fontId);
    ASSERT(pFont != nullptr);
    if (pFont == nullptr) {
        return;
    }
    IRender* pRender = nullptr;
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        pRender = pWindow->GetRender();
    }
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }

    //Set the caret height based on the font height
    MeasureStringParam measureParam;
    measureParam.pFont = pFont;
    UiRect fontRect = pRender->MeasureString(_T("T"), measureParam);
    m_nRowHeight = fontRect.Height();
    ASSERT(m_nRowHeight > 0);
    int32_t nCaretHeight = fontRect.Height();
    int32_t nCaretWidth = Dpi().GetScaleInt(1);
    CreateCaret(nCaretWidth, nCaretHeight);

    //Set the basic unit of the scrollbar scrolling one line
    SetVerScrollUnitPixels(m_nRowHeight, false);

    m_nSelXPos = -1;
}

IFont* RichEdit::GetIFontInternal(const DString& fontId) const
{
    ASSERT(!fontId.empty());
    const DpiManager& dpi = Dpi();
    uint32_t nZoomPercent = GetZoomPercent();
    ASSERT(nZoomPercent != 0);
    dpi.ScaleInt(nZoomPercent);
    IFont* pFont = GlobalManager::Instance().Font().GetIFont(fontId, nZoomPercent);
    ASSERT(pFont != nullptr);
    return pFont;
}

UiFont RichEdit::GetFontInfo() const
{
    UiFont uiFont;
    IFont* pFont = GlobalManager::Instance().Font().GetIFont(GetCurrentFontId(), Dpi());
    if (pFont != nullptr) {
        uiFont.m_fontName = pFont->FontName();
        uiFont.m_fontSize = pFont->FontSize();
        uiFont.m_bBold = pFont->IsBold();
        uiFont.m_bUnderline = pFont->IsUnderline();
        uiFont.m_bItalic = pFont->IsItalic();
        uiFont.m_bStrikeOut = pFont->IsStrikeOut();
    }
    return uiFont;
}

bool RichEdit::SetFontInfo(const UiFont& fontInfo)
{
    ASSERT(fontInfo.m_fontSize > 0);
    if (fontInfo.m_fontSize <= 0) {
        return false;
    }
    ASSERT(!fontInfo.m_fontName.empty());
    if (fontInfo.m_fontName.empty()) {
        return false;
    }
    //Remove the old font information
    DString internalFontId = GetInternalFontId();
    if (GlobalManager::Instance().Font().HasFontId(internalFontId)) {
        GlobalManager::Instance().Font().RemoveFontId(internalFontId);
    }
    //Add the font information (remove DPI scaling)
    UiFont orgFontInfo = fontInfo;
    const DpiManager& dpi = Dpi();
    if (dpi.GetDisplayScaleFactor() > 0) {
        dpi.UnscaleInt(orgFontInfo.m_fontSize);
        if (dpi.GetScaleInt(orgFontInfo.m_fontSize) != fontInfo.m_fontSize) {
            //Calculate the original font size
            int32_t nFontSizeMax = fontInfo.m_fontSize;
            if (dpi.GetDisplayScaleFactor() < 100) {
                nFontSizeMax = dpi.GetUnscaleInt(fontInfo.m_fontSize) + 1;
            }
            for (int32_t nFontSize = 1; nFontSize <= nFontSizeMax; ++nFontSize) {
                int32_t nScaledFontSize = dpi.GetScaleInt(nFontSize);
                if (nScaledFontSize == fontInfo.m_fontSize) {
                    orgFontInfo.m_fontSize = nFontSize;
                    break;
                }
            }
        }
    }
    GlobalManager::Instance().Font().AddFont(internalFontId, orgFontInfo, false);

    OnFontChanged(internalFontId);
    return true;
}

void RichEdit::OnFontChanged(const DString& fontId)
{
    SetFontIdInternal(fontId);
    //Clear the drawing cache and redraw
    ClearCacheAndRedraw();
}

void RichEdit::OnZoomPercentChanged(uint32_t nOldZoomPercent, uint32_t nNewZoomPercent)
{
    //Remove the old font cache to free memory
    if (nOldZoomPercent != 100) {
        uint32_t nZoomPercent = Dpi().GetScaleInt(nOldZoomPercent);
        DString internalFontId = GetInternalFontId();
        if (GlobalManager::Instance().Font().HasFontId(internalFontId)) {
            GlobalManager::Instance().Font().RemoveIFont(internalFontId, nZoomPercent);
        }
        DString fontId = GetFontId();
        if (GlobalManager::Instance().Font().HasFontId(fontId)) {
            GlobalManager::Instance().Font().RemoveIFont(fontId, nZoomPercent);
        }
    }

    SetFontIdInternal(GetCurrentFontId());

    //Clear the drawing cache and redraw
    ClearCacheAndRedraw();

    //Trigger the kEventZoom event
    SendEvent(kEventZoom, (WPARAM)nNewZoomPercent, 0);
}

void RichEdit::SetTextColor(const DString& dwTextColor)
{
    if (m_sTextColor != dwTextColor) {
        m_sTextColor = dwTextColor;
        if (IsEnabled()) {
            Redraw();
        }
    }    
}

DString RichEdit::GetTextColor() const
{
    if (!m_sTextColor.empty()) {
        return m_sTextColor.c_str();
    }
    else {
        return GlobalManager::Instance().Color().GetDefaultTextColor();
    }
}

void RichEdit::SetDisabledTextColor(const DString& dwTextColor)
{
    if (m_sDisabledTextColor != dwTextColor) {
        m_sDisabledTextColor = dwTextColor;
        if (!IsEnabled()) {
            Redraw();
        }
    }    
}

DString RichEdit::GetDisabledTextColor() const
{
    if (!m_sDisabledTextColor.empty()) {
        return m_sDisabledTextColor.c_str();
    }
    else if (!m_sTextColor.empty()) {
        return m_sTextColor.c_str();
    }
    else {
        return GlobalManager::Instance().Color().GetDefaultDisabledTextColor();
    }
}

void RichEdit::SetSelectionBkColor(const DString& selectionBkColor)
{
    if (m_sSelectionBkColor != selectionBkColor) {
        m_sSelectionBkColor = selectionBkColor;
        Invalidate();
    }
}

DString RichEdit::GetSelectionBkColor() const
{
    return m_sSelectionBkColor.c_str();
}

void RichEdit::SetInactiveSelectionBkColor(const DString& selectionBkColor)
{
    if (m_sInactiveSelectionBkColor != selectionBkColor) {
        m_sInactiveSelectionBkColor = selectionBkColor;
        Invalidate();
    }
}

DString RichEdit::GetInactiveSelectionBkColor() const
{
    return m_sInactiveSelectionBkColor.c_str();
}

void RichEdit::SetCurrentRowBkColor(const DString& currentRowBkColor)
{
    m_sCurrentRowBkColor = currentRowBkColor;
}

DString RichEdit::GetCurrentRowBkColor() const
{
    return m_sCurrentRowBkColor.c_str();
}

void RichEdit::SetInactiveCurrentRowBkColor(const DString& currentRowBkColor)
{
    m_sInactiveCurrentRowBkColor = currentRowBkColor;
}

DString RichEdit::GetInactiveCurrentRowBkColor() const
{
    return m_sInactiveCurrentRowBkColor.c_str();
}

float RichEdit::GetRowSpacingMul() const
{
    return m_fRowSpacingMul;
}

void RichEdit::SetRowSpacingMul(float fRowSpacingMul)
{
    if (m_fRowSpacingMul != fRowSpacingMul) {
        m_fRowSpacingMul = fRowSpacingMul;
        if (m_fRowSpacingMul <= 0.01f) {
            m_fRowSpacingMul = 1.0f;
        }
        //Clear the drawing cache and redraw
        ClearCacheAndRedraw();
    }
}

float RichEdit::GetRowSpacingAdd() const
{
    return m_fRowSpacingAdd;
}

void RichEdit::SetRowSpacingAdd(float fRowSpacingAdd)
{
    if (m_fRowSpacingAdd != fRowSpacingAdd) {
        m_fRowSpacingAdd = fRowSpacingAdd;
        if (m_fRowSpacingAdd <= 0.0001f) {
            m_fRowSpacingAdd = 0.0f;
        }
        //Clear the drawing cache and redraw
        ClearCacheAndRedraw();
    }
}

int32_t RichEdit::GetLimitText() const
{
    return m_nLimitText;
}

void RichEdit::SetLimitText(int32_t iChars)
{
    if (iChars < 0) {
        iChars = 0;
    }
    m_nLimitText = iChars;
    if (m_nLimitText > 0) {
        //There is a limit
        int32_t nTextLen = GetTextLength();
        if (nTextLen > m_nLimitText) {
            //Truncate the current text
            DStringW text = m_pTextData->GetText();
            m_pTextData->TruncateLimitText(text, m_nLimitText);
            SetText(text);
        }
    }    
}

DString RichEdit::GetLimitChars() const
{
    if (m_pLimitChars != nullptr) {
        return StringConvert::WStringToT(m_pLimitChars.get());
    }
    else {
        return DString();
    }
}

void RichEdit::SetLimitChars(const DString& limitChars)
{
    m_pLimitChars.reset();
    DStringW limitCharsW = StringConvert::TToWString(limitChars);
    if (!limitCharsW.empty()) {
        size_t nLen = limitCharsW.size() + 1;
        m_pLimitChars.reset(new DStringW::value_type[nLen]);
        memset(m_pLimitChars.get(), 0, nLen * sizeof(DStringW::value_type));
        StringUtil::StringCopy(m_pLimitChars.get(), nLen, limitCharsW.c_str());
    }
}

int32_t RichEdit::GetTextLength() const
{
    return (int32_t)m_pTextData->GetTextLength();
}

int32_t RichEdit::InsertText(int32_t nInsertAfterChar, const DString& text, bool bCanUndo)
{
    int32_t nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
    ReplaceSel(text, bCanUndo);
    return nRet;
}

int32_t RichEdit::AppendText(const DString& text, bool bCanUndo, bool bScrollBottom)
{
    int32_t nRet = SetSel(-1, -1);
    ReplaceSel(text, bCanUndo);
    if (bScrollBottom) {
        int64_t nScrollRangeY = GetScrollRange().cy;
        if (nScrollRangeY > 0) {
            SetScrollPosY(nScrollRangeY);
        }
    }
    return nRet;
}

bool RichEdit::IsEmpty() const
{
    return m_pTextData->IsEmpty();
}

DString RichEdit::GetText() const
{
    
#ifdef DUI_UNICODE
    return m_pTextData->GetText();
#else
    return StringConvert::WStringToUTF8(m_pTextData->GetText());
#endif
}

DStringA RichEdit::GetTextA() const
{
    return StringConvert::WStringToUTF8(m_pTextData->GetText());
}

DStringW RichEdit::GetTextW() const
{
    return m_pTextData->GetText();
}

void RichEdit::SetText(const DStringW& strText)
{
    //Current memory usage: for 2MB of UTF16 text, the Debug version uses about 23MB of memory, and the Release version uses about 12MB of memory.
    bool bChanged = false;
    if (IsPasswordMode()) {
        //Password mode
        DStringW passwordText = strText;
        RemoveInvalidPasswordChar(passwordText);
        bChanged = m_pTextData->SetText(passwordText);
    }
    else {
        bChanged = m_pTextData->SetText(strText);
    }
    if (bChanged && IsInited()) {
        //Recalculate the character area
        Redraw();

        //When the text changes, move the selection point to the end of the text
        int32_t nTextLen = (int32_t)m_pTextData->GetTextLength();
        InternalSetSel(nTextLen, nTextLen);

        UpdateScrollRange();
        OnTextChanged();
    }
}

void RichEdit::SetText(const DStringA& strText)
{
    //Current memory usage: for 2MB of UTF16 text, the Debug version uses about 23MB of memory, and the Release version uses about 12MB of memory.
    bool bChanged = false;
    DStringW text = StringConvert::UTF8ToWString(strText);
    if (IsPasswordMode()) {
        //Password mode
        DStringW passwordText = text;
        RemoveInvalidPasswordChar(passwordText);
        bChanged = m_pTextData->SetText(passwordText);
    }
    else {
        bChanged = m_pTextData->SetText(text);
    }
    if (bChanged && IsInited()) {
        //Recalculate the character area
        Redraw();

        //When the text changes, move the selection point to the end of the text
        int32_t nTextLen = (int32_t)m_pTextData->GetTextLength();
        InternalSetSel(nTextLen, nTextLen);

        UpdateScrollRange();
        OnTextChanged();
    }
}

void RichEdit::SetTextNoEvent(const DString& strText)
{
    bool bOldValue = m_bDisableTextChangeEvent;
    m_bDisableTextChangeEvent = true;
    SetText(strText);
    m_bDisableTextChangeEvent = bOldValue;
}

void RichEdit::SetTextId(const DString& strTextId)
{
    DString strText = GlobalManager::Instance().Lang().GetStringViaID(strTextId);
    SetText(strText);
}

bool RichEdit::GetModify() const
{ 
    return m_bModified;
}

void RichEdit::SetModify(bool bModified)
{ 
    m_bModified = bModified;
}

void RichEdit::GetSel(int32_t& nStartChar, int32_t& nEndChar) const
{
    const int32_t nTextLen = GetTextLength();
    nStartChar = m_nSelStartIndex;
    nEndChar = m_nSelEndCharIndex;

    ASSERT((nStartChar <= nTextLen) && (nStartChar >= 0));
    ASSERT((nEndChar <= nTextLen) && (nEndChar >= 0));
    if ((nStartChar < 0) || (nStartChar > nTextLen) || (nEndChar < 0) || (nEndChar > nTextLen)) {
        nStartChar = nTextLen;
        nEndChar = nTextLen;
    }
}

int32_t RichEdit::SetSel(int32_t nStartChar, int32_t nEndChar)
{
    m_nSelXPos = -1;
    int32_t nSelStartChar = InternalSetSel(nStartChar, nEndChar);
    //Ensure the character at the end of the selection is visible
    EnsureCharVisible(nEndChar);
    return nSelStartChar;
}

int32_t RichEdit::InternalSetSel(int32_t nStartChar, int32_t nEndChar)
{
    if (nStartChar < 0) {
        nStartChar = -1;
    }
    if (nEndChar < 0) {
        nEndChar = -1;
    }

    const int32_t nTextLen = GetTextLength();
    if (nTextLen <= 0) {
        //No selected text
        nStartChar = 0;
        nEndChar = 0;
    }
    else if ((nStartChar == -1) && (nEndChar == 0)) {
        //No selected text, get the original caret position
        nStartChar = m_nSelEndCharIndex;
        nEndChar = m_nSelEndCharIndex;
        if (nStartChar >= nTextLen) {
            nStartChar = nTextLen;
            nEndChar = nTextLen;
        }
    }
    else {
        if (nStartChar < 0) {
            nStartChar = nTextLen;
        }
        if (nEndChar < 0) {
            nEndChar = nTextLen;
        }
        if (nStartChar > nEndChar) {
            std::swap(nStartChar, nEndChar);
        }
        ASSERT(nEndChar >= nStartChar);
        if (nStartChar >= nTextLen) {
            nStartChar = nTextLen;
        }
        if (nEndChar >= nTextLen) {
            nEndChar = nTextLen;
        }
        if (nStartChar == nEndChar) {
            //No selected text
        }
        else if (nStartChar >= nTextLen) {
            //No selected text
            nStartChar = nTextLen;
            nEndChar = nTextLen;
        }
        else {
            //There is selected text, select the text range, hide the caret
            ASSERT(nEndChar > nStartChar);
        }
    }

    ASSERT((nStartChar <= nTextLen) && (nStartChar >= 0));
    ASSERT((nEndChar <= nTextLen) && (nEndChar >= 0));

    bool bChanged = (m_nSelStartIndex != nStartChar) || (m_nSelEndCharIndex != nEndChar);
    m_nSelStartIndex = nStartChar;
    m_nSelEndCharIndex = nEndChar;

    //Adjust the caret position based on the selected text, and control whether the caret is shown or hidden
    SetCaretPos(nEndChar);

    if (bChanged) {
        //Redraw (but do not call the Redraw() function to avoid clearing the drawing cache)
        Invalidate();

        //Trigger the text selection change event
        SendEvent(kEventSelChanged);
    }
    return nStartChar;
}

void RichEdit::EnsureCharVisible(int32_t nCharIndex)
{
    UiRect rcDrawRect = GetTextDrawRect(GetRect());
    if (rcDrawRect.IsEmpty()) {
        return;
    }
    UiPoint pt = PosFromChar(nCharIndex);    
    if (!rcDrawRect.ContainsPt(pt)) {
        if (pt.y < rcDrawRect.top) {
            //Scroll up
            UiSize64 scrollPos = GetScrollPos();
            scrollPos.cy -= (rcDrawRect.top - pt.y);
            SetScrollPos(scrollPos);
        }
        else if (pt.y >= rcDrawRect.bottom) {
            //Scroll down
            UiSize64 scrollPos = GetScrollPos();
            scrollPos.cy += (pt.y - rcDrawRect.bottom);
            scrollPos.cy += m_nRowHeight;
            SetScrollPos(scrollPos);
        }

        if (pt.x < rcDrawRect.left) {
            //Scroll left
            UiSize64 scrollPos = GetScrollPos();
            scrollPos.cx -= (rcDrawRect.left - pt.x);
            SetScrollPos(scrollPos);
        }
        else if (pt.x >= rcDrawRect.right) {
            //Scroll right
            UiSize64 scrollPos = GetScrollPos();
            scrollPos.cx += (pt.x - rcDrawRect.right);
            scrollPos.cx += m_pTextData->GetCharWidthValue(nCharIndex);
            SetScrollPos(scrollPos);
        }
    }
}

bool RichEdit::FindRichText(const FindTextParam& findParam, TextCharRange& chrgText) const
{
    DStringW findText = StringConvert::TToWString(findParam.findText);
    if (findText.empty()) {
        return false;
    }
    int32_t nFoundStartChar = -1;
    int32_t nFoundEndChar = -1;
    bool bRet = m_pTextData->FindRichText(findParam.bMatchCase, findParam.bMatchWholeWord, findParam.bFindDown,
                                          findParam.chrg.cpMin, findParam.chrg.cpMax, findText,
                                          nFoundStartChar, nFoundEndChar);
    chrgText.cpMin = nFoundStartChar;
    chrgText.cpMax = nFoundEndChar;
    return bRet;
}

bool RichEdit::IsRichText() const
{
    return false;
}

DString RichEdit::GetSelectionTextColor() const
{
    ASSERT(0);
    return DString();
}

void RichEdit::SetSelectionTextColor(const DString& /*textColor*/)
{
    ASSERT(0);
}

bool RichEdit::ReplaceSel(const DString& newText, bool bCanUndo)
{
    if (IsReadOnly() || !IsEnabled()) {
        //In read-only or Disable state, editing is forbidden
        return false;
    }
    int32_t nStartChar = -1;
    int32_t nEndChar = -1;
    GetSel(nStartChar, nEndChar);
    bool bRet = false;
    DStringW text = StringConvert::TToWString(newText);
    if (IsPasswordMode()) {        
        RemoveInvalidPasswordChar(text);
        bRet = m_pTextData->ReplaceText(nStartChar, nEndChar, text, false);
    }
    else {
        bRet = m_pTextData->ReplaceText(nStartChar, nEndChar, text, bCanUndo);
    }
    if (bRet) {
        nEndChar = nStartChar + (int32_t)text.size();
        SetSel(nStartChar, nEndChar);
        UpdateScrollRange();
        OnTextChanged();
    }    
    return bRet;
}

DString RichEdit::GetSelText() const
{
    int32_t nStartChar = -1;
    int32_t nEndChar = -1;
    GetSel(nStartChar, nEndChar);
    DStringW text = m_pTextData->GetTextRange(nStartChar, nEndChar);
#ifdef DUI_UNICODE
    return text;
#else
    return StringConvert::WStringToUTF8(text);
#endif
}

bool RichEdit::HasSelText() const
{
    ASSERT((int32_t)m_pTextData->GetText().size() == GetTextLength());

    int32_t nSelStartChar = -1;
    int32_t nSelEndChar = -1;
    GetSel(nSelStartChar, nSelEndChar);
    return (nSelEndChar - nSelStartChar) > 0 ? true : false;
}

int32_t RichEdit::SetSelAll()
{
    return SetSel(0, -1);
}

void RichEdit::SetSelNone()
{
    SetSel(-1, 0);
}

DString RichEdit::GetTextRange(int32_t nStartChar, int32_t nEndChar) const
{
    return StringConvert::WStringToT(m_pTextData->GetTextRange(nStartChar, nEndChar));
}

void RichEdit::HideSelection(bool bHideSelection)
{
    SetHideSelection(bHideSelection);
}

void RichEdit::SetHideSelection(bool bHideSelection)
{
    if (m_bHideSelection != bHideSelection) {
        m_bHideSelection = bHideSelection;
        Invalidate();
    }
}

bool RichEdit::IsHideSelection() const
{
    return m_bHideSelection;
}

void RichEdit::SetFocusBottomBorderSize(int32_t nBottomBorderSize)
{
    ASSERT(nBottomBorderSize > 0);
    if (nBottomBorderSize < 0) {
        nBottomBorderSize = 0;
    }
    m_nFocusBottomBorderSize = ui::TruncateToUInt8(nBottomBorderSize);
}

int32_t RichEdit::GetFocusBottomBorderSize() const
{
    return (int32_t)(uint32_t)m_nFocusBottomBorderSize;
}

void RichEdit::SetFocusBottomBorderColor(const DString& bottomBorderColor)
{
    m_sFocusBottomBorderColor = bottomBorderColor;
}

DString RichEdit::GetFocusBottomBorderColor() const
{
    return m_sFocusBottomBorderColor.c_str();
}

bool RichEdit::CanRedo() const
{
    if (IsReadOnly() || !IsEnabled()) {
        return false;
    }
    return m_pTextData->CanRedo();
}

bool RichEdit::Redo()
{
    if (!CanRedo()) {
        return false;
    }
    m_nSelXPos = -1;
    int32_t nEndCharIndex = 0;
    bool bRet = m_pTextData->Redo(nEndCharIndex);
    if (bRet) {
        SetSel(nEndCharIndex, nEndCharIndex);
        OnTextChanged();
    }
    return bRet;
}

bool RichEdit::CanUndo() const
{
    if (IsReadOnly() || !IsEnabled()) {
        return false;
    }
    return m_pTextData->CanUndo();
}

bool RichEdit::Undo()
{
    if (!CanUndo()) {
        return false;
    }
    m_nSelXPos = -1;
    int32_t nEndCharIndex = 0;
    bool bRet = m_pTextData->Undo(nEndCharIndex);
    if (bRet) {
        SetSel(nEndCharIndex, nEndCharIndex);
        OnTextChanged();
    }
    return bRet;
}

void RichEdit::Clear()
{
    if (IsReadOnly() || !IsEnabled() || !HasSelText()) {
        return;
    }
    
    int32_t nStartChar = -1;
    int32_t nEndChar = -1;
    GetSel(nStartChar, nEndChar);
    if (nEndChar > nStartChar) {
        bool bCanUndo = !IsPasswordMode();
        bool bRet = m_pTextData->ReplaceText(nStartChar, nEndChar, L"", bCanUndo);
        if (bRet) {
            SetSel(nStartChar, nStartChar);
            m_nSelXPos = -1;
            Invalidate();
            UpdateScrollRange();
            OnTextChanged();
        }
    }
}

void RichEdit::Copy()
{
    if (IsPasswordMode()) {
        //Copying is not supported in password mode
        return;
    }
    int32_t nStartChar = -1;
    int32_t nEndChar = -1;
    GetSel(nStartChar, nEndChar);
    DStringW text = m_pTextData->GetTextRange(nStartChar, nEndChar);
    Clipboard::SetClipboardText(text);
}

void RichEdit::Cut()
{
    if (IsReadOnly() || !IsEnabled() || IsPasswordMode()) {
        //Cutting is not supported in read-only mode, password mode, or Disable state
        return;
    }

    int32_t nStartChar = -1;
    int32_t nEndChar = -1;
    GetSel(nStartChar, nEndChar);
    DStringW text = m_pTextData->GetTextRange(nStartChar, nEndChar);
    if (!text.empty()) {
        Clipboard::SetClipboardText(text);
        bool bCanUndo = !IsPasswordMode();
        bool bRet = m_pTextData->ReplaceText(nStartChar, nEndChar, L"", bCanUndo);
        if (bRet) {
            SetSel(nStartChar, nStartChar);
            m_nSelXPos = -1;
            Invalidate();
            UpdateScrollRange();
            OnTextChanged();
        }
    }
}

void RichEdit::Paste()
{
    if (!CanPaste()) {
        return;
    }
    DStringW text;
    Clipboard::GetClipboardText(text);
    if (IsPasswordMode()) {
        RemoveInvalidPasswordChar(text);
    }
    if (!text.empty()) {
        int32_t nStartChar = -1;
        int32_t nEndChar = -1;
        GetSel(nStartChar, nEndChar);
        bool bCanUndo = !IsPasswordMode();
        bool bRet = m_pTextData->ReplaceText(nStartChar, nEndChar, text, bCanUndo);
        if (bRet) {
            int32_t nNewSel = nStartChar + (int32_t)text.size();
            SetSel(nNewSel, nNewSel);
            m_nSelXPos = -1;
            Invalidate();
            UpdateScrollRange();
            OnTextChanged();
        }        
    }
}

bool RichEdit::CanPaste() const
{
    if (IsReadOnly() || !IsEnabled()) {
        return false;
    }
    if (IsPasteLimited()) {
        return false;
    }
    return true;
}

int32_t RichEdit::GetLineCount() const
{
    return m_pTextData->GetRowCount();
}

DString RichEdit::GetLine(int32_t nIndex, int32_t nMaxLength) const
{
    if (nMaxLength < 1) {
        return DString();
    }
    DStringW rowText = m_pTextData->GetRowText(nIndex);
    if ((int32_t)rowText.size() > nMaxLength) {
        rowText.resize((size_t)nMaxLength);
    }
    return StringConvert::WStringToT(rowText);
}

int32_t RichEdit::LineIndex(int32_t nLine) const
{
    return m_pTextData->RowIndex(nLine);
}

int32_t RichEdit::LineLength(int32_t nLine) const
{
    return m_pTextData->RowLength(nLine);
}

bool RichEdit::LineScroll(int32_t nLines)
{
    int32_t nCharIndex = m_pTextData->RowIndex(nLines);
    if (nCharIndex < 0) {
        return false;
    }
    UiPoint pt = PosFromChar(nCharIndex);
    UiRect rcDrawRect = GetTextDrawRect(GetRect());
    if (!rcDrawRect.ContainsPt(pt)) {
        if (pt.y < rcDrawRect.top) {
            //Scroll up
            UiSize64 scrollPos = GetScrollPos();
            scrollPos.cy -= (rcDrawRect.top - pt.y);
            SetScrollPos(scrollPos);
        }
        else if (pt.y >= rcDrawRect.bottom) {
            //Scroll down
            UiSize64 scrollPos = GetScrollPos();
            scrollPos.cy += (pt.y - rcDrawRect.bottom);
            scrollPos.cy += m_nRowHeight;
            SetScrollPos(scrollPos);
        }
    }
    return true;
}

int32_t RichEdit::LineFromChar(int32_t nIndex) const
{
    return m_pTextData->RowFromChar(nIndex);
}

void RichEdit::EmptyUndoBuffer()
{
    m_pTextData->EmptyUndoBuffer();
}

void RichEdit::SetUndoLimit(uint32_t nLimit)
{
    m_pTextData->SetUndoLimit(nLimit);
}

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
HWND RichEdit::GetWindowHWND() const
{
    auto window = GetWindow();
    return window ? window->NativeWnd()->GetHWND() : nullptr;
}

#endif

void RichEdit::OnScrollOffsetChanged(const UiSize& /*oldScrollOffset*/, const UiSize& newScrollOffset)
{
    //After the scrollbar position changes, redrawing is needed, but recalculation is not, to avoid affecting the drawing speed
    Invalidate();

    m_pTextData->SetScrollOffset(newScrollOffset);
}

void RichEdit::SetWindow(Window* pWindow)
{
    BaseClass::SetWindow(pWindow);
    if (pWindow != nullptr) {
        IRender* pRender = pWindow->GetRender();
        ASSERT(pRender != nullptr);
        m_pTextData->SetRender(pRender);
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        m_pTextData->SetRenderFactory(pRenderFactory);
    }
    else {
        m_pTextData->SetRender(nullptr);
        m_pTextData->SetRenderFactory(nullptr);
    }
}

void RichEdit::LineUp(int32_t deltaValue)
{
    BaseClass::LineUp(deltaValue);
}

void RichEdit::LineDown(int32_t deltaValue)
{
    BaseClass::LineDown(deltaValue);
}

void RichEdit::PageUp()
{
    UiSize64 sz = GetScrollPos();
    int32_t iOffset = GetPageScrollDeltaValue(false);
    sz.cy -= iOffset;
    SetScrollPos(sz);
}

void RichEdit::PageDown()
{
    UiSize64 sz = GetScrollPos();
    int32_t iOffset = GetPageScrollDeltaValue(true);
    sz.cy += iOffset;
    SetScrollPos(sz);
}

void RichEdit::HomeUp()
{
    BaseClass::HomeUp();
}

void RichEdit::EndDown(bool arrange)
{
    BaseClass::EndDown(arrange);
}

void RichEdit::LineLeft(int32_t deltaValue)
{
    BaseClass::LineLeft(deltaValue);
}

void RichEdit::LineRight(int32_t deltaValue)
{
    BaseClass::LineRight(deltaValue);
}

void RichEdit::PageLeft()
{
    BaseClass::PageLeft();
}

void RichEdit::PageRight()
{
    BaseClass::PageRight();
}

void RichEdit::HomeLeft()
{
    BaseClass::HomeLeft();
}

void RichEdit::EndRight()
{
    BaseClass::EndRight();
}

void RichEdit::OnSetEnabled(bool bChanged)
{
    BaseClass::OnSetEnabled(bChanged);
    if (IsEnabled()) {
        SetState(kControlStateNormal);
    }
    else {
        SetState(kControlStateDisabled);
    }
    if (bChanged) {
        Redraw();
    }
}

uint32_t RichEdit::GetControlFlags() const
{
    return IsEnabled() && IsAllowTabStop() ? UIFLAG_TABSTOP : UIFLAG_DEFAULT;
}

bool RichEdit::IsInLimitChars(DStringW::value_type charValue) const
{
    //When false is returned: input is forbidden
    if (m_pLimitChars == nullptr) {
        return true;
    }
    const DStringW::value_type* ch = m_pLimitChars.get();
    if ((ch == nullptr) || (*ch == L'\0')) {
        return true;
    }
    bool bInLimitChars = false;
    while (*ch != L'\0') {
        if (*ch == charValue) {
            bInLimitChars = true;
            break;
        }
        ++ch;
    }
    return bInLimitChars;
}

bool RichEdit::IsPasteLimited() const
{
    if (m_pLimitChars != nullptr) {
        //Restricted characters have been set
        DStringW strClipText;
        Clipboard::GetClipboardText(strClipText);
        if (!strClipText.empty()) {
            size_t count = strClipText.size();
            for (size_t index = 0; index < count; ++index) {
                if (strClipText[index] == L'\0') {
                    break;
                }
                if (!IsInLimitChars(strClipText[index])) {
                    //Some characters are not in the list, pasting is forbidden
                    return true;
                }
            }
        }
    }
    else if (IsNumberOnly()) {
        //Number mode
        DStringW strClipText;
        Clipboard::GetClipboardText(strClipText);
        if (!strClipText.empty()) {
            size_t count = strClipText.size();
            for (size_t index = 0; index < count; ++index) {
                if (strClipText[index] == L'\0') {
                    break;
                }
                if (strClipText[index] == L'-') {
                    if ((index == 0) && (strClipText.size() > 1)) {
                        //Allow the first character to be a minus sign
                        continue;
                    }
                }
                if ((strClipText[index] > L'9') || (strClipText[index] < L'0')) {
                    //There are non-number characters, pasting is forbidden
                    return true;
                }
            }
        }
    }
    return false;
}

void RichEdit::Paint(IRender* pRender, const UiRect& rcPaint)
{
    if (pRender == nullptr) {
        return;
    }
    
    bool bNeedPaint = true;
    if (pRender->IsClipEmpty()) {
        bNeedPaint = false;
    }    
    UiRect rcTemp; //The dirty area within this control, the area to be drawn this time
    if (!UiRect::Intersect(rcTemp, rcPaint, GetBoxShadowExpandedRect(GetRect()))) {//If the area containing box-shadow is a dirty area, drawing is required
        bNeedPaint = false;
    }

    if (!bNeedPaint) {
        return;
    }

    Control::Paint(pRender, rcPaint);

    UiRect rcDrawText = GetTextDrawRect(GetRect());
    if (rcDrawText.IsEmpty()) {
        //The drawing area is empty, do not draw
        return;
    }

    //Check and recalculate the text area as needed
    m_pTextData->CheckCalcTextRects();

    //Draw the background color of the current editing row
    if (!IsReadOnly() && IsEnabled()) {
        if (IsEmpty()) {
            //The text is empty, only draw when focused
            if (m_bActive) {
                PaintCurrentRowBkColor(pRender, rcPaint);
            }
        }
        else {
            //The text is not empty, always draw
            PaintCurrentRowBkColor(pRender, rcPaint);
        }
    }

    //Draw the selection background color
    PaintSelectionColor(pRender, rcPaint);

    //Draw the text
    if (!IsPasswordMode()) {
        //Non-password mode, draw using the drawing cache
        std::vector<RichTextData> richTextDataList;
        GetRichTextForDraw(richTextDataList);

        std::shared_ptr<DrawRichTextCache> spDrawRichTextCache = m_pTextData->GetDrawRichTextCache();
        if (spDrawRichTextCache != nullptr) {
            //Check whether the cache is invalid
            if (!pRender->IsValidDrawRichTextCache(rcDrawText, richTextDataList, spDrawRichTextCache)) {
                spDrawRichTextCache.reset();
                m_pTextData->ClearDrawRichTextCache();
            }
        }

        //Draw the text
        UiSize szScrollOffset = GetScrollOffset();
        if (spDrawRichTextCache != nullptr) {
            //Draw through the cache
            rcDrawText.Offset(0, m_pTextData->GetTextRectOfssetY());
            pRender->DrawRichTextCacheData(spDrawRichTextCache, rcDrawText, szScrollOffset, m_pTextData->GetTextRowXOffset(), GetAlpha());
        }
        else if (!richTextDataList.empty()) {
            spDrawRichTextCache.reset();

            IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
            ASSERT(pRenderFactory != nullptr);
            pRender->CreateDrawRichTextCache(rcDrawText, szScrollOffset, pRenderFactory, richTextDataList, spDrawRichTextCache);
            ASSERT(spDrawRichTextCache != nullptr);
            if (spDrawRichTextCache != nullptr) {
                ASSERT(pRender->IsValidDrawRichTextCache(rcDrawText, richTextDataList, spDrawRichTextCache));
                //Draw through the cache
                rcDrawText.Offset(0, m_pTextData->GetTextRectOfssetY());
                pRender->DrawRichTextCacheData(spDrawRichTextCache, rcDrawText, szScrollOffset, m_pTextData->GetTextRowXOffset(), GetAlpha());
                m_pTextData->SetDrawRichTextCache(spDrawRichTextCache);
            }
        }
    }
    else {
        //Password mode, do not use the drawing cache
        DStringW passwordText = m_pTextData->GetText();
        //Handle the display characters
        ReplacePasswordChar(passwordText);

        DString fontId = GetCurrentFontId();
        ASSERT(!fontId.empty());
        if (!passwordText.empty() && !fontId.empty()) {
            UiRect rcDrawRect = GetRichTextDrawRect();
            UiColor dwClrColor = GetUiColor(GetTextColor());
            if (!IsEnabled()) {
                dwClrColor = GetUiColor(GetDisabledTextColor());
            }
            ASSERT(!dwClrColor.IsEmpty());

            DrawStringParam drawParam;
            drawParam.pFont = GetIFontInternal(fontId);
            drawParam.uFormat = GetTextStyle();
            drawParam.textRect = rcDrawRect;
            drawParam.dwTextColor = dwClrColor;

#ifdef DUI_UNICODE
            pRender->DrawString(passwordText, drawParam);
#else
            pRender->DrawString(StringConvert::WStringToUTF8(passwordText), drawParam);
#endif
            
        }
    }

    //Draw the caret
    PaintCaret(pRender, rcPaint);
}

void RichEdit::PaintChild(IRender* pRender, const UiRect& rcPaint)
{
    UiRect rcTemp;
    if (!UiRect::Intersect(rcTemp, rcPaint, GetRect())) {
        return;
    }

    ScrollBar* pVScrollBar = GetVScrollBar();
    ScrollBar* pHScrollBar = GetHScrollBar();
    if (m_items.size() > 0) {
        UiRect rc = GetRectWithoutPadding();
        if ((pVScrollBar != nullptr) && pVScrollBar->IsValid()) {
            rc.right -= pVScrollBar->GetFixedWidth().GetInt32();
        }
        if ((pHScrollBar != nullptr) && pHScrollBar->IsValid()) {
            rc.bottom -= pHScrollBar->GetFixedHeight().GetInt32();
        }

        if (!UiRect::Intersect(rcTemp, rcPaint, rc)) {
            for (auto it = m_items.begin(); it != m_items.end(); ++it) {
                auto pControl = *it;
                if ((pControl == nullptr) || !pControl->IsVisible()) {
                    continue;
                }
                UiRect controlPos = pControl->GetPos();
                if (!UiRect::Intersect(rcTemp, rcPaint, controlPos)) {
                    continue;
                }
                if (pControl->IsFloat()) {
                    if (!UiRect::Intersect(rcTemp, GetRect(), controlPos)) {
                        continue;
                    }
                    pControl->AlphaPaint(pRender, rcPaint);
                }
            }
        }
        else {
            AutoClip childClip(pRender, rcTemp);
            for (auto it = m_items.begin(); it != m_items.end(); ++it) {
                auto pControl = *it;
                if ((pControl == nullptr) || !pControl->IsVisible()) {
                    continue;
                }
                UiRect controlPos = pControl->GetPos();
                if (!UiRect::Intersect(rcTemp, rcPaint, controlPos)) {
                    continue;
                }
                if (pControl->IsFloat()) {
                    if (!UiRect::Intersect(rcTemp, GetRect(), controlPos)) {
                        continue;
                    }
                    pControl->AlphaPaint(pRender, rcPaint);
                }
                else {
                    if (!UiRect::Intersect(rcTemp, rc, controlPos)) {
                        continue;
                    }
                    pControl->AlphaPaint(pRender, rcPaint);
                }
            }
        }
    }

    if ((pVScrollBar != nullptr) && pVScrollBar->IsVisible()) {
        UiRect verBarPos = pVScrollBar->GetPos();
        if (UiRect::Intersect(rcTemp, rcPaint, verBarPos)) {
            pVScrollBar->AlphaPaint(pRender, rcPaint);
        }
    }

    if ((pHScrollBar != nullptr) && pHScrollBar->IsVisible()) {
        UiRect horBarPos = pHScrollBar->GetPos();
        if (UiRect::Intersect(rcTemp, rcPaint, horBarPos)) {
            pHScrollBar->AlphaPaint(pRender, rcPaint);
        }
    }
}

void RichEdit::PaintBorder(IRender* pRender)
{
    BaseClass::PaintBorder(pRender);
    if (!IsFocused() || IsReadOnly() || !IsEnabled()) {
        return;
    }
    //Draw the bottom border line
    DString borderColor = GetFocusBottomBorderColor();
    int32_t borderSize = GetFocusBottomBorderSize();
    if ((borderSize > 0) && !borderColor.empty()) {
        UiColor dwBorderColor = GetUiColor(borderColor);
        UiRect rcBorder = GetRect();
        float fRoundWidth = 0;
        float fRoundHeight = 0;
        GetBorderRound(fRoundWidth, fRoundHeight);

        float fBottomBorderWidth = Dpi().GetScaleFloat(borderSize);
        rcBorder.right -= int32_t(fRoundWidth + 0.5f);
        rcBorder.left -= int32_t(fRoundWidth + 0.5f);
        UiPointF pt1((float)rcBorder.left, (float)rcBorder.bottom - fBottomBorderWidth / 2);
        UiPointF pt2((float)rcBorder.right, (float)rcBorder.bottom - fBottomBorderWidth / 2);
        DrawBorderLine(pRender, pt1, pt2, fBottomBorderWidth, dwBorderColor, GetBorderDashStyle());
    }
}

void RichEdit::CreateCaret(int32_t xWidth, int32_t yHeight)
{
    m_iCaretWidth = xWidth;
    m_iCaretHeight = yHeight;
    if (m_iCaretWidth < 0) {
        m_iCaretWidth = 0;
    }
    if (m_iCaretHeight < 0) {
        m_iCaretHeight = 0;
    }
}

void RichEdit::GetCaretSize(int32_t& xWidth, int32_t& yHeight) const
{
    xWidth = m_iCaretWidth;
    yHeight = m_iCaretHeight;
}

void RichEdit::ShowCaret(bool fShow)
{
    Window* pWindow = GetWindow();
    if (fShow && (pWindow != nullptr)) {
        m_bIsCaretVisiable = true;
        m_drawCaretFlag.Cancel();
        std::function<void()> closure = UiBind(&RichEdit::ChangeCaretVisiable, this);
        GlobalManager::Instance().Timer().AddTimer(m_drawCaretFlag.GetWeakFlag(), closure, 500);
    }
    else {
        m_bIsCaretVisiable = false;
        m_drawCaretFlag.Cancel();
    }

    if (m_bTextInputMode) {
        //Set the position of the input box
        int32_t xPos = 0;
        int32_t yPos = 0;
        GetCaretPos(xPos, yPos);

        int32_t xWidth = 0;
        int32_t yHeight = 0;
        GetCaretSize(xWidth, yHeight);

        UiRect rc = GetRect();
        UiSize szScrollOffset = GetScrollOffset();
        rc.Offset(-szScrollOffset.cx, -szScrollOffset.cy);

        UiPoint scrollOffset = GetScrollOffsetInScrollBox();
        rc.Offset(-scrollOffset.x, -scrollOffset.y);

        UiRect inputRect;
        inputRect.left = xPos - scrollOffset.x;
        inputRect.top = yPos - scrollOffset.y;
        inputRect.right = inputRect.left + (rc.right - inputRect.left);
        if (inputRect.right <= inputRect.left) {
            inputRect.right = inputRect.left + 2;
        }
        inputRect.bottom = inputRect.top + m_nRowHeight; //Set the height the same as the row height
        ASSERT(m_nRowHeight > 0);

        //Set the input area
        int32_t nCursorOffset = xWidth + Dpi().GetScaleInt(1); //The distance between the IME candidate box and the current caret position (horizontal), to avoid covering the caret        
        pWindow->NativeWnd()->SetTextInputArea(&inputRect, nCursorOffset);
    }

    Invalidate();
}

void RichEdit::SetCaretColor(const DString& dwColor)
{
    m_sCaretColor = dwColor;
}

DString RichEdit::GetCaretColor() const
{
    return m_sCaretColor.c_str();
}

UiRect RichEdit::GetCaretRect() const
{
    int32_t xPos = 0;
    int32_t yPos = 0;
    GetCaretPos(xPos, yPos);

    int32_t xWidth = 0;
    int32_t yHeight = 0;
    GetCaretSize(xWidth, yHeight);

    UiRect rc = { xPos, yPos, xPos + xWidth, yPos + yHeight };
    return rc;
}

void RichEdit::SetCaretPos(int32_t xPos, int32_t yPos)
{
    SetCaretPos(UiPoint(xPos, yPos));
}

void RichEdit::SetCaretPos(const UiPoint& pt)
{
    int32_t nCharPosIndex = CharFromPos(pt);
    SetCaretPos(nCharPosIndex);
}

void RichEdit::SetCaretPos(int32_t nCharPosIndex)
{
    UiPoint cursorPos = m_pTextData->CaretPosFromChar(nCharPosIndex);
    SetCaretPosInternal(cursorPos.x, cursorPos.y);
}

void RichEdit::SetCaretPosInternal(int32_t xPos, int32_t yPos)
{
    //The caret coordinates are relative to the top-left corner of the current control as the origin
    UiSize szScrollOffset = GetScrollOffset();
    xPos += szScrollOffset.cx;
    yPos += szScrollOffset.cy;

    m_iCaretPosX = xPos;
    m_iCaretPosY = yPos;
    ShowCaret(m_bActive && !HasSelText());   
}

void RichEdit::GetCaretPos(int32_t& xPos, int32_t& yPos) const
{
    xPos = m_iCaretPosX;
    yPos = m_iCaretPosY;

    UiSize szScrollOffset = GetScrollOffset();
    xPos -= szScrollOffset.cx;
    yPos -= szScrollOffset.cy;
}

void RichEdit::ChangeCaretVisiable()
{
    m_bIsCaretVisiable = !m_bIsCaretVisiable;
    Invalidate();
}

void RichEdit::PaintCaret(IRender* pRender, const UiRect& /*rcPaint*/)
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    if (IsReadOnly() && m_bNoCaretReadonly) {
        return;
    }
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    if (m_bIsCaretVisiable && !m_bIsComposition) {
#else
    if (m_bIsCaretVisiable) {
#endif
        int32_t xPos = 0;
        int32_t yPos = 0;
        GetCaretPos(xPos, yPos);

        int32_t xWidth = 0;
        int32_t yHeight = 0;
        GetCaretSize(xWidth, yHeight);

        UiRect rcDrawText = GetTextDrawRect(GetRect());
        UiRect rcCaret(xPos, yPos, xPos + xWidth, yPos + yHeight);
        if(rcCaret.Intersect(rcDrawText)) {
            //When the caret is within the text display area, draw the caret
            UiColor dwClrColor(0xFF000000);
            if (!m_sCaretColor.empty()) {
                dwClrColor = this->GetUiColor(m_sCaretColor.c_str());
            }
            pRender->DrawLine(UiPointF(xPos + 1, yPos), UiPointF(xPos + 1, yPos + yHeight), dwClrColor, (float)xWidth);
        }
    }
}

void RichEdit::PaintCurrentRowBkColor(IRender* pRender, const UiRect& /*rcPaint*/)
{
    if (pRender == nullptr) {
        return;
    }
    if (IsHideSelection() && !m_bActive) {
        //When the control is inactive, hide the selection
        return;
    }

    UiColor currentRowBkColor;
    DString strCurrentRowBkColor;
    if (!m_bActive) {
        //Non-focused state
        strCurrentRowBkColor = GetInactiveCurrentRowBkColor();
    }
    else {
        //Focused state
        strCurrentRowBkColor = GetCurrentRowBkColor();
    }    
    if (!strCurrentRowBkColor.empty()) {
        currentRowBkColor = GetUiColor(strCurrentRowBkColor);
    }
    if (currentRowBkColor.IsEmpty()) {
        return;
    }

    int32_t nStartChar = 0;
    int32_t nEndChar = 0;
    GetSel(nStartChar, nEndChar);
    UiRect rowRect = m_pTextData->GetCharRowRect(nEndChar);
    if (!rowRect.IsEmpty()) {
        pRender->FillRect(UiRectF::MakeFromRect(rowRect), currentRowBkColor);
    }
}

void RichEdit::PaintSelectionColor(IRender* pRender, const UiRect& /*rcPaint*/)
{
    if (pRender == nullptr) {
        //No need to draw the selection background
        return;
    }
    if (IsHideSelection() && !m_bActive) {
        //When the control is inactive, hide the selection
        return;
    }

    int32_t nSelStartChar = -1;
    int32_t nSelEndChar = -1;
    GetSel(nSelStartChar, nSelEndChar);
    if (nSelStartChar >= nSelEndChar) {
        //No selected text
        return;
    }

    //The rectangle range of the selection in each row
    std::map<int32_t, UiRectF> rowTextRectFs;
    m_pTextData->GetCharRangeRects(nSelStartChar, nSelEndChar, rowTextRectFs);
    if(rowTextRectFs.empty()) {
        //No need to draw (no selected text)
        return;
    }

    UiColor selectionColor;
    DString selectionBkColor;
    if (m_bActive) {
        //Focused state
        selectionBkColor = GetSelectionBkColor();
    }
    else {
        //Non-focused state
        selectionBkColor = GetInactiveSelectionBkColor();
    }    
    if (!selectionBkColor.empty()) {
        selectionColor = GetUiColor(selectionBkColor);
    }
    if (selectionColor.IsEmpty()) {
        selectionColor = UiColor(UiColors::SkyBlue);
    }

    UiRect rcTemp;
    const UiRect rcDrawText = GetTextDrawRect(GetRect());

    //Draw the background row by row
    UiRect rowRect;
    for (auto iter = rowTextRectFs.begin(); iter != rowTextRectFs.end(); ++iter) {
        const UiRectF& rectF = iter->second;
        if (rectF.IsEmpty()) {
            //Empty row, draw a line
            rowRect.left = (int32_t)rectF.left;
            rowRect.right = (int32_t)ui::CEILF(rectF.right);
            rowRect.top = (int32_t)rectF.top;
            rowRect.bottom = (int32_t)ui::CEILF(rectF.bottom);
            if (rowRect.left == rowRect.right) {
                rowRect.right = rowRect.left + Dpi().GetScaleInt(2);
            }
        }
        else {
            rowRect.left = (int32_t)rectF.left;
            rowRect.right = (int32_t)ui::CEILF(rectF.right);
            rowRect.top = (int32_t)rectF.top;
            rowRect.bottom = (int32_t)ui::CEILF(rectF.bottom);
        }
        if (UiRect::Intersect(rcTemp, rcDrawText, rowRect)) {
            pRender->FillRect(UiRectF::MakeFromRect(rowRect), selectionColor);
        }
    }
}

void RichEdit::SetPromptMode(bool bPrompt)
{
    if (bPrompt != m_bAllowPrompt) {
        m_bAllowPrompt = bPrompt;
        Invalidate();
    }    
}

bool RichEdit::AllowPromptMode() const
{
    return m_bAllowPrompt;
}

DString RichEdit::GetPromptText() const
{
    DString strText = m_sPromptText.c_str();
    if (strText.empty() && !m_sPromptTextId.empty()) {
        strText = GlobalManager::Instance().Lang().GetStringViaID(m_sPromptTextId.c_str());
    }
    return strText;
}

void RichEdit::SetPromptText(const DString& strText)
{
    if (m_sPromptText != strText) {
        m_sPromptText = strText;
        Invalidate();
    }
}

void RichEdit::SetPromptTextId(const DString& strTextId)
{
    if (m_sPromptTextId != strTextId) {
        m_sPromptTextId = strTextId;
        Invalidate();
    }
}

void RichEdit::SetPromptTextColor(const DString& promptColor)
{
    if (m_sPromptColor != promptColor) {
        m_sPromptColor = promptColor;
        Invalidate();
    }
}

DString RichEdit::GetPromptTextColor() const
{
    if (!m_sPromptColor.empty()) {
        return m_sPromptColor.c_str();
    }
    else if (!m_sDisabledTextColor.empty()) {
        return m_sDisabledTextColor.c_str();
    }
    else if (!m_sTextColor.empty()) {
        return m_sTextColor.c_str();
    }
    return DString();
}

void RichEdit::PaintPromptText(IRender* pRender)
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    if (!AllowPromptMode()) {
        return;
    }
    DString promptText = GetPromptText();
    if (promptText.empty()) {
        return;
    }
    DString promptTextColor = GetPromptTextColor();
    if (promptTextColor.empty()) {
        return;
    }
    DString fontId = GetCurrentFontId();
    if (fontId.empty()) {
        return;
    }
    if (GetTextLength() != 0) {
        return;
    }

    DrawStringParam drawParam;
    drawParam.pFont = GetIFontInternal(fontId);
    drawParam.uFormat = GetTextStyle();
    drawParam.textRect = GetRichTextDrawRect();
    drawParam.dwTextColor = GetUiColor(promptTextColor);
    pRender->DrawString(promptText, drawParam);
}

DString RichEdit::GetFocusedImage()
{
    if (m_pFocusedImage != nullptr) {
        return m_pFocusedImage->GetImageString();
    }
    return DString();
}

void RichEdit::SetFocusedImage( const DString& strImage )
{
    if (m_pFocusedImage == nullptr) {
        m_pFocusedImage = new Image;
    }
    m_pFocusedImage->SetImageString(strImage, Dpi());
    Invalidate();
}

void RichEdit::PaintStateImages(IRender* pRender)
{
    if (IsReadOnly()) {
        return;
    }

    if (IsFocused()) {
        if (m_pFocusedImage != nullptr) {
            PaintImage(pRender, m_pFocusedImage);
        }        
        PaintPromptText(pRender);
    }
    else {
        BaseClass::PaintStateImages(pRender);
        PaintPromptText(pRender);
    }
}

void RichEdit::SetNoSelOnKillFocus(bool bNoSel)
{
    m_bNoSelOnKillFocus = bNoSel;
}

void RichEdit::SetSelAllOnFocus(bool bSelAll)
{
    m_bSelAllOnFocus = bSelAll;
}

void RichEdit::SetNoCaretReadonly()
{
    m_bNoCaretReadonly = true;
}

void RichEdit::ClearImageCache()
{
    BaseClass::ClearImageCache();
    if (m_pFocusedImage != nullptr) {
        m_pFocusedImage->ClearImageCache();
    }    
}

void RichEdit::SetTextPadding(UiPadding padding, bool bNeedDpiScale)
{
    ASSERT((padding.left >= 0) && (padding.top >= 0) && (padding.right >= 0) && (padding.bottom >= 0));
    if ((padding.left < 0) || (padding.top < 0) ||
        (padding.right < 0) || (padding.bottom < 0)) {
        return;
    }
    if (bNeedDpiScale) {
        Dpi().ScalePadding(padding);
    }
    if (!GetTextPadding().Equals(padding)) {
        m_rcTextPadding.left = TruncateToUInt16(padding.left);
        m_rcTextPadding.top = TruncateToUInt16(padding.top);
        m_rcTextPadding.right = TruncateToUInt16(padding.right);
        m_rcTextPadding.bottom = TruncateToUInt16(padding.bottom);
        RelayoutOrRedraw();
    }
}

UiPadding RichEdit::GetTextPadding() const
{
    return UiPadding(m_rcTextPadding.left, m_rcTextPadding.top, m_rcTextPadding.right, m_rcTextPadding.bottom);
}

void RichEdit::SetUseControlCursor(bool bUseControlCursor)
{
    m_bUseControlCursor = bUseControlCursor;
}

void RichEdit::AttachSelChanged(const EventCallback& callback, EventCallbackID callbackID)
{ 
    AttachEvent(kEventSelChanged, callback, callbackID);
}

void RichEdit::SetZoomPercent(uint32_t nZoomPercent)
{
    ASSERT(nZoomPercent != 0);
    if (nZoomPercent == 0) {
        return;
    }
    if (nZoomPercent > MAX_ZOOM_PERCENT) {
        //Limit: zoom in up to 8 times
        nZoomPercent = MAX_ZOOM_PERCENT;
    }
    if (m_nZoomPercent != nZoomPercent) {
        uint32_t nOldZoomPercent = m_nZoomPercent;        
        m_nZoomPercent = TruncateToInt16(nZoomPercent);
        uint32_t nNewZoomPercent = m_nZoomPercent;
        OnZoomPercentChanged(nOldZoomPercent, nNewZoomPercent);
    }
}

uint32_t RichEdit::GetZoomPercent() const
{
    return m_nZoomPercent;
}

void RichEdit::SetEnableWheelZoom(bool bEnable)
{
    m_bEnableWheelZoom = bEnable;
}

bool RichEdit::IsEnableWheelZoom(void) const
{
    return m_bEnableWheelZoom;
}

void RichEdit::SetEnableDefaultContextMenu(bool bEnable)
{
    if (m_bEnableDefaultContextMenu != bEnable) {
        m_bEnableDefaultContextMenu = bEnable;
        if (bEnable) {
            AttachContextMenu([this](const ui::EventArgs& args) {
                if (args.eventType == ui::kEventContextMenu) {
                    ui::UiPoint pt = args.ptMouse;
                    if ((pt.x != -1) && (pt.y != -1)) {
                        //The context menu generated by a right mouse button click                        
                        ShowPopupMenu(pt);
                    }
                    else {
                        //Press Shift + F10, the context menu is generated by the system
                        pt = { 100, 100 };
                        ShowPopupMenu(pt);
                    }
                }
                return true;
                });
        }
        else {
            DetachEvent(kEventContextMenu);
        }
    }
}

bool RichEdit::IsEnableDefaultContextMenu() const
{
    return m_bEnableDefaultContextMenu;
}

void RichEdit::ShowPopupMenu(const ui::UiPoint& point)
{
    RichEdit* pRichEdit = this;
    if ((pRichEdit == nullptr) || !pRichEdit->IsEnabled() || pRichEdit->IsPasswordMode()) {
        return;
    }

    //If no text is selected, move the caret to the current click position
    int32_t nStartChar = 0; 
    int32_t nEndChar = 0;
    pRichEdit->GetSel(nStartChar, nEndChar);
    if (nStartChar == nEndChar) {
        int32_t pos = m_pTextData->CharFromPos(point);
        if (pos >= 0) {
            pRichEdit->SetSel(pos, pos);
            pRichEdit->GetSel(nStartChar, nEndChar);
        }
    }
    
    DString skinFolder = _T("public/menu/");
    Menu* menu = new Menu(GetWindow());//The parent window needs to be set, otherwise the program status bar becomes inactive when the menu pops up
    menu->SetSkinFolder(skinFolder);
    DString xml(_T("rich_edit_menu.xml"));

    //Do not hide the currently selected text while the menu is displayed
    bool bOldHideSelection = IsHideSelection();
    SetHideSelection(false);

    //Menu close event
    std::weak_ptr<WeakFlag> richEditFlag = GetWeakFlag();
    menu->AttachWindowCloseMsg([this, richEditFlag, bOldHideSelection](const ui::EventArgs&) {
        if (!richEditFlag.expired()) {
            //Restore the HideSelection property
            SetHideSelection(bOldHideSelection);
        }
        return true;
        });

    //The coordinates of the menu popup position should be screen coordinates
    UiPoint pt = point;
    ClientToScreen(pt);
    menu->ShowMenu(xml, pt);

    ui::MenuItem* menu_item = nullptr;
    //Update the command state and add menu command responses
    bool hasSelText = nEndChar > nStartChar ? true : false;
    menu_item = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("edit_menu_copy")));
    if (menu_item != nullptr) {
        if (!hasSelText) {
            menu_item->SetEnabled(false);
        }
        menu_item->AttachClick([pRichEdit](const ui::EventArgs& /*args*/) {
            pRichEdit->Copy();
            return true;
            });
    }
    menu_item = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("edit_menu_cut")));
    if (menu_item != nullptr) {
        if (!hasSelText) {
            menu_item->SetEnabled(false);
        }
        else if (pRichEdit->IsReadOnly()) {
            menu_item->SetEnabled(false);
        }
        menu_item->AttachClick([pRichEdit](const ui::EventArgs& /*args*/) {
            pRichEdit->Cut();
            return true;
            });
    }
    menu_item = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("edit_menu_paste")));
    if (menu_item != nullptr) {
        if (!pRichEdit->CanPaste()) {
            menu_item->SetEnabled(false);
        }
        else if (pRichEdit->IsReadOnly()) {
            menu_item->SetEnabled(false);
        }
        menu_item->AttachClick([pRichEdit](const ui::EventArgs& /*args*/) {
            pRichEdit->Paste();
            return true;
            });
    }
    menu_item = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("edit_menu_del")));
    if (menu_item != nullptr) {
        if (!hasSelText) {
            menu_item->SetEnabled(false);
        }
        else if (pRichEdit->IsReadOnly()) {
            menu_item->SetEnabled(false);
        }
        menu_item->AttachClick([pRichEdit](const ui::EventArgs& /*args*/) {
            pRichEdit->Clear();
            return true;
            });
    }
    menu_item = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("edit_menu_sel_all")));
    if (menu_item != nullptr) {
        if ((nStartChar == 0) && (nEndChar == pRichEdit->GetTextLength())) {
            menu_item->SetEnabled(false);
        }
        menu_item->AttachClick([pRichEdit](const ui::EventArgs& /*args*/) {
            pRichEdit->SetSelAll();
            return true;
            });
    }
    menu_item = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("edit_menu_undo")));
    if (menu_item != nullptr) {
        if (!pRichEdit->CanUndo()) {
            menu_item->SetEnabled(false);
        }
        else if (pRichEdit->IsReadOnly()) {
            menu_item->SetEnabled(false);
        }
        menu_item->AttachClick([pRichEdit](const ui::EventArgs& /*args*/) {
            pRichEdit->Undo();
            return true;
            });
    }
    menu_item = dynamic_cast<ui::MenuItem*>(menu->FindControl(_T("edit_menu_redo")));
    if (menu_item != nullptr) {
        if (!pRichEdit->CanRedo()) {
            menu_item->SetEnabled(false);
        }
        else if (pRichEdit->IsReadOnly()) {
            menu_item->SetEnabled(false);
        }
        menu_item->AttachClick([pRichEdit](const ui::EventArgs& /*args*/) {
            pRichEdit->Redo();
            return true;
            });
    }
}

void RichEdit::OnTextChanged()
{
    //Set the modified flag
    SetModify(true);
    if (!m_bDisableTextChangeEvent) {
        SendEvent(kEventTextChanged);
    }
}

bool RichEdit::SetSpinClass(const DString& spinClass)
{
    DString spinBoxClass;
    DString spinBtnUpClass;
    DString spinBtnDownClass;
    std::list<DString> classNames = StringUtil::Split(spinClass, _T(","));
    if (classNames.size() == 3) {
        auto iter = classNames.begin();
        spinBoxClass = *iter++;
        spinBtnUpClass = *iter++;
        spinBtnDownClass = *iter++;
    }
    if (!spinClass.empty()) {
        ASSERT(!spinBoxClass.empty() && !spinBtnUpClass.empty() && !spinBtnDownClass.empty());
    }
    if (!spinBoxClass.empty() && !spinBtnUpClass.empty() && !spinBtnDownClass.empty()) {        
        Button* pUpButton = nullptr;
        Button* pDownButton = nullptr;
        if (m_pSpinBox == nullptr) {
            m_pSpinBox = new VBox(GetWindow());
            AddItem(m_pSpinBox);

            pUpButton = new Button(GetWindow());
            m_pSpinBox->AddItem(pUpButton);

            pDownButton = new Button(GetWindow());
            m_pSpinBox->AddItem(pDownButton);
        }
        else {
            pUpButton = dynamic_cast<Button*>(m_pSpinBox->GetItemAt(0));
            pDownButton = dynamic_cast<Button*>(m_pSpinBox->GetItemAt(1));            
        }

        ASSERT((pUpButton != nullptr) && (pDownButton != nullptr));
        if ((pUpButton == nullptr) || (pDownButton == nullptr)) {
            RemoveItem(m_pSpinBox);
            m_pSpinBox = nullptr;
            return false;
        }
        m_pSpinBox->SetClass(spinBoxClass);
        pUpButton->SetClass(spinBtnUpClass);
        pDownButton->SetClass(spinBtnDownClass);
        
        //Attach event handlers
        pUpButton->DetachEvent(kEventClick);
        pUpButton->AttachClick([this](const EventArgs& /*args*/){
            AdjustTextNumber(1);
            return true;
            });

        pUpButton->DetachEvent(kEventMouseButtonDown);
        pUpButton->AttachButtonDown([this](const EventArgs& /*args*/) {
            StartAutoAdjustTextNumberTimer(1);
            return true;
            });

        pUpButton->DetachEvent(kEventMouseButtonUp);
        pUpButton->AttachButtonUp([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            return true;
            });

        pUpButton->DetachEvent(kEventMouseLeave);
        pUpButton->AttachMouseLeave([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            return true;
            });

        pDownButton->DetachEvent(kEventClick);
        pDownButton->AttachClick([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            AdjustTextNumber(-1);
            return true;
            });

        pDownButton->DetachEvent(kEventMouseButtonDown);
        pDownButton->AttachButtonDown([this](const EventArgs& /*args*/) {
            StartAutoAdjustTextNumberTimer(-1);
            return true;
            });

        pDownButton->DetachEvent(kEventMouseButtonUp);
        pDownButton->AttachButtonUp([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            return true;
            });

        pDownButton->DetachEvent(kEventMouseLeave);
        pDownButton->AttachMouseLeave([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            return true;
            });
        return true;
    }
    else {
        if (m_pSpinBox != nullptr) {
            RemoveItem(m_pSpinBox);
            m_pSpinBox = nullptr;
        }    
    }
    return false;
}

bool RichEdit::SetEnableSpin(bool bEnable, const DString& spinClass, int32_t nMin, int32_t nMax)
{
    bool bRet = false;
    if (bEnable) {
        ASSERT(!spinClass.empty());
        if (spinClass.empty()) {
            return false;
        }
        bRet = SetSpinClass(spinClass);
        if (bRet) {
            SetNumberOnly(true);
            if ((nMin != 0) || (nMax != 0)) {
                SetMaxNumber(nMax);
                SetMinNumber(nMin);
            }            
        }
    }
    else {
        bool hasSpin = m_pSpinBox != nullptr;
        SetSpinClass(_T(""));
        bRet = true;
        if (hasSpin) {
            SetNumberOnly(false);
            SetMaxNumber(INT_MAX);
            SetMinNumber(INT_MIN);
        }        
    }
    return bRet;
}

int64_t RichEdit::GetTextNumber() const
{
    DString text = GetText();
    if (text.empty()) {
        return 0;
    }
    int64_t n = StringUtil::StringToInt64(text);
    return n;
}

void RichEdit::SetTextNumber(int64_t nValue)
{
    int32_t nSelStartChar = -1;
    int32_t nSelEndChar = -1;
    GetSel(nSelStartChar, nSelEndChar);
    if (!m_numberFormat.empty()) {
        SetText(StringUtil::Printf(m_numberFormat.c_str(), nValue));
    }
    else {
        SetText(StringUtil::Int64ToString(nValue));
    }
    if ((nSelStartChar == nSelEndChar) && (nSelStartChar >= 0) && (nSelStartChar <= GetTextLength())) {
        SetSel(nSelStartChar, nSelStartChar);
    }
}

void RichEdit::AdjustTextNumber(int32_t nDelta)
{
    ASSERT(IsNumberOnly());
    if (IsNumberOnly()) {
        const int64_t nOldValue = GetTextNumber();
        int64_t nNewValue = nOldValue + nDelta;
        if (((GetMinNumber() != INT_MIN) || (GetMaxNumber() != INT_MAX))) {
            if (nNewValue > GetMaxNumber()) {
                //Exceeds the maximum number, correct it
                nNewValue = GetMaxNumber();
                if ((nDelta == 1) && (nOldValue == GetMaxNumber())) {
                    //Cycle around
                    nNewValue = GetMinNumber();
                }
            }
            else if (nNewValue < GetMinNumber()) {
                //Below the minimum number, correct it
                nNewValue = GetMinNumber();
                if ((nDelta == -1) && (nOldValue == GetMinNumber())) {
                    //Cycle around
                    nNewValue = GetMaxNumber();
                }
            }
        }
        if (nNewValue != nOldValue) {
            SetTextNumber(nNewValue);
        }
    }
}

void RichEdit::StartAutoAdjustTextNumberTimer(int32_t nDelta)
{
    if (nDelta != 0) {
        //Start the timer
        m_flagAdjustTextNumber.Cancel();
        std::function<void()> closure = UiBind(&RichEdit::StartAutoAdjustTextNumber, this, nDelta);
        GlobalManager::Instance().Timer().AddTimer(m_flagAdjustTextNumber.GetWeakFlag(), closure, 1000, 1);
    }
}

void RichEdit::StartAutoAdjustTextNumber(int32_t nDelta)
{
    if (nDelta != 0) {
        //Start the timer
        m_flagAdjustTextNumber.Cancel();
        std::function<void()> closure = UiBind(&RichEdit::AdjustTextNumber, this, nDelta);
        GlobalManager::Instance().Timer().AddTimer(m_flagAdjustTextNumber.GetWeakFlag(), closure, 120);
    }
}

void RichEdit::StopAutoAdjustTextNumber()
{
    m_flagAdjustTextNumber.Cancel();
}

void RichEdit::SetClearBtnClass(const DString& btnClass)
{
    if (!btnClass.empty()) {
        ASSERT(m_pClearButton == nullptr);
        if (m_pClearButton != nullptr) {
            return;
        }
        Button* pClearButton = new Button(GetWindow());
        pClearButton->SetClass(btnClass);
        pClearButton->SetNoFocus();
        pClearButton->SetVisible(false);
        AddItem(pClearButton);
        m_pClearButton = pClearButton;

        //Respond to the button click event
        pClearButton->AttachClick([this](const EventArgs& /*args*/) {
            SetText(_T(""));
            return true;
            });
    }
}

void RichEdit::SetShowPasswordBtnClass(const DString& btnClass)
{
    if (!btnClass.empty()) {
        ASSERT(m_pShowPasswordButton == nullptr);
        if (m_pShowPasswordButton != nullptr) {
            return;
        }
        Button* pButton = new Button(GetWindow());
        pButton->SetClass(btnClass);
        pButton->SetNoFocus();
        pButton->SetVisible(false);
        AddItem(pButton);
        m_pShowPasswordButton = pButton;

        //Respond to the button click event
        pButton->AttachClick([this](const EventArgs& /*args*/) {
            SetShowPassword(false);
            return true;
            });
        pButton->AttachButtonDown([this](const EventArgs& /*args*/) {
            SetShowPassword(true);
            return true;
            });
        pButton->AttachButtonUp([this](const EventArgs& /*args*/) {
            SetShowPassword(false);
            return true;
            });
        pButton->AttachMouseLeave([this](const EventArgs& /*args*/) {
            SetShowPassword(false);
            return true;
            });
    }
    else {
        if (m_pShowPasswordButton != nullptr) {
            RemoveItem(m_pShowPasswordButton);
            m_pShowPasswordButton = nullptr;
        }
    }
}

void RichEdit::SetTextHAlignType(HorAlignType alignType)
{
    if (m_pTextData->GetHAlignType() != alignType) {
        m_pTextData->SetTextHAlignType(alignType);
        //Clear the drawing cache and redraw
        ClearCacheAndRedraw();
    }
}

HorAlignType RichEdit::GetHAlignType() const
{
    return m_pTextData->GetHAlignType();
}

void RichEdit::SetTextVAlignType(VerAlignType alignType)
{
    if (m_pTextData->GetVAlignType() != alignType) {
        m_pTextData->SetTextVAlignType(alignType);
        //Clear the drawing cache and redraw
        ClearCacheAndRedraw();
    }
}

VerAlignType RichEdit::GetVAlignType() const
{
    return m_pTextData->GetVAlignType();
}

UiPoint RichEdit::PosFromChar(int32_t lChar) const
{
    return m_pTextData->PosFromChar(lChar);
}

int32_t RichEdit::CharFromPos(UiPoint pt)
{
    pt.Offset(GetScrollOffsetInScrollBox());
    return m_pTextData->CharFromPos(pt);
}

uint16_t RichEdit::GetTextStyle() const
{
    uint32_t uTextStyle = 0;
    HorAlignType hAlignType = GetHAlignType();
    if (hAlignType == HorAlignType::kAlignCenter) {
        uTextStyle |= TEXT_HCENTER;
    }
    else if (hAlignType == HorAlignType::kAlignRight) {
        uTextStyle |= TEXT_RIGHT;
    }
    else {
        uTextStyle |= TEXT_LEFT;
    }

    VerAlignType vAlignType = GetVAlignType();
    if (vAlignType == VerAlignType::kAlignCenter) {
        uTextStyle |= TEXT_VCENTER;
    }
    else if (vAlignType == VerAlignType::kAlignBottom) {
        uTextStyle |= TEXT_BOTTOM;
    }
    else {
        uTextStyle |= TEXT_TOP;
    }

    //Password mode, no automatic wrapping
    if (IsWordWrap() && !IsPasswordMode()) {
        uTextStyle |= TEXT_WORD_WRAP;
    }

    //Single-line/multi-line attribute (if not set, it is multi-line mode)
    if (!IsMultiLine()) {
        uTextStyle |= TEXT_SINGLELINE;        
    }
    return ui::TruncateToUInt16(uTextStyle);
}

bool RichEdit::GetRichTextForDraw(std::vector<RichTextData>& richTextDataList) const
{
    std::vector<std::wstring_view> textView;
    m_pTextData->GetTextView(textView);
    GetRichTextForDraw(textView, richTextDataList);
    return !richTextDataList.empty();
}

bool RichEdit::GetRichTextForDraw(const std::vector<std::wstring_view>& textView,
                                  std::vector<RichTextData>& richTextDataList,
                                  size_t nStartLine,
                                  const std::vector<size_t>& modifiedLines) const
{
    if (nStartLine != (size_t)-1) {
        if (!modifiedLines.empty()) {
            ASSERT(modifiedLines[0] == nStartLine);
            if (modifiedLines[0] != nStartLine) {
                return false;
            }
        }
    }
    richTextDataList.clear();
    if (textView.empty()) {
        return false;
    }
    DString sFontId = GetCurrentFontId();
    ASSERT(!sFontId.empty());
    IFont* pFont = GetIFontInternal(sFontId);
    ASSERT(pFont != nullptr);
    if (pFont == nullptr) {
        return false;
    }
    RichTextData richTextData;    
    //Default text attributes
    richTextData.m_textStyle = GetTextStyle();
    //Default text color
    if (!IsEnabled()) {
        richTextData.m_textColor = GetUiColor(GetDisabledTextColor());
    }
    else {
        richTextData.m_textColor = GetUiColor(GetTextColor());
    }
    if (richTextData.m_textColor.IsEmpty()) {
        richTextData.m_textColor = UiColor(UiColors::Black);
    }
    //The font information of the text
    richTextData.m_pFontInfo.reset(new UiFontEx);
    richTextData.m_pFontInfo->m_fontName = pFont->FontName();
    richTextData.m_pFontInfo->m_fontSize = pFont->FontSize();
    richTextData.m_pFontInfo->m_bBold = pFont->IsBold();
    richTextData.m_pFontInfo->m_bUnderline = pFont->IsUnderline();
    richTextData.m_pFontInfo->m_bItalic = pFont->IsItalic();
    richTextData.m_pFontInfo->m_bStrikeOut = pFont->IsStrikeOut();

    //Row spacing multiplier
    richTextData.m_fRowSpacingMul = GetRowSpacingMul();
    richTextData.m_fRowSpacingAdd = GetRowSpacingAdd();

    if (nStartLine != (size_t)-1) {
        //Incremental drawing, only draw the changed parts
        size_t nLineIndex = 0;
        const size_t nCount = modifiedLines.size();
        richTextDataList.reserve(nCount);
        for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
            nLineIndex = modifiedLines[nIndex];
            ASSERT(nLineIndex < textView.size());
            if (nLineIndex < textView.size()) {
                ASSERT(!textView[nLineIndex].empty());
                if (!textView[nLineIndex].empty()) {
                    richTextData.m_textView = textView[nLineIndex];
                    richTextDataList.emplace_back(richTextData);
                }
            }
            else {
                return false;
            }
        }
    }
    else {
        //Draw everything
        const size_t nCount = textView.size();
        richTextDataList.reserve(nCount);
        for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
            ASSERT(!textView[nIndex].empty());
            if (!textView[nIndex].empty()) {
                richTextData.m_textView = textView[nIndex];
                richTextDataList.emplace_back(richTextData);
            }
        }
    }    
    return !richTextDataList.empty();
}

UiRect RichEdit::GetRichTextDrawRect() const
{
    return GetTextDrawRect(GetRect());
}

uint8_t RichEdit::GetDrawAlpha() const
{
    return GetAlpha();
}

void RichEdit::OnTextRectsChanged()
{
    //Update the caret position
    int32_t nSelStartChar = -1;
    int32_t nSelEndChar = -1;
    GetSel(nSelStartChar, nSelEndChar);
    if (nSelStartChar == nSelEndChar) {
        SetCaretPos(nSelStartChar);
        EnsureCharVisible(nSelStartChar);
    }

    //Update the scrollbar range
    UpdateScrollRange();
}

int32_t RichEdit::GetTextRowHeight() const
{
    return m_nRowHeight;
}

int32_t RichEdit::GetTextCaretWidth() const
{
    return m_iCaretWidth;
}

bool RichEdit::IsTextPasswordMode() const
{
    return IsPasswordMode();
}

UiSize RichEdit::EstimateText(UiSize szAvailable)
{
    UiSize fixedSize;
    IRender* pRender = nullptr;
    if (GetWindow() != nullptr) {
        pRender = GetWindow()->GetRender();
    }
    if (pRender == nullptr) {
        return fixedSize;
    }

    int32_t nWidth = szAvailable.cx;
    if (GetFixedWidth().IsStretch()) {
        //If it is a stretch type, use the external width
        nWidth = CalcStretchValue(GetFixedWidth(), szAvailable.cx);
    }
    else if (GetFixedWidth().IsInt32()) {
        nWidth = GetFixedWidth().GetInt32();
    }
    else if (GetFixedWidth().IsAuto()) {
        //When the width is auto, the width is not limited
        nWidth = GetMaxWidth();
    }

    //Maximum height, not limited
    int32_t nHeight = INT_MAX;

    UiRect rc;
    rc.left = 0;
    rc.right = rc.left + nWidth;
    rc.top = 0;
    rc.bottom = rc.top + nHeight;

    const UiPadding rcTextPadding = GetTextPadding();
    const UiPadding rcPadding = GetControlPadding();
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

    //Calculate the size of the area occupied by the drawing
    UiRect rect = m_pTextData->EstimateTextDisplayBounds(rc);

    fixedSize.cx = rect.Width();
    if (fixedSize.cx > 0) {
        fixedSize.cx += (rcTextPadding.left + rcTextPadding.right);
        fixedSize.cx += (rcPadding.left + rcPadding.right);
    }

    fixedSize.cy = rect.Height();
    if ((fixedSize.cy == 0) && GetFixedHeight().IsAuto() && (GetTextLength() == 0)) {
        //When the text is empty and the height is "auto", set the height to the row height, keeping consistent with the Windows version
        fixedSize.cy = m_nRowHeight > 0 ? m_nRowHeight : 0;
    }
    if (fixedSize.cy > 0) {
        fixedSize.cy += (rcTextPadding.top + rcTextPadding.bottom);
        fixedSize.cy += (rcPadding.top + rcPadding.bottom);
    }
    return fixedSize;
}

UiRect RichEdit::GetTextDrawRect(const UiRect& rc) const
{
    UiRect rcAvailable = rc;
    rcAvailable.Deflate(GetTextPadding());
    rcAvailable.Deflate(GetControlPadding());

    //Estimate according to the state with scrollbars, to avoid needing a second estimation to get the final result
    if (!GetScrollBarFloat() && (GetVScrollBar() != nullptr)) {
        if (IsVScrollBarAtLeft()) {
            rcAvailable.left += GetVScrollBar()->GetFixedWidth().GetInt32();
        }
        else {
            rcAvailable.right -= GetVScrollBar()->GetFixedWidth().GetInt32();
        }
    }
    if (!GetScrollBarFloat() && (GetHScrollBar() != nullptr)) {
        rcAvailable.bottom -= GetHScrollBar()->GetFixedHeight().GetInt32();
    }
    rcAvailable.Validate();
    return rcAvailable;
}

UiSize64 RichEdit::CalcRequiredSize(const UiRect& rc, bool bEstimateOnly)
{
    //Calculate the size of the child controls
    UiSize64 requiredSize = BaseClass::CalcRequiredSize(rc, bEstimateOnly);
    if (requiredSize.cx > rc.Width()) {
        requiredSize.cx = 0;
    }
    else if (requiredSize.cy > rc.Height()) {
        requiredSize.cy = 0;
    }

    //Evaluate the text area: the current control area minus the padding, minus the width or height occupied by the scrollbar
    UiRect rcAvailable = GetTextDrawRect(rc);
    UiSize szAvailable(rcAvailable.Width(), rcAvailable.Height());

    //Estimate the image area size
    UiSize imageSize = EstimateImage(szAvailable, EstimateImageType::kBoth);
    if (imageSize.cx > rc.Width()) {
        imageSize.cx = 0;
    }
    else if (imageSize.cy > rc.Height()) {
        imageSize.cy = 0;
    }

    //Estimate the text area size; the function calculation already includes the padding
    UiSize textSize = EstimateText(szAvailable);
    //The text area needs to include the width and height of the scrollbar (only when the scrollbar is set to non-floating)
    if (!GetScrollBarFloat() && (GetVScrollBar() != nullptr) && GetVScrollBar()->IsValid()) {
        if (IsVScrollBarAtLeft()) {
            textSize.cx += GetVScrollBar()->GetFixedWidth().GetInt32();
        }
        else {
            textSize.cx += GetVScrollBar()->GetFixedWidth().GetInt32();
        }
    }
    if (!GetScrollBarFloat() && (GetHScrollBar() != nullptr) && GetHScrollBar()->IsValid()) {
        textSize.cy += GetHScrollBar()->GetFixedHeight().GetInt32();
    }

    UiSize szControlSize;
    szControlSize.cx = std::max(imageSize.cx, textSize.cx);
    szControlSize.cy = std::max(imageSize.cy, textSize.cy);

    //The text size is authoritative; the size of child controls or background images does not affect whether the control shows scrollbars
    UiSize64 szSize = requiredSize;
    if (szSize.cx < szControlSize.cx) {
        szSize.cx = szControlSize.cx;
    }
    if (szSize.cy < szControlSize.cy) {
        szSize.cy = szControlSize.cy;
    }
    return szSize;
}

void RichEdit::Redraw()
{
    m_nSelXPos = -1;
    m_pTextData->ClearDrawRichTextCache();
    Invalidate();
}

void RichEdit::ClearCacheAndRedraw()
{
    m_pTextData->SetCacheDirty(true);
    Redraw();
    UpdateScrollRange();
}

////////////////////////////////////////////////////////////
bool RichEdit::OnSetCursor(const EventArgs& msg)
{
    if (m_bUseControlCursor) {
        //Use the caret set by the Control
        return BaseClass::OnSetCursor(msg);
    }
    if (!IsEnabled()) {
        //In the disabled state, use the default cursor
        return BaseClass::OnSetCursor(msg);
    }

    SetCursor(IsReadOnly() ? CursorType::kCursorArrow : CursorType::kCursorIBeam);
    return true;
}

bool RichEdit::OnSetFocus(const EventArgs& /*msg*/)
{
    m_bActive = true;

    UiPoint cursorPos;
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        pWindow->GetCursorPos(cursorPos);
        pWindow->ScreenToClient(cursorPos);
    }

    //When gaining focus, open the IME
    if (pWindow != nullptr) {
        bool bEnableIME = IsVisible() && !IsReadOnly() && IsEnabled();
        pWindow->NativeWnd()->SetImeOpenStatus(bEnableIME);
        if (bEnableIME) {
            UiRect inputRect = GetRect();
            UiPoint scrollOffset = GetScrollOffsetInScrollBox();
            inputRect.Offset(-scrollOffset.x, -scrollOffset.y);
            pWindow->NativeWnd()->SetTextInputArea(&inputRect, 0);
        }
    }

    if ((pWindow != nullptr) && IsVisible() && !IsReadOnly() && IsEnabled()) {
        m_bTextInputMode = true;
    }
    else {
        m_bTextInputMode = false;
    }

    //Update the caret position
    SetCaretPos(cursorPos);

    //Set whether to show the caret
    ShowCaret(!HasSelText());

    if ((m_pClearButton != nullptr) && !IsReadOnly()) {
        m_pClearButton->SetFadeVisible(true);
    }
    if ((m_pShowPasswordButton != nullptr) && IsPasswordMode() && !IsShowPassword()) {
        m_pShowPasswordButton->SetFadeVisible(true);
    }

    //Do not call the base class method
    if (GetState() == kControlStateNormal) {
        SetState(kControlStateHot);
    }

    Invalidate();
    return true;
}

bool RichEdit::OnKillFocus(const EventArgs& msg)
{
    m_bActive = false;
    ShowCaret(false);
    m_bSelAllEver = false;
    if (m_bNoSelOnKillFocus && IsReadOnly() && IsEnabled()) {
        SetSelNone();
    }
    m_bTextInputMode = false;

    if (m_pClearButton != nullptr) {
        m_pClearButton->SetFadeVisible(false);
    }
    if (m_pShowPasswordButton != nullptr) {
        m_pShowPasswordButton->SetFadeVisible(false);
    }
    return BaseClass::OnKillFocus(msg);
}

void RichEdit::CheckSelAllOnFocus()
{
    if (IsEnabled() && !m_bSelAllEver) {
        m_bSelAllEver = true;
        if (m_bSelAllOnFocus) {
            SetSelAll();
            if (IsMultiLine()) {
                HomeUp();
            }
            else {
                HomeLeft();
            }
        }
    }
}

bool RichEdit::OnImeStartComposition(const EventArgs& /*msg*/)
{
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    HWND hWnd = GetWindowHWND();
    if (hWnd == nullptr) {
        return true;
    }

    HIMC hImc = ::ImmGetContext(hWnd);
    if (hImc == nullptr) {
        return true;
    }

    COMPOSITIONFORM    cfs = { 0, };
    UiPoint ptScrollOffset = GetScrollOffsetInScrollBox();
    POINT pt;

    int32_t xPos = 0;
    int32_t yPos = 0;
    GetCaretPos(xPos, yPos);

    pt.x = xPos - ptScrollOffset.x;
    pt.y = yPos - ptScrollOffset.y;

    //pt.y += (m_iCaretHeight + lf.lfHeight) / 4;
    cfs.dwStyle = CFS_POINT;
    if (pt.x < 1) {
        pt.x = 1;
    }
    if (pt.y < 1) {
        pt.y = 1;
    }
    cfs.ptCurrentPos = pt;
    ::ImmSetCompositionWindow(hImc, &cfs);
    ::ImmReleaseContext(hWnd, hImc);
    m_bIsComposition = true;
#endif
    return true;
}

bool RichEdit::OnImeEndComposition(const EventArgs& /*msg*/)
{
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    m_bIsComposition = false;
#endif
    return true;
}

void RichEdit::HandleEvent(const EventArgs& msg)
{
    if (IsDisabledEvents(msg)) {
        //If it is a mouse or keyboard message and the control is Disabled, forward it to the parent control
        Box* pParent = GetParent();
        if (pParent != nullptr) {
            pParent->SendEventMsg(msg);
        }
        else {
            BaseClass::HandleEvent(msg);
        }
        return;
    }
    if (msg.eventType == kEventKeyDown) {
        //Intercept the base class KeyDown event and handle it first
        if (OnKeyDown(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseWheel) {
        bool bCtrlDown = IsKeyDown(msg, ModifierKey::kControl);
        if (bCtrlDown && IsEnableWheelZoom()) {
            //Ctrl + mouse wheel, adjust the zoom ratio
            OnMouseWheel(msg.eventData, bCtrlDown);
            return;
        }
    }
    BaseClass::HandleEvent(msg);
}

bool RichEdit::OnKeyDown(const EventArgs& msg)
{
    //This function implements the various keyboard shortcuts supported
    if (msg.vkCode == kVK_SHIFT) {
        //Record the start position of the selection (when the Shift key is held down, this OnKeyDown event is triggered continuously)
        if (m_nShiftStartIndex == -1) {
            int32_t nSelEnd = 0;
            GetSel(m_nShiftStartIndex, nSelEnd);
        }
    }
    if (msg.vkCode == kVK_CONTROL) {
        //Record the start position of the selection (when the Ctrl key is held down, this OnKeyDown event is triggered continuously)
        if (m_nCtrlStartIndex == -1) {
            int32_t nSelEnd = 0;
            GetSel(m_nCtrlStartIndex, nSelEnd);
        }
    }
    if (OnCtrlArrowKeyDownScrollView(msg)) {
        return true;
    }
    else if (OnArrowKeyDown(msg)) {
        return true;
    }
    else if ((msg.vkCode == kVK_RETURN) || (msg.vkCode == kVK_TAB)) {
        OnInputChar(msg);
    }
    else if (msg.vkCode == kVK_ESCAPE) {
        //ESC key
        SendEvent(kEventEsc);
    }
    else if (msg.vkCode == kVK_DELETE) {
        //Delete key: delete the next character
        OnInputChar(msg);
    }
    else if (msg.vkCode == kVK_BACK) {
        //Backspace key: delete the previous character
        OnInputChar(msg);
    }
    else if ((msg.vkCode == 'A') && IsKeyDown(msg, ModifierKey::kControl)) {
        //Ctrl + A: select all
        SetSelAll();
    }
    else if ((msg.vkCode == 'C') && IsKeyDown(msg, ModifierKey::kControl)) {
        //Ctrl + C, copy
        Copy();
    }
    else if ((msg.vkCode == kVK_INSERT) && IsKeyDown(msg, ModifierKey::kControl)) {
        //Ctrl + Insert, copy
        Copy();
    }
    else if ((msg.vkCode == 'X') && IsKeyDown(msg, ModifierKey::kControl)) {
        //Ctrl + X, cut
        Cut();
    }
    else if ((msg.vkCode == 'V') && IsKeyDown(msg, ModifierKey::kControl)) {
        //Ctrl + V, paste
        Paste();
    }
    else if ((msg.vkCode == kVK_INSERT) && IsKeyDown(msg, ModifierKey::kShift)) {
        //Shift + Insert, paste
        Paste();
    }
    else if ((msg.vkCode == 'Z') && IsKeyDown(msg, ModifierKey::kControl)) {
        //Ctrl + Z, undo
        Undo();
    }
    else if ((msg.vkCode == 'Y') && IsKeyDown(msg, ModifierKey::kControl)) {
        //Ctrl + Y, redo
        Redo();
    }
    else if ((msg.vkCode == 'W') && IsKeyDown(msg, ModifierKey::kControl)) {
        //Ctrl + W, toggle automatic wrapping
        if (IsMultiLine() && !IsPasswordMode() && IsEnabled()) {
            SetWordWrap(!IsWordWrap());
        }
    }
    return true;
}

bool RichEdit::OnCtrlArrowKeyDownScrollView(const EventArgs& msg)
{
    bool bCtrlArrowKeyDown = false;
    if (IsKeyDown(msg, ModifierKey::kControl)) {
        if ((msg.vkCode == kVK_DOWN) || (msg.vkCode == kVK_UP) ||
            (msg.vkCode == kVK_NEXT) || (msg.vkCode == kVK_PRIOR) ||
            (msg.vkCode == kVK_HOME) || (msg.vkCode == kVK_END)) {
            bCtrlArrowKeyDown = true;
        }
    }
    if (!bCtrlArrowKeyDown) {
        return false;
    }

    if (msg.vkCode == kVK_HOME) {
        //Ctrl + Home
        InternalSetSel(0, 0);
        m_nSelXPos = -1;
    }
    else if (msg.vkCode == kVK_END) {
        //Ctrl + End
        int32_t nTextLen = GetTextLength();
        InternalSetSel(nTextLen, nTextLen);
        m_nSelXPos = -1;
    }
    //Hold Ctrl + arrow keys to trigger the ScrollBox functionality
    ScrollBar* pVScrollBar = GetVScrollBar();
    ScrollBar* pHScrollBar = GetHScrollBar();
    if ((pVScrollBar != nullptr) && pVScrollBar->IsValid() && pVScrollBar->IsEnabled()) {
        switch (msg.vkCode) {
        case kVK_DOWN:
            LineDown(GetLineScrollDeltaValue(true));
            break;
        case kVK_UP:
            LineUp(GetLineScrollDeltaValue(false));
            break;
        case kVK_NEXT:
            PageDown();
            break;
        case kVK_PRIOR:
            PageUp();
            break;
        case kVK_HOME:
            HomeUp();
            break;
        case kVK_END:
            EndDown(false);
            break;
        default:
            break;
        }
    }
    else if ((pHScrollBar != nullptr) && pHScrollBar->IsValid() && pHScrollBar->IsEnabled()) {
        switch (msg.vkCode) {
        case kVK_DOWN:
            LineRight();
            break;
        case kVK_UP:
            LineLeft();
            break;
        case kVK_NEXT:
            PageRight();
            break;
        case kVK_PRIOR:
            PageLeft();
            break;
        case kVK_HOME:
            HomeLeft();
            break;
        case kVK_END:
            EndRight();
            break;
        default:
            break;
        }
    }
    return true;
}

bool RichEdit::OnArrowKeyDown(const EventArgs& msg)
{
    bool bArrowKeyDown = false;
    if ((msg.vkCode == kVK_LEFT) || (msg.vkCode == kVK_RIGHT) ||
        (msg.vkCode == kVK_DOWN) || (msg.vkCode == kVK_UP)    ||
        (msg.vkCode == kVK_NEXT) || (msg.vkCode == kVK_PRIOR) ||
        (msg.vkCode == kVK_HOME) || (msg.vkCode == kVK_END)) {
        bArrowKeyDown = true;
    }
    if (!bArrowKeyDown) {
        return false;
    }
    if (IsNumberOnly() && !IsReadOnly() && IsEnabled() && ((msg.vkCode == kVK_UP) || (msg.vkCode == kVK_DOWN))) {
        //Number mode, the arrow keys adjust the number value
        if (msg.vkCode == kVK_UP) {
            AdjustTextNumber(1);
        }
        else {
            AdjustTextNumber(-1);
        }        
        return true;
    }

    if (msg.vkCode == kVK_LEFT) {
        //Left key
        m_nSelXPos = -1;
        int32_t nSelStartChar = -1;
        int32_t nSelEndChar = -1;
        GetSel(nSelStartChar, nSelEndChar);
        ASSERT(nSelEndChar >= nSelStartChar);

        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        bool bCtrlDown = IsKeyDown(msg, ModifierKey::kControl);
        if (bShiftDown && (m_nShiftStartIndex != -1)) {
            //Shift + Left key
            if (nSelEndChar <= m_nShiftStartIndex) {
                nSelStartChar = m_pTextData->GetPrevValidCharIndex(nSelStartChar);
                nSelEndChar = m_nShiftStartIndex;
                m_bSelForward = false;
            }
            else {
                nSelStartChar = m_nShiftStartIndex;
                nSelEndChar = m_pTextData->GetPrevValidCharIndex(nSelEndChar);
                m_bSelForward = true;
            }
            InternalSetSel(nSelStartChar, nSelEndChar);
            EnsureCharVisible(nSelStartChar);
        }
        else if (bCtrlDown && (m_nCtrlStartIndex != -1)) {
            //Ctrl + Left key
            if (nSelEndChar <= m_nCtrlStartIndex) {
                nSelStartChar = m_pTextData->GetPrevValidWordIndex(nSelStartChar);
                nSelEndChar = m_nCtrlStartIndex;
                m_bSelForward = false;
            }
            else {
                nSelStartChar = m_nCtrlStartIndex;
                nSelEndChar = m_pTextData->GetPrevValidWordIndex(nSelEndChar);
                m_bSelForward = true;
            }
            InternalSetSel(nSelStartChar, nSelEndChar);
            EnsureCharVisible(nSelStartChar);
        }
        else {
            //Left key
            nSelEndChar = m_pTextData->GetPrevValidCharIndex(nSelEndChar);
            InternalSetSel(nSelEndChar, nSelEndChar);            
            EnsureCharVisible(nSelEndChar);
            m_bSelForward = true;
        }
    }
    else if (msg.vkCode == kVK_RIGHT) {
        m_nSelXPos = -1;
        //Right key
        int32_t nSelStartChar = -1;
        int32_t nSelEndChar = -1;
        GetSel(nSelStartChar, nSelEndChar);
        ASSERT(nSelEndChar >= nSelStartChar);

        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        bool bCtrlDown = IsKeyDown(msg, ModifierKey::kControl);
        if (bShiftDown && (m_nShiftStartIndex != -1)) {
            //Shift + Right key
            if (nSelEndChar <= m_nShiftStartIndex) {
                nSelStartChar = m_pTextData->GetNextValidCharIndex(nSelStartChar);
                nSelEndChar = m_nShiftStartIndex;
                m_bSelForward = false;
            }
            else {
                nSelStartChar = m_nShiftStartIndex;
                nSelEndChar = m_pTextData->GetNextValidCharIndex(nSelEndChar);
                m_bSelForward = true;
            }
            InternalSetSel(nSelStartChar, nSelEndChar);
            EnsureCharVisible(nSelEndChar);
        }
        else if (bCtrlDown && (m_nCtrlStartIndex != -1)) {
            //Ctrl + Right key
            if (nSelEndChar <= m_nCtrlStartIndex) {
                nSelStartChar = m_pTextData->GetNextValidWordIndex(nSelStartChar);
                nSelEndChar = m_nCtrlStartIndex;
                m_bSelForward = false;
            }
            else {
                nSelStartChar = m_nCtrlStartIndex;
                nSelEndChar = m_pTextData->GetNextValidWordIndex(nSelEndChar);
                m_bSelForward = true;
            }
            InternalSetSel(nSelStartChar, nSelEndChar);
            EnsureCharVisible(nSelEndChar);
        }
        else {
            //Right key
            nSelEndChar = m_pTextData->GetNextValidCharIndex(nSelEndChar);
            InternalSetSel(nSelEndChar, nSelEndChar);
            EnsureCharVisible(nSelEndChar);
            m_bSelForward = true;
        }
    }
    else if (msg.vkCode == kVK_DOWN) {
        //Down key
        int32_t nSelStartChar = -1;
        int32_t nSelEndChar = -1;
        GetSel(nSelStartChar, nSelEndChar);
        ASSERT(nSelEndChar >= nSelStartChar);

        ASSERT(m_nRowHeight > 0);
        UiPoint pt = PosFromChar(nSelEndChar);
        if (m_nSelXPos == -1) {
            m_nSelXPos = pt.x;
        }
        else {
            pt.x = m_nSelXPos;
        }
        pt.y += m_nRowHeight;
        nSelEndChar = CharFromPos(pt);

        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        if (bShiftDown) {
            //Shift + Down key
            InternalSetSel(nSelStartChar, nSelEndChar);
        }
        else {
            InternalSetSel(nSelEndChar, nSelEndChar);
        }
        EnsureCharVisible(nSelEndChar);
        m_bSelForward = true;
    }
    else if (msg.vkCode == kVK_UP) {
        //Up key
        int32_t nSelStartChar = -1;
        int32_t nSelEndChar = -1;
        GetSel(nSelStartChar, nSelEndChar);
        ASSERT(nSelEndChar >= nSelStartChar);

        ASSERT(m_nRowHeight > 0);
        UiPoint pt = PosFromChar(nSelStartChar);
        if (m_nSelXPos == -1) {
            m_nSelXPos = pt.x;
        }
        else {
            pt.x = m_nSelXPos;
        }
        pt.y -= m_nRowHeight;
        nSelStartChar = CharFromPos(pt);

        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        if (bShiftDown) {
            //Shift + Up key
            InternalSetSel(nSelStartChar, nSelEndChar);
        }
        else {
            InternalSetSel(nSelStartChar, nSelStartChar);
        }
        EnsureCharVisible(nSelStartChar);
        m_bSelForward = false;
    }
    else if (msg.vkCode == kVK_NEXT) {
        //PageDown key
        int32_t nPageHeight = GetPageScrollDeltaValue(true);
        ASSERT(nPageHeight > 0);

        int32_t nSelStartChar = -1;
        int32_t nSelEndChar = -1;
        GetSel(nSelStartChar, nSelEndChar);
        ASSERT(nSelEndChar >= nSelStartChar);

        UiPoint pt = PosFromChar(nSelEndChar);
        if (m_nSelXPos == -1) {
            m_nSelXPos = pt.x;
        }
        else {
            pt.x = m_nSelXPos;
        }
        pt.y += nPageHeight;
        nSelEndChar = CharFromPos(pt);

        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        if (bShiftDown) {
            //Shift + PageDown key
            InternalSetSel(nSelStartChar, nSelEndChar);
        }
        else {
            InternalSetSel(nSelEndChar, nSelEndChar);
        }
        EnsureCharVisible(nSelEndChar);
        m_bSelForward = true;
    }
    else if (msg.vkCode == kVK_PRIOR) {
        //PageUp key
        int32_t nPageHeight = GetPageScrollDeltaValue(false);
        ASSERT(nPageHeight > 0);

        int32_t nSelStartChar = -1;
        int32_t nSelEndChar = -1;
        GetSel(nSelStartChar, nSelEndChar);
        ASSERT(nSelEndChar >= nSelStartChar);

        UiPoint pt = PosFromChar(nSelStartChar);
        if (m_nSelXPos == -1) {
            m_nSelXPos = pt.x;
        }
        else {
            pt.x = m_nSelXPos;
        }
        pt.y -= nPageHeight;
        nSelStartChar = CharFromPos(pt);
        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        if (bShiftDown) {
            //Shift + PageUp key
            InternalSetSel(nSelStartChar, nSelEndChar);
        }
        else {
            InternalSetSel(nSelStartChar, nSelStartChar);
        }
        EnsureCharVisible(nSelStartChar);
        m_bSelForward = false;
    }
    else if (msg.vkCode == kVK_HOME) {
        m_nSelXPos = -1;
        //HOME key
        int32_t nSelStartChar = -1;
        int32_t nSelEndChar = -1;
        GetSel(nSelStartChar, nSelEndChar);
        ASSERT(nSelEndChar >= nSelStartChar);

        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        if (bShiftDown && (m_nShiftStartIndex != -1)) {
            //Shift + Home key
            int32_t nShiftRowStartIndex = m_pTextData->GetRowStartCharIndex(m_nShiftStartIndex);
            int32_t nRowStartIndex = m_pTextData->GetRowStartCharIndex(nSelStartChar);
            int32_t nRowStartIndex2 = m_pTextData->GetRowStartCharIndex(nSelEndChar);
            if (nRowStartIndex == nRowStartIndex2) {
                //In the same row, no need to distinguish the operation direction
                if (nShiftRowStartIndex == nRowStartIndex) {
                    InternalSetSel(nRowStartIndex, m_nShiftStartIndex);
                }
                else {
                    InternalSetSel(nRowStartIndex, nSelEndChar);
                }
                EnsureCharVisible(nRowStartIndex);
            }
            else {
                //In different rows
                if (m_bSelForward) {
                    //Operation direction: forward
                    nSelEndChar = m_pTextData->GetRowStartCharIndex(nSelEndChar);
                    InternalSetSel(nSelStartChar, nSelEndChar);
                }
                else {
                    //Operation direction: backward
                    nSelStartChar = m_pTextData->GetRowStartCharIndex(nSelStartChar);
                    InternalSetSel(nSelStartChar, nSelEndChar);
                }
                EnsureCharVisible(nSelStartChar);
            }
        }
        else {
            //Home key
            nSelStartChar = m_pTextData->GetRowStartCharIndex(nSelStartChar);
            InternalSetSel(nSelStartChar, nSelStartChar);
            EnsureCharVisible(nSelStartChar);
            m_bSelForward = true;
        }
    }
    else if (msg.vkCode == kVK_END) {
        m_nSelXPos = -1;
        //END key
        int32_t nSelStartChar = -1;
        int32_t nSelEndChar = -1;
        GetSel(nSelStartChar, nSelEndChar);
        ASSERT(nSelEndChar >= nSelStartChar);

        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        if (bShiftDown && (m_nShiftStartIndex != -1)) {
            //Shift + End key
            int32_t nShiftRowEndIndex = m_pTextData->GetRowEndCharIndex(m_nShiftStartIndex);
            int32_t nRowEndIndex = m_pTextData->GetRowEndCharIndex(nSelStartChar);
            int32_t nRowEndIndex2 = m_pTextData->GetRowEndCharIndex(nSelEndChar);
            if (nRowEndIndex == nRowEndIndex2) {
                //In the same row, no need to distinguish the operation direction
                if (nShiftRowEndIndex == nRowEndIndex) {
                    InternalSetSel(m_nShiftStartIndex, nRowEndIndex);
                }
                else {
                    InternalSetSel(nSelStartChar, nRowEndIndex);
                }
                EnsureCharVisible(nRowEndIndex);
            }
            else {
                //In different rows
                if (m_bSelForward) {
                    //Operation direction: forward
                    nSelEndChar = m_pTextData->GetRowEndCharIndex(nSelEndChar);
                    InternalSetSel(nSelStartChar, nSelEndChar);
                }
                else {
                    //Operation direction: backward
                    nSelStartChar = m_pTextData->GetRowEndCharIndex(nSelStartChar);
                    InternalSetSel(nSelStartChar, nSelEndChar);
                }
                EnsureCharVisible(nSelEndChar);
            }
        }
        else {
            //End key
            nSelEndChar = m_pTextData->GetRowEndCharIndex(nSelEndChar);
            InternalSetSel(nSelEndChar, nSelEndChar);
            EnsureCharVisible(nSelEndChar);
            m_bSelForward = true;
        }
    }
    return true;
}

int32_t RichEdit::GetLineScrollDeltaValue(bool bLineDown) const
{
    int32_t nLineDeltaValue = DUI_NOSET_VALUE;
    if (m_nRowHeight > 0) {
        nLineDeltaValue = m_nRowHeight;
        UiSize64 scrollPos = GetScrollPos();
        if ((scrollPos.cy % nLineDeltaValue) != 0) {
            //Ensure alignment by row
            if (bLineDown) {
                nLineDeltaValue = nLineDeltaValue - (scrollPos.cy % nLineDeltaValue);
            }
            else {
                nLineDeltaValue = scrollPos.cy % nLineDeltaValue;
            }
        }
    }
    return nLineDeltaValue;
}

int32_t RichEdit::GetPageScrollDeltaValue(bool bPageDown) const
{
    UiRect rcDrawRect = GetTextDrawRect(GetRect());
    int32_t nPageDeltaValue = rcDrawRect.Height();
    //Align by row
    if (m_nRowHeight > 0) {
        UiSize64 scrollPos = GetScrollPos();
        if ((scrollPos.cy % m_nRowHeight) != 0) {
            //Ensure alignment by row
            if (bPageDown) {
                nPageDeltaValue = nPageDeltaValue - (scrollPos.cy % m_nRowHeight);
            }
            else {
                nPageDeltaValue += scrollPos.cy % m_nRowHeight;
                nPageDeltaValue -= m_nRowHeight;
            }
            if (nPageDeltaValue <= 0) {
                nPageDeltaValue = rcDrawRect.Height();
            }
        }        
    }    
    return nPageDeltaValue;
}

void RichEdit::CheckKeyDownStartIndex(const EventArgs& msg)
{
    if ((m_nShiftStartIndex != -1) && !IsKeyDown(msg, ModifierKey::kShift)) {
        //Restore the start position of the selection
        m_nShiftStartIndex = -1;
    }
    else if ((m_nCtrlStartIndex != -1) && !IsKeyDown(msg, ModifierKey::kControl)) {
        //Restore the start position of the selection
        m_nCtrlStartIndex = -1;
    }
}

bool RichEdit::OnKeyUp(const EventArgs& msg)
{
    CheckKeyDownStartIndex(msg);
    return BaseClass::OnKeyUp(msg);
}

bool RichEdit::OnChar(const EventArgs& msg)
{
    if (msg.modifierKey & ModifierKey::kIsSystemKey) {
        //Do not handle
        return true;
    }
    if ((msg.vkCode == kVK_RETURN) || (msg.vkCode == kVK_TAB) || (msg.vkCode == kVK_DELETE) || (msg.vkCode == kVK_BACK)) {
        //The handling of the Enter key, TAB key, Delete key, and Backspace key is unified in KEYDOWN
        return true;
    }
    //Input a character
    OnInputChar(msg);
    return true;
}

bool RichEdit::ButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    //When the mouse is clicked, check the key state (because when holding Shift/Ctrl, if a key combination is pressed, the Up message of Shift/Ctrl can be lost, causing an abnormal state)
    CheckKeyDownStartIndex(msg);

    OnLButtonDown(msg.ptMouse, msg.GetSender(), IsKeyDown(msg, ModifierKey::kShift));
    return bRet;
}

bool RichEdit::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    OnLButtonUp(msg.ptMouse, msg.GetSender());
    return bRet;
}

bool RichEdit::ButtonDoubleClick(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDoubleClick(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    OnLButtonDoubleClick(msg.ptMouse, msg.GetSender());
    return bRet;
}

bool RichEdit::RButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::RButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    OnRButtonDown(msg.ptMouse, msg.GetSender());
    return bRet;
}

bool RichEdit::RButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::RButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    OnRButtonUp(msg.ptMouse, msg.GetSender());
    return bRet;
}

bool RichEdit::MouseMove(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseMove(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    OnMouseMove(msg.ptMouse, msg.GetSender());
    return bRet;
}

bool RichEdit::MouseWheel(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseWheel(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    OnMouseWheel(msg.eventData, IsKeyDown(msg, ModifierKey::kControl));
    return bRet;
}

bool RichEdit::OnWindowKillFocus(const EventArgs& msg)
{
    bool bRet = BaseClass::OnWindowKillFocus(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    OnWindowKillFocus();
    return bRet;
}

void RichEdit::OnLButtonDown(const UiPoint& ptMouse, Control* pSender, bool bShiftDown)
{
    if (m_bInMouseMove) {
        m_bInMouseMove = false;
        Invalidate();
    }
    m_bMouseDownInView = (pSender == this) ? true : false;
    m_bMouseDown = true;
    m_pMouseSender = pSender;
    UiSize64 scrollPos = GetScrollPos();
    m_ptMouseDown.cx = ptMouse.x + scrollPos.cx;
    m_ptMouseDown.cy = ptMouse.y + scrollPos.cy;

    //Adjust the caret position to the mouse click position
    int32_t nCharPosIndex = CharFromPos(ptMouse);
    if (bShiftDown && (m_nShiftStartIndex != -1)) {
        //When the Shift key is held down, select the range from the original start point
        InternalSetSel(m_nShiftStartIndex, nCharPosIndex);
        m_bSelForward = (nCharPosIndex >= m_nShiftStartIndex) ? true : false;
    }
    else {
        InternalSetSel(nCharPosIndex, nCharPosIndex);
        m_bSelForward = true;
    }
    m_nSelXPos = -1;

    CheckSelAllOnFocus();
}

void RichEdit::OnLButtonUp(const UiPoint& /*ptMouse*/, Control* /*pSender*/)
{
    if (m_bInMouseMove) {
        m_bInMouseMove = false;
        Invalidate();
    }
    m_bMouseDownInView = false;
    m_bMouseDown = false;
    m_pMouseSender = nullptr;
}

void RichEdit::OnLButtonDoubleClick(const UiPoint& ptMouse, Control* /*pSender*/)
{
    if (IsReadOnly()) {
        //In read-only mode, double-click selects all text
        SetSelAll();
    }
    else {
        //In non-read-only mode, select a word
        int32_t nCharPosIndex = CharFromPos(ptMouse);
        int32_t nWordStartIndex = 0;
        int32_t nWordEndIndex = 0;
        if (m_pTextData->GetCurrentWordIndex(nCharPosIndex, nWordStartIndex, nWordEndIndex)) {
            InternalSetSel(nWordStartIndex, nWordEndIndex);
            m_nSelXPos = -1;
        }        
    }
}

void RichEdit::OnRButtonDown(const UiPoint& ptMouse, Control* pSender)
{
    if (m_bInMouseMove) {
        m_bInMouseMove = false;
        Invalidate();
    }
    m_bMouseDownInView = (pSender == this) ? true : false;
    m_bRMouseDown = true;
    m_pMouseSender = pSender;
    UiSize64 scrollPos = GetScrollPos();
    m_ptMouseDown.cx = ptMouse.x + scrollPos.cx;
    m_ptMouseDown.cy = ptMouse.y + scrollPos.cy;

    if (!HasSelText()) {
        //Adjust the caret position to the mouse click position
        int32_t nCharPosIndex = CharFromPos(ptMouse);
        InternalSetSel(nCharPosIndex, nCharPosIndex);
        m_nSelXPos = -1;
    }

    CheckSelAllOnFocus();
}

void RichEdit::OnRButtonUp(const UiPoint& /*ptMouse*/, Control* /*pSender*/)
{
    if (m_bInMouseMove) {
        m_bInMouseMove = false;
        Invalidate();
    }
    m_bMouseDownInView = false;
    m_bRMouseDown = false;
    m_pMouseSender = nullptr;
}

void RichEdit::OnMouseMove(const UiPoint& ptMouse, Control* pSender)
{
    if ((m_bMouseDown || m_bRMouseDown) &&
        (pSender != nullptr) &&
        (m_pMouseSender == pSender) && pSender->IsMouseFocused()) {
        UiSize64 scrollPos = GetScrollPos();
        m_ptMouseMove.cx = ptMouse.x + scrollPos.cx;
        m_ptMouseMove.cy = ptMouse.y + scrollPos.cy;

        //Only when the mouse moves more than the specified number of pixels does the drag operation begin, to avoid recognizing a normal click as a frame selection
        constexpr const int32_t minPt = 8;
        if (!m_bInMouseMove) {
            if ((std::abs(m_ptMouseMove.cx - m_ptMouseDown.cx) > minPt) ||
                (std::abs(m_ptMouseMove.cy - m_ptMouseDown.cy) > minPt)) {
                //Start the frame selection operation
                m_bInMouseMove = true;
                OnCheckScrollView();
            }
        }
        else {
            //Scroll the view as needed and update the mouse position after scrolling            
            OnCheckScrollView();
        }
    }
    else if (m_bInMouseMove) {
        m_bInMouseMove = false;
        Invalidate();
    }
}

void RichEdit::OnMouseWheel(int32_t wheelDelta, bool bCtrlDown)
{
    if (bCtrlDown && IsEnableWheelZoom()) {
        bool bZoomIn = wheelDelta > 0 ? true : false;
        uint32_t nZoomPercent = GetNextZoomPercent(GetZoomPercent(), bZoomIn);
        SetZoomPercent(nZoomPercent);        
    }
}

uint32_t RichEdit::GetNextZoomPercent(uint32_t nOldZoomPercent, bool bZoomIn) const
{
    //Set the zoom ratio by looking up a table (zooming in and out can restore to the original ratio)
    std::vector<uint32_t> zoomPercentList;
    uint32_t nZoomPercent = 100;
    while (nZoomPercent > 1) {
        nZoomPercent = (uint32_t)(nZoomPercent * 0.90f);
        zoomPercentList.insert(zoomPercentList.begin(), nZoomPercent);
    }
    nZoomPercent = 100;
    while (nZoomPercent < MAX_ZOOM_PERCENT) {
        zoomPercentList.insert(zoomPercentList.end(), nZoomPercent);
        nZoomPercent = (uint32_t)(nZoomPercent * 1.10f);
    }
    zoomPercentList.insert(zoomPercentList.end(), MAX_ZOOM_PERCENT);

    bool bFound = false;
    nZoomPercent = nOldZoomPercent;
    const size_t nPercentCount = zoomPercentList.size();
    for (size_t nPercentIndex = 0; nPercentIndex < nPercentCount; ++nPercentIndex) {
        if ((zoomPercentList[nPercentIndex] > nZoomPercent) || (nPercentIndex == (nPercentCount - 1))) {
            if (nPercentIndex <= 1) {
                size_t nCurrentIndex = 0;//Currently the first element
                if (bZoomIn) {
                    //Zoom in
                    nZoomPercent = zoomPercentList[nCurrentIndex + 1];
                }
                else {
                    //Zoom out (the minimum value has been reached, cannot zoom out further)
                    nZoomPercent = zoomPercentList[nCurrentIndex];
                }
            }
            else if (zoomPercentList[nPercentIndex] > nZoomPercent) {
                size_t nCurrentIndex = nPercentIndex - 1;//Middle element
                if (bZoomIn) {
                    //Zoom in
                    nZoomPercent = zoomPercentList[nCurrentIndex + 1];
                }
                else {
                    //Zoom out
                    nZoomPercent = zoomPercentList[nCurrentIndex - 1];
                }
            }
            else if (nPercentIndex == (nPercentCount - 1)) {
                size_t nCurrentIndex = nPercentCount - 1;//Currently the last element
                if (bZoomIn) {
                    //Zoom in (the maximum value has been reached, cannot zoom in further)
                    nZoomPercent = zoomPercentList[nCurrentIndex];
                }
                else {
                    //Zoom out
                    nZoomPercent = zoomPercentList[nCurrentIndex - 1];
                }
            }
            bFound = true;
            break;
        }
    }

    if (!bFound) {
        //If the table lookup fails, zoom in or out proportionally
        if (bZoomIn) {
            //Zoom in
            nZoomPercent = (uint32_t)(nZoomPercent * 1.10f);
            if (nZoomPercent == nOldZoomPercent) {
                //Avoid the value being too small to zoom in
                ++nZoomPercent;
            }
        }
        else {
            //Zoom out
            nZoomPercent = (uint32_t)(nZoomPercent * 0.91f);
        }
    }
    if (nZoomPercent < 1) {
        nZoomPercent = 1;
    }
    return nZoomPercent;
}

void RichEdit::OnWindowKillFocus()
{
    if (m_bInMouseMove) {
        Invalidate();
    }
    m_bMouseDownInView = false;
    m_bMouseDown = false;
    m_bRMouseDown = false;
    m_bInMouseMove = false;
    m_pMouseSender = nullptr;
}

void RichEdit::OnCheckScrollView()
{
    if (!m_bInMouseMove) {
        //Cancel the timer
        m_scrollViewFlag.Cancel();
        return;
    }
    bool bScrollView = false;
    const UiSize64 scrollPos = GetScrollPos();
    UiSize64 pt = m_ptMouseMove;
    pt.cx -= scrollPos.cx;
    pt.cy -= scrollPos.cy;
    const UiSize64 ptMouseMove = pt; //Record the original value

    if (m_bInMouseMove) {
        int32_t nHScrollValue = DUI_NOSET_VALUE;
        int32_t nVScrollValue = DUI_NOSET_VALUE;
        UiRect viewRect = GetRect();
        if (pt.cx <= viewRect.left) {
            //Scroll the view left
            LineLeft(nHScrollValue);
            bScrollView = true;
        }
        else if (pt.cx >= viewRect.right) {
            //Scroll the view right
            LineRight(nHScrollValue);
            bScrollView = true;
        }
        if (pt.cy <= viewRect.top) {
            //Scroll the view up
            LineUp(nVScrollValue);
            bScrollView = true;
        }
        else if (pt.cy >= viewRect.bottom) {
            //Scroll the view down
            LineDown(nVScrollValue);
            bScrollView = true;
        }
    }

    if (bScrollView) {
        UiSize64 scrollPosNew = GetScrollPos();
        if (scrollPos != scrollPosNew) {
            //Update the mouse position
            m_ptMouseMove.cx = ptMouseMove.cx + scrollPosNew.cx;
            m_ptMouseMove.cy = ptMouseMove.cy + scrollPosNew.cy;
        }

        //Start the timer
        m_scrollViewFlag.Cancel();
        GlobalManager::Instance().Timer().AddTimer(m_scrollViewFlag.GetWeakFlag(),
                                                   UiBind(&RichEdit::OnCheckScrollView, this),
                                                   50, 1); //Only execute once
    }
    else {
        //Cancel the timer
        m_scrollViewFlag.Cancel();
    }

    OnFrameSelection(m_ptMouseDown, m_ptMouseMove);
}

void RichEdit::OnFrameSelection(UiSize64 ptMouseDown64, UiSize64 ptMouseMove64)
{
    const UiSize64 scrollPos = GetScrollPos();//Used to restore the coordinate system of the character drawing area
    UiPoint ptMouseDown;    
    ptMouseDown.x = ui::TruncateToInt32(ptMouseDown64.cx - scrollPos.cx);
    ptMouseDown.y = ui::TruncateToInt32(ptMouseDown64.cy - scrollPos.cy);

    UiPoint ptMouseMove;
    ptMouseMove.x = ui::TruncateToInt32(ptMouseMove64.cx - scrollPos.cx);
    ptMouseMove.y = ui::TruncateToInt32(ptMouseMove64.cy - scrollPos.cy);

    //Trigger redraw, but no recalculation is needed
    Invalidate();

    int32_t nStart = CharFromPos(ptMouseDown);
    int32_t nEnd = CharFromPos(ptMouseMove);
    m_bSelForward = nEnd >= nStart ? true : false;

    InternalSetSel(nStart, nEnd);
    m_nSelXPos = -1;
}

void RichEdit::UpdateScrollRange()
{
    //Done through the SetPos of the base class
    SetPos(GetPos());
}

void RichEdit::OnInputChar(const EventArgs& msg)
{
    m_nSelXPos = -1;
    if (IsReadOnly() || !IsEnabled()) {
        //In read-only or Disable state, editing is forbidden
        return;
    }

    //Preprocessing of the TAB key and Enter key
    bool bEnableInputChar = true;
    if (msg.vkCode == kVK_TAB) {
        //TAB key pressed
        if (!m_bWantTab) {
            //Do not accept the TAB key, trigger the TAB key event
            bEnableInputChar = false;
            SendEvent(kEventTab);
        }
        else {
            //Accept the TAB key as an input character
            bEnableInputChar = true;
        }

        if (bEnableInputChar && IsPasswordMode()) {
            //In password mode, entering TAB characters is not supported
            bEnableInputChar = false;
        }
    }
    if (msg.vkCode == kVK_RETURN) {
        //Enter key pressed
        bool bShiftDown = IsKeyDown(msg, ModifierKey::kShift);
        bool bCtrlDown = IsKeyDown(msg, ModifierKey::kControl);
        if (bCtrlDown && !bShiftDown) {
            if (!m_bWantCtrlReturn) {
                //Do not accept Ctrl + Enter, trigger the Enter key event
                bEnableInputChar = false;
                SendEvent(kEventReturn);
            }
            else {
                //Accept Ctrl + Enter as an input character
                bEnableInputChar = true;
            }
        }
        else {
            if (!m_bWantReturn) {
                //Do not accept the Enter key, trigger the Enter key event
                bEnableInputChar = false;
                SendEvent(kEventReturn);                
            }
            else {
                //Accept the Enter key as an input character
                bEnableInputChar = true;
            }
        }
        if (bEnableInputChar && (!IsMultiLine() || IsPasswordMode())) {
            //In single-line mode or password mode, entering newline characters is not supported
            bEnableInputChar = false;
        }
    }

    if (!bEnableInputChar) {
        //No need to edit the text
        return;
    }

    //Get the text input this time
    DStringW text;
    if ((msg.vkCode == kVK_RETURN) || (msg.vkCode == kVK_TAB) || (msg.vkCode == kVK_DELETE) || (msg.vkCode == kVK_BACK)) {
        //The handling of the Enter key, TAB key, Delete key, and Backspace key, no input text
        #if defined(DUI_BUILD_FOR_SDL)
        ASSERT(msg.eventData != SDL_EVENT_TEXT_INPUT);
#endif
        if (msg.vkCode == kVK_RETURN) {
            //Enter: convert to a newline: "\r\n" or "\n"
#if defined (DUI_BUILD_FOR_WIN)
            text = L"\r\n";
#else
            text = L"\n";
#endif
        }
        else if (msg.vkCode == kVK_TAB) {
            //TAB key
            text = L"\t";
        }
    }
    else {
        #if defined(DUI_BUILD_FOR_SDL)
        ASSERT(msg.eventData == SDL_EVENT_TEXT_INPUT);
#endif
        ASSERT(msg.vkCode == kVK_None);
        #if defined(DUI_BUILD_FOR_SDL)
        if ((msg.eventData == SDL_EVENT_TEXT_INPUT) && (msg.wParam != 0) && (msg.lParam > 0)) {
#else
        if ((msg.wParam != 0) && (msg.lParam > 0)) {
#endif
            //The currently entered character or string (e.g. for Chinese input, the candidate word is entered at once, unlike the Windows SDK which enters character by character)
            text = (DStringW::value_type*)msg.wParam;
        }
    }

    //In password mode: remove illegal characters
    if (!text.empty() && IsPasswordMode()) {
        RemoveInvalidPasswordChar(text);
    }

    //In number mode, check whether there are characters that are not allowed (number mode: only numbers are allowed)
    if (!text.empty() && IsNumberOnly()) {
        size_t nTextIndex = 0;
        if (text[0] == _T('-')) {
            //The first character is a minus sign, corresponding to a negative number
            if (GetTextLength() > 0) {
                //Not the first character, entering a minus sign is forbidden
                return;
            }
            else if (GetMinNumber() >= 0) {
                //The minimum number is 0 or positive, entering a minus sign is forbidden
                return;
            }
            nTextIndex += 1;
        }
        const size_t nTextCount = text.size();
        for (; nTextIndex < nTextCount; ++nTextIndex) {
            if (text[nTextIndex] < L'0' || text[nTextIndex] > L'9') {
                //Non-number characters encountered, input is forbidden
                return;
            }
        }
    }

    //In restricted-character mode, check whether there are characters that are not allowed
    if (!text.empty() && (m_pLimitChars != nullptr)) {
        const size_t nTextCount = text.size();
        for (size_t nIndex = 0; nIndex < nTextCount; ++nIndex) {
            if (!IsInLimitChars(text[nIndex])) {
                //The character is not in the list, input is forbidden
                return;
            }
        }
    }

    //Input characters
    int32_t nSelStartChar = -1;
    int32_t nSelEndChar = -1;
    GetSel(nSelStartChar, nSelEndChar);
    ASSERT(nSelEndChar >= nSelStartChar);
    if (nSelEndChar < nSelStartChar) {
        //Error
        return;
    }

    if (msg.vkCode == kVK_DELETE) {
        //Delete key
        if (nSelEndChar > nSelStartChar) {
            //There is selected text: delete the selected content
            text.clear();
        }
        else {
            //No selected text: delete the next character
            text.clear();
            bool bMatchWord = IsKeyDown(msg, ModifierKey::kControl); //Ctrl + Delete key, delete the word after the caret
            if (bMatchWord) {
                nSelEndChar = m_pTextData->GetNextValidWordIndex(nSelStartChar);
            }
            else {
                nSelEndChar = m_pTextData->GetNextValidCharIndex(nSelStartChar);
            }            
        }
        if ((nSelEndChar == nSelStartChar) && text.empty()) {
            //The deletion condition is not satisfied
            return;
        }
    }
    else if (msg.vkCode == kVK_BACK) {
        //Backspace key
        if (nSelEndChar > nSelStartChar) {
            //There is selected text: delete the selected content
            text.clear();
        }
        else {
            //No selected text: delete the previous character
            text.clear();
            bool bMatchWord = IsKeyDown(msg, ModifierKey::kControl); //Ctrl + Backspace key, delete the word before the caret
            if (bMatchWord) {
                nSelStartChar = m_pTextData->GetPrevValidWordIndex(nSelStartChar);
            }
            else {
                nSelStartChar = m_pTextData->GetPrevValidCharIndex(nSelStartChar);
            }            
        }
        if ((nSelEndChar == nSelStartChar) && text.empty()) {
            //The deletion condition is not satisfied
            return;
        }
    }
    else if (text.empty()) {
        //No input text
        return;
    }

    //Whether to check number mode
    bool bCheckNumberOnly = IsNumberOnly() && ((GetMinNumber() != INT_MIN) || (GetMaxNumber() != INT_MAX));
    DStringW oldText;
    if (bCheckNumberOnly) {
        oldText = m_pTextData->GetText();
    }

    bool bRet = m_pTextData->ReplaceText(nSelStartChar, nSelEndChar, text, true);
    if (!bRet) {
        return;
    }
    int32_t nNewSelChar = nSelStartChar + (int32_t)text.size();
    InternalSetSel(nNewSelChar, nNewSelChar);

    //Flash password function (only when inputting from the end, the character flash function is provided)
    if (IsPasswordMode()) {
        m_falshPasswordFlag.Cancel();
    }
    if (IsPasswordMode() && !IsShowPassword() && IsFlashPasswordChar()) {
        if (nNewSelChar == GetTextLength()) {
            m_bInputPasswordChar = true;            
            std::function<void()> closure = UiBind(&RichEdit::StopFlashPasswordChar, this);
            GlobalManager::Instance().Timer().AddTimer(m_falshPasswordFlag.GetWeakFlag(), closure, 1500);
        }
    }

    //Update the scrollbar
    UpdateScrollRange();

    //Ensure the caret is visible
    EnsureCharVisible(nNewSelChar);

    bool bTextChanged = true;
    if (bCheckNumberOnly) {
        //In number mode, check whether the number corresponding to the text is within the range
        DString newText = GetText();
        if (!newText.empty()) {
            int64_t n = StringUtil::StringToInt64(newText);
            if (n < GetMinNumber()) {
                //Exceeds the minimum number, correct it
                int32_t newValue = GetMinNumber();
                SetTextNoEvent(StringUtil::Printf(_T("%d"), newValue));
                bTextChanged = (oldText != m_pTextData->GetText()) ? true : false;
            }
            else if (n > GetMaxNumber()) {
                //Exceeds the maximum number, correct it
                int32_t newValue = GetMaxNumber();
                SetTextNoEvent(StringUtil::Printf(_T("%d"), newValue));
                bTextChanged = (oldText != m_pTextData->GetText()) ? true : false;
            }
        }
    }

    //Trigger the text change event
    if (bTextChanged) {
        OnTextChanged();
    }
}

} // namespace ui

#endif //DUI_BUILD_FOR_SDL

