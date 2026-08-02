#include "DataProvider.h"
#include "Item.h"


int g_index = 1;

DataProvider::DataProvider():
    m_nTotal(0),
    m_bMultiSelect(true)
{
}


DataProvider::~DataProvider()
= default;

ui::Control* DataProvider::CreateElement(ui::VirtualListBox* pVirtualListBox)
{
    ASSERT(pVirtualListBox != nullptr);
    if (pVirtualListBox == nullptr) {
        return nullptr;
    }
    ASSERT(pVirtualListBox->GetWindow() != nullptr);
    //The Item template is built in code (item.xml is no longer loaded)
    Item* item = new Item(pVirtualListBox->GetWindow());
    return item;
}

bool DataProvider::FillElement(ui::Control* pControl, size_t nElementIndex)
{
    std::lock_guard<std::mutex> guard(m_lock);
    Item* pItem = dynamic_cast<Item*>(pControl);
    ASSERT(pItem != nullptr);
    ASSERT(nElementIndex < m_vTasks.size());
    if ((pItem == nullptr) || (nElementIndex >= m_vTasks.size())) {
        return false;
    }
    const DownloadTask& task = m_vTasks[nElementIndex];
    DString img = _T("icon.png");
    DString title = ui::StringUtil::Printf(_T("%s [%02d]"), task.sName, task.nId);
    pItem->InitSubControls(img, title, nElementIndex);
    return true;
}

size_t DataProvider::GetElementCount() const
{
    // Lock
    std::lock_guard<std::mutex> guard(m_lock);
    return m_vTasks.size();
}

void DataProvider::SetElementSelected(size_t nElementIndex, bool bSelected)
{
    std::lock_guard<std::mutex> guard(m_lock);
    const size_t nCount = m_vTasks.size();
    if (nElementIndex < nCount) {
        m_vTasks[nElementIndex].bSelected = bSelected;
        if (bSelected && !m_bMultiSelect) {
            for (size_t index = 0; index < nCount; ++index) {
                if ((index != nElementIndex) && m_vTasks[index].bSelected) {
                    m_vTasks[index].bSelected = false;
                }
            }
        }
    }
}

bool DataProvider::IsElementSelected(size_t nElementIndex) const
{
    std::lock_guard<std::mutex> guard(m_lock);
    bool bSelected = false;
    if (nElementIndex < m_vTasks.size()) {
        bSelected = m_vTasks[nElementIndex].bSelected;
    }
    return bSelected;
}

void DataProvider::GetSelectedElements(std::vector<size_t>& selectedIndexs) const
{
    selectedIndexs.clear();
    std::lock_guard<std::mutex> guard(m_lock);
    size_t nCount = m_vTasks.size();
    for (size_t nElementIndex = 0; nElementIndex < nCount; ++nElementIndex) {
        if (m_vTasks[nElementIndex].bSelected) {
            selectedIndexs.push_back(nElementIndex);
        }
    }
}

bool DataProvider::IsMultiSelect() const
{
    return m_bMultiSelect;
}

void DataProvider::SetMultiSelect(bool bMultiSelect)
{
    m_bMultiSelect = bMultiSelect;
}

void DataProvider::SetTotal(int nTotal)
{
    if (nTotal == m_nTotal) return;
    if (nTotal <= 0) return;
    m_nTotal = nTotal;

    // Lock
    m_lock.lock();
    for (auto task : m_vTasks) {
        delete [] task.sName;
    }
    m_vTasks.clear();
    DString name = _T("Task Name");
    m_vTasks.reserve(nTotal);
    for (auto i=0; i < nTotal; i++)
    {
        DownloadTask task;
        task.nId = i;
        //DString is not used here, because it consumes too much memory when the data volume reaches tens of millions or more
        task.sName = new DString::value_type[name.size() + 1];
        ui::StringUtil::StringCopy(task.sName, name.size() + 1, name.c_str());
        m_vTasks.emplace_back(std::move(task));
    }
    m_lock.unlock();

    // Notify TileBox of the change in total data count
    EmitCountChanged();
}

void DataProvider::RemoveTask(size_t nIndex)
{    
    m_lock.lock();
    bool bUpdated = false;
    if (nIndex < m_vTasks.size()) {
        auto iter = m_vTasks.begin() + nIndex;
        m_vTasks.erase(iter);
        bUpdated = true;
    }
    m_lock.unlock();

    if (bUpdated) {
        // Notify TileBox of the change in total data count
        EmitCountChanged();
    }    
}

void DataProvider::ChangeTaskName(size_t nIndex, const DString& sName)
{
    m_lock.lock();
    bool bUpdated = false;
    if (nIndex < m_vTasks.size()) {
        delete m_vTasks[nIndex].sName;
        m_vTasks[nIndex].sName = new DString::value_type[sName.size() + 1];
        ui::StringUtil::StringCopy(m_vTasks[nIndex].sName, sName.size() + 1, sName.c_str());
        bUpdated = true;
    }
    m_lock.unlock();

    // Send a data change notification
    if (bUpdated) {
        EmitDataChanged(nIndex, nIndex);
    }    
}
