#ifndef UI_BOX_XMLBOX_H_
#define UI_BOX_XMLBOX_H_

#include "dui/Core/Box.h"
#include "dui/Utils/FilePath.h"

namespace ui
{
struct XmlPreviewAttributes;
class Shadow;

/** A container that supports loading and previewing XML files of the UI library, which can be used to preview the display effect of the controls defined in the XML file
*/
class DUI_API XmlBox : public Box
{
    typedef Box BaseClass;
public:
    explicit XmlBox(Window* pWindow);
    virtual ~XmlBox() override;

    //Virtual functions of the base class
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

public:
    /** Set the path of the XML file
    * @param [in] xmlPath The path of the XML file
    * @return Returns true if the XML data is loaded successfully, otherwise returns false
    */
    bool SetXmlFilePath(const FilePath& xmlPath);

    /** Get the path of the XML file
    */
    const FilePath& GetXmlFilePath() const;

    /** Set the path of the image resources (the resource root directory corresponding to the XML file)
    */
    void SetResPath(const FilePath& resPath);

    /** Get the path of the image resources (the resource root directory corresponding to the XML file)
    */
    const FilePath& GetResPath() const;

    /** Clear the preview control content of the UI
    */
    void ClearXmlBox();

    /** Get the full path of the loaded XML file (if it is a disk file, it is the absolute path)
    */
    const FilePath& GetXmlFileFullPath() const;

public:
    /** The callback function for the XML load completion event
     * @param [in] xmlPath The path of the XML file
     * @param [in] bSuccess true means the XML was loaded successfully, false means the XML failed to load
     */
    using LoadXmlCallback = std::function<void (const ui::FilePath& xmlPath, bool bSuccess)>;

    /** Add a callback function for the XML load completion event
    * @param [in] callback The callback function
    * @param [in] callbackId The ID of the callback function, used when removing the callback function; the caller must ensure the uniqueness of the ID
    */
    void AddLoadXmlCallback(LoadXmlCallback callback, size_t callbackId = 0);

    /** Remove a callback function for the XML load completion event
    * @param [in] callbackId The ID of the callback function to remove
    */
    void RemoveLoadXmlCallback(size_t callbackId);

protected:
    //Used to initialize xml attributes
    virtual void OnInit() override;

private:
    /** Load and fill in the controls defined in the XML
    */
    bool LoadXmlData(const FilePath& xmlPath);

    /** XML load completion notification
    */
    void OnXmlDataLoaded(const FilePath& xmlPath, bool bSuccess);

    /** Clear the loaded XML data and the preview control content of the UI
    * @param [in] xmlPreviewAttributesNew The latest window preview common attributes
    */
    void ClearLoadedXmlData(const XmlPreviewAttributes& xmlPreviewAttributesNew);

    /** Remove the elements in the old list that are contained in the new list
    */
    void RemoveValuesInNewList(std::vector<DString>& oldList, const std::vector<DString>& newList) const;

    /** Get the XML data and the XML path
    * @param [in] xmlInputPath The input XML path
    * @param [in] windowResPath The resource path of the window
    * @param [out] xmlFileData The read XML file data
    * @param [out] xmlOutputPath The output XML path, used to find the paths of the included XML files during parsing
    * @param [out] xmlResPath The resource file path corresponding to the XML file
    */
    bool ReadXmlFileData(const FilePath& xmlInputPath, const FilePath& windowResPath,
                         std::vector<unsigned char>& xmlFileData, FilePath& xmlOutputPath, FilePath& xmlResPath) const;

    /** Parse out the first-level directory from the relative path
    */
    FilePath GetFirstDirectory(const FilePath& resPath) const;

    /** Parse out the resource path from the absolute path
    */
    FilePath GetResDirectory(FilePath xmlFilePath, const FilePath& windowResPath) const;

private:
    /** The path of the XML file
    */
    FilePath m_xmlPath;

    /** The path of the image resources (the resource root directory corresponding to the XML file)
    */
    FilePath m_resPath;

    /** The image resource path corresponding to the current XML file (the resource root directory corresponding to the XML file)
    */
    FilePath m_xmlResPath;

    /** The full path of the loaded XML file (if it is a disk file, it is the absolute path)
    */
    FilePath m_xmlFileFullPath;

    /** The associated window common attributes and global attributes; when switching XML, these attributes need to be removed from the window object to avoid mutual interference
    */
    std::unique_ptr<XmlPreviewAttributes> m_pXmlPreviewAttributes;

    /** The sub-container interface added to the container
    */
    Box* m_pSubBox;

    /** The window shadow
    */
    std::unique_ptr<Shadow> m_pShadow;

    /** The data related to the callback functions of the XML load completion event
    */
    struct LoadXmlCallbackData
    {
        LoadXmlCallback m_callback;
        size_t m_callbackId;
    };
    std::vector<LoadXmlCallbackData> m_loadXmlCallbacks;
};

} //namespace ui

#endif // UI_BOX_XMLBOX_H_
