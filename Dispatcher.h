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
#include <iostream>

class Dispatcher{
public:
    Dispatcher(boost::asio::io_service &io_service);
    void dealMessage(std::string msg);
protected:
private:
    Json::Reader reader;
    MessageCenter msg_center;
    Generator gs;
    TaskList taskLst;
    MapManager map_manager;
    GameSever  game_server;
    NodeServer node;
    GeneratorLog log;
};

#endif
