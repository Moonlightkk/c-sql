//
// Created by Wu Mianzhi on 2023/2/3.
//

#ifndef C_SQL_FILEIO_H
#define C_SQL_FILEIO_H
#include "base.h"
#include "ConditionFunctor.hpp"
#include "U8String.h"
#include <filesystem>

/* .dat file structure example
 * typeid:I/S/N
 * cn:column name(lower characters)
 * Icn1 Scn2 Scn3 Icn4
 * ... ... ... ...
 * NOTE: .dat file does not have space as split character actually
 */

namespace fop {
    using std::ios_base, std::int32_t;
    using namespace std::filesystem;

    class DatabaseDir {
    private:
        using cfi = cond::ConditionFunctor<int>;
        using cfs = cond::ConditionFunctor<string>;
        static constexpr bool valStr = false;
        static constexpr bool valInt = true;
        friend class FileIO;
        static constexpr unsigned bufferSize = 4096;
        static constexpr char tableSuffix[] = ".dat";
        static constexpr char idxSuffix[] = ".idx";
        /*
         * Members for generic operations
         */
        directory_entry dir;
        unordered_map<string, bool> tableList;//True if index exists
        bool isValid = false;

        /*
         * Members for table operations: select, create, delete, insert
         */
        unordered_map<string, unsigned> columnOffset;
        vector<string> columnOrder;
        map<string, bool> columnType;// True = int, False = string
        std::fstream fp;
        string currentTable;
        bool isOpen = false;

        /*
         * Members for index and primary
         */
        string primaryColumn;
        map<ValType, unsigned> index;
        std::fstream idxp;
        bool isIndexValid = false;

        void openTable(const string& name);
        void closeTable();
        void writeHead();

        [[maybe_unused]] void writeHead(const map<string, bool> &cols);
        void readHead();
        void writeIndex();
        void readIndex();
        /*
         * read record where get pointer of fp pointed: exchange current line with last line
         * NOTE: get pointer should point to a valid start of record when method called
         */
        ValType readElem(const string& columnName);
    public:
        DatabaseDir() = default;
        ~DatabaseDir();
        void assign(const path &dirPath);
        void clear();
        bool createTable(const string &tableName, const vector<pair<string, bool>> &columns, const string &primary, char* error);
        bool createTable(const string &tableName, const vector<pair<string, bool>> &columns, char* error);
        bool dropTable(const string &tableName, char* error);
        bool insert(const string &tableName, const vector<ValType>& values, char* error);
        /*
         * remove records, using cfi&cfs as condition control
         */
        bool remove(const string &tableName, const string &columnName, const cfi &cf, char* error);
        bool remove(const string &tableName, const string &columnName, const cfs &cf, char* error);
        bool remove(const string &tableName, char* error);
        bool select(const string &tableName, const string& resColumnName, const string &columnName, const cfs &cf, vector<vector<string>>& result, char* error);
        bool select(const string &tableName, const string& resColumnName, const string &columnName, const cfi &cf, vector<vector<string>>& result,  char* error);
        bool select(const string &tableName, const string &resColumnName, vector<vector<string>> &result, char *error);

    };

/* Interact with <dbname> file */
    class FileIO {
    public:
        /** Constructor and destructor
         *
         */
        FileIO() = default;
        ~FileIO();
        /**
         * get currently open database name
         * @return
         */
        string dataName();

        /**
         * Drop database: delete dir in file system
         * @param name
         * @return
         */
        bool dropDatabase(const string &name);

        /**
         * Drop table
         * @param name
         * @return
         */
        bool dropTable(const string &name);

        /**
         * select columns where condition satisfy
         * @param tableName name of column's table
         * @param columnName column need to be selected, * for all columns
         * @param condition condition of column, empty if no condition
         * @return
         */
        vector<vector<string>> select(const string &tableName, const string &columnName, const string &condition);

        /**
         * Insert new record into table
         * @param table
         * @param values
         * @return
         */
        bool insert(const string &table, const map<string, string>& values);

        bool remove(const string &table, const string &cond);
        /**
         * Create new table, with primary key specified
         * @param tableName
         * @param columns
         * @param primary
         * @return
         */
        bool create(const string &tableName, const map<string, string>& columns, const string& primary);
        /**
         * Create new table
         * @param tableName
         * @param columns
         * @return
         */
        bool create(const string &tableName, const map<string, string>& columns);
        /**
         * Create new database
         * @param dbName: name of database
         * @return
         */
        bool create(const string &dbName);

        /**
         * Alter current database: change the dir
         * @param dataBaseName
         * @return
         */
        bool use(const string &dataBaseName);

        /**
         * Convert int32_t to bytes array
         * @param str
         * @return
         */
        static int32_t bytesToI32(const vector<unsigned char> &str);

        /**
         * Convert bytes array to int32_t
         * @param val
         * @return
         */
        static vector<unsigned char> i32ToBytes(int32_t val);

        /**
         * Init file system: change working path, init static variables
         */
        void initFileSystem();

        friend class DatabaseDir;

    private:
        DatabaseDir database;
        bool isDBOpened = false;
        static constexpr unsigned bufferSize = 4096;
        static constexpr char _opList[3] = {'=', '<', '>'};
        unordered_map<char, std::function<bool(int, int)>> _iopMap;
        unordered_map<char, std::function<bool(string , string)>> _sopMap;
    protected:
    };
}//namespace fop



#endif //C_SQL_FILEIO_H
