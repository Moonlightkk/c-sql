//
// Created by lizhi on 2023/2/6.
//

#ifndef C_SQL_BASE_H
#define C_SQL_BASE_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <variant>


using std::cout, std::cin, std::endl;
using std::string, std::getline;
using std::stringstream;
using std::vector;
using std::map, std::pair, std::unordered_map;
using std::move;
using std::transform;
using std::setw, std::setfill, std::left;
using std::variant;

using Database = string;
using Table = string;
using Type = string;
using Column = string;

// values type, union of string and int
using ValType = variant<string, int>;

#endif //C_SQL_BASE_H
