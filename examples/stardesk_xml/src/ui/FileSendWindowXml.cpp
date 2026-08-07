#include "FileSendWindowXml.h"

#include "examples/stardesk/src/app/Language.h"

#include "dui/Utils/FileDialog.h"

namespace sdk {

FileSendWindowXml::FileSendWindowXml(const std::string& peerHost, uint16_t peerFilePort,
                                     const uint8_t token[32])
    : m_peerHost(peerHost), m_peerFilePort(peerFilePort)
{
    std::memcpy(m_token, token, 32);
}

FileSendWindowXml::~FileSendWindowXml()
{
}

DString FileSendWindowXml::GetSkinFolder()
{
    return _T("stardesk_xml");
}

DString FileSendWindowXml::GetSkinFile()
{
    return _T("file_send.xml");
}

void FileSendWindowXml::OnInitWindow()
{
    ui::VBox* dropCard = dynamic_cast<ui::VBox*>(FindControl(_T("drop_card")));
    if (dropCard) {
        dropCard->AttachDropData([this](const ui::EventArgs& args) {
            OnFileDropped(args);
            return true;
        });
    }
    ui::Button* browse = dynamic_cast<ui::Button*>(FindControl(_T("browse_btn")));
    if (browse) {
        browse->AttachClick([this](const ui::EventArgs&) {
            OnBrowse();
            return true;
        });
    }
    ui::Button* send = dynamic_cast<ui::Button*>(FindControl(_T("send_btn")));
    if (send) {
        send->AttachClick([this](const ui::EventArgs&) {
            OnSend();
            return true;
        });
    }
    m_list = dynamic_cast<ui::VBox*>(FindControl(_T("file_list")));
    BaseClass::OnInitWindow();
}

// ---------------------------------------------------------------- file list

void FileSendWindowXml::AddFiles(const std::vector<ui::FilePath>& files)
{
    bool added = false;
    for (const ui::FilePath& f : files) {
        if (f.IsExistsFile()) {
            m_files.push_back(f);
            added = true;
        }
    }
    if (added) {
        RebuildRows();
    }
}

void FileSendWindowXml::OnFileDropped(const ui::EventArgs& args)
{
    std::vector<ui::FilePath> files;
#ifdef DUI_BUILD_FOR_WIN
    if (args.wParam == ui::kControlDropTypeWindows) {
        const ui::ControlDropData_Windows* data =
            (const ui::ControlDropData_Windows*)args.lParam;
        if (data != nullptr) {
            for (const DString& path : data->m_fileList) {
                files.emplace_back(ui::StringConvert::TToUTF8(path));
            }
        }
    }
#else
    if (args.wParam == ui::kControlDropTypeSDL) {
        const ui::ControlDropData_SDL* data = (const ui::ControlDropData_SDL*)args.lParam;
        if (data != nullptr && !data->m_bTextData) {
            for (const DString& path : data->m_fileList) {
                files.emplace_back(ui::StringConvert::TToUTF8(path));
            }
        }
    }
#endif
    AddFiles(files);
}

void FileSendWindowXml::OnBrowse()
{
    std::vector<ui::FilePath> files;
    ui::FileDialog dlg;
    if (dlg.BrowseForFiles(this, files)) {
        AddFiles(files);
    }
}

void FileSendWindowXml::RebuildRows()
{
    if (m_list == nullptr) {
        return;
    }
    m_list->RemoveAllItems();
    if (m_files.empty()) {
        ui::Label* empty = new ui::Label(this);
        empty->SetClass(_T("listitem"));
        empty->SetText(SDK_TR("file.empty"));
        empty->SetFontId(_T("system_12"));
        empty->SetAttribute(_T("textcolor"), _T("gray"));
        empty->SetFixedHeight(ui::UiFixedInt(24), true, true);
        m_list->AddItem(empty);
        return;
    }
    for (const ui::FilePath& f : m_files) {
        ui::Label* row = new ui::Label(this);
        row->SetClass(_T("listitem"));
        row->SetText(ui::StringConvert::UTF8ToT(f.GetFileName()) +
                     _T("   (") + ui::StringUtil::Printf(_T("%llu KB"),
                         (unsigned long long)(f.GetFileSize() / 1024)) + _T(")"));
        row->SetFontId(_T("system_12"));
        row->SetFixedHeight(ui::UiFixedInt(26), true, true);
        m_list->AddItem(row);
    }
}

void FileSendWindowXml::OnSend()
{
    if (m_files.empty()) {
        return;
    }
    std::string err;
    App::Instance().FileTx().SendFiles(
        m_peerHost, m_peerFilePort, m_token, m_files,
        [this](const FileTransfer::Item& item) {
            PostToUI(ui::UiBind(this, [this, item]() { OnItemUpdated(item.id, item); }));
        },
        err);
}

void FileSendWindowXml::OnItemUpdated(uint32_t id, const FileTransfer::Item& item)
{
    m_items[id] = item;
    if (m_list == nullptr) {
        return;
    }
    // refresh matching rows
    m_list->RemoveAllItems();
    size_t updated = 0;
    for (const ui::FilePath& f : m_files) {
        DString status;
        for (const auto& kv : m_items) {
            if (kv.second.name == ui::StringConvert::TToUTF8(f.GetFileName())) {
                switch (kv.second.state) {
                case FileTransfer::ItemState::Transferring:
                    status = ui::StringUtil::Printf(_T("  %llu / %llu KB"),
                                                    (unsigned long long)(kv.second.transferred / 1024),
                                                    (unsigned long long)(kv.second.size / 1024));
                    break;
                case FileTransfer::ItemState::Done:
                    status = _T("  ") + SDK_TR("file.done");
                    break;
                case FileTransfer::ItemState::Failed:
                    status = _T("  ") + SDK_TR("file.failed");
                    break;
                case FileTransfer::ItemState::Rejected:
                    status = _T("  ") + SDK_TR("file.rejected");
                    break;
                case FileTransfer::ItemState::Cancelled:
                    status = _T("  ") + SDK_TR("file.cancelled");
                    break;
                default:
                    status = _T("  ") + SDK_TR("file.waiting");
                    break;
                }
                ++updated;
                break;
            }
        }
        ui::Label* row = new ui::Label(this);
        row->SetClass(_T("listitem"));
        row->SetText(ui::StringConvert::UTF8ToT(f.GetFileName()) + status);
        row->SetFontId(_T("system_12"));
        row->SetFixedHeight(ui::UiFixedInt(26), true, true);
        m_list->AddItem(row);
    }
    (void)updated;
}

} // namespace sdk
