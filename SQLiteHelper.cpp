#include "SQLiteHelper.h"

#include <sstream>
#include <boost/filesystem.hpp>
#include <json/json.h>
using namespace boost::filesystem;
using namespace std;

SQLiteHelper::SQLiteHelper(){
    pDB = NULL;
    init_database();
}

void SQLiteHelper::init_database(){
    if(!exists(path("dispatcher.db"))){
        int res = sqlite3_open("sql.db", &pDB);
        if(res){}
        string map_info = "CREATE TABLE map_info(map INTEGER,num INTEGER,info VARCHAR(100));";
        string log_table = "CREATE TABLE log(id INTEGER primary key,map INTEGER,amount INTEGER,size INTEGER,time VARCHAR(20));";
        string slow_log = "CREATE TABLE slow_log(id INTEGER primary key,map INTEGER,duration INTEGER,ip char(15),gen_time varchar(20));";
        char * errMsg;
        sqlite3_exec(pDB , map_info.c_str() ,0 ,0, &errMsg);
        sqlite3_exec(pDB , log_table.c_str() ,0 ,0, &errMsg);
        sqlite3_exec(pDB , slow_log.c_str() ,0 ,0, &errMsg);
    }else{
        if(pDB == NULL)
            sqlite3_open("sql.db", &pDB);
    }
}
void SQLiteHelper::exec_select_sql(const char * c_sql){

}
void SQLiteHelper::exec_insert_sql(const char * c_sql){
    char * errMsg;
    init_database();
    sqlite3_exec(pDB,"begin transaction;",0,0, &errMsg);
    sqlite3_exec(pDB,c_sql,0,0, &errMsg);
    sqlite3_exec(pDB,"commit transaction;",0,0, &errMsg);
}



string SQLiteHelper::gen_log_to_json(int map_type_id,int bucket){
    Json::Value root;
    Json::FastWriter writer;

    stringstream strsql;
    strsql << "select * from log where map =";
    strsql << map_type_id <<" ORDER BY id DESC limit 0,"<< 24 * bucket <<";";

    cout << "strsql:" << strsql.str() << endl;
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
    sqlite3_finalize(stmt);
    return writer.write(root);
}
void SQLiteHelper::add_gen_log(int map_type_id,int amount,int gen_size){
    std::stringstream strsql;
    strsql << "select * from log where map =";
    strsql  << map_type_id <<"  ORDER BY id DESC limit 0,1;";

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
        cout << "insert strsql:" << strsql.str() << endl;
        sqlite3_exec(pDB,strsql.str().c_str(),0,0,0);
    }
    sqlite3_finalize(stmt);
}
void SQLiteHelper::insert_gen_log(int map_type_id,string time){
    std::stringstream strsql;
    strsql << "insert into log(map,amount,size,time)  values(";
    strsql  << map_type_id << ",0,0,'"<< time <<"');";
    cout << "strsql:" << strsql.str() << endl;
    exec_insert_sql(strsql.str().c_str());
}
std::string SQLiteHelper::get_now_time(){

    std::string strsql = "select * from log ORDER BY id DESC limit 0,1;";
    sqlite3_stmt * stmt = NULL;
    int res = sqlite3_prepare(pDB,strsql.c_str(),-1,&stmt,0);
    // sqlite_step每次返回SQLITE_ROW就表示一行数据
    while((res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        strsql.assign((const char *)sqlite3_column_text(stmt,4));
    }
    sqlite3_finalize(stmt);
    return strsql;
}
int  SQLiteHelper::has_gen_log_data(){
    std::string strsql = "select * from log ORDER BY id DESC limit 0,1;";
    sqlite3_stmt * stmt = NULL;
    int res = sqlite3_prepare(pDB,strsql.c_str(),-1,&stmt,0);
    sqlite3_finalize(stmt);
    return res == SQLITE_ROW;
}
