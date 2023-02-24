//
// Created by Wu Mianzhi on 2023/2/5.
//
#include "../include/FileIO.h"

using namespace fop;

DatabaseDir::~DatabaseDir()
{
    clear();
}

void DatabaseDir::assign(const path &dirPath)
{
    dir.assign(dirPath);
    isValid = true;
    directory_iterator tableIter(dirPath);
    for (auto&& file : tableIter)
    {
        if (file.path().extension() == tableSuffix)
        {
            string fileName = file.path().stem().string();
            if (tableList.find(fileName) == tableList.end())
            {
                tableList[fileName] = false;
            }
        }
        if (file.path().extension() == idxSuffix)
        {
            string fileName = file.path().stem().string();
            tableList[fileName] = true;
        }
    }
}

void DatabaseDir::clear()
{
    tableList.clear();
    isValid = false;
}

void DatabaseDir::writeHead()
{
    for (auto&& col : columnOrder)
    {
        string colName;
        if (columnType[col]) {
            colName = col + "I";
            const char* buffer = colName.c_str();
            long sz = colName.size();
            fp.write(buffer, sz);
        }
        else
        {
            colName = col + "S";
            const char* buffer = colName.c_str();
            long sz = colName.size();
            fp.write(buffer, sz);
        }
    }
    fp.write("\n", 1);
}

[[maybe_unused]] void DatabaseDir::writeHead(const map<string, bool> &cols)
{
    for (auto&& columnWithType : cols)
    {
        string colName;
        if (columnWithType.second) {
            colName = columnWithType.first + "I";
            const char* buffer = colName.c_str();
            long sz = colName.size();
            fp.write(buffer, sz);
        }
        else
        {
            colName = columnWithType.first + "S";
            const char* buffer = colName.c_str();
            long sz = colName.size();
            fp.write(buffer, sz);
        }
    }
    fp.write("\n", 1);
}

void DatabaseDir::readHead()
{
    unsigned offset = 0;
    unsigned hoffset = 0;
    string colName;
    while (fp.peek() != '\n')
    {
        char ch;
        fp.get(ch);
        if (ch >= 'a')
            colName.push_back(ch);
        else
        {
            columnOffset[colName] = offset;
            columnOrder.push_back(colName);
            if (ch == 'I') {
                offset += 4;
                columnType[columnOrder.back()] = true;
            }
            else if (ch == 'S') {
                offset += 256;
                columnType[columnOrder.back()] = false;
            }
            colName.clear();
        }
        hoffset++;
    }
    fp.seekg(ios_base::beg);
}

void DatabaseDir::openTable(const string &tableName) {
    path tablePath = dir.path();
    tablePath.append(tableName);
    tablePath.concat(tableSuffix);
    fp.open(tablePath, ios_base::in|ios_base::out|ios_base::binary);
    readHead();
    isOpen = true;
    currentTable = tableName;
}

void DatabaseDir::closeTable()
{
    fp.close();
    isOpen = false;
    columnOffset.clear();
    columnType.clear();
    columnOrder.clear();
    isIndexValid = false;

}

void DatabaseDir::writeIndex()
{
    if (!tableList[currentTable])
        return;// do nothing if table doesn't have any index
    path idxPath = dir.path();
    idxPath.append(currentTable).concat(idxSuffix);
    idxp.open(idxPath, ios_base::out|ios_base::trunc);//rewrite whole index map every time
    idxp<<primaryColumn<<endl;//write primary key name
    for (auto&& indexNode : index)
    {
        auto value = indexNode.first;
        auto offset = indexNode.second;
        int* pInt = std::get_if<int>(&value);
        string* pString = std::get_if<string>(&value);
        if (pInt)
            idxp << *pInt << " " << offset<<endl;
        if (pString)
            idxp << *pString << " " << offset << endl;
    }
    idxp.close();
}

