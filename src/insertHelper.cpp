//
// Created by lizhi on 2023/2/6.
//

#include "utils/insertHelper.h"

bool _insert(const Table &table, const vector <string>& values) {
    // 如果没有找到table
    // TODO

    // cout << "Table '" << table << "' does not exist" << endl;
    // return false;

    cout << "_insert into '" << table << "' (";
    for (auto it = values.begin(); it != values.end(); it++) {
        if (it+1 != values.end())
            cout << *it << ", ";
        else
            cout << *it;
    }
    cout << ")" << endl;
    // TODO _insert values

        // 验证数据合法性
        // TODO

        // return false;

    // insert成功返回true
    return true;
}

