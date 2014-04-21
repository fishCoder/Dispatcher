#ifndef _COMMON_DISPATCHER_
#define _COMMON_DISPATCHER_

#include "GeneratorClass.h"
#include "GameServerClass.h"
#include "NodeServer.h"
#include "MessageCenter.h"
#include "TaskList.h"
#include "MapManager.h"
#include "GeneratorLog.h"
#include "MessageCenter.h"
#include "Output.h"
#include <iostream>



class Dispatcher{
public:
    Dispatcher(boost::asio::io_service &io_service);
    /**
    *处理消息 最后所以接收到的消息都将传递到这个函数
    */
    void dealMessage(std::string msg);
protected:
private:

    /**
    *MessageCenter 用于回调处理消息函数
    */
    MessageCenter msg_center;
    /**
    *Generator 负责与生成器通信
    */
    Generator gs;
    /**
    *TaskList 任务列表，存储生成地图任务
    */
    TaskList taskLst;
    /**
    *MapManager 负责与管理redis的数据
    */
    MapManager map_manager;
    /**
    *GameSever 负责与游戏服务器通信
    */
    GameSever  game_server;
    NodeServer node;
    /**
    *GeneratorLog 日志
    */
    GeneratorLog log;
};

#endif
