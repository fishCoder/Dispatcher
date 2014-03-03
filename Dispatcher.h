#ifndef _COMMON_DISPATCHER_
#define _COMMON_DISPATCHER_

#include "GeneratorServer.h"
#include "GameServerClass.h"
#include "NodeServer.h"
#include "MessageList.h"
#include "TaskList.h"
#include "boost/threadpool.hpp"
#include "MapManager.h"

#include <iostream>

class Dispatcher{
public:
    Dispatcher(boost::asio::io_service &io_service);
    void handleMessage();
    void supervise();
protected:
private:
    boost::threadpool::pool thread_pool;
    MessageList msgLst;
    TaskList taskLst;
    MapManager map_manager;
    GeneratorServer gs;
    GameSever  game_server;
    NodeServer node;
};

#endif
