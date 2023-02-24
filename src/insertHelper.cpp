//
// Created by lizhi on 2023/2/6.
//

#include "utils/insertHelper.h"

bool _insert(const Table &table, const vector <string>& values) {
//    cout << "_insert into '" << table << "' (";
//    for (auto it = values.begin(); it != values.end(); it++) {
//        if (it+1 != values.end())
//            cout << *it << ", ";
//        else
//            cout << *it;
//    }
//    cout << ")" << endl;
    DB.insert(table, values);
    return true;
}

