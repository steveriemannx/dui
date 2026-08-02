#ifndef UI_CORE_WINDOWBUILDER_H_
#define UI_CORE_WINDOWBUILDER_H_

#include "duilib/Core/UiTypes.h"
#include "duilib/Utils/FilePath.h"

namespace pugi
{
    //Definitions related to the XML parser
    class xml_document;
    class xml_node;
}

namespace ui 
{

class Box;
class Window;
class Control;
class RichTextSlice;
class RichTextImpl;
class WindowCreateAttributes;

/** The callback function for creating controls
*/
typedef std::function<Control* (const DString&)> CreateControlCallback;

/** The attribute list used to support XML preview; some attributes need to be removed from the associated window object after preview to restore the original state
*/
struct XmlPreviewAttributes
{
    /** The attribute list of the window tag (Window) in the XML
    */
    std::map<DString, DString> m_windowAttributes;

    /** The list of Class attributes added under the window during this parsing
    */
    std::vector<DString> m_windowClassList;

    /** The list of TextColor attributes added under the window during this parsing
    */
    std::vector<DString> m_windowTextColorList;

    /** The list of FontId attributes added to the global attributes during this parsing
    */
    std::vector<DString> m_globalFontIdList;
};

/** Parse XML files and create controls and layouts
*/
class DUILIB_API WindowBuilder
{
public:
    WindowBuilder();
    ~WindowBuilder();

    WindowBuilder(const WindowBuilder&) = delete;
    WindowBuilder& operator = (const WindowBuilder&) = delete;

public:
    /** Parse the XML file content
    * @param [in] xmlFileData The text content of the file; the string must start with the character '<';
    * @param [in] xmlFilePath Optional parameter providing the XML file path; when the XML data contains an Include tag, the included XML file will be looked up by the XML path
    * @return Returns true on success, otherwise false
    */
    bool ParseXmlData(const DString& xmlFileData, const FilePath& xmlFilePath = FilePath());
    bool ParseXmlData(const std::vector<unsigned char>& xmlFileData, const FilePath& xmlFilePath = FilePath());

    /** Parse the XML file content
    * @param [in] xmlFilePath The path of the XML file
    * @param [in] windowResPath The window resource subdirectory, used to look up the XML file (when no file path is specified)
    * @return Returns true on success, otherwise false
    */
    bool ParseXmlFile(const FilePath& xmlFilePath, const FilePath& windowResPath = FilePath());

    /** Create the window layout etc. from the XML file or data already parsed in the cache (i.e., the result of parsing via CreateFromXmlData and CreateFromXmlFile)
    * @param [in] pWindow The associated window; must not be nullptr, because DPI adaptation requires DPI scaling of the control sizes etc.
    * @param [in] pCallback The function that creates controls (or containers) by Class name, suitable for custom controls
    * @param [in] pParent The parent container; the nodes parsed from this XML file become child nodes of the pParent container
    * @param [in] pUserDefinedBox The user-defined parent container; the nodes parsed from this XML file become child nodes of the pUserDefinedBox container
    * @return If pUserDefinedBox is not nullptr, returns pUserDefinedBox; otherwise returns the first node interface generated after parsing the XML (which may be a Control or a Box)
    */
    Control* CreateControls(Window* pWindow,
                            CreateControlCallback pCallback = CreateControlCallback(),
                            Box* pParent = nullptr, 
                            Box* pUserDefinedBox = nullptr);

    /** Convert a control to a container; internally does some assertion handling to ensure an error is reported when the conversion fails
    */
    Box* ToBox(Control* pControl) const;

    /** Parse the window attributes
    *   (Only parses the attributes needed to create the window; some window attributes can only be specified at creation time and cannot be modified after the window is created, so they must be read out first and passed in as parameters when creating the window)
    */
    bool ParseWindowCreateAttributes(WindowCreateAttributes& createAttributes);

public:
    /** Parse the window attributes (attribute names are stored in the Map keys, and attribute values are stored in the attribute values)
    */
    bool ParseWindowAttributes(std::map<DString, DString>& windowAttributes) const;

    /** Get the list of Class attributes added under the window during this parsing
    */
    const std::vector<DString>& GetWindowClassList() const;

