//
// Created by lizhi on 2023/2/6.
//

#ifndef C_SQL_CREATE_H
#define C_SQL_CREATE_H
#include "base.h"

bool _create_database(const Database& db_name);
unordered_map<Column, Type> table_create_helper(vector<string> argv, string& primary);
bool _create_table(const Table& tb_name, const unordered_map<Column, Type>& Field, const string& primary);

#endif //C_SQL_CREATE_H
