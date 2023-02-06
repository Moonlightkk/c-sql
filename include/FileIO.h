//
// Created by Wu Mianzhi on 2023/2/3.
//

#ifndef C_SQL_FILEIO_H
#define C_SQL_FILEIO_H
#include <string>
#include <string_view>
#include <functional>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>

using std::string, std::string_view;
using std::function;
using std::vector, std::map, std::unordered_map;
using std::ios_base, std::int32_t;
/* Do condition judgement */
template<class ValType>
class ConditionFunctor
{
public:
    bool operator() (int var);
    bool operator() (string_view sv);
    void init(ValType constVar, function<bool(ValType, ValType)> op);
private:
    ValType _constVar;
    function<bool (ValType, ValType)> _operator;
    bool isInited = false;
};

/* Interact with <dbname> file */
class FileIO
{
public:
    /** Constructor
     *
     */
    FileIO();
    /** Constructor\n
     * The constructor open the <filename> database file if it is existed
     * @param fileName:database name
     */
    FileIO(const string& fileName);
    //@note 1
    string dataName();
    string drop(const string& name, bool isTable);
    string select(const string& table, const string& cond);
    string insert(const string& table);
    string remove(const string& table, const string& cond);
    string create(const string& tableName, const map<string, int> column,  bool isPrimary);
    string use(const string& name);
    /**
     * Convert int32_t to bytes array
     * @param str
     * @return
     */
    static int32_t bytesToI32(const vector<unsigned char> & str);
    /**
     * Convert bytes array to int32_t
     * @param val
     * @return
     */
    static vector<unsigned char> i32ToBytes(int32_t val);
    bool isValid;
private:
    unsigned rowOffset;
    unordered_map<string, unsigned> columnOffset;
    map<string, unsigned> index;
    string dbName;
    std::fstream fp;
    string readPos(unsigned offset);

protected:
};

#endif //C_SQL_FILEIO_H