    /** Get the list of TextColor attributes added under the window during this parsing
    */
    const std::vector<DString>& GetWindowTextColorList() const;

    /** Get the list of FontId attributes added to the global attributes during this parsing
    */
    const std::vector<DString>& GetGlobalFontIdList() const;

public:
    /** Parse the formatted text content and set it on the RichText Control object
    * @param [in] xmlText The formatted text content
    * @param [in] pControl The interface of the RichText control
    */
    static bool ParseRichTextXmlText(const DString& xmlText, Control* pControl);
    
    /** Parse the formatted text content and set it on the RichText Control object
    * @param [in] xmlNode The XML node corresponding to the formatted text content
    * @param [in] pControl The interface of the RichText control
    * @param [in] pTextSlice The text slice node interface; if pTextSlice is not nullptr, the parsing result of the XML node will be filled into pTextSlice; otherwise it is filled into pControl
    */
    static bool ParseRichTextXmlNode(const pugi::xml_node& xmlNode, Control* pControl, RichTextSlice* pTextSlice = nullptr);

private:
    /** Parse the formatted text content and set it on the RichText Control object
    * @param [in] xmlNode The XML node corresponding to the formatted text content
    * @param [in] pControl The interface of the RichText control
    * @param [in] pTextSlice The text slice node interface; if pTextSlice is not nullptr, the parsing result of the XML node will be filled into pTextSlice; otherwise it is filled into pControl
    */
    static bool ParseRichTextXmlNode(const pugi::xml_node& xmlNode, RichTextImpl* pRichTextImpl, RichTextSlice* pTextSlice = nullptr);

private:
    /** Parse the window attributes (root XML node name: "Window")
    */
    void ParseWindowAttributes(Window* pWindow, const pugi::xml_node& root) const;

    /** Parse the shared resource attributes under the window (root XML node name: "Window"); these attributes can only be used by this window
    */
    void ParseWindowShareAttributes(Window* pWindow, const pugi::xml_node& root);

    /** Parse the attributes of the global resources (root XML node name: "Global"); these attributes can be used by all windows
    */
    void ParseGlobalAttributes(const pugi::xml_node& root);

    /** Parse the child nodes of an XML node
    * @param [in] xmlNode The xml node
    * @param [in] pParent The parent control, which may be a normal control (the parameter is only passed in and not used) or a container (converted to a container when used)
    * @return Returns the first created node, which may be a normal control or a container
    */
    Control* ParseXmlNodeChildren(const pugi::xml_node& xmlNode, Control* pParent = nullptr, Window* pWindow = nullptr);

    /** Create a control (or container) based on the Class name of the control
    */
    Control* CreateControlByClass(const DString& strControlClass, Window* pWindow);

    /** Create XML events (the XML nodes are <Event> or <BubbledEvent>)
    *   Example:
    *   <Option text="Single Choice" margin="8,0,0,0" border_round="2,2" valign="center">
    *       <Event type="buttonup" receiver="tree" apply_attribute="multi_select={false}" />
    *   </Option>
    */
    void AttachXmlEvent(bool bBubbled, const pugi::xml_node& node, Control* pParent);

    /** Determine whether the XML file exists
    */
    bool IsXmlFileExists(const FilePath& xmlFilePath) const;

    /** Parse the font node
    */
    void ParseFontXmlNode(const pugi::xml_node& xmlNode);

private:
    
    /** The XML document object currently being parsed
    */
    std::unique_ptr<pugi::xml_document> m_xml;

    /** The callback interface for creating Controls
    */
    CreateControlCallback m_createControlCallback;

    /** The path of the XML file currently being parsed
    */
    FilePath m_xmlFilePath;

private:
    /** The list of Class attributes added under the window during this parsing
    */
    std::vector<DString> m_windowClassList;

    /** The list of TextColor attributes added under the window during this parsing
    */
    std::vector<DString> m_windowTextColorList;

    /** The list of FontId attributes added to the global attributes during this parsing
    */
    std::vector<DString> m_globalFontIdList;
};

} // namespace ui

#endif // UI_CORE_WINDOWBUILDER_H_
