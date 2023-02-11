//
// Created by Wu Mianzhi on 2023/2/3.
//

#ifndef C_SQL_FILEIO_H
#define C_SQL_FILEIO_H
#include "base.h"
#include "ConditionFunctor.hpp"
#include <filesystem>

/* .dat file structure example
 * typeid:I/S/N
 * cn:column name(lower characters)
 * Nid Icn1 Scn2 Scn3 Icn4
 * 1 ... ... ... ...
 * NOTE: .dat file does not have space as split character actually
 */

/* .idx file structure example
 *
 *
 */
namespace fop {
    using std::ios_base, std::int32_t;
    using namespace std::filesystem;

    class DatabaseDir {
    private:
        using cfi = cond::ConditionFunctor<int>;
        using cfs = cond::ConditionFunctor<string>;
        friend class FileIO;
        static constexpr unsigned headMaxSz = 20480;
        static constexpr char tableSuffix[] = ".dat";
        static constexpr char idxSuffix[] = ".idx";
        directory_entry dir;
        unordered_map<string, bool> tableList;
        bool isValid = false;

        /*
         * Members for table operations: select, create, delete, insert
         *
         */
        unsigned rowOffset = 0;
        unsigned headOffset = 0;
        unordered_map<string, unsigned> columnOffset;
        map<string, bool> columnType;// True = int, False = string
        std::fstream fp;
        std::fstream idxp;
        bool isOpen = false;
        void openTable(const string& name);
        void closeTable();
        void writeHead();
        void writeHead(const map<string, bool> &cols);
        void readHead();
    public:
        DatabaseDir() = default;
        ~DatabaseDir();
        void assign(const path &dirPath);
        void clear();
        string createTable(const string &tableName, const map<string, bool> &columns, const string &primary);
        string createTable(const string &tableName, const map<string, bool> &columns);
        string dropTable(const string &tableName);
        string insert(const string &tableName, const vector<ValType>& values);
        string remove(const string &tableName, const cfi &cf);
        string remove(const string &tableName, const cfs &cf);
        string select(const string &tableName);

    };

/* Interact with <dbname> file */
    class FileIO {
    public:
        /** Constructor
         *
         */
        FileIO() = default;
        ~FileIO();
        string dataName();

        string dropDatabase(const string &name);

        string dropTable(const string &name);

        string select(const string &table);

        string insert(const string &table, const vector<ValType>& values);

        string remove(const string &table, const string &cond);

        string create(const string &tableName, const map<string, bool> &columns, const string& primary);
        string create(const string &tableName, const map<string, bool> &columns);
        string create(const string &dbName);

        /**
         * Alter current database: change the dir
         * @param dataBaseName
         * @return
         */
        string use(const string &dataBaseName);

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

        static void initFileSystem();

        friend class DatabaseDir;

    private:
        DatabaseDir database;
        bool isDBOpened = false;

    protected:
    };
}//namespace fop



#endif //C_SQL_FILEIO_H
