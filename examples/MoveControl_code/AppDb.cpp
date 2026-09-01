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
        item.m_icon = DUI_T("1.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("City Services");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "2";
        item.m_icon = DUI_T("2.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("Weather Forecast");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "3";
        item.m_icon = DUI_T("3.png");
        item.m_isFrequent = true;
        item.m_name = DUI_T("Attendance");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "4";
        item.m_icon = DUI_T("4.png");
        item.m_isFrequent = true;
        item.m_name = DUI_T("Payroll");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "5";
        item.m_icon = DUI_T("5.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("Test 5");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "6";
        item.m_icon = DUI_T("5.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("Test 6");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "7";
        item.m_icon = DUI_T("5.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("Test 7");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "8";
        item.m_icon = DUI_T("5.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("Test 8");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "9";
        item.m_icon = DUI_T("5.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("Test 9");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "10";
        item.m_icon = DUI_T("5.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("Test 10");
        app_list.emplace_back(item);
    }
    {
        AppItem item;
        item.m_id = "11";
        item.m_icon = DUI_T("5.png");
        item.m_isFrequent = false;
        item.m_name = DUI_T("Test 11");
        app_list.emplace_back(item);
    }
    
    return true;
}

bool AppDb::SaveToDb(const AppItem& item)
{
    // insert or replace into ....
    return true;
}
