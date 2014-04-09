
#ifndef _COMMON
#define _COMMON

#include <json/json.h>
#include <redisclient.h>
#include <map>
#include "TaskList.h"
#include "RedisConnPool.h"
#include "Output.h"

class MapManager{
public:
    MapManager(TaskList & _taskLst);

    //初始化redis
    void init_redis();

    std::pair<unsigned int,std::string> get_field_map(int map_type_id,std::string & map_data,std::string & loot_npc_data);
    //对地图数量
    void useMapKey(shared_ptr_redis ptr_redis,std::string str_map_id,int type);

    //删除一个地图数据后，增加reqmap中的删除数量，删除数量大于一定
    void addDelMapType(int map_type_id);

    //redis中该地图模板是否为空
    bool isexsit(int map_type_id);

protected:
private:

    //after deteling , how mang we need to generate
    //key=地图模板id value=被删除的地图数量
    std::map<int,int>  reqMap;

    int total_used;

    TaskList & taskLst;

    redis::client rc;

    RedisConnPool redisPool;
};
#endif
