#include "Dispatcher.h"
#include "json/json.h"
#include <unistd.h>
#include <boost/bind.hpp>



#define TaskComplete 1


#define RequestMap   100
#define DeleteGameServer 101

#define CONFIGURE_DB 200

#include "MessageList.h"



Dispatcher::Dispatcher(boost::asio::io_service &io_service):tpool(2),
                                                            msgLst(),
                                                            taskLst(),
                                                            gs(io_service,msgLst),
                                                            gsm(io_service,msgLst,taskLst)

{
    for(int i=0 ; i<100; i++)
        taskLst.push_back("{\"type\":3,\"map\":1001,\"num\":100}");

}
void Dispatcher::handleMessage(){
    std::cout << "Dispatcher::handleMessage()" << std::endl;
    Json::Reader reader;

    while(true){
        tpool.schedule(boost::bind(&Dispatcher::supervise,this));
        try{
            if(!msgLst.empty()){
                Json::Value root;
                std::string msg =  msgLst.pop_front();
                std::cout << "Dispatcher::handleMessage() msg:" << msg <<std::endl;

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
                    std::cout << "Parse : RequestMap" <<std::endl;
                    id = root.get("id",0).asInt();
                    _map = root.get("map",-1).asInt();
                    tpool.schedule(boost::bind(&GameSeverManager::sendMap,&gsm,id,_map,root.get("sence",-1).asInt()));
                //    gsm.sendMap(id,_map);
                    break;
                case DeleteGameServer:
                    std::cout << "Parse : DeleteGameServer" <<std::endl;
                    id = root.get("id",0).asInt();
                    tpool.schedule(boost::bind(&GameSeverManager::delGameServer,&gsm,id));
                 //   gsm.delGameServer(id);
                    break;
                }
            }
        }catch(...){
            std::cout << "catch Dispatcher::handleMessage()" << std::endl;
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
        std::cout << "catch Dispatcher::supervise()" << std::endl;
    }

}


