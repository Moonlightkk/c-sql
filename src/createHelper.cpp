//
// Created by lizhi on 2023/2/6.
//
#include "utils/createHelper.h"

bool _create_database(const Database& db_name) {
//    TODO

    return true;
}


unordered_map<Column, Type> table_create_helper(vector<string> argv, string& primary) {
    unordered_map<Column, Type> field;
    try {
        for (auto it = argv.begin(); it != argv.end(); it++) {
            if (*it == "primary") {
                if (!primary.empty()) {
                    cout << "only one primary key allowed" << endl;
                    return {};
                }
                primary = *(it - 2);
            } else {
                pair<Column, Type> each_field;
                each_field.first = *it++;
                each_field.second = *it;
                field.insert(each_field);
            }
        }
    }catch (...) {
        cout << "Wrong with arguments!" << endl;
        return {};
    }
    return field;
}

bool _create_table(const Table& tb_name, const unordered_map<Column, Type>& Field, const string& primary) {
    if (Field.empty()) {
        return false;
    }
    cout << "table " << tb_name << endl;
    cout << "+----------+----------+" << endl;
    for (auto & it : Field) {
        if (it.first == primary) {
            cout << "+ " << setw(9) << setfill(' ') << left << it.first + " (p)";
        } else {
            cout << "+ " << setw(9) << setfill(' ') << left << it.first;
        }
        cout << "+ " << setw(9) << setfill(' ') << left << it.second;
        cout << "+" << endl;
    }
    cout << "+----------+----------+" << endl;

//    创建表操作    成功 return true 失败 return false
//    TODO
//    if ( /* expression */ ) {
//        return true;
//    } else {
//        return false;
//    }
    return true;
}

