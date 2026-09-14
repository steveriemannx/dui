#include "AppDb.h"

AppDb::AppDb()
{
    // Add operations to open the db here 
}

AppDb::~AppDb()
{
    // Add operations to close the db here
}

bool AppDb::LoadFromDb(std::vector<AppItem>& app_list)
{
    app_list.clear();
    //select from....
    // Hard-coded for now
    {
        AppItem item;
        item.m_id = "1";
        item.m_icon = "1.png";
        item.m_isFrequent = false;
        item.m_name = "City Services";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "2";
        item.m_icon = "2.png";
        item.m_isFrequent = false;
        item.m_name = "Weather Forecast";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "3";
        item.m_icon = "3.png";
        item.m_isFrequent = true;
        item.m_name = "Attendance";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "4";
        item.m_icon = "4.png";
        item.m_isFrequent = true;
        item.m_name = "Payroll";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "5";
        item.m_icon = "5.png";
        item.m_isFrequent = false;
        item.m_name = "Test 5";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "6";
        item.m_icon = "5.png";
        item.m_isFrequent = false;
        item.m_name = "Test 6";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "7";
        item.m_icon = "5.png";
        item.m_isFrequent = false;
        item.m_name = "Test 7";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "8";
        item.m_icon = "5.png";
        item.m_isFrequent = false;
        item.m_name = "Test 8";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "9";
        item.m_icon = "5.png";
        item.m_isFrequent = false;
        item.m_name = "Test 9";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "10";
        item.m_icon = "5.png";
        item.m_isFrequent = false;
        item.m_name = "Test 10";
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "11";
        item.m_icon = "5.png";
        item.m_isFrequent = false;
        item.m_name = "Test 11";
        app_list.emplace_back(item);
    }
    
    return true;
}

bool AppDb::SaveToDb(const AppItem& item)
{
    // insert or replace into ....
    return true;
}
