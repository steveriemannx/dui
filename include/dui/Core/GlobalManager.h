#ifndef UI_CORE_GLOBALMANAGER_H_
#define UI_CORE_GLOBALMANAGER_H_

#include "dui/Core/WindowBuilder.h"
#include "dui/Core/ColorManager.h"
#include "dui/Core/FontManager.h"
#include "dui/Core/ImageManager.h"
#include "dui/Core/ZipManager.h"
#include "dui/Core/LangManager.h"
#include "dui/Core/DpiManager.h"
#include "dui/Core/TimerManager.h"
#include "dui/Core/ThreadManager.h"
#include "dui/Core/ResourceParam.h"
#include "dui/Core/CursorManager.h"
#include "dui/Core/IconManager.h"
#include "dui/Core/WindowManager.h"
#include "dui/Image/ImageDecoderFactory.h"

#include <string>
#include <vector>
#include <map>
#include <thread>

namespace ui 
{
/** The render engine factory interface
*/
class IRenderFactory;

/** The global attribute management utility class
*   A utility class for managing some global attributes, including global styles (global.xml) and language settings, etc.
*/
class DUI_API GlobalManager
{
private:
    GlobalManager();
    ~GlobalManager();
    GlobalManager(const GlobalManager&) = delete;
    GlobalManager& operator = (const GlobalManager&) = delete;

public:
    /** Get the singleton object
    */
    static GlobalManager& Instance();

    /** Get the default resource directory
    * @param [in] bMacOsAppBundle Whether the MacOS platform uses the App Bundle
    */
    static FilePath GetDefaultResourcePath(bool bMacOsAppBundle);

public:
    /** The global settings initialization function
     * @param [in] resParam The resource related parameter, with the following options depending on the resource type
     *                      1. In the form of local files, all resources exist as local files
     *                         Use the LocalFilesResParam type as the parameter
     *                      2. The resource files are packaged into a zip archive and exist as local files
     *                         Use the ZipFileResParam type as the parameter
     *                      3. The resource files are packaged into a zip archive and placed in the resource file of the exe/dll
     *                         Use the ResZipFileResParam type as the parameter
     * @param [in] dpiInitParam The setting parameter of the DPI awareness mode and DPI value
     * @param [in] callback The global callback function when creating custom controls
     */
    bool Startup(const ResourceParam& resParam,
                 DpiInitParam dpiInitParam = DpiInitParam(),
                 const CreateControlCallback& callback = nullptr);

    /** Release the global resources
     */
    void Shutdown();

public:
    /** Set the path where the skin resources are located
     *   If resType == kLocalFiles, the local path (absolute path) where the resources are located needs to be set
     *   If resType == kZipFile or resType == kResZip, set the starting directory (relative path) where the resources are located, e.g. _T("resources\\")
     */
    void SetResourcePath(const FilePath& strPath);

    /** Get the path where the current resources are located
     */
    const FilePath& GetResourcePath() const;

    /** Reload the skin resources (dynamic skin changing can be implemented through this interface)
    * @param [in] resParam The resource related parameter, with the following options depending on the resource type
     *                      1. In the form of local files, all resources exist as local files
     *                         Use the LocalFilesResParam type as the parameter
     *                      2. The resource files are packaged into a zip archive and exist as local files
     *                         Use the ZipFileResParam type as the parameter
     *                      3. The resource files are packaged into a zip archive and placed in the resource file of the exe/dll
     *                         Use the ResZipFileResParam type as the parameter
     * @param [in] bInvalidate Whether to refresh the UI display: true means refresh the UI display after updating the language file, false means do not refresh the UI display
    */
    bool ReloadResource(const ResourceParam& resParam, bool bInvalidate = false);

    /** Set the platform related data, used on the Windows platform; when using a dynamic link library, set it to the module handle of the DLL (HMODULE)
    */
    void SetPlatformData(void* pPlatformData);

    /** Get the platform related data, used on the Windows platform; when using a dynamic link library, return the module handle of the DLL (HMODULE); if it is a static link library, return nullptr
    */
    void* GetPlatformData() const;

    /** Set the path where the font files are located
    */
    void SetFontFilePath(const FilePath& strPath);

