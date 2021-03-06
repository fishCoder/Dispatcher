#ifndef _COMMON_SQLITE_HELPER_
#define _COMMON_SQLITE_HELPER_

#include "sqlite3.h"
#include "iostream"


class SQLiteHelper{
public:
    SQLiteHelper();
    void init_database();
    void exec_insert_sql(const char * c_sql);

    std::string gen_log_to_json(int map_type_id ,int bucket);
    void add_gen_log(int map_type_id,int amount,int gen_size);
    void insert_gen_log(int map_type_id,std::string time);

    std::string get_now_time();
    int  has_gen_log_data();
private:
    sqlite3 * pDB;
};


#endif // _COMMON_SQLITE_HELPER_
