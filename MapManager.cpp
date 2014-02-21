#include "MapManager.h"
#include "TaskGenerator.h"
#include <boost/lexical_cast.hpp>


#define MAX_USE_NUM 1
#define DEL_MIN_LIMITED 100


MapManager::MapManager(TaskList & _taskLst):taskLst(_taskLst){
    rc.reset(new redis::client("localhost",6379));
    total_used = 0;
}

boost::shared_ptr<std::string>  MapManager::getMap(int type){
    std::string skey = rc->srandmember(boost::lexical_cast<std::string>(type));
    int key = boost::lexical_cast<int>(skey);
    boost::shared_ptr<std::string> rmap(new  std::string(rc->hget(skey,"use")));
    useMapKey(key,type);
    return rmap;
}

void MapManager::useMapKey(int key,int type){
    std::string _key = boost::lexical_cast<std::string>(key);
    int use_time = boost::lexical_cast<int>(rc->hget(_key,"use"));

    if(use_time >= MAX_USE_NUM){
        total_used++;
        rc->del(_key);
        addDelType(type);
    }else{
        rc->sadd(boost::lexical_cast<std::string>(type),boost::lexical_cast<std::string>(key));
        rc->hset(_key,"use",boost::lexical_cast<std::string>(++use_time));
    }

 //   if(total_used > DEL_MIN_LIMITED){
 //       TaskGenerator tg;
 //       tg.repMapToTask(reqMap,taskLst);
 //   }
}

void MapManager::addDelType(int type){
    std::map<int,int>::iterator m_itr = reqMap.find(type);
    if(m_itr==reqMap.end()){
        reqMap.insert(std::pair<int,int>(type,0));
    }else{
        m_itr->second++;
    }
}


bool MapManager::isexsit(int type){
    int num = rc->scard(boost::lexical_cast<std::string>(type));
    return num!=0;
}


