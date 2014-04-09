#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "Output.h"
#include "Dispatcher.h"

using namespace std;
/**
*以精灵进程方式启动
*/
int daemon_init(){
    pid_t pid;
    if((pid=fork())<0){
        return -1;
    }else if(pid != 0){
        exit(0);
    }
    setsid();
    umask(0);
    return 0;
}

int main(int argc,char ** argv)
{
    if(argc>1)daemon_init();

    boost::asio::io_service io_serv;
    Dispatcher dispatcher(io_serv);

    //boost::thread th(boost::bind(&boost::asio::io_service::run,&io_serv));
    io_serv.run();

    return 0;
}
