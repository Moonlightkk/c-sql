//
// Created by lizhi on 2023/2/6.
//

#include <terminal.h>

map<string, func> function_map;

void start_terminal() {
    std::ios_base::sync_with_stdio(false);
    cout << "Welcome to c-sql monitor.  Commands end with ;." << endl;
    cout << "Type 'help;' or for help. Type 'clear;' to clear the current input statement." << endl;
    cout << "Type 'exit;' to exit." << endl;
    bool flag = true;
    string input;
    string command;
    cout << "c-sql>>>";
    while (flag && getline(cin, input)) {
        if (input.empty()) {
            cout << "c-sql>>>";
            continue;
        }
        transform(input.begin(), input.end(), input.begin(), tolower);
        if (input.at(input.length() - 1) != ';') {
            command += input;
            command += " ";
            cout << "\\";
        } else {
            bool isLegal = true;
            command += input;

            // 丐版 输入合法性判断
            for (auto &ch: command) {
                if (!((ch >= 'a' && ch <= 'z') || ch == ';' || ch == ' ' || ch == '(' || ch == ')' || ch == ',' ||
                      isdigit(ch) || ch == '*')) {
                    isLegal = false;
                    cout << "Command format error. Use 'help;' to get help." << endl;
                    cout << "c-sql>>>";
                    break;
                }
            }

            // 替换 ( ) ,
            for (auto it = command.begin(); it != command.end(); it++) {
                if (*it == '(' || *it == ')' || *it == ',') {
                    if (*(it + 1) == ' ') {
                        command.erase(it);
                    } else {
                        *it = ' ';
                    }
                }
            }

            if (isLegal) {
                command_processor(command, flag);
            }
            command = "";
        }
    }
}

void command_processor(const string &command, bool &flag) {
//    cout << command << endl;
    string _command = command;
    _command.pop_back();
    stringstream ss(_command);
    string sql_operation;
    ss >> sql_operation;
    vector<string> argv;
    string argc;
    while (ss >> argc) {
        if (argc == ";") {
            break;
        }
        argv.push_back(argc);
    }
    if (sql_handler(sql_operation, argv, flag)) {
        // 命令有效后操作
//        cout << "success!" << endl;
    } else {
        cout << "Incorrect command '" + command << "'." << endl;
        cout << "Use 'help;' to get help." << endl;
        cout << "c-sql>>>";
    }
}

bool sql_handler(const string &sql_operation, vector<string> argv, bool &flag) {
    if (sql_operation == "exit") {
        flag = false;
        exit();
        return true;
    } else if (sql_operation == "clear") {
        clear();
        cout << "c-sql>>>";
        return true;
    } else if (sql_operation == "help") {
        help();
        cout << "c-sql>>>";
        return true;
    } else if (sql_operation == "show") {
        show();
        cout << "c-sql>>>";
        return true;
    }
    if (function_map.empty()) {
        if (!operation_register()) {
            flag = false;
        }
    }
    if (function_map.find(sql_operation) != function_map.end()) {
        if (!function_map[sql_operation](move(argv))) {
            return false;
        }
        cout << "c-sql>>>";
        return true;
    }
    return false;
}

bool operation_register() {
    try {
        function_map.insert(function_pair("create", sql_create));
        function_map.insert(function_pair("drop", sql_drop));
        function_map.insert(function_pair("delete", sql_delete));
        function_map.insert(function_pair("select", sql_select));
        function_map.insert(function_pair("insert", sql_insert));
        function_map.insert(function_pair("use", sql_use));
        return true;
    } catch (...) {
        cout << "option register failed!" << endl;
        return false;
    }
}

void clear() {
    system("cls");
}

void exit() {

    cout << "shutting down server..." << endl;
}

void help() {
    cout << "No help so far." << endl;
}

void show() {
    cout << "show database" << endl;
}

bool sql_create(vector<string> argv) {
    // 合法性
    if (argv.size() < 2) {
        return false;
    }

    if (argv[0] == "database") {
        if (_create_database(argv[1])) {
            cout << "create database " << argv[1] << endl;
            return true;
        } else {
            return false;
        }
    } else if (argv[0] == "table") {
//        cout << "table" << endl;
        argv.erase(argv.begin());
        Table tb_name = argv[0];
        argv.erase(argv.begin());
        string primary;
        map<Column, Type> mp = table_create_helper(argv, primary);
        if (_create_table(tb_name, mp, primary)) {
            // 操作成功后的操作
            // TODO

            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool sql_select(vector<string> argv) {
//    cout << "sql_select" << endl;
    vector<vector<string>> res;
    if (argv.size() == 3 || argv.size() == 5) {
        return _select(res, argv[2], argv[0]);
    }
    return false;
}

bool sql_insert(vector<string> argv) {
//    cout << "sql_insert" << endl;
    if (argv[1] != "values" || argv.size() < 3) {
        return false;
    }
    vector<string> values(argv.begin() + 2, argv.end());
    return _insert(argv[0], values);
}

bool sql_drop(vector<string> argv) {
    if (argv.size() != 2) {
        return false;
    }
    if (argv[0] == "database") {
        return _drop_database(argv[1]);
    } else if (argv[0] == "table") {
        return _drop_table(argv[1]);
    } else {
        return false;
    }
}

bool sql_delete(vector<string> argv) {
//    cout << "sql_delete" << endl;
    if (argv.size() == 1 || argv.size() == 3) {
        return _delete(argv[0], argv[2]);
    }
    return false;
}

bool sql_use(vector<string> argv) {
    if (argv.size() != 1) {
        return false;
    }
    string db_name = argv[0];
    cout << "DataBase " << db_name << endl;
    DB.use(db_name);
    return true;
}