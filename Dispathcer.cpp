#include "Dispatcher.h"
#include "json/json.h"
#include <unistd.h>
#include <boost/bind.hpp>
#include "Configure.h"


#define TaskComplete 1


#define RequestMap   100
#define DeleteGameServer 101

#define CONFIGURE_DB 200
#define CONFIGURE_SCHEME 201
#define CONFIGURE_REQ 202
#include "MessageList.h"



Dispatcher::Dispatcher(boost::asio::io_service &io_service):tpool(2),
                                                            msgLst(),
                                                            taskLst(),
                                                            gs(io_service,msgLst),
                                                            gsm(io_service,msgLst,taskLst),
                                                            node(io_service,msgLst)

{
//    for(int i=0 ; i<100; i++)
//        taskLst.push_back("{\"type\":3,\"map\":1001,\"num\":100}");
}
void Dispatcher::handleMessage(){
    Json::Reader reader;

    while(true){
        //tpool.schedule(boost::bind(&Dispatcher::supervise,this));
        supervise();
        try{
            if(!msgLst.empty()){
                Json::Value root;
                Configure configure;

                std::string msg =  msgLst.pop_front();
                std::cout << "[Dispatcher->handleMessage] :" << msg <<std::endl;
//               std::cout << "[Dispatcher] tasklist size() " << taskLst.size() <<std::endl;

                if(!reader.parse(msg,root))continue;
                int id = 0;
                int _map = 0;
                switch(root.get("type",-1).asInt()){
                case TaskComplete:
                    std::cout << "Parse : TaskComplete" <<std::endl;
                    id = root.get("id",0).asInt();
                    if(taskLst.empty())
                        gs.setState(id,0);
                    else{
                        std::string task = taskLst.pop_front();
                        gs.sendTask(id,task);
                    }
                    break;
                case RequestMap:
                    std::cout << "[Dispatcher->Parse] : RequestMap" <<std::endl;
                    id = root.get("id",0).asInt();
                    _map = root.get("map",-1).asInt();
                    tpool.schedule(boost::bind(&GameSeverManager::sendMap,&gsm,id,_map,root.get("sence",-1).asInt()));
                    break;
                case DeleteGameServer:
                    id = root.get("id",0).asInt();
                    std::cout << "[Dispatcher->Parse]: DeleteGameServer : " << id << std::endl;
                    tpool.schedule(boost::bind(&GameSeverManager::delGameServer,&gsm,id));
                    break;
                case CONFIGURE_SCHEME:
                    std::cout << "[Dispatcher->Parse]: CONFIGURE_SCHEME" <<std::endl;
                    tpool.schedule(boost::bind(&Configure::modify_configure_scheme,&configure,root.get("scheme","{}").asString()));
                    break;
                case CONFIGURE_DB:
                    std::cout << "[Dispatcher->Parse]: CONFIGURE_DB" <<std::endl;
                    //tpool.schedule(boost::bind(&Configure::configure_redis_map,&configure,taskLst));
                    configure.configure_redis_map(taskLst);
                    break;
                case CONFIGURE_REQ:
                    std::cout << "[Dispatcher->Parse]: request a scheme" <<std::endl;
                    id = root.get("id",0).asInt();
                    std::string config_scheme;
                    configure.read_config_file(config_scheme);
                    node.async_write(id,config_scheme);
                    break;
                }
            }
        }catch(...){
            std::cout << " [Dispatcher->handleMessage]:catch exception" << std::endl;
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


