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

string DatabaseDir::createTable(const string &tableName, const map<string, bool> &columns)
{
    string info;
    path tablePath = dir.path() / path(tableName);
    std::ofstream table(tablePath);
    table.close();
    fp.open(tablePath, ios_base::in|ios_base::out);
    writeHead(columns);
    fp.close();
    return info;
}

string DatabaseDir::dropTable(const string &tableName)
{
    auto table = tableList.extract(tableName);
    path tablePath(tableName + tableSuffix);
    std::filesystem::remove(tablePath);
    if (table.mapped())
    {
        path idxPath(tableName + idxSuffix);
        std::filesystem::remove(idxPath);
    }
}

string DatabaseDir::select(const string &tableName)
{
    string info;
    path tablePath = dir.path() / path(tableName);
    fp.open(tablePath, ios_base::in|ios_base::out);
    readHead();
    cout << headOffset <<endl;
    for (auto&& col : columnType)
    {
        cout << col.first << ":" << col.second << endl;
    }
    return info;
}

string DatabaseDir::insert(const string &tableName, const vector<ValType>& values)
{
    string info;
    path tablePath = dir.path() / path(tableName);
    fp.open(tablePath, ios_base::in|ios_base::out);
    readHead();
    fp.seekp(ios_base::end);

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
}

string FileIO::select(const string &table)
{
    string info;
    info = database.select(table);
    return info;
}