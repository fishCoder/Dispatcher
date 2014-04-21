#include "MapManager.h"
#include "TaskGenerator.h"
#include "Packege.h"
#include "Configure.h"


#include <json/json.h>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>



#define MAX_GET_TIMES 5


using namespace boost::posix_time;

MapManager::MapManager(TaskList & _taskLst):taskLst(_taskLst),rc(param.redis_address,param.redis_port),redisPool(){
    total_used = 0;
}

void MapManager::init_redis(){
    /**
    *当程序启动时调用此函数，从地图配置文件中读取数据
    *将读取的数据和redis存储中的数据比较，当redis地图数量小于配置数量时
    *则生成不足的数量，使之和配置数据吻合
    */
    Configure config;
    std::string json_config;
    config.read_config_file(json_config);


    int index_redis = redisPool.get_redis_index();
    shared_ptr_redis ptr_redis = redisPool[index_redis];
    ScopeRedis r(index_redis,redisPool);

    Json::Reader reader;
    Json::Value root;
    if(!reader.parse(json_config,root)){
        std::cout << "[MapManager]:地图配置文件格式出错" << std::endl;
        return;
    }

    map<int,int> reqMap;//map的key是地图模板id value是数量

    for(unsigned int i=0 ; i < root.size() ; i++){
        Json::Value tmp_root = root[i];
        int map_type_id = tmp_root["map"].asInt();
        int map_amount = tmp_root["num"].asInt();

        std::stringstream ss_map_type;
        ss_map_type << "map_type_" << map_type_id;

        int redis_map_amout = ptr_redis->scard(ss_map_type.str());
        if(map_amount>redis_map_amout){

            int gen_amount = map_amount-redis_map_amout;
            reqMap.insert(pair<int,int>(map_type_id,gen_amount));

        }else{

        }
    }

    TaskGenerator generator;
    //生成任务
    generator.repMapToTask(reqMap,taskLst);
}

std::pair<unsigned int,std::string> MapManager::get_field_map(int map_type_id,std::string & map_data,std::string & loot_npc_data){

    //boost::mutex::scoped_lock  lock(mm_mtx);
    std::string str_map_id;

    /**
    *从redis连接池获取一个redis实例指针
    */
    int index_redis = redisPool.get_redis_index();
    shared_ptr_redis ptr_redis = redisPool[index_redis];
    ScopeRedis r(index_redis,redisPool);

    /**
    *根据map类型随机从redis的set中取出一个实例地图的id
    *注：这个redisclient库的srandmember接口会在取出一个id后删除该id
    */
    //std::cout << "[MapManager][get_field_map] map_type_id :" << map_type_id << std::endl;
    /**
    *地图类型id(key)：map_type_xxxx
    */
    std::stringstream ss_map_type;
    ss_map_type << "map_type_" << map_type_id;

    /**
    *地图实例id(key)：ins_xxxxxxxxxx
    * ins = instance
    */

    str_map_id = ptr_redis->srandmember(ss_map_type.str());

    //std::cout << "[MapManager][get_field_map] str_map_id :" << str_map_id << std::endl;
    ptr_redis->sadd(boost::lexical_cast<std::string>(ss_map_type.str()),str_map_id);


    useMapKey(ptr_redis,str_map_id,map_type_id);


    map_data.assign(ptr_redis->hget(str_map_id,"map"));
    loot_npc_data.assign(ptr_redis->hget(str_map_id,"npc"));
    std::string str_verify_code = ptr_redis->hget(str_map_id,"code");

    unsigned int verify_code = boost::lexical_cast<unsigned int>(str_verify_code);

    return std::pair<unsigned int,std::string>(verify_code,str_map_id);
}



void MapManager::useMapKey(shared_ptr_redis ptr_redis,std::string str_map_id,int map_type_id){
    /**
    *取出该地图实例的使用次数
    */
    std::string str_use_time = ptr_redis->hget(str_map_id,"use");


    std::stringstream ss_map_type;
    ss_map_type << "map_type_" << map_type_id;

    int use_time = boost::lexical_cast<int>(str_use_time);
    use_time++;
    if(use_time >= param.MAX_USE_NUM){
        /**
        *如果这个地图实例使用次数超过设定的次数
        *设定在其存活时间expire（单位：秒）
        *然后在该地图类型set中删除该id
        */
        ptr_redis->expire(str_map_id,param.EXSIT_TIME);
        ptr_redis->srem(ss_map_type.str(),str_map_id);
        addDelMapType(map_type_id);
    }else{
        ptr_redis->hset(str_map_id,"use",boost::lexical_cast<std::string>(use_time));

    }

}

void MapManager::addDelMapType(int map_type_id){
    /**
    *当删除某类型的地图后，该地图类型的待生成数量加一
    *当待生成数量等于生成任务大小【param.TASK_SIZE】
    *生成一个任务
    */
    std::map<int,int>::iterator m_itr = reqMap.find(map_type_id);
    if(m_itr==reqMap.end()){
        reqMap.insert(std::pair<int,int>(map_type_id,0));
    }else{
        m_itr->second++;
        if(m_itr->second>=param.TASK_SIZE){
            //std::cout << "[MapManager]: delete map_type_id :"<< map_type_id << "  amount:" << m_itr->second << std::endl;
            TaskGenerator gen;
            gen.gen_task(map_type_id,m_itr->second,taskLst);
            m_itr->second=0;
        }
    }
}


bool MapManager::isexsit(int map_type_id){

    int index_redis = redisPool.get_redis_index();
    shared_ptr_redis ptr_redis = redisPool[index_redis];
    ScopeRedis r(index_redis,redisPool);

    std::stringstream ss_map_type;
    ss_map_type << "map_type_" << map_type_id;

    int num = ptr_redis->scard(ss_map_type.str());
    //redisPool.set_redis_free(index_redis);
    return num >= 5;
}
