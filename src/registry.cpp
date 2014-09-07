#include <stdexcept>
#include "registry.h"
#include "iconfig.h"
#include "pal.h"

void dps::Registry::create_database()
{
    try {
        this->create_table("CREATE TABLE IF NOT EXISTS nodes (" \
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
                            "uid VARCHAR(255) NOT NULL UNIQUE);");

        this->create_table("CREATE TABLE IF NOT EXISTS signals (" \
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
                            "name VARCHAR(255) NOT NULL UNIQUE);");

        this->create_table("CREATE TABLE IF NOT EXISTS signal_params (" \
                           "id INTEGER PRIMARY KEY AUTOINCREMENT,"\
                           "signal_id INTEGER REFERENCES signals(id) on delete cascade,"\
                            "name VARCHAR(255) NOT NULL,"\
                            "type integer(2) not null,"\
                            "UNIQUE(signal_id, name));");

        this->create_table("CREATE TABLE IF NOT EXISTS signal_publishers("\
                           "signal_id integer not null references signals(id) on delete cascade,"\
                           "publisher_id integer not null references nodes(id) on delete cascade," \
                           "PRIMARY KEY(signal_id, publisher_id))");

        this->create_table("CREATE TABLE IF NOT EXISTS subscriptions("\
                           "id integer PRIMARY KEY AUTOINCREMENT,"\
                           "signal_id integer not null references signals(id) on delete cascade,"\
                           "subscriber_id integer not null references nodes(id) on delete cascade)");

        this->create_table("CREATE TABLE subscription_conditions("\
                           "subscription_id integer not null references subscriptions(id) on delete cascade,"
                           "relation integer not null,"
                           "value blob not null,"\
                           "PRIMARY KEY(subscription_id, relation))");

    } catch (const sqlite3x::database_error& e){
        throw string("Failed to create database:\n") + e.what();
    }
}

void dps::Registry::create_table(const string& sql)
{
    sqlite3x::sqlite3_command st( this->db, sql );
    st.executenonquery();
}

dps::Registry::Registry()
{
    string path_prefix = PAL::path_get_data_dir();
    string db_path = path_prefix+"/" + dps::settings()->get_value("database","dps") + ".db";

    this->db.open(db_path);

    this->create_database();
}

dps::Registry::~Registry(){
}

dps::Statement dps::Registry::get_statement(const string &sql, const vector<variant_t>& params)
{
    return Statement(this->db, sql, params);
}


dps::Statement::Statement(sqlite3_connection &conn, const string &sql, const vector<variant_t>& params)
{
    this->cmd = new sqlite3x::sqlite3_command(conn, sql);
    int i=1;
    for (auto param: params){
        switch (param.which()){
        case 0://int
            this->cmd->bind(i,boost::get<int>(param));
            break;
        case 1://double
            this->cmd->bind(i,boost::get<double>(param));
            break;
        case 2://string
            this->cmd->bind(i,boost::get<string>(param));
            break;
        default:break;
        }
        i++;
    }
}

dps::Statement::Statement(dps::Statement &&st)
{
    this->cmd=st.cmd;
    st.cmd=nullptr;
}

dps::Statement::~Statement()
{
    delete this->cmd;
}

void dps::Statement::exec()
{
    this->cmd->executenonquery();
}
dps::variant_t dps::Statement::exec(dps::SigParamType type)
{
    switch (type){
    case SigParamType::Int:
        return this->cmd->executeint();
        break;
    case SigParamType::Double:
        return this->cmd->executedouble();
        break;
    case SigParamType::String:
        return this->cmd->executedouble();
        break;
    default:
        return 0;
    }
}
