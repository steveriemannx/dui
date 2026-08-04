#ifndef UI_UTILS_PROCESS_SINGLETON_DATA_H_
#define UI_UTILS_PROCESS_SINGLETON_DATA_H_

#include "dui/dui_defs.h"

#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <cstring>

namespace ui
{
/** Cross-process singleton implementation (communication protocol part)
*/
class DUI_API ProcessSingletonData
{
public:
    /** Protocol header definition
    */
    struct ProtocolHeader {
        uint32_t dwMagic;
        uint32_t dwVersion;
        uint32_t dwDataLength;
        uint32_t dwReserved;
    };
    
    /** Magic number of the protocol header
    */
    static constexpr uint32_t PROTOCOL_MAGIC = 0xC0DAFE12;
    
    /** Protocol version number
    */
    static constexpr uint32_t PROTOCOL_VERSION = 202503;
    
    /** Maximum length of data per communication
    */
    static constexpr size_t MAX_DATA_SIZE = 4096;
    
public:
    /** Serialize the protocol
    * @param vecArgs Arguments to be serialized; each item is at most 1024 bytes and the total data length is at most 4096 bytes
    */
    static std::string SerializeArguments(const std::vector<std::string>& vecArgs) 
    {
        std::stringstream ssData;
        for (const auto& strArg : vecArgs) {
            if (strArg.length() > 1024) {
                throw std::invalid_argument("Argument exceeds maximum length");
            }
            ssData << strArg << '\x1E';  // Use the unit separator
        }

        std::string strPayload = ssData.str();
        if (strPayload.size() > MAX_DATA_SIZE) {
            throw std::length_error("Total arguments exceed buffer size");
        }

        ProtocolHeader stHeader{
            PROTOCOL_MAGIC,
            PROTOCOL_VERSION,
            static_cast<uint32_t>(strPayload.size()),
            0
        };

        std::string strPackage(reinterpret_cast<char*>(&stHeader), sizeof(stHeader));
        strPackage += strPayload;
        return strPackage;
    }

    // Deserialize the protocol
    static std::vector<std::string> DeserializeData(const std::string& strPackage) 
    {
        if (strPackage.size() < sizeof(ProtocolHeader)) {
            throw std::runtime_error("Invalid package header");
        }

        ProtocolHeader stHeader;
        memcpy(&stHeader, strPackage.data(), sizeof(stHeader));

        if (stHeader.dwMagic != PROTOCOL_MAGIC) {
            throw std::runtime_error("Protocol magic mismatch");
        }

        if (stHeader.dwVersion > PROTOCOL_VERSION) {
            throw std::runtime_error("Unsupported protocol version");
        }

        if (stHeader.dwDataLength > MAX_DATA_SIZE) {
            throw std::runtime_error("Data length exceeds limit");
        }

        std::vector<std::string> vecArgs;
        std::string strPayload = strPackage.substr(sizeof(stHeader), stHeader.dwDataLength);
        std::string::size_type nPos = 0;
        while ((nPos = strPayload.find('\x1E')) != std::string::npos) {
            vecArgs.push_back(strPayload.substr(0, nPos));
            strPayload.erase(0, nPos + 1);
        }
        return vecArgs;
    }
};

}

#endif // UI_UTILS_PROCESS_SINGLETON_DATA_H_
