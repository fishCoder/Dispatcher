#include "SQLiteHelper.h"

#include <sstream>
#include <boost/filesystem.hpp>
#include <json/json.h>
using namespace boost::filesystem;
using namespace std;

SQLiteHelper::SQLiteHelper(){
    init_database();
}

void SQLiteHelper::init_database(){
    if(!exists(path("dispatcher.db"))){
        int res = sqlite3_open("sql.db", &pDB);
        if(res){}
        string map_info = "create table map_info(map INTEGER,num INTEGER,info VARCHAR(100));";
        string log_table = "create table log(id INTEGER primary key,INTEGER map,INTEGER amount,size INTEGER,time CHAR(15));";
    }else{
        sqlite3_open("sql.db", &pDB);
    }
}
void SQLiteHelper::exec_select_sql(const char * c_sql){

}
void SQLiteHelper::exec_insert_sql(const char * c_sql){
    char * errMsg;
    sqlite3_exec(pDB,"begin transaction;",0,0, &errMsg);
    sqlite3_exec(pDB,c_sql,0,0, &errMsg);
    sqlite3_exec(pDB,"commit transaction;",0,0, &errMsg);
}



string SQLiteHelper::gen_log_to_json(int map_type_id,int bucket){
    Json::Value root;
    Json::FastWriter writer;

    stringstream strsql;
    strsql << "select * from log where map =";
    strsql << map_type_id <<"ORDER BY id DESC limit 0,"<< 24 * bucket <<";";

    sqlite3_stmt * stmt = NULL;
    int res = sqlite3_prepare(pDB,strsql.str().c_str(),-1,&stmt,0);
    int index = 0 ;
    // sqlite_step每次返回SQLITE_ROW就表示一行数据
    while((res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        // 获取第一列值
        int id = sqlite3_column_int(stmt,0);
        int gen_amount = sqlite3_column_int(stmt,2);
        int gen_size = sqlite3_column_int(stmt,3);
        string str_time((const char *)sqlite3_column_text(stmt,4));

        root[index]["id"] = id;
        root[index]["map"] = map_type_id;
        root[index]["amount"] = gen_amount;
        root[index]["size"] = gen_size;
        root[index]["time"] = str_time;


        index++;
    }

    return writer.write(root);
}
void SQLiteHelper::add_gen_log(int map_type_id,int amount,int gen_size){
    std::stringstream strsql;
    strsql << "select * from log where map =";
    strsql  << map_type_id <<"ORDER BY id DESC limit 0,1;";

    sqlite3_stmt * stmt = NULL;
    int res = sqlite3_prepare(pDB,strsql.str().c_str(),-1,&stmt,0);
    // sqlite_step每次返回SQLITE_ROW就表示一行数据
    while((res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        // 获取第一列值
        int id = sqlite3_column_int(stmt,0);
        int gen_amount = sqlite3_column_int(stmt,2);
        int last_gen_size = sqlite3_column_int(stmt,3);
        std::stringstream strsql;
        strsql << "update log set amount = "<< (gen_amount+amount) <<", size ="<<(last_gen_size+gen_size);
        strsql << " where id=" << id << ";";
        sqlite3_exec(pDB,strsql.str().c_str(),0,0,0);
    }
}
void SQLiteHelper::insert_gen_log(int map_type_id,string time){
    std::stringstream strsql;
    strsql << "insert into log(map,amount,size,time)  values(";
    strsql  << map_type_id << ",0,0,"<< time <<");";
    exec_insert_sql(strsql.str().c_str());
}
