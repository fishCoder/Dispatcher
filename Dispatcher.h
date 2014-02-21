#ifndef _COMMON_DISPATCHER_
#define _COMMON_DISPATCHER_

#include "GeneratorServer.h"
#include "GameServerManager.h"
#include "MessageList.h"
#include "TaskList.h"
#include "boost/threadpool.hpp"
#include <iostream>

class Dispatcher:boost::asio::coroutine{
public:
    Dispatcher(boost::asio::io_service &io_service);
    void handleMessage();
    void supervise();
protected:
private:
    boost::threadpool::pool tpool;
    MessageList msgLst;
    TaskList taskLst;
    GeneratorServer gs;
    GameSeverManager gsm;
};

#endif
