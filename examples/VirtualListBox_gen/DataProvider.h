#ifndef EXAMPLES_DATA_PROVIDER_H_
#define EXAMPLES_DATA_PROVIDER_H_

// dui
#include "dui/dui.h"

#include <map>
#include <mutex>

struct DownloadTask
{
    int nId; //Unique ID
    bool bSelected = false;//Selection state
    DString::value_type* sName = nullptr;//Name
};

class DataProvider : public ui::VirtualListBoxElement
{
public:
    DataProvider();
    virtual ~DataProvider() override;

    /** Create a data element
    * @param [in] pVirtualListBox The interface of the associated virtual list
    * @return Returns the pointer of the created data element
    */
    virtual ui::Control* CreateElement(ui::VirtualListBox* pVirtualListBox) override;

    /** Fill the specified data element
    * @param[in] pControl Pointer to the data element control
    * @param[in] nElementIndex Index ID of the data element, range: [0, GetElementCount())
    */
    virtual bool FillElement(ui::Control* pControl, size_t nElementIndex) override;

    /**
    * @brief Get the total number of child elements
    * @return Returns the total number of child elements
    */
    virtual size_t GetElementCount() const override;

    /** Set the selection state
    * @param [in] nElementIndex Index ID of the data element, range: [0, GetElementCount())
    * @param [in] bSelected true means selected, false means not selected
    */
    virtual void SetElementSelected(size_t nElementIndex, bool bSelected) override;

    /** Get the selection state
    * @param [in] nElementIndex Index ID of the data element, range: [0, GetElementCount())
    * @return true means selected, false means not selected
    */
    virtual bool IsElementSelected(size_t nElementIndex) const override;

    /** Get the list of selected elements
    * @param [in] selectedIndexs Returns the list of currently selected elements, valid range: [0, GetElementCount())
    */
    virtual void GetSelectedElements(std::vector<size_t>& selectedIndexs) const override;

    /** Whether multi-selection is supported
    */
    virtual bool IsMultiSelect() const override;

    /** Set whether multi-selection is supported, called by the UI layer, keep consistent with the UI control
    * @return bMultiSelect true means multi-selection is supported, false means not supported
    */
    virtual void SetMultiSelect(bool bMultiSelect) override;

public:
    void SetTotal(int nTotal);
    void RemoveTask(size_t nIndex);
    void ChangeTaskName(size_t nIndex, const DString& sName);

private:
    int m_nTotal;
    std::vector<DownloadTask> m_vTasks;
    bool m_bMultiSelect; //Whether multi-selection is supported
    mutable std::mutex m_lock;
};

#endif //EXAMPLES_DATA_PROVIDER_H_
