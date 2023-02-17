//
// Created by lizhi on 2023/2/6.
//

#ifndef C_SQL_SELECTHELPER_H
#define C_SQL_SELECTHELPER_H

#include "base.h"

bool _select(vector<vector<string>> &res, const Table& table, const Column& column, const string& cond = "");
void printRes(vector<vector<string>> &res);

#endif //C_SQL_SELECTHELPER_H
