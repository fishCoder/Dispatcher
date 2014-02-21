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
 //   boost::thread thd1(boost::bind(&Dispatcher::supervise,&dispatcher));
    io_serv.run();
    cout << "Hello world!" << endl;
    return 0;
}
