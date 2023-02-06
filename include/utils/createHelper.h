//
// Created by lizhi on 2023/2/6.
//

#ifndef C_SQL_CREATE_H
#define C_SQL_CREATE_H
#include "base.h"

bool create_database(const Database& db_name);
map<Column, Type> table_create_helper(vector<string> argv);
bool create_table(const Table& tb_name, map<Column, Type> Field);

#endif //C_SQL_CREATE_H
