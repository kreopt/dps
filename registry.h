#ifndef REGISTRY_H
#define REGISTRY_H
#include <string>

#include <sqlite3x/sqlite3x.hpp>

#include "dps.h"

using namespace std;
using namespace sqlite3x;
namespace dps {
    // TODO: abstract database driver
    class Statement{
        sqlite3x::sqlite3_command* cmd;
    public:
        Statement(sqlite3_connection &conn, const string &sql, const vector<variant_t>& params);
        Statement(Statement&& st);
        ~Statement();
        void exec();
        variant_t exec(SigParamType);

    };

    class Registry
    {
        sqlite3_connection db;

        void        create_database();
        void        create_table(const string& sql);

    public:
        Registry();
        ~Registry();

        Statement  get_statement(const string& sql, const vector<variant_t>& params);
        inline sqlite3x::int64_t last_insert_id(){return db.insertid();}

    };
}
#endif // REGISTRY_H
