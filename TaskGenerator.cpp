#include "TaskGenerator.h"
#include "TaskList.h"
#include "Output.h"
#include <json/json.h>



void TaskGenerator::repMapToTask(std::map<int,int> &reqMap,TaskList &lst){
    std::map<int,int>::iterator itr;
    Json::FastWriter  writer;
    for(itr = reqMap.begin();itr!=reqMap.end();itr++){
        int map_type_id = itr->first;
        int map_num = itr->second;
        while(hasNext(map_num)){
            Json::Value root;
            root["type"] = 3;
            root["map"] = map_type_id;
            root["num"] = param.TASK_SIZE;
            map_num -= param.TASK_SIZE;
        //    std::cout << "[TaskGenerator]: task :" << writer.write(root) << std::endl;
            lst.push_back(writer.write(root));
        }
        if(map_num > 0){
            Json::Value root;
            root["type"] = 3;
            root["map"] = map_type_id;
            root["num"] = map_num;
            lst.push_back(writer.write(root));
        }
        reqMap.erase(map_type_id);
        //std::cout << "[TaskGenerator]: tasklist size :" << lst.size() << std::endl;

    }
}
bool TaskGenerator::hasNext(int num){
    return num >= param.TASK_SIZE;
}

void TaskGenerator::gen_task(int map_type_id,int amount,TaskList &lst){
    Json::FastWriter  writer;
    Json::Value root;
    root["type"] = 3;
    root[MAP_ID] = map_type_id;
    root[AMOUNT] = amount;
    lst.push_back(writer.write(root));
}

void TaskGenerator::urgent_gen_task(int map_type_id,TaskList &lst){
    Json::FastWriter  writer;
    Json::Value root;
    root["type"] = 3;
    root[MAP_ID] = map_type_id;
    root[AMOUNT] = param.TASK_SIZE;
    lst.push_front(writer.write(root));
}
