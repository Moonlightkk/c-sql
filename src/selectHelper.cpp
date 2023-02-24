//
// Created by lizhi on 2023/2/6.
//

#include "utils/selectHelper.h"

bool _select(vector<vector<string>> &res, const Table &table, const Column &column, const string &cond) {
    res = DB.select(table, column, cond);
    if (!res.empty()) {
        printRes(res);
    } else {
//        return false;
    }
    return true;
}

void printRes(vector<vector<string>> &res) {
    cout << "+----------------+----------------+" << endl;
    for (const auto& row: res) {
        for (const auto& col: row) {
            cout << "+ " << setw(15) << setfill(' ') << left << col;
            cout << "+";
        }
        cout << endl;
    }
    cout << "+----------------+----------------+" << endl;
}

