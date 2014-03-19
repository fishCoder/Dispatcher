#include "GeneratorLog.h"
#include "Configure.h"
#include "json/json.h"
GeneratorLog::GeneratorLog(int _time_bucket):sqlite(){
    this->start_time = NULL;
    this->time_bucket = _time_bucket;

}

void GeneratorLog::add_record(int map_type_id,int amount ,int map_size){
    if(is_over_bucket()){
        start_record();
    }
    sqlite.add_gen_log(map_type_id,amount,map_size);
}
void GeneratorLog::start_record(){
    if(start_time == NULL && !sqlite.has_gen_log_data()){
        start_time = new ptime(second_clock::local_time());
        ptime old_time(from_iso_string(sqlite.get_now_time()));
        time_duration td = *start_time - old_time;
        if(td.total_seconds() < time_bucket){
            start_time = new ptime(from_iso_string(sqlite.get_now_time()));
            return;
        }
    }
    else{
        delete start_time;
        start_time = new ptime(second_clock::local_time());
    }

    Configure config;
    std::string str_config;
    config.read_config_file(str_config);
    Json::Value root;
    Json::Reader reader;
    reader.parse(str_config,root);
    for(unsigned int i=0;i<root.size();i++){
        int map_type_id = root[i].get("map",-1).asInt();
        sqlite.insert_gen_log(map_type_id,to_iso_string(*start_time));
    }

}
bool GeneratorLog::is_over_bucket(){
    if(start_time == NULL) return true;
    ptime now_time = second_clock::local_time();
    time_duration td = now_time - *start_time;
    return  td.total_seconds() > time_bucket;
}


std::string GeneratorLog::get_bucket_log(int map_type_id,int bucket){
    return sqlite.gen_log_to_json(map_type_id,bucket);
}

void GeneratorLog::slow_gen_log(int map_type_id,int duration,std::string time){
    std::stringstream sql;
    sql << "insert slow_log(map,duration,time) values("<<map_type_id<<","<<duration<<","<<time<<");";
    sqlite.exec_insert_sql(sql.str().c_str());
}
