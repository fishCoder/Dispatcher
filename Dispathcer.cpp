#include "Dispatcher.h"
#include "json/json.h"
#include <unistd.h>
#include <boost/bind.hpp>
#include "Configure.h"
#include "Output.h"
#include "Packege.h"
#include "TaskGenerator.h"

#define TaskComplete 1


#define RequestMap   100
#define DeleteGameServer 101

#define CONFIGURE_DB 200
#define CONFIGURE_SCHEME 201
#define CONFIGURE_REQ 202
#define CHANGE_CONFIG 203

#define BUCKET_LOG 300

#include "MessageList.h"



Dispatcher::Dispatcher(boost::asio::io_service &io_service):thread_pool(2),
                                                            msgLst(),
                                                            taskLst(),
                                                            map_manager(taskLst),
                                                            gs(io_service,msgLst),
                                                            game_server(io_service,msgLst),
                                                            node(io_service,msgLst),
                                                            log(3600)

{
//    for(int i=0 ; i<100; i++)
//        taskLst.push_back("{\"type\":3,\"map\":1001,\"num\":100}");
}
void Dispatcher::handleMessage(){
    Json::Reader reader;

    while(true){
        //tpool.schedule(boost::bind(&Dispatcher::supervise,this));
        supervise();

        if(!msgLst.empty()){
            Json::Value root;
            Configure configure;

            std::string msg =  msgLst.pop_front();
            std::cout << "[Dispatcher] :" << msg <<std::endl;
//               std::cout << "[Dispatcher] tasklist size() " << taskLst.size() <<std::endl;

            if(!reader.parse(msg,root))continue;
            int _h_socket = 0;
            int field_map_id = 0;
            switch(root.get(OPERATE_TYEP,-1).asInt()){
            case TaskComplete:
                std::cout << "Parse : TaskComplete" <<std::endl;
                _h_socket = root.get(_H_SOCKET,0).asInt();
                {;
                    int map_type_id = root.get("map",-1).asInt();
                    int amount      = root.get("amount",-1).asInt();
                    int map_size    = root.get("size",-1).asInt();
                    log.add_record(map_type_id,amount,map_size);
                }
                if(taskLst.empty())
                    gs.setState(_h_socket,0);
                else{
                    std::string task = taskLst.pop_front();
                    gs.sendTask(_h_socket,task);
                }
                break;
            case RequestMap:
                std::cout << "[Dispatcher->Parse] : RequestMap" <<std::endl;
                _h_socket = root.get(_H_SOCKET,0).asInt();
                field_map_id = root.get("map",-1).asInt();
                if(map_manager.isexsit(field_map_id))
                    thread_pool.schedule(boost::bind(&GameSever::send_field_map,&game_server, _h_socket,field_map_id,root.get(SENCE_ID,-1).asUInt(), map_manager));
                else{
                    TaskGenerator task_generator;
                    task_generator.urgent_gen_task(field_map_id,taskLst);

                    reply_map reply;
                    reply.result = 0;
                    game_server.async_write(_h_socket,&reply,sizeof(reply_map));
                }

                break;
            case CONFIGURE_SCHEME:
                std::cout << "[Dispatcher->Parse]: CONFIGURE_SCHEME" <<std::endl;
                thread_pool.schedule(boost::bind(&Configure::modify_configure_scheme,&configure,root.get(CFG_SCHEME,"{}").asString()));
                break;
            case CONFIGURE_DB:
                std::cout << "[Dispatcher->Parse]: RUN_CONFIGURE_DB" <<std::endl;
                //thread_pool.schedule(boost::bind(&Configure::configure_redis_map,&configure,taskLst));
                configure.configure_redis_map(taskLst);
                break;
            case CONFIGURE_REQ:
                std::cout << "[Dispatcher->Parse]: request a scheme" <<std::endl;
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

    }

}

void Dispatcher::supervise(){
    try{
        int gid = gs.getFreeGenrator();
        if(!taskLst.empty()&&gid>0){
            gs.sendTask(gid,taskLst.pop_front());
            gs.setState(gid,1);
        }
    }catch(...){
        std::cout << "[Dispatcher] catch supervise" << std::endl;
    }

}


