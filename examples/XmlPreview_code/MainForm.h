//MainForm.h
#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"

/** Main window implementation of the application
*/
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin directory
    * @return The subclass must implement and return the window skin directory
    */
    virtual DString GetSkinFolder() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin XML description file
    * @return The subclass must implement and return the window skin XML description file
    *         The returned content can be the XML file content (a string starting with the character '<'),
    *         or a file path (a string not starting with the character '<'); the file must be found under the GetSkinFolder() path
    */
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Build the UI in pure code (corresponding to the xml_preview.xml layout)
    */
    void BuildUI();

private:
    /** Start the auto-refresh timer
    */
    void StartRefreshTimer(int32_t nIntervalSeconds);

    /** Stop the auto-refresh timer
    */
    void StopRefreshTimer();

    /** Check and refresh the preview result of the XML file
    */
    void CheckXmlPreview();

private:
    /** The path of the XML file currently being previewed
    */
    ui::FilePath m_xmlFilePath;

    /** XML file content, used to compare whether it has changed
    */
    std::vector<uint8_t> m_xmlFileData;

    /** The current auto-refresh timer event interval
    */
    int32_t m_nIntervalSeconds;

    /** The current timer ID
    */
    size_t m_timerId;
};

#endif //EXAMPLES_MAIN_FORM_H_
