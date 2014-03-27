#include <unistd.h>
#include <boost/bind.hpp>

#include "Dispatcher.h"
#include "json/json.h"
#include "Configure.h"
#include "Output.h"
#include "Packege.h"
#include "TaskGenerator.h"
#include "Output.h"

#define TaskComplete 1
#define GeneratorOverTime 2

#define RequestMap   100
#define DeleteGameServer 101

#define CONFIGURE_DB 200
#define CONFIGURE_SCHEME 201
#define CONFIGURE_REQ 202
#define CHANGE_CONFIG 203

#define BUCKET_LOG 300



struct _settings_param_ param;

Dispatcher::Dispatcher(boost::asio::io_service &io_service):reader(),
                                                            msg_center(),
                                                            gs(io_service,msg_center),
                                                            taskLst(gs),
                                                            map_manager(taskLst),
                                                            game_server(io_service,msg_center),
                                                            node(io_service,msg_center),
                                                            log(param.TIME_BUCKET)

{
    msg_center.set_handle_func(boost::bind(&Dispatcher::dealMessage,this,_1));
}

void Dispatcher::dealMessage(std::string msg){
    Json::Value root;
    Configure configure;
    if(!reader.parse(msg,root))return;
    int _h_socket = 0;
    int field_map_id = 0;
    switch(root.get(OPERATE_TYEP,-1).asInt()){
    case TaskComplete:
        std::cout << "[Dispatcher->Parse]: TaskComplete" <<std::endl;
        _h_socket = root.get(_H_SOCKET,0).asInt();
        {
            int map_type_id = root.get("map",-1).asInt();
            int amount      = root.get("amount",-1).asInt();
            int map_size    = root.get("size",-1).asInt();
            log.add_record(map_type_id,amount,map_size);
        }

        if(!taskLst.empty()){
            std::string task = taskLst.pop_front();
            gs.send_task(_h_socket,task);
        }else{
            if(root.get("first",1).asInt())
                gs.set_generator_free(_h_socket);
        }
        break;
    case GeneratorOverTime:
        {
            _h_socket = root.get("id",0).asInt();
            std::string ip = gs.find_gen_by_id(_h_socket)->ptr_socket->remote_endpoint().address().to_string();
            log.slow_gen_log(root.get("map",-1).asInt(),root.get("duration",-1).asInt(),ip);
        }

        break;
    case RequestMap:
        std::cout << "[Dispatcher->Parse] : RequestMap" <<std::endl;
        _h_socket = root.get(_H_SOCKET,0).asInt();
        field_map_id = root.get("map",-1).asInt();
        if(map_manager.isexsit(field_map_id))
            game_server.send_field_map(_h_socket,field_map_id,root.get(SENCE_ID,-1).asUInt(), map_manager);
        else{
            //TaskGenerator task_generator;
            //task_generator.urgent_gen_task(field_map_id,taskLst);
            game_server.sendError(_h_socket,root.get(SENCE_ID,-1).asUInt());
        }

        break;
    case CONFIGURE_SCHEME:
        std::cout << "[Dispatcher->Parse]: CONFIGURE_SCHEME" << std::endl;
        configure.modify_configure_scheme(root.get(CFG_SCHEME,"{}").asString());
        break;
    case CONFIGURE_DB:
        std::cout << "[Dispatcher->Parse]: RUN_CONFIGURE_DB" << std::endl;
        configure.configure_redis_map(taskLst);
        break;
    case CONFIGURE_REQ:
        std::cout << "[Dispatcher->Parse]: request a scheme" << std::endl;
        _h_socket = root.get("id",0).asInt();
        {
            std::string config_scheme;
            configure.read_config_file(config_scheme);
            node.async_write(_h_socket,config_scheme);
        }
        break;
    case CHANGE_CONFIG:
        std::cout << "[Dispatcher->Parse]: CHANGE A CONFIG " <<std::endl;
        configure.change_config_scheme(root.get("map",0).asInt(),root.get("num",0).asInt(),root.get("opr",-1).asInt());
        break;
    case BUCKET_LOG:
        _h_socket = root.get("id",0).asInt();
        {
            int map_type_id = root.get("map",0).asInt();
            int bucket = root.get("bucket",0).asInt();
            std::string json_log = log.get_bucket_log(map_type_id,bucket);
            node.async_write(_h_socket,json_log);
        }
        break;
    }
}



