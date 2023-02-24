//
// Created by lizhi on 2023/2/6.
//

#include "utils/deleteHelper.h"

void _delete_table(const Table& table){
    // 删除整张表数据
//    cout << "delete table " << table << endl;
    DB.remove(table, string());

}

void _delete_cond(const Table& table, const string& cond) {
    // 删除某些数据
//    cout << "delete table " << table << " with condition " << cond << endl;
    DB.remove(table, cond);
}


bool _delete(const Table &table, const string &cond) {
    if (cond.empty())
        _delete_table(table);
    else
        _delete_cond(table, cond);
}


