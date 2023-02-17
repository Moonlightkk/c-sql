//
// Created by lizhi on 2023/2/6.
//

#include "utils/deleteHelper.h"

void _delete_table(const Table& table){
    // 删除整张表数据
    cout << "delete table " << table << endl;
    // TODO

}

void _delete_cond(const Table& table, const string& cond) {
    // 删除某些数据
    cout << "delete table " << table << " with condition " << cond << endl;
    // TODO

}


bool _delete(const Table &table, const string &cond) {
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

    if (has_cond) {
        _delete_table(table);
        return true;
    } else {
        _delete_cond(table, cond);
        return false;
    }
    return false;
}


