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

int daemon_init(){
    pid_t pid;
    if((pid=fork())<0){
        return -1;
    }else if(pid != 0){
        exit(0);
    }
    setsid();
    //chdir();
    umask(0);
    return 0;
}

int main()
{
//    daemon_init();

    boost::asio::io_service io_serv;
    Dispatcher dispatcher(io_serv);
    io_serv.run();

    return 0;
}
