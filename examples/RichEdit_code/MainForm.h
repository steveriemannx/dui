#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"

#include "RichEditFindReplace.h"

class FindForm;
class ReplaceForm;
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    /** Resource-related interface
     * The GetSkinFolder interface sets the skin resource path of the window you are drawing
     * The GetSkinFile interface sets the xml description file of the window you are drawing
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, allowing subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Called after the window has been closed, allowing subclasses to do some cleanup work
    */
    virtual void OnCloseWindow() override;

    /** Key pressed (WM_KEYDOWN or WM_SYSKEYDOWN)
    * @param [in] vkCode virtual key code
    * @param [in] modifierKey key modifier flags, valid values: ModifierKey::kFirstPress, ModifierKey::kAlt
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return the message handling result; if the application handled this message, it should return zero
    */
    virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

    /** Key pressed (WM_KEYUP or WM_SYSKEYUP)
    * @param [in] vkCode virtual key code
    * @param [in] modifierKey key modifier flags, valid values: ModifierKey::kAlt
    * @param [in] nativeMsg the raw message content received from the system
    * @param [out] bHandled whether the message has been handled; returning true means the message was handled successfully and no longer needs to be passed to the window procedure; returning false means the message continues to be passed to the window procedure
    * @return the message handling result; if the application handled this message, it should return zero
    */
    virtual LRESULT OnKeyUpMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

    // Find/Replace interface
    void FindRichText(const DString& findText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog);
    void ReplaceRichText(const DString& findText, const DString& replaceText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog);
    void ReplaceAllRichText(const DString& findText, const DString& replaceText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog);

    // Get the RichEdit interface
    ui::RichEdit* GetRichEdit() const;

private:
    // Load the default text content
    void LoadRichEditData();

    // Open, save, and save-as file operations
    void OnOpenFile();
    void OnSaveFile();
    void OnSaveAsFile();

    bool LoadFile(const ui::FilePath& filePath);
    bool SaveFile(const ui::FilePath& filePath);

    // Update the save state
    void UpdateSaveStatus();

private:
    // Find/Replace
    void OnFindText();
    void OnFindNext();
    void OnReplaceText();

private:// Font-related settings
    // Update the state of the font buttons
    void UpdateFontStatus();

    // Update the font size state
    void UpdateFontSizeStatus();

    // Set the font name
    void SetFontName(const DString& fontName);

    // Set the font size
    void SetFontSize(const DString& fontSize);

    // Adjust font size: bIncreaseFontSize is true to increase the font size, false to decrease it
    void AdjustFontSize(bool bIncreaseFontSize);

    // Set font style (bold)
    void SetFontBold(bool bBold);

    // Set font style (italic)
    void SetFontItalic(bool bItalic);

    // Set font style (underline)
    void SetFontUnderline(bool bUnderline);

    // Set font style (strikethrough)
    void SetFontStrikeOut(bool bStrikeOut);

    /** Set the text color
    */
    void SetTextColor(const DString& newColor);

private:
    // Update the zoom ratio
    void UpdateZoomValue();

    // Initialize the color Combo button
    void InitColorCombo();

    // Show the color picker window
    void ShowColorPicker();

    /** Get the next zoom percentage value
    * @param [in] nOldZoomPercent the current zoom percentage
    * @param [in] bZoomIn true means zoom in, false means zoom out
    */
    uint32_t GetNextZoomPercent(uint32_t nOldZoomPercent, bool bZoomIn) const;

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

    // Determine whether a file extension is an RTF file
    bool IsRtfFile(const DString& filePath) const;

    static DWORD CALLBACK StreamReadCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR* pcb);
    static DWORD CALLBACK StreamWriteCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR* pcb);

    // Call the system default dialog to set the font
    void OnSetFont();

    // Get the font structure
    bool GetRichEditLogFont(LOGFONTW& lf) const;

    // Initialize font information
    void InitCharFormat(const LOGFONTW& lf, CHARFORMAT2W& charFormat) const;

    /** Get the character format of the RichEdit control
*/
    void GetCharFormat(CHARFORMAT2W& charFormat) const;

    /** Set the character format of the RichEdit control
    */
    void SetCharFormat(CHARFORMAT2W& charFormat);

    // Convert the font size to the font height of the Rich Edit control
    int32_t ConvertToFontHeight(int32_t fontSize) const;

#endif

private:

    // RichEdit control interface
    ui::RichEdit* m_pRichEdit;

    // The currently open file
    ui::FilePath m_filePath;

    // The text of the Save button
    DString m_saveBtnText;

    // Find
    FindForm* m_pFindForm;

    // Replace
    ReplaceForm* m_pReplaceForm;

    // Find/Replace implementation
    RichEditFindReplace m_findReplace;

private:
    // Font name list
    std::vector<DString> m_fontList;

    // Font size list
    std::vector<ui::FontSizeInfo> m_fontSizeList;
};
#endif //EXAMPLES_MAIN_FORM_H_