    /** Get the path where the font files are located
    */
    const FilePath& GetFontFilePath() const;

public:
    /** Set the path where the language files are located, which can be a relative or absolute path (in the multi-language version, all language files are placed in this directory)
    *   If it is an absolute path, the language files are searched in this absolute path
    *   If it is a relative path, the resource files are searched by the relative path under the resource path determined by resType and resourcePath
    */
    void SetLanguagePath(const FilePath& strPath);

    /** Get the path where the language files are located
    */
    const FilePath& GetLanguagePath() const;

    /** Get the language file name, without the path
    */
    const DString& GetLanguageFileName() const;

    /** Reload the language resources (dynamic multi-language switching can be implemented through this interface)
     * @param [in] languagePath The path where the language files are located
                   If it is an absolute path, the language files are searched in the directory of this absolute path;
                   If it is empty, the language file path initialized at Startup is used;
                   If it is a relative path, it corresponds to the relative path in the zip package
     * @param [in] languageFileName The file name of the currently used language file (without the path)
     * @param [in] bInvalidate Whether to refresh the UI display: true means refresh the UI display after updating the language file, false means do not refresh the UI display
     */
    bool ReloadLanguage(const FilePath& languagePath = FilePath(),
                        const DString& languageFileName = _T("zh_CN.txt"),
                        bool bInvalidate = false);

    /** Get the language file list and display names (to support multi-language switching)
    * @param [in] languageNameID The string ID used to read the display name; if empty, the display name is not read
    * @param [out] languageList Returns the list of language files and display names
    */
    bool GetLanguageList(std::vector<std::pair<DString, DString>>& languageList,
                         const DString& languageNameID = _T("LANGUAGE_DISPLAY_NAME")) const;

public:
    /** Add a global Class attribute
     * @param[in] strClassName The global Class name
     * @param[in] strControlAttrList The attribute list, XML escaping is required
     */
    void AddClass(const DString& strClassName, const DString& strControlAttrList);

    /** Get the value of a global class attribute
     * @param[in] strClassName The global class name
     * @return Returns the class attribute value as a string
     */
    DString GetClassAttributes(const DString& strClassName) const;

    /** Remove all class attributes from the global attributes
     * @return Returns the draw area object
     */
    void RemoveAllClasss();

public:
    /** Stop an internal thread (the 3 threads kThreadWorker/kThreadImage1/kThreadImage2 are started by default internally; they can be stopped if not needed)
    */
    bool StopInnerThread(int32_t nThreadIdentifier);

    /** Start an internal thread (kThreadWorker/kThreadNetwork/kThreadImage1/kThreadImage2)
    */
    bool StartInnerThread(int32_t nThreadIdentifier);

public:
    /** Get the draw interface class object
    */
    IRenderFactory* GetRenderFactory();

    /** Get the color manager
    */
    ColorManager& Color();

    /** Get the font manager
    */
    FontManager& Font();

    /** Get the image manager
    */
    ImageManager& Image();

    /** The image format decoders
    */
    ImageDecoderFactory& ImageDecoders();

    /** Get the ICON resource manager
    */
    IconManager& Icon();

    /** Get the Zip manager
    */
    ZipManager& Zip();

    /** Get the DPI manager
    */
    DpiManager& Dpi();

    /** Get the timer manager
    */
    TimerManager& Timer();

    /** Get the thread manager
    */
    ThreadManager& Thread();

    /** The multi-language support manager
    */
    LangManager& Lang();

    /** The cursor manager
    */
    CursorManager& Cursor();

    /** The window manager
    */
    WindowManager& Windows();

public:
    /** Return the corresponding resource path according to the resource loading method
     * @param[in] path The resource path to get
     * @param [in] windowResPath The resource relative directory corresponding to the window, e.g. "controls\\"
     * @param [in] windowXmlPath The relative directory where the XML corresponding to the window is located, e.g. "controls\\menu\\"
     * @param [in] resPath The resource file path, e.g. "../public/button/btn_wnd_gray_min_hovered.png"
     * @param [in] pControl The Control interface associated with the resource
     * @param [out] bLocalPath Returns true if the file is a local path, returns false if the file is a path inside the zip package
     * @param [out] bResPath Returns true if the file is within the program resource path, returns false if it is not
     * @return Returns the available complete resource path; returns empty if the resource path does not exist
               The valid path formats returned are as follows:
              (1) If a ZIP package is used, returns: "resources\themes\default\public\button\btn_wnd_gray_min_hovered.png"
              (2) If no ZIP package is used, returns: "<program directory>\resources\themes\default\public\button\btn_wnd_gray_min_hovered.png"
     */
    FilePath GetExistsResFullPath(const FilePath& windowResPath, const FilePath& windowXmlPath, const FilePath& resPath);
    FilePath GetExistsResFullPath(const FilePath& windowResPath, const FilePath& windowXmlPath,
                                  const FilePath& resPath, const Control* pControl,
                                  bool& bLocalPath, bool& bResPath);

