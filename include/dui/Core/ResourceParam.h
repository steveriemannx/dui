#ifndef UI_CORE_RESOURCE_PARAM_H_
#define UI_CORE_RESOURCE_PARAM_H_

#include "dui/Utils/FilePath.h"
#include <cstdint>

namespace ui
{
/** The type of resource (ZIP modes removed, only local files and memory)
*/
enum class ResourceType
{
    /** In the form of local files; all resources exist as local files
    */
    kLocalFiles,

    /** The resources are packaged into a custom binary archive (Qt qrc style, no zip container),
     *  embedded in the executable and accessed directly from memory
     */
    kMemoryRes
};

/** Basic parameters required to load global resources (base class; a subclass must be used when in use, see the definitions below)
*/
class DUI_API ResourceParam
{
protected:
    explicit ResourceParam(ResourceType resourceType): resType(resourceType) { }
    ~ResourceParam() { }

protected:
    /** The type of resource
    */
    ResourceType resType;

public:
    /** Get the resource type
    */
    ResourceType GetResType() const { return resType; }

    /** The location of the resource path (kLocalFiles: absolute path where resources reside)
    */
    FilePath resourcePath;

public:
    /** The theme path name (relative path)
    */
#if defined (DUI_BUILD_FOR_WIN)
    FilePath themePath = FilePath(_T("themes\\default"));
#else
    FilePath themePath = FilePath(_T("themes/default"));
#endif

    /** The path where the external font files reside
    */
    FilePath fontFilePath = FilePath(_T("fonts"));

    /** The path where the language files reside; can be a relative path or an absolute path (in the multilingual version, all language files are placed in this directory)
    *   If it is an absolute path, the language files are looked up in this absolute path
    *   If it is a relative path, the resource files are looked up by the relative path under the resource path determined by resType and resourcePath
    */
    FilePath languagePath = FilePath(_T("lang"));

    /** The file name of the currently used language file (without the path)
    */
    DString languageFileName = _T("zh_CN.txt");

    /** The file name of the global resource description XML file; default: "global.xml"
    */
    DString globalXmlFileName = _T("global.xml");
};

/** Parameters required to load global resources (local file form, corresponding to resource type: kLocalFiles)
*/
class DUI_API LocalFilesResParam: public ResourceParam
{
public:
    LocalFilesResParam() : ResourceParam(ResourceType::kLocalFiles)
    {
    }

    /** Construct parameters in the local file form
    * @param [in] resPath The path where the local resource files reside (absolute path)
    */
    explicit LocalFilesResParam(const FilePath& resPath) :
        ResourceParam(ResourceType::kLocalFiles)
    {
        resourcePath = resPath;
    }
};

/** Parameters required to load global resources (the resources are packaged into a custom binary
 *  archive embedded in the executable, corresponding to resource type: kMemoryRes; Qt qrc style)
 *  The embedded data is generated at build time by the embed_resources tool (cmake/embed_resources.cpp)
 */
class DUI_API MemoryResParam : public ResourceParam
{
public:
    MemoryResParam() : ResourceParam(ResourceType::kMemoryRes)
    {
    }

    /** Constructor with the embedded resource data
     * @param[in] data Pointer to the embedded resource data (must remain valid for the lifetime of
     *                 the application, e.g. a static array embedded in the executable)
     * @param[in] size Size of the embedded resource data
     */
    MemoryResParam(const uint8_t* data, size_t size) :
        ResourceParam(ResourceType::kMemoryRes), pData(data), nSize(size)
    {
    }

    /** The pointer to the embedded resource data (must remain valid for the lifetime of the application,
     *  e.g. a static array embedded in the executable)
    */
    const uint8_t* pData = nullptr;

    /** The size of the embedded resource data
    */
    size_t nSize = 0;
};

} // namespace ui

#endif // UI_CORE_RESOURCE_PARAM_H_
