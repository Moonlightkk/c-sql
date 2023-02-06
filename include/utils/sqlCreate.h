//
// Created by lizhi on 2023/2/6.
//

#ifndef C_SQL_CREATE_H
#define C_SQL_CREATE_H
#include "base.h"

enum CreateType {
    database,
    table
};

bool create_database(string db_name);
template <typename... Args> bool create_table(string tb_name, Args... args);

#endif //C_SQL_CREATE_H