    /** The callback function for resource loading failure; a new resource search path can be provided in the callback function to retry finding the resource
     * @param [in] pControl The Control interface associated with the resource
     * @param [in] resPath The resource file path, e.g. "btn_wnd_gray_min_hovered.png"
     * @param [in,out] windowResPath The resource relative directory corresponding to the window, a new path can be returned
     * @param [in,out] windowXmlPath The relative directory where the XML corresponding to the window is located, a new path can be returned
     * @return true means a new resource search path has been provided and the resource search needs to be retried; false means no more search attempts will be made
     */
    using ResNotFoundCallback = std::function<bool(const Control* pControl,
                                                   const FilePath& resPath,
                                                   FilePath& windowResPath,
                                                   FilePath& windowXmlPath)>;
    /** Add a callback function for resource loading failure
    * @param [in] callback The callback function
    * @param [in] callbackId The ID of the callback function, used when removing the callback function; the caller ensures the uniqueness of the ID
    */
    void AddResNotFoundCallback(ResNotFoundCallback callback, size_t callbackId);

    /** Remove a callback function for resource loading failure
    * @param [in] callbackId The ID of the callback function to remove
    */
    void RemoveResNotFoundCallback(size_t callbackId);

    /** Determine whether a path is in the public subdirectory
    */
    bool IsResInPublicPath(const FilePath& resPath) const;

public:
    /** Usage notes for the CreateBox/CreateBoxWithCache and FillBox/FillBoxWithCache functions
     *  Assume the basic structure of the input XML file is as follows:
     *  <Window>
     *      <VBox bkcolor="white">
     *          <HBox height="30" margin="0,10,0,0">
     *              <Label text="Test"/>
     *          </HBox>
     *      </VBox>
     *  </Window>
     *  1. The root node of the XML file can be Global, Window, or other names:
     *           If it is Global: it can contain public resource definitions similar to "global.xml" (such as Class, etc.), and these resources are globally valid
     *           If it is Window: it can contain in-window public resource definitions similar to Window (such as Class, etc.), and these resources are valid within the window; the attributes of the Window tag are not parsed
     *           If it is another name, there is no special logic
     *  2. CreateBoxWithCache and FillBoxWithCache: the parsed XML result is cached, suitable for scenarios where the XML file is called repeatedly, improving performance (saving XML parsing time)
     *  3. The second-level nodes of the XML file (the nodes under the Window node in the above XML file): need to be containers, not Controls
     *  3. CreateBox/CreateBoxWithCache: parse the XML, create and return the corresponding second-level container node (i.e. the VBox node under Window in the XML):
     *                                    in the above XML file, a VBox node is created, and the function returns the VBox pointer, including the attributes of the VBox in the XML
     *  4. FillBox/FillBoxWithCache: parse the XML file, parse and create the third-level nodes, fill the third-level nodes (HBox) and their child nodes into the pUserDefinedBox container passed in the function parameter,
     *                               and also parse the attributes of the second-level node (VBox) and set them into the pUserDefinedBox container
     *                               This function does not create the container of the second-level node (i.e. the VBox node in the above XML); the function considers the node passed as pUserDefinedBox to be the corresponding VBox node, created externally
     */

    /** Create a Box according to the XML (creates the container corresponding to the second-level node, and returns the container corresponding to the second-level node)
     * @param [in] pWindow The associated window, must not be nullptr, because DPI adaptation requires DPI scaling of the control sizes etc.
     * @param [in] strXmlPath The XML file path
     * @param [in] callback The callback handling function for custom controls
     * @return The object pointer of the specified layout module
     */
    Box* CreateBox(Window* pWindow, const FilePath& strXmlPath, CreateControlCallback callback = CreateControlCallback());

