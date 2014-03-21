#include "MapManager.h"
#include "TaskGenerator.h"
#include "Packege.h"
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
#include "Output.h"


#define MAX_GET_TIMES 5


using namespace boost::posix_time;



MapManager::MapManager(TaskList & _taskLst):taskLst(_taskLst){
    rc.reset(new redis::client(param.redis_address,param.redis_port));
    total_used = 0;
}



std::pair<unsigned int,std::string> MapManager::get_field_map(int map_type_id,std::string & map_data,std::string & loot_npc_data){
    int use_times = 0;
    std::string str_map_id;
    for(int i=0 ; i<MAX_GET_TIMES ; i++){
        str_map_id = rc->srandmember(boost::lexical_cast<std::string>(map_type_id));
        use_times = boost::lexical_cast<int>(rc->hget(str_map_id,"use"));
        if(use_times < param.MAX_USE_NUM){
            useMapKey(str_map_id,map_type_id);
            break;
        }
    }
    map_data.assign(rc->hget(str_map_id,"map"));
    loot_npc_data.assign(rc->hget(str_map_id,"npc"));
    std::string str_verify_code = rc->hget(str_map_id,"code");

    unsigned int verify_code = boost::lexical_cast<unsigned int>(str_verify_code);

    useMapKey(str_map_id,map_type_id);

    return std::pair<unsigned int,std::string>(verify_code,str_map_id);
}



void MapManager::useMapKey(std::string str_map_id,int type){
    int use_time = boost::lexical_cast<int>(rc->hget(str_map_id,"use"));

    if(use_time == param.MAX_USE_NUM){
        rc->expire(str_map_id,param.EXSIT_TIME);
        addDelMapType(type);
        std::cout << "[MapManager]:delete a key ["<< type <<"]" << std::endl;
    /*    if(is_over_time(str_map_id)){
            total_used++;
            rc->del(str_map_id);
            addDelMapType(type);
        }*/
    }else{
        rc->sadd(boost::lexical_cast<std::string>(type),str_map_id);
        rc->hset(str_map_id,"use",boost::lexical_cast<std::string>(++use_time));
        if(use_time == param.MAX_USE_NUM){
            ptime now = second_clock::local_time();
            rc->hset(str_map_id,"time",to_iso_string(now));
        }
    }
}

void MapManager::addDelMapType(int type){
    std::map<int,int>::iterator m_itr = reqMap.find(type);
    if(m_itr==reqMap.end()){
        reqMap.insert(std::pair<int,int>(type,0));
    }else{
        m_itr->second++;
        if(m_itr->second>=param.TASK_SIZE){
            std::cout << "[MapManager]: delete map_type_id :"<< type << "  amount:" << m_itr->second << std::endl;
            TaskGenerator gen;
            gen.gen_task(type,m_itr->second,taskLst);
            m_itr->second=0;
        }
    }
}

bool MapManager::is_over_time(std::string str_map_id){
    std::string str_time = rc->hget(str_map_id,"time");
    ptime last_time = time_from_string(str_time);
    ptime now_time(second_clock::local_time());
    time_duration td = now_time - last_time;
    return td.seconds()>param.EXSIT_TIME;
}

bool MapManager::isexsit(int type){
    int num = rc->scard(boost::lexical_cast<std::string>(type));
    return num!=0;
}


