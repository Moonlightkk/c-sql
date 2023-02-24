//
// Created by lizhi on 2023/2/6.
//

#ifndef C_SQL_DROPHELPER_H
#define C_SQL_DROPHELPER_H
#include "base.h"
#include "fileHelper.h"
#include <FileIO.h>

bool _drop_database(const Database& database);
bool _drop_table(const Table& table);

#endif //C_SQL_DROPHELPER_H
