#include "MapManager.h"
#include "TaskGenerator.h"
#include "Packege.h"
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>

#define MAX_USE_NUM 3
#define MAX_GET_TIMES 5
#define DEL_MIN_LIMITED 100
#define INTERVAL_DAYS 7

using namespace boost::posix_time;

MapManager::MapManager(TaskList & _taskLst):taskLst(_taskLst){
    rc.reset(new redis::client("localhost",6379));
    total_used = 0;
}



std::pair<std::string,int> MapManager::get_field_map(int map_type_id){
    int use_times = 0;
    std::string str_map_id;
    for(int i=0 ; i<5 ; i++){
        str_map_id = rc->srandmember(boost::lexical_cast<std::string>(map_type_id));
        use_times = boost::lexical_cast<int>(rc->hget(str_map_id,"use"));
        if(use_times < MAX_USE_NUM){
            useMapKey(str_map_id,map_type_id);
            break;
        }
    }
    std::string map_data = rc->hget(str_map_id,"map");
    std::string str_verify_code = rc->hget(str_map_id,"code");

    unsigned int verify_code = boost::lexical_cast<unsigned int>(str_verify_code);

    useMapKey(str_map_id,map_type_id);

    return std::pair<std::string,int>(map_data,verify_code);
}



void MapManager::useMapKey(std::string str_map_id,int type){
    int use_time = boost::lexical_cast<int>(rc->hget(str_map_id,"use"));

    if(use_time >= MAX_USE_NUM){
        if(is_over_time(str_map_id)){
            total_used++;
            rc->del(str_map_id);
            addDelType(type);
        }
    }else{
        rc->sadd(boost::lexical_cast<std::string>(type),str_map_id);
        rc->hset(str_map_id,"use",boost::lexical_cast<std::string>(++use_time));
        if(use_time == MAX_USE_NUM){
            ptime now = second_clock::local_time();
            rc->hset(str_map_id,"time",to_iso_string(now));
        }
    }

    if(total_used > DEL_MIN_LIMITED){
        TaskGenerator tg;
        tg.repMapToTask(reqMap,taskLst);
    }
}

void MapManager::addDelType(int type){
    std::map<int,int>::iterator m_itr = reqMap.find(type);
    if(m_itr==reqMap.end()){
        reqMap.insert(std::pair<int,int>(type,0));
    }else{
        m_itr->second++;
    }
}

bool MapManager::is_over_time(std::string str_map_id){
    std::string str_time = rc->hget(str_map_id,"time");
    ptime last_time = time_from_string(str_time);
    ptime now_time(second_clock::local_time());
    time_duration td = now_time - last_time;
    return (td.hours()/24)>INTERVAL_DAYS;
}

bool MapManager::isexsit(int type){
    int num = rc->scard(boost::lexical_cast<std::string>(type));
    return num!=0;
}