    /** Create a Box according to the XML (creates the container corresponding to the second-level node, and returns the container corresponding to the second-level node); the XML parsing result is kept in the cache and the XML is not re-parsed on the next call, to improve performance
     * @param [in] pWindow The associated window, must not be nullptr, because DPI adaptation requires DPI scaling of the control sizes etc.
     * @param [in] strXmlPath The XML file path
     * @param [in] callback The callback handling function for custom controls
     * @return The object pointer of the specified layout module
     */
    Box* CreateBoxWithCache(Window* pWindow, const FilePath& strXmlPath, CreateControlCallback callback = CreateControlCallback());

    /** According to the XML parsing result, append the content of the third-level nodes into pUserDefinedBox, but the container corresponding to the second-level node is not created; the function considers the container corresponding to the third-level nodes to be pUserDefinedBox, created externally
     *  (Note: this function skips the root node and the first-level child nodes of the XML file, and directly parses and appends the content of the third-level nodes into pUserDefinedBox as its child nodes)
     * @param [in] pUserDefinedBox The box pointer to be filled, must not be nullptr, and the control must be associated with a window
     * @param [in] strXmlPath The XML file path
     * @param [in] callback The callback handling function for custom controls
     */
    bool FillBox(Box* pUserDefinedBox, const FilePath& strXmlPath, CreateControlCallback callback = CreateControlCallback());

    /** According to the XML parsing result, append the content of the third-level nodes into pUserDefinedBox, but the container corresponding to the second-level node is not created; the function considers the container corresponding to the third-level nodes to be pUserDefinedBox, created externally
     *  (Note: this function skips the root node and the first-level child nodes of the XML file, and directly parses and appends the content of the third-level nodes into pUserDefinedBox as its child nodes)
     *  The XML parsing result is kept in the cache and the XML is not re-parsed on the next call, to improve performance
     * @param [in] pUserDefinedBox The box pointer to be filled, must not be nullptr, and the control must be associated with a window
     * @param [in] strXmlPath The XML file path
     * @param [in] callback The callback handling function for custom controls
     */
    bool FillBoxWithCache(Box* pUserDefinedBox, const FilePath& strXmlPath, CreateControlCallback callback = CreateControlCallback());

    /** The global callback function after a custom control is created
     * @param [in] strControlName The name of the custom control
     * @return Returns the object pointer of a custom control
     */
    Control* CreateControl(const DString& strControlName);

    /** Add a control creation function for creating user custom controls
    */
    void AddCreateControlCallback(const CreateControlCallback& pfnCreateControlCallback);

    /** Create a Box according to the XML path (creates the container corresponding to the second-level node, and returns the container corresponding to the second-level node), used to generate a preview of the XML effect
     * @param [in] pWindow The associated window, must not be nullptr, because DPI adaptation requires DPI scaling of the control sizes etc.
     * @param [in] xmlFilePath The XML file path
     * @param [in] xmlPreviewAttributes The newly added window shared attributes and global attributes when parsing the XML file
     * @return The object pointer of the specified layout module
     */
    Box* CreateBoxForXmlPreview(Window* pWindow, const FilePath& xmlFilePath, XmlPreviewAttributes& xmlPreviewAttributes);

    /** Create a Box according to the XML data (creates the container corresponding to the second-level node, and returns the container corresponding to the second-level node), used to generate a preview of the XML effect
     * @param [in] pWindow The associated window, must not be nullptr, because DPI adaptation requires DPI scaling of the control sizes etc.
     * @param [in] xmlFileData The XML file data; if empty, xmlFilePath is used
     * @param [in] xmlPreviewAttributes The newly added window shared attributes and global attributes when parsing the XML file
     * @param [in] xmlFilePath An optional parameter when xmlFileData is not empty, providing the XML file path; when the XML data contains an Include tag, the included XML file is searched by the XML path
     * @return The object pointer of the specified layout module
     */
    Box* CreateBoxForXmlPreview(Window* pWindow,
                                const std::vector<unsigned char>& xmlFileData,
                                XmlPreviewAttributes& xmlPreviewAttributes,
                                const FilePath& xmlFilePath = FilePath());

public:
    /** Determine whether the current thread is the UI thread
    */
    bool IsInUIThread() const;