void DatabaseDir::readIndex()
{
    if (!tableList[currentTable])
        return;// do nothing if table doesn't have any index
    path idxPath = dir.path();
    idxPath.append(currentTable).concat(idxSuffix);
    idxp.open(idxPath, ios_base::in);
    idxp >> primaryColumn;
    bool type = columnType[primaryColumn];
    while (!isspace(idxp.peek()))
    {
        ValType val;
        unsigned offset;
        if (type)// type = Integer
        {
            int value;
            idxp >> value;
            idxp >> offset;
            val = value;
            index[val] = offset;
        }
        else // type = String
        {
            string value;
            idxp >> value;
            idxp >> offset;
            val = value;
            index[val] = offset;
        }
    }
    isIndexValid = true;
    idxp.close();
}

bool DatabaseDir::createTable(const string &tableName, const vector<pair<string, bool>> &columns, char* const error)
{
    /* Check table exist */
    if (tableList.find(tableName) != tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << "> already exists ";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    path tablePath = dir.path();
    tablePath.append(tableName);
    tablePath.concat(tableSuffix);
    std::ofstream table(tablePath);
    table.close();
    for (auto&& col : columns)
    {
        columnOrder.push_back(col.first);
        columnType[col.first] = col.second;
    }
    fp.open(tablePath, ios_base::in|ios_base::out|ios_base::binary);
    writeHead();
    fp.close();
    closeTable();
    tableList[tableName] = false;
    return true;
}

bool DatabaseDir::createTable(const string &tableName, const vector<pair<string, bool>> &columns, const string &primary, char* const error)
{
    primaryColumn = primary;
    bool res = createTable(tableName, columns, error);
    if (!res)
        return false;
    tableList[tableName] = true;
    //create tableName.idx
    path idxPath = dir.path();
    idxPath.append(tableName).concat(idxSuffix);
    idxp.open(idxPath, ios_base::out|ios_base::trunc);//rewrite whole index map every time
    idxp<<primaryColumn<<endl;//write primary key name
    idxp.close();

    index.clear();
    writeIndex();
    return true;
}

bool DatabaseDir::dropTable(const string &tableName, char* const error)
{
    /* Check table exist */
    if (tableList.find(tableName) == tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << "> doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    auto table = tableList.extract(tableName);
    path tablePath = dir.path();
    tablePath.append(tableName);
    tablePath.concat(tableSuffix);
    std::filesystem::remove(tablePath);
    if (table.mapped())
    {
        path idxPath = dir.path();
        idxPath.append(tableName).concat(idxSuffix);
        std::filesystem::remove(idxPath);
    }
    return true;
}

bool DatabaseDir::select(const string &tableName, const string &resColumnName, const string &columnName, const cfi &cf,
                         vector<vector<string>> &result, char *const error)
{
    /* Check table exist */
    if (tableList.find(tableName) == tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << "> doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    openTable(tableName);

    /* Check column exist */
    if (!resColumnName.empty() && columnType.find(resColumnName) == columnType.end())
    {
        stringstream errorInfo;
        errorInfo << "Column:<" << resColumnName << "> doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }

    char lineBuffer[bufferSize];
    string head;
    fp.getline(lineBuffer, bufferSize);
    head.append(lineBuffer);
    vector<string> resHead;
    if (resColumnName.empty())
    {
        for (const auto &colName: columnOrder) {
            resHead.push_back(colName);
        }
    }
    else
        resHead.push_back(resColumnName);
    result.push_back(resHead);
    auto record = fp.tellg();
    vector<char> line;
    while (fp.getline(lineBuffer, bufferSize)) {
        auto nextRecord = fp.tellg();
        for (int i = 0; i < fp.gcount() - 1; i++)
        {
            line.push_back(lineBuffer[i]);
        }
        line.push_back('\n');
        fp.seekg(record);
        auto value = std::get<int>(readElem(columnName));
        if (cf(value)) {
            unsigned colOffset;
            vector<string> lineElement;
            for (const auto& colName : resHead)
            {
                string element;
                stringstream stream;
                colOffset = columnOffset[colName];
                bool colType = columnType[colName];
                if (colType)
                {
                    vector<unsigned char> bytes;
                    for (unsigned i = 0; i < 4; i++)
                    {
                        bytes.push_back(line[colOffset + i]);
                    }
                    stream << FileIO::bytesToI32(bytes);
                    element = stream.str();
                }
                else
                {
                    char buffer[256];
                    for (unsigned i = 0; i < 256; i++)
                    {
                        buffer[i] = line[colOffset + i];
                    }
                    element.append(buffer);
                }
                lineElement.push_back(element);
            }
            result.push_back(lineElement);
        }
        fp.seekg(nextRecord);
        record = nextRecord;
        line.clear();
    }
    closeTable();
    return true;
}

bool DatabaseDir::select(const string &tableName, const string &resColumnName, const string &columnName, const cfs &cf,
                         vector<vector<string>> &result, char *const error)
{
    /* Check table exist */
    if (tableList.find(tableName) == tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << ">doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    openTable(tableName);

    /* Check column exist */
    if (!resColumnName.empty() && columnType.find(resColumnName) == columnType.end())
    {
        stringstream errorInfo;
        errorInfo << "Column:<" << resColumnName << "> doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }

    char lineBuffer[bufferSize];
    string head;
    fp.getline(lineBuffer, bufferSize);
    head.append(lineBuffer);
    vector<string> resHead;
    if (resColumnName.empty())
    {
        for (const auto &colName: columnOrder) {
            resHead.push_back(colName);
        }
    }
    else
        resHead.push_back(resColumnName);
    result.push_back(resHead);
    auto record = fp.tellg();
    vector<char> line;
    while (fp.getline(lineBuffer, bufferSize)) {
        auto nextRecord = fp.tellg();
        for (int i = 0; i < fp.gcount() - 1; i++)
        {
            line.push_back(lineBuffer[i]);
        }
        line.push_back('\n');
        fp.seekg(record);
        auto value = std::get<string>(readElem(columnName));
        if (cf(value)) {
            unsigned colOffset;
            vector<string> lineElement;
            for (const auto& colName : resHead)
            {
                string element;
                stringstream stream;
                colOffset = columnOffset[colName];
                bool colType = columnType[colName];
                if (colType)
                {
                    vector<unsigned char> bytes;
                    for (unsigned i = 0; i < 4; i++)
                    {
                        bytes.push_back(line[colOffset + i]);
                    }
                    stream << FileIO::bytesToI32(bytes);
                    element = stream.str();
                }
                else
                {
                    char buffer[256];
                    for (unsigned i = 0; i < 256; i++)
                    {
                        buffer[i] = line[colOffset + i];
                    }
                    element.append(buffer);
                }
                lineElement.push_back(element);
            }
            result.push_back(lineElement);
        }
        fp.seekg(nextRecord);
        record = nextRecord;
        line.clear();
    }
    closeTable();
    return true;
}

bool DatabaseDir::select(const string &tableName, const string &resColumnName, vector<vector<string>> &result, char *const error)
{
    //vector<vector<string>> _result;
    /* Check table exist */
    if (tableList.find(tableName) == tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << ">doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    openTable(tableName);

    /* Check column exist */
    if (!resColumnName.empty() && columnType.find(resColumnName) == columnType.end())
    {
        stringstream errorInfo;
        errorInfo << "Column:<" << resColumnName << "> doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }

    char lineBuffer[bufferSize];
    string head;
    fp.getline(lineBuffer, bufferSize);
    head.append(lineBuffer);
    vector<string> resHead;
    if (resColumnName.empty())
    {
        for (const auto &colName: columnOrder) {
            resHead.push_back(colName);
        }
    }
    else
        resHead.push_back(resColumnName);
    result.push_back(resHead);
    auto record = fp.tellg();
    vector<char> line;
    while (fp.getline(lineBuffer, bufferSize)) {
        auto nextRecord = fp.tellg();
        for (int i = 0; i < fp.gcount() - 1; i++)
        {
            line.push_back(lineBuffer[i]);
        }
        line.push_back('\n');
        fp.seekg(record);
        unsigned colOffset;
        vector<string> lineElement;
        for (const auto &colName: resHead) {
            string element;
            stringstream stream;
            colOffset = columnOffset[colName];
            bool colType = columnType[colName];
            if (colType) {
                vector<unsigned char> bytes;
                for (unsigned i = 0; i < 4; i++) {
                    bytes.push_back(line[colOffset + i]);
                }
                stream << FileIO::bytesToI32(bytes);
                element = stream.str();
            } else {
                char buffer[256];
                for (unsigned i = 0; i < 256; i++) {
                    buffer[i] = line[colOffset + i];
                }
                element.append(buffer);
            }
            lineElement.push_back(element);
        }
        result.push_back(lineElement);
        fp.seekg(nextRecord);
        record = nextRecord;
        line.clear();
    }
    closeTable();
    return true;
}


bool DatabaseDir::insert(const string &tableName, const vector<ValType>& values, char *const error)
{
    /* Check table exist */
    if (tableList.find(tableName) == tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << ">doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    openTable(tableName);

    readIndex();
    fp.seekp(0, ios_base::end);
    unsigned position = fp.tellp();
    /* Check column and input amount */
    if (values.size() != columnType.size())
    {
        stringstream errorInfo;
        errorInfo << "Input values amount doesn't match: get " << values.size() << " , need " << columnType.size() << " !" << endl;
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    } //the amount of values doesn't match column's
    auto valueIter = values.begin();
    for (auto&& col : columnOrder)
    {

        auto value = &(*valueIter);
        valueIter++;
        const int* pIntegerVal = std::get_if<int>(value);
        const string* pStringVal = std::get_if<string>(value);
        auto offset = columnOffset[col];
        bool colType = columnType[col];
        if (pIntegerVal && colType)
        {
            auto bytes = FileIO::i32ToBytes(*pIntegerVal);
            for (auto byte : bytes)
                fp.put(byte);
        }
        else if (pStringVal && !colType)
        {
            U8String u8str(*pStringVal);
            fp.write(u8str.toRawChars(), 256);
        }
        else
        {
            stringstream errorInfo("Input values type doesn't match!");
            string errorResult = errorInfo.str();
            std::copy(errorResult.begin(), errorResult.end(), error);
            return false; // value type doesn't match
        }
        if (isIndexValid)
        {
            if (col == primaryColumn)
            {
                if (index.find(*value) != index.end())
                {
                    stringstream errorInfo("Input values have duplicate primary keys!");
                    string errorResult = errorInfo.str();
                    std::copy(errorResult.begin(), errorResult.end(), error);
                    return false; //duplicate primary key
                }
                index[*value] = position;
            }
        }
    }
    fp.write("\n", 1);
    writeIndex();
    closeTable();
    return true;
}

ValType DatabaseDir::readElem(const string &columnName)
{
    auto currentGetPointer = fp.tellg();
    auto offset = columnOffset[columnName];
    char buffer[260];
    vector<char> line;
    ValType result;
    fp.seekg(offset, ios_base::cur);
        auto c = fp.tellg();
        if (columnType[columnName])
        {
            auto b = fp.tellg();
            fp.read(buffer, 4);
            vector<unsigned char> bytes;
            for (int i = 0; i < 4; i++)
                bytes.push_back(buffer[i]);
            result = FileIO::bytesToI32(bytes);
        }
        else
        {
            auto b = fp.tellg();
            fp.read(buffer, 256);
            stringstream stream(buffer);
            U8String val;
            stream >> val;
            result = val.toFormatString();
        }
        fp.seekg(currentGetPointer);
        return result;
}

bool DatabaseDir::remove(const string &tableName, const string &columnName, const cfi &cf, char *const error)
{
    path auxTablePath = dir.path();
    auxTablePath.append("auxTable");
    auxTablePath.concat(tableSuffix);

    path currentTablePath = dir.path();
    currentTablePath.append(tableName).concat(tableSuffix);

    std::ofstream aux(auxTablePath, ios_base::out|ios_base::binary);
    /* Check table exist */
    if (tableList.find(tableName) == tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << ">doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    openTable(tableName);

    /* Check column exist */
    if (!columnName.empty() && columnType.find(columnName) == columnType.end())
    {
        stringstream errorInfo;
        errorInfo << "Column:<" << columnName << "> doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }

    // check whether cf's type is same with column
    char lineBuffer[bufferSize];
    string head;
    fp.getline(lineBuffer, bufferSize);
    head.append(lineBuffer);
    aux.write(head.c_str(), head.size());
    aux.put('\n');
    auto record = fp.tellg();
    vector<char> line;
    while (fp.getline(lineBuffer, bufferSize)) {
        auto nextRecord = fp.tellg();
        for (int i = 0; i < fp.gcount() - 1; i++)
        {
            line.push_back(lineBuffer[i]);
        }
        line.push_back('\n');
        fp.seekg(record);
        auto value = std::get<int>(readElem(columnName));
        if (!cf(value)) {
            for (auto character : line)
                aux.put(character);
        }
        fp.seekg(nextRecord);
        record = nextRecord;
        line.clear();
    }
    closeTable();
    aux.close();

    std::filesystem::remove(currentTablePath);
    std::filesystem::rename(auxTablePath, currentTablePath);
    return true;
}

bool DatabaseDir::remove(const string &tableName, const string &columnName, const cfs &cf, char *const error)
{
    path auxTablePath = dir.path();
    auxTablePath.append("auxTable");
    auxTablePath.concat(tableSuffix);

    path currentTablePath = dir.path();
    currentTablePath.append(tableName).concat(tableSuffix);

    std::ofstream aux(auxTablePath, ios_base::out|ios_base::binary);
    /* Check table exist */
    if (tableList.find(tableName) == tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << ">doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    openTable(tableName);

    /* Check column exist */
    if (!columnName.empty() && columnType.find(columnName) == columnType.end())
    {
        stringstream errorInfo;
        errorInfo << "Column:<" << columnName << "> doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }

    char lineBuffer[bufferSize];
    string head;
    fp.getline(lineBuffer, bufferSize);
    head.append(lineBuffer);
    aux.write(head.c_str(), head.size());
    aux.put('\n');
    auto record = fp.tellg();
    vector<char> line;
    while (fp.getline(lineBuffer, bufferSize)) {
        auto nextRecord = fp.tellg();
        for (int i = 0; i < fp.gcount() - 1; i++)
        {
            line.push_back(lineBuffer[i]);
        }
        line.push_back('\n');
        fp.seekg(record);
        auto value = std::get<string>(readElem(columnName));
        if (!cf(value)) {
            for (auto character : line)
                aux.put(character);
        }
        fp.seekg(nextRecord);
        record = nextRecord;
        line.clear();
    }
    closeTable();
    aux.close();

    std::filesystem::remove(currentTablePath);
    std::filesystem::rename(auxTablePath, currentTablePath);
    return true;
}

bool DatabaseDir::remove(const string &tableName, char *const error)
{
    path auxTablePath = dir.path();
    auxTablePath.append("auxTable");
    auxTablePath.concat(tableSuffix);

    path currentTablePath = dir.path();
    currentTablePath.append(tableName).concat(tableSuffix);

    std::ofstream aux(auxTablePath, ios_base::out|ios_base::binary);
    /* Check table exist */
    if (tableList.find(tableName) == tableList.end())
    {
        stringstream errorInfo;
        errorInfo << "Table:<" << tableName << ">doesn't find!";
        string errorResult = errorInfo.str();
        std::copy(errorResult.begin(), errorResult.end(), error);
        return false;
    }
    openTable(tableName);

    char lineBuffer[bufferSize];
    string head;
    fp.getline(lineBuffer, bufferSize);
    head.append(lineBuffer);
    aux.write(head.c_str(), head.size());
    aux.put('\n');
    closeTable();
    aux.close();

    std::filesystem::remove(currentTablePath);
    std::filesystem::rename(auxTablePath, currentTablePath);
    return true;
}

vector<unsigned char> FileIO::i32ToBytes(int32_t val)
{
    vector<unsigned char> buffer;
    auto bitMode = static_cast<std::uint32_t> (val);
    buffer.push_back(static_cast< unsigned char>(bitMode >> 24));
    buffer.push_back(static_cast< unsigned char>(bitMode >> 16));
    buffer.push_back(static_cast< unsigned char>(bitMode >> 8));
    buffer.push_back(static_cast< unsigned char>(bitMode >> 0));
    return buffer;
}

int32_t FileIO::bytesToI32(const vector<unsigned char> &bytes)
{
    int32_t buf = 0;
    buf = buf + (static_cast<int32_t>(bytes[0]) << 24);
    buf = buf + (static_cast<int32_t>(bytes[1]) << 16);
    buf = buf + (static_cast<int32_t>(bytes[2]) << 8);
    buf = buf + (static_cast<int32_t>(bytes[3]) << 0);
    return buf;
}


void FileIO::initFileSystem()
{
    path workingDir = "../data";
    current_path(workingDir);
    _iopMap.insert(pair('=', std::equal_to<>{}));
    _iopMap.insert(pair('<', std::less<>{}));
    _iopMap.insert(pair('>', std::greater<>{}));
    _sopMap.insert(pair('=', std::equal_to<>{}));
    _sopMap.insert(pair('<', std::less<>{}));
    _sopMap.insert(pair('>', std::greater<>{}));
}

FileIO::~FileIO()
{
    database.clear();
    isDBOpened = false;
}

bool FileIO::create(const string& tableName, const map<string, string>& columns)
{
    bool res;
    string error;
    char errorBuffer[bufferSize];
    errorBuffer[0] = 0;
    vector<pair<string, bool>> _columns;
    for (const auto& column : columns)
    {
        /* type string need to be changed */
        if (column.second == "int")
            _columns.emplace_back(column.first, true);
        else
            _columns.emplace_back(column.first, false);
    }
    res = database.createTable(tableName, _columns, errorBuffer);
    error.append(errorBuffer);
    if (!error.empty())
        cout << error << endl;
    return res;
}

//bool FileIO::create(const string &tableName,const map<string, string>& columns, const string &primary)
//{
//    bool res;
//    string error;
//    char errorBuffer[bufferSize];
//    errorBuffer[0] = 0;
//    vector<pair<string, bool>> _columns;
//    for (const auto& column : columns)
//    {
//        /* type string need to be changed */
//        if (column.second == "int")
//            _columns.emplace_back(column.first, true);
//        else
//            _columns.emplace_back(column.first, false);
//    }
//    res = database.createTable(tableName, _columns, primary, errorBuffer);
//    error.append(errorBuffer);
//    if (!error.empty())
//        cout << error << endl;
//    return res;
//}
bool FileIO::insert(const string &table, const vector<string>& values)
{
    bool res;
    string error;
    char errorBuffer[bufferSize];
    errorBuffer[0] = 0;
    vector<ValType> _values;

    for (auto value : values)
    {
        int i_value;
        try
        {
            i_value = stoi(value);
            _values.emplace_back(i_value);
        }
        catch (std::invalid_argument&) {
            _values.emplace_back(value);
        }
    }

    res = database.insert(table, _values, errorBuffer);
    error.append(errorBuffer);
    if (!error.empty())
    {
        cout << error << endl;
    }
    return res;
}

bool FileIO::use(const string &dataBaseName)
{
    path dbPath(dataBaseName);
    if (!is_directory(dbPath))
    {
        /* raise exception */
        cout << "Database:<" << dataBaseName <<"> does not exist" << endl;
        return false;
    }
    database.clear();
    database.assign(dbPath);
    isDBOpened = true;
    return true;
}

bool FileIO::dropDatabase(const string &dataBaseName)
{
    path dbPath(dataBaseName);
    if (!is_directory(dbPath))
    {
        /* raise exception */
        cout << "Database:<" << dataBaseName <<"> does not exist" << endl;
        return false;
    }
    database.clear();
    remove_all(dbPath);
    return true;
}

bool FileIO::dropTable(const string &name)
{
    bool res;
    string error;
    char errorBuffer[bufferSize];
    errorBuffer[0] = 0;
    res = database.dropTable(name, errorBuffer);
    error.append(errorBuffer);
    if (!error.empty())
        cout << error << endl;
    return res;
}

bool FileIO::create(const string &dataBaseName)
{
    path dbPath(dataBaseName);
    if (is_directory(dbPath))
    {
        /* raise exception */
        cout << "Database:<" << dataBaseName <<"> already exists" << endl;
        return false;
    }
    create_directory(dataBaseName);
    return true;
}

vector<vector<string>> FileIO::select(const string &tableName, const string &columnName, const string &cond)
{
    vector<vector<string>> res;
    bool st;
    string error;
    char errorBuffer[bufferSize];
    errorBuffer[0] = 0;
    string _columnName = columnName;
    if (_columnName == "*")
        _columnName.clear();
    res.clear();
    if (cond.empty())
        st = database.select(tableName, _columnName, res, errorBuffer);
    else
    {
        string constValue;
        string column;
        unsigned op;
        int value;
        for (char i : _opList)
        {
            op = cond.find(i);
            if (op < cond.size())
            {
                constValue = cond.substr(op+1);
                column = cond.substr(0, op);
                break;
            }
        }
        try {
            value = std::stoi(constValue);
            constValue.clear();
        }
        catch (std::invalid_argument&) {}
        if (constValue.empty())
        {
            cond::ConditionFunctor<int> cf(value, _iopMap[cond[op]]);
            st = database.select(tableName, _columnName, column, cf, res, errorBuffer);
        }
        else
        {
            cond::ConditionFunctor<string> cf(constValue, _sopMap[cond[op]]);
            st = database.select(tableName, _columnName, column, cf, res, errorBuffer);
        }
    }

    error.append(errorBuffer);
    if (!st)
    {
        res.clear();
        cout << error << endl;
    }

//    for (const auto& line : res)
//    {
//        for (const auto& elem : line)
//            cout << elem << ",";
//        cout << endl;
//    }
    return res;
}

bool FileIO::insert(const string &table, const map<string, string>& values)
{
    bool res;
    string error;
    char errorBuffer[bufferSize];
    errorBuffer[0] = 0;
    vector<ValType> _values;

    for (auto value : values)
    {
        int i_value;
        try
        {
            i_value = stoi(value.second);
            _values.emplace_back(i_value);
        }
        catch (std::invalid_argument&) {
            _values.emplace_back(value.second);
        }
    }

    res = database.insert(table, _values, errorBuffer);
    error.append(errorBuffer);
    if (!error.empty())
    {
        cout << error << endl;
    }
    return res;
}

bool FileIO::remove(const string &tableName, const string &cond)
{
    bool res;
    string error;
    char errorBuffer[bufferSize];
    errorBuffer[0] = 0;

    if (cond.empty())
        res = database.remove(tableName, errorBuffer);
    else
    {
        string constValue;
        string column;
        unsigned op;
        int value;
        for (char i : _opList)
        {
            op = cond.find(i);
            if (op < cond.size())
            {
                constValue = cond.substr(op+1);
                column = cond.substr(0, op);
                break;
            }
        }
        try {
            value = std::stoi(constValue);
            constValue.clear();
        }
        catch (std::invalid_argument&) {}
        if (constValue.empty())
        {
            cond::ConditionFunctor<int> cf(value, _iopMap[cond[op]]);
            res = database.remove(tableName, column, cf, errorBuffer);
        }
        else
        {
            cond::ConditionFunctor<string> cf(constValue, _sopMap[cond[op]]);
            res = database.remove(tableName, column, cf, errorBuffer);
        }
    }

    return res;
}

string FileIO::dataName()
{
    return database.dir.path().string();
}