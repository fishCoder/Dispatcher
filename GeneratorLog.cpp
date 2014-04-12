#include "GeneratorLog.h"

GeneratorLog::GeneratorLog(int _time_bucket):sqlite(){
    this->start_time = NULL;
    this->time_bucket = _time_bucket;
    start_record();
}
void GeneratorLog::add_record(int map_type_id,int amount ,int map_size){
    if(is_over_bucket()){
        start_record();
        sqlite.insert_gen_log(map_type_id,to_iso_string(*start_time));
    }
    sqlite.add_gen_log(map_type_id,amount,map_size);
}
void GeneratorLog::start_record(){
    if(start_time != NULL) delete start_time;
    this->start_time = new ptime(second_clock::local_time());

}
bool GeneratorLog::is_over_bucket(){
    ptime now_time(second_clock::local_time());
    time_duration td = now_time - *start_time;
    return  td.total_seconds() > time_bucket;
}
void GeneratorLog::save_to_file(int map_type_id,int map_size){

}
std::string GeneratorLog::get_bucket_log(int map_type_id,int bucket){
    return sqlite.gen_log_to_json(map_type_id,bucket);
}
