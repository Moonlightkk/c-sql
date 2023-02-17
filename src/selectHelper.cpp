//
// Created by lizhi on 2023/2/6.
//

#include "utils/selectHelper.h"

bool _select(vector<vector<string>> &res, const Table &table, const Column &column, const string &cond) {
    string cond_col, cond_val, cond_op;
    bool has_cond = false;
    if (!cond.empty()) {
        has_cond = true;
        bool is_col = true;
        for (auto ch: cond) {
            if (ch == '=' || ch == '<' || ch == '>') {
                cond_op.push_back(ch);
                is_col = false;
            } else {
                if (is_col) {
                    cond_col.push_back(ch);
                } else {
                    cond_val.push_back(ch);
                }
            }
        }
    }
    cout << cond_col << " " << cond_op << " " << cond_val << endl;

    if (column == "*") {
        cout << "select all" << endl;
        // TODO  _select *


        printRes(res);
        return true;
    } else {
        if (!has_cond) {
            cout << "select " << column << " from " << table << endl;
            // 获取数据写入res中
            // TODO


            printRes(res);
            return true;
        } else {
            cout << "select " << column << "from " << table << " with condition" << cond << endl;
            // cond_op 操作符
            // cond_col 列
            // cond_val 值
            // 条件查询，数据写入res
            // TODO

            printRes(res);
            return true;
        }
    }
//    return false;
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