    /** Trigger an assertion error if this function is not called in the UI thread
    */
    void AssertUIThread() const;

    /** Add a function to be executed on exit
    */
    void AddAtExitFunction(std::function<void()> atExitFunction);

public:
    /** Set whether control animations are enabled (e.g. fade in/out animations)
    * @param [in] bEnable true means control animations are enabled, false means control animations are not enabled
    */
    void SetAnimationEnabled(bool bEnable);

    /** Get whether control animations are enabled (e.g. fade in/out animations)
    */
    bool IsAnimationEnabled() const;

private:
    /** Remove all images from the cache
     */
    void RemoveAllImages();

    /** Check whether the image file path exists
    * @param [in,out] imageFullPath Cleared if it does not exist, kept if it exists
    * @param [out] bLocalPath Returns true if the file is a local path, returns false if the file is a path inside the zip package
    */
    void CheckImagePath(FilePath& imageFullPath, bool& bLocalPath);

    /** Return the corresponding resource path according to the resource loading method
    */
    FilePath FindExistsResFullPath(const FilePath& windowResPath, const FilePath& windowXmlPath,
                                   const FilePath& resPath, bool& bLocalPath, bool& bResPath);

private:
    /** Data related to the resource loading failure callback function
    */
    struct ResNotFoundCallbackData
    {
        ResNotFoundCallback m_callback;
        size_t m_callbackId;
    };
    std::vector<ResNotFoundCallbackData> m_resNotFoundCallbacks;

    /** The render engine management interface
    */
    std::unique_ptr<IRenderFactory> m_renderFactory;

    /** The global resource path, modified when changing the skin (absolute path)
    */
    FilePath m_resourcePath;

    /** Platform related data (an optional parameter; if not filled in, the default value nullptr is used)
    *   Windows platform: the module handle (HMODULE) where the resources are located; if nullptr, the handle of the current exe is used (optional parameter)
    */
    void* m_platformData;

    /** The global font file path (absolute path)
    */
    FilePath m_fontFilePath;

    /** The global language file path (absolute path)
    */
    FilePath m_languagePath;

    /** The global language file name (without the path)
    */
    DString m_languageFileName;

    /** The window building management interface, KEY is the XML file path, VALUE is the window building management interface (the already parsed XML, avoiding repeated parsing)
    */
    std::map<FilePath, std::unique_ptr<WindowBuilder>> m_builderMap;

    /** The control creation functions, used for creating user custom controls
    */
    std::vector<CreateControlCallback> m_pfnCreateControlCallbackList;

    /** The name (KEY) and attribute list (VALUE) of each Class (e.g. the Classes defined in global.xml)
    */
    std::map<DString, DString> m_globalClass;

    /** The main thread ID
    */
    std::thread::id m_dwUiThreadId;

    /** The color manager
    */
    ColorManager m_colorManager;

    /** The font manager
    */
    FontManager m_fontManager;

    /** The image manager
    */
    ImageManager m_imageManager;

    /** The image format decoders
    */
    ImageDecoderFactory m_imageDecoderFactory;

    /** The ZIP package manager
    */
    ZipManager m_zipManager;

    /** The DPI manager
    */
    DpiManager m_dpiManager;

    /** The timer manager
    */
    TimerManager m_timerManager;

    /** The thread manager
    */
    ThreadManager m_threadManager;

    /** The multi-language manager
    */
    LangManager m_langManager;

    /** The ICON resource manager
    */
    IconManager m_iconManager;

    /** The cursor manager
    */
    CursorManager m_cursorManager;

    /** The window manager
    */
    WindowManager m_windowManager;

    /** The functions to be executed on exit
    */
    std::vector<std::function<void()>> m_atExitFunctions;

    /** The thread pool used inside the library
    */
    std::vector <std::shared_ptr<FrameworkThread>> m_threadList;

    /** Whether control animations are enabled (e.g. fade in/out animations)
    */
    bool m_bAnimationEnabled;
};

} // namespace ui

#endif // UI_CORE_GLOBALMANAGER_H_
