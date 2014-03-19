
#ifndef _COMMON
#define _COMMON

#include <json/json.h>
#include <redisclient.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include "TaskList.h"



class MapManager{
public:
    MapManager(TaskList & _taskLst);

    std::pair<unsigned int,std::string> get_field_map(int map_type_id,std::string & map_data,std::string & loot_npc_data);

    void useMapKey(std::string str_map_id,int type);
    void addDelType(int type);
    bool isexsit(int type);
    bool is_over_time(std::string str_map_id);

protected:
private:

    //after deteling , how mang we need to generate
    std::map<int,int>  reqMap;
    int total_used;
    TaskList & taskLst;
    boost::shared_ptr<redis::client> rc;

};
#endif
