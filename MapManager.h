
#ifndef _COMMON
#define _COMMON

#include <json/json.h>
#include <redisclient.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <boost/shared_ptr.hpp>
#include "TaskList.h"



class MapManager{
public:
    MapManager(TaskList & _taskLst);

    boost::shared_ptr<std::string> getMap(int type);

    void useMapKey(int key,int type);
    void addDelType(int type);
    bool isexsit(int type);

protected:
private:

    //after deteling , how mang we need to generate
    std::map<int,int>  reqMap;
    int total_used;
    TaskList & taskLst;
    boost::shared_ptr<redis::client> rc;

};
#endif
