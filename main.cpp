#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "Dispatcher.h"

using namespace std;



int main()
{
    boost::asio::io_service io_serv;
    Dispatcher dispatcher(io_serv);
    boost::thread thd(boost::bind(&Dispatcher::handleMessage,&dispatcher));
    io_serv.run();

    return 0;
}
