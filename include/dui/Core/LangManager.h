#ifndef UI_CORE_MULTILANG_H_
#define UI_CORE_MULTILANG_H_

#include "dui/Utils/FilePath.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ui 
{

/** Multi-language support
*/
class DUI_API LangManager
{
public:
    LangManager();
    ~LangManager();
    LangManager(const LangManager&) = delete;
    LangManager& operator = (const LangManager&) = delete;

public:
    /** Load all language mapping tables from a local file
     * @param[in] strFilePath The full path of the language file
     */
    bool LoadStringTable(const FilePath& strFilePath);

    /** Load all language mapping tables from memory
     * @param[in] fileData The data of the language mapping table to load
     */
    bool LoadStringTable(const std::vector<uint8_t>& fileData);

    /** Clear the multi-language resources
    */
    void ClearStringTable();

public:
    /** Get the string of the specified language by ID
     * @param[in] id The ID of the specified string
     * @return Returns the language string corresponding to the ID
     */
    DString GetStringViaID(const DString& id);

private:
    /** Analyze the content of the language mapping table
     * @param[in] list The list of mapping table content read out
     */
    bool AnalyzeStringTable(const std::vector<DString>& list);

private:
    /** Mapping table of the string ID and its value
    */
    std::unordered_map<DString, DString> m_stringTable;
};

}
#endif //UI_CORE_MULTILANG_H_
