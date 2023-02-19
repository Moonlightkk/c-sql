//
// Created by Wu Mianzhi on 2023/2/5.
//
#include "../include/FileIO.h"

using namespace fop;

DatabaseDir::~DatabaseDir()
{
    cout << "destruct databasedir" << endl;
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
    for (auto&& columnWithType : columnType)
    {
        string colName;
        if (columnWithType.second) {
            colName = "I" + columnWithType.first;
            const char* buffer = colName.c_str();
            long sz = colName.size();
            fp.write(buffer, sz);
        }
        else
        {
            colName = "S" + columnWithType.first;
            const char* buffer = colName.c_str();
            long sz = colName.size();
            fp.write(buffer, sz);
        }
    }
    fp.write("\n", 1);
}

void DatabaseDir::writeHead(const map<string, bool> &cols)
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
            cout << colName << endl;
            columnOffset[colName] = offset;
            if (ch == 'I') {
                offset += 4;
                columnType[colName] = true;
            }
            else if (ch == 'S') {
                offset += 256;
                columnType[colName] = false;
            }
            colName.clear();
        }
        hoffset++;
    }
    headOffset = hoffset + 1;
}

void DatabaseDir::openTable(const string &tableName) {
    path tablePath = dir.path();
    tablePath.append(tableName);
    tablePath.concat(tableSuffix);
    fp.open(tablePath, ios_base::in|ios_base::out);
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
    rowOffset = 0;
    headOffset = 0;
    isIndexValid = false;

}

void DatabaseDir::writeIndex()
{
    if (!tableList[currentTable])
        return;// do nothing if table doesn't have index
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
            cout << *pInt << " " << offset<<endl;
        if (pString)
            cout << *pString << " " << offset << endl;
    }
    idxp.close();
}

void DatabaseDir::readIndex()
{
    if (!tableList[currentTable])
        return;// do nothing if table doesn't have index
    path idxPath = dir.path();
    idxPath.append(currentTable).concat(idxSuffix);
    idxp.open(idxPath, ios_base::in);
    idxp >> primaryColumn;
    bool type = columnType[primaryColumn];
    while (!idxp.eof())
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

string DatabaseDir::createTable(const string &tableName, const map<string, bool> &columns)
{
    string info;
    path tablePath = dir.path();
    tablePath.append(tableName);
    tablePath.concat(tableSuffix);
    std::ofstream table(tablePath);
    table.close();
    fp.open(tablePath, ios_base::in|ios_base::out);
    writeHead(columns);
    fp.close();
    tableList[tableName] = false;
    return info;
}

string DatabaseDir::createTable(const string &tableName, const map<string, bool> &columns, const string &primary)
{
    string info;
    primaryColumn = primary;
    createTable(tableName, columns);
    tableList[tableName] = true;

    //create tableName.idx
    path idxPath = dir.path();
    idxPath.append(tableName).concat(idxSuffix);
    idxp.open(idxPath, ios_base::out|ios_base::trunc);//rewrite whole index map every time
    idxp<<primaryColumn<<endl;//write primary key name
    idxp.close();
    return info;
}

string DatabaseDir::dropTable(const string &tableName)
{
    string info;
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
    return info;
}

string DatabaseDir::select(const string &tableName)
{
    string info;
    openTable(tableName);
    cout << headOffset <<endl;
    for (auto&& col : columnType)
    {
        cout << col.first << ":" << col.second << endl;
    }
    closeTable();
    return info;
}

string DatabaseDir::insert(const string &tableName, const vector<ValType>& values)
{
    string info;
    openTable(tableName);
    readIndex();
    fp.seekp(ios_base::end);
    unsigned position = fp.tellp();
    if (values.size() != columnType.size())
        return info; //the amount of values doesn't match column's
    auto valueIter = values.begin();
    for (auto&& columnWithType : columnType)
    {

        auto value = &(*valueIter);
        const int* pIntegerVal = std::get_if<int>(value);
        const string* pStringVal = std::get_if<string>(value);
        auto offset = columnOffset[columnWithType.first];
        bool colType = columnWithType.second;
        if (pIntegerVal && colType)
        {
            fp << *pIntegerVal;
        }
        else if (pStringVal && !colType)
        {
            U8String u8str(*pStringVal);
            fp << u8str.toRawString();
        }
        else
        {
            return info; // value type doesn't match
        }
        if (isIndexValid)
        {
            if (columnWithType.first == primaryColumn)
            {
                if (index.find(*value) != index.end())
                    return info;//duplicate primary key
                index[*value] = position;
            }
        }
    }
    writeIndex();
    closeTable();
    return info;
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
}

FileIO::~FileIO()
{
    database.clear();
    isDBOpened = false;
}

string FileIO::create(const string& tableName, const map<string, bool>& columns)
{
    string info;
    info = database.createTable(tableName, columns);
    return info;
}

string FileIO::create(const string &tableName, const map<string, bool> &columns, const string &primary)
{
    string info;
    info = database.createTable(tableName, columns, primary);
    return info;
}

string FileIO::use(const string &dataBaseName)
{
    string info;
    path dbPath(dataBaseName);
    if (!is_directory(dbPath))
    {
        /* raise exception */
        cout << "no dir" << endl;
        return info;
    }
    database.clear();
    database.assign(dbPath);
    isDBOpened = true;
}

string FileIO::dropDatabase(const string &dataBaseName)
{
    string info;
    path dbPath(dataBaseName);
    if (!is_directory(dbPath))
    {
        /* raise exception */
        cout << "no dir" << endl;
        return info;
    }
    database.clear();
    remove_all(dbPath);
    return info;
}

string FileIO::dropTable(const string &name)
{
    string info;
    info = database.dropTable(name);
    return info;
}

string FileIO::create(const string &dataBaseName)
{
    string info;
    path dbPath(dataBaseName);
    if (is_directory(dbPath))
    {
        /* raise exception */
        cout << "dir exist" << endl;
        return info;
    }
    create_directory(dataBaseName);
    return info;
}

string FileIO::select(const string &table)
{
    string info;
    info = database.select(table);
    return info;
}

