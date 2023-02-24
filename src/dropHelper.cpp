//
// Created by lizhi on 2023/2/6.
//
#include "utils/dropHelper.h"


bool _drop_database(const Database& database) {
    // 如果没有找到database
    // return false;
    DB.dropDatabase(database);
    cout << "Database " << database << " dropped." << endl;

    return true;
}

bool _drop_table(const Table& table) {
    // return false;
    DB.dropTable(table);
    cout << "Table " << table << " dropped." << endl;

    return true;
}
