//
// Created by lizhi on 2023/2/6.
//

#ifndef C_SQL_TERMINAL_H
#define C_SQL_TERMINAL_H

#include "helpers.h"

using func = bool(*)(vector<string> argv);
using function_pair = pair<string, func>;


//enum operation {
//    create,
//    drop,
//    alter,
//    _select,
//    _insert,
//    rename,
//    upadte,
//};


extern map<string, func> function_map;

void start_terminal();
void command_processor(const string& command, bool& flag);
bool sql_handler(const string& sql_operation, vector<string> argv, bool& flag);
bool operation_register();

void clear();
void exit();
void help();
void show();


bool sql_create(vector<string> argv);
bool sql_select(vector<string> argv);
bool sql_insert(vector<string> argv);
bool sql_drop(vector<string> argv);
bool sql_delete(vector<string> argv);
bool sql_use(vector<string> argv);


#endif //C_SQL_TERMINAL_H
