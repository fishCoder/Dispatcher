
#include "Dispatcher.h"
#include "json/json.h"
#include "Configure.h"
#include "Output.h"
#include "Packege.h"
#include "TaskGenerator.h"


#include <unistd.h>
#include <boost/bind.hpp>
#include <boost/date_time.hpp>


#define TaskComplete 1
#define GeneratorOverTime 2

#define RequestMap   100
#define DeleteGameServer 101

#define CONFIGURE_DB 200
#define CONFIGURE_SCHEME 201
#define CONFIGURE_REQ 202
#define CHANGE_CONFIG 203

#define BUCKET_LOG 300

#define DELETE_GAME_SERVER 400
#define DELETE_GENERATOR   401

using namespace boost::posix_time;

struct _settings_param_ param;


Dispatcher::Dispatcher(boost::asio::io_service &io_service):
                                                            msg_center(io_service),
                                                            gs(io_service,msg_center),
                                                            taskLst(gs),
                                                            map_manager(taskLst),
                                                            game_server(io_service,msg_center),
                                                            node(io_service,msg_center),
                                                            log(param.TIME_BUCKET)

{
    /**
    *绑定消息处理回调函数
    */
    msg_center.set_handle_func(boost::bind(&Dispatcher::dealMessage,this,_1));
    //初始化redis
    map_manager.init_redis();
}

void Dispatcher::dealMessage(std::string msg){
    /**
    *所有的接收的消息都在这里处理
    */
    ptime begintime = microsec_clock::local_time();

    Json::Reader reader;
    Json::Value root;
    Configure configure;
    if(!reader.parse(msg,root)){
        std::cout << "[Dispatcher->Parse]:  parse failed : " << msg <<std::endl;
        return;
    }
    int _h_socket = 0;
    int field_map_id = 0;
    switch(root.get(OPERATE_TYEP,-1).asInt()){
    case TaskComplete:
        //std::cout << "[Dispatcher->Parse]: TaskComplete" <<std::endl;
        /**
        *收到这个消息有两种情况
        *一，生成器完成了任务，返回消息。调度器查询任务队列是否为空，不为空，则取出一个任务发给生成器；空则设置调度器状态为空闲
        *二，生成器初次连接时，消息中会多一个first字段，可以判断是否是初次连接
        */
        _h_socket = root.get(_H_SOCKET,0).asInt();
        {
            //地图类型id
            int map_type_id = root.get("map",-1).asInt();
            //生成数量
            int amount      = root.get("amount",-1).asInt();
            //生成地图大小
            int map_size    = root.get("size",-1).asInt();
            //生成一条记录
            log.add_record(map_type_id,amount,map_size);
        }

        if(!taskLst.empty()){
            std::string task = taskLst.pop_front();
            gs.send_task(_h_socket,task);
        }else{
            //if(root.get("first",1).asInt())
                gs.set_generator_free(_h_socket);
        }
        break;
    case GeneratorOverTime:
        {
            /**
            *当有生成地图超过规定时间时 生成器发送这个信息
            */
            _h_socket = root.get("id",0).asInt();
            std::string ip = gs.find_gen_by_id(_h_socket)->ptr_socket->remote_endpoint().address().to_string();
            log.slow_gen_log(root.get("map",-1).asInt(),root.get("duration",-1).asInt(),ip);
        }

        break;
    case RequestMap:
        //std::cout << "[Dispatcher->Parse] : RequestMap" <<std::endl;
        /**
        *游戏服务器请求地图
        *判断这类型的地图数据库中是否有剩余
        */
        _h_socket = root.get(_H_SOCKET,0).asInt();
        field_map_id = root.get("map",-1).asInt();
        //std::cout << "[Dispatcher][map_send] : " << ++map_send <<std::endl;
        if(map_manager.isexsit(field_map_id))
            game_server.send_field_map(_h_socket,field_map_id,root.get(SENCE_ID,-1).asUInt(), map_manager);
        else{
            //TaskGenerator task_generator;
            game_server.sendError(_h_socket,root.get(SENCE_ID,-1).asUInt());
            std::cout << "[Dispatcher->Parse]["<<_h_socket<<"] : urgent_gen_task" <<std::endl;
            //log.no_map_log(field_map_id,taskLst.size());
            gs.urgent_gen_task(taskLst);

        }
        break;
    case CONFIGURE_SCHEME:
        std::cout << "[Dispatcher->Parse]: CONFIGURE_SCHEME" << std::endl;
        configure.modify_configure_scheme(root.get(CFG_SCHEME,"{}").asString());
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
    case BUCKET_LOG:
        /**
        *node服务器请求地图生成日志数据
        */
        _h_socket = root.get("id",0).asInt();
        {
            int map_type_id = root.get("map",0).asInt();
            int bucket = root.get("bucket",0).asInt();

            std::string json_log = log.get_bucket_log(map_type_id,bucket);
            node.async_write(_h_socket,json_log);
        }
        break;
    case DELETE_GAME_SERVER:
        /**
        *删除一个游戏服务器连接
        */
        std::cout << "[Dispatcher->Parse]: DELETE_GAME_SERVER" <<std::endl;
        game_server.delete_socket_by_id(root.get("hsocket",0).asInt());
        break;
    case DELETE_GENERATOR:
        /**
        *删除一个生成器连接
        */
        std::cout << "[Dispatcher->Parse]: DELETE_GENERATOR" <<std::endl;
        gs.delete_socket_by_id(root.get("hsocket",0).asInt());
        break;
    }
    ptime endtime = microsec_clock::local_time();
    time_duration td = endtime - begintime;
    int duration = td.total_milliseconds();
    if(duration>param.SLOW_DEAL){
        log.slow_deal_log(msg,duration);
    }
    //std::cout << "[Dispatcher][dealMessage]total time :" << duration << std::endl;
}



