#include "Configure.h"

#include <fstream>
#include <iostream>
#include <json/json.h>
#include <map>
#include <boost/lexical_cast.hpp>

#include "TaskGenerator.h"


using namespace std;

void Configure::init_configure(){

}
void Configure::save_config_file(string & str_config){
    try{
        ofstream ofs;
        ofs.open("/home/lin/config.dat");
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
        for(unsigned int i=0 ; i < root.size() ; i++){

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
void Configure::change_config_scheme(int map_type_id,int map_amount,int opr_type){
    if(opr_type==-1)return;
    std::string json_config;
    read_config_file(json_config);

    Json::Reader reader;
    Json::FastWriter writer;
    Json::Value root;
    if(!reader.parse(json_config,root)) return ;
    try{
        if(opr_type==OPR_DEL){
            Json::Value tmp_root;
            for(unsigned int i=0 ; i < root.size() ; i++){
                if(root[i]["map"].asInt() != map_type_id){
                    tmp_root.append(root[i]);
                }
            }
            string str_config = writer.write(tmp_root);
            save_config_file(str_config);
            return;
        }
        if(opr_type==OPR_ADD){
            Json::Value child;
            child["map"] = map_type_id;
            child["num"] = map_amount;
            root.append(child);
        }else{
            for(unsigned int i=0 ; i < root.size() ; i++){
                if(root[i]["map"].asInt() == map_type_id){
                    root[i]["num"] = map_amount ;
                    break;
                }
            }
        }
        string str_config = writer.write(root);
        save_config_file(str_config);
    }catch(...){
        std::cout << "[NodeServer] error json format :" << json_config << std::endl;
    }
}


