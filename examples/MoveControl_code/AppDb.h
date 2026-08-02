#ifndef EXAMPLES_APP_DB_H_
#define EXAMPLES_APP_DB_H_

#include "AppItemUi.h"

// Db operations related to the app
// This class is best implemented as a singleton
class AppDb
{
public:
    static AppDb& GetInstance()
    {
        static AppDb db;
        return db;
    }

    AppDb();
    ~AppDb();

    bool LoadFromDb(std::vector<AppItem>& app_list);
    bool SaveToDb(const AppItem& item);

private:

};

#endif //EXAMPLES_APP_DB_H_
