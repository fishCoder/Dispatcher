#include "Configure.h"

#include <fstream>
#include <iostream>
#include <json/json.h>
#include <map>

#include "TaskGenerator.h"


using namespace std;

void Configure::init_configure(){

}
void Configure::save_config_file(string & str_config){
    try{
        ofstream ofs;
        ofs.open("config.dat");
        ofs << str_config ;
        ofs.close();
    }catch (...){

    }
}
void Configure::read_config_file(string & str_config){
    try{
        ifstream ifs;
        ifs.open("/home/lin/config.dat");
        ifs >> str_config ;
        ifs.close();
    }catch (...){

    }
}
void  Configure::configure_redis_map(TaskList & taskLst){
    string json_config;
    read_config_file(json_config);
//    std::cout << "[NodeServer]: json_config :" << json_config << std::endl;
    Json::Reader reader;
    Json::Value root;
    if(!reader.parse(json_config,root)) return ;

    map<int,int> reqMap;
    try{
        for(int i=0 ; i < root.size() ; i++){

            Json::Value tmp_root = root[i];
            int map_id = tmp_root["map"].asInt();
            int map_num = tmp_root["num"].asInt();
            reqMap.insert(pair<int,int>(map_id,map_num));
        }
    }catch(...){
        std::cout << "[NodeServer] error json format :" << json_config << std::endl;
    }

    TaskGenerator generator;
    generator.repMapToTask(reqMap,taskLst);
}
void Configure::modify_configure_scheme(std::string json_config){
    save_config_file(json_config);
}
void Configure::modify_and_config_reids(TaskList & taskLst,std::string json_config){
    modify_configure_scheme(json_config);
    configure_redis_map(taskLst);
}
