//
// Created by lizhi on 2023/2/6.
//
#include "utils/createHelper.h"

bool create_database(const Database& db_name) {
//    TODO

    return true;
}


map<Column, Type> table_create_helper(vector<string> argv) {
    map<Column, Type> field;
    try {
        for (auto it = argv.begin(); it != argv.end(); it++) {
            pair<Column, Type> each_field;
            each_field.first = *it++;
            each_field.second = *it;
            field.insert(each_field);
        }
    }catch (...) {
        cout << "Wrong with arguments!" << endl;
        return map<Column, Type>();
    }
    return field;
}

bool create_table(const Table& tb_name, map<Column, Type> field) {
    if (field.empty()) {
        return false;
    }
    cout << "create table " << tb_name << endl;
    cout << "+----------+----------+" << endl;
    for (auto it = field.begin(); it != field.end(); it++) {
        cout << "+ " << setw(9) << setfill(' ') << left << it->first;
        cout << "+ " << setw(9) << setfill(' ') << left << it->second;
        cout << "+" << endl;
    }
    cout << "+--------+--------+" << endl;

//    创建表操作    成功 return true 失败 return false
//    TODO
//    if ( /* expression */ ) {
//        return true;
//    } else {
//        return false;
//    }
    return true;
}

