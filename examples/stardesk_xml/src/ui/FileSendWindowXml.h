#ifndef STARDESK_XML_UI_FILE_SEND_WINDOW_H_
#define STARDESK_XML_UI_FILE_SEND_WINDOW_H_

#include "dui/dui.h"

#include "examples/stardesk/src/app/AppConfig.h"

#include <map>

namespace sdk {

/** File-send window, XML mode: skin in stardesk_xml/file_send.xml. */
class FileSendWindowXml : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    FileSendWindowXml(const std::string& peerHost, uint16_t peerFilePort,
                      const uint8_t token[32]);
    virtual ~FileSendWindowXml() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void OnInitWindow() override;

private:
    void AddFiles(const std::vector<ui::FilePath>& files);
    void OnFileDropped(const ui::EventArgs& args);
    void OnBrowse();
    void OnSend();
    void OnItemUpdated(uint32_t id, const FileTransfer::Item& item);
    void RebuildRows();

    std::string m_peerHost;
    uint16_t m_peerFilePort = 0;
    uint8_t m_token[32] = {0};
    std::vector<ui::FilePath> m_files;
    std::map<uint32_t, FileTransfer::Item> m_items;
    ui::VBox* m_list = nullptr;
};

} // namespace sdk

#endif // STARDESK_XML_UI_FILE_SEND_WINDOW_H_
