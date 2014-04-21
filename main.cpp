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
void signalProc(int sig)
{
        switch(sig)
        {
                case SIGPIPE:
                        cout << "[SIGNAL] : SIGPIPE" << endl;
                break;
                case SIGSEGV:
                        cout << "[SIGNAL] : SIGSEGV" << endl;
                        exit(-1);
                break;
                default:
                        cout << "Unknow signal!" << endl;
                break;
        }
        return;
}


int main(int argc,char ** argv)
{
    if(argc>1)daemon_init();

    signal(SIGPIPE, signalProc);
    signal(SIGSEGV, signalProc);
    boost::asio::io_service io_serv;
    Dispatcher dispatcher(io_serv);


    boost::thread_group threadGroup;

    for(int i=0;i<param.THREAD_POOL_NUM;i++){
        threadGroup.create_thread(boost::bind(&boost::asio::io_service::run,&io_serv));
    }
    threadGroup.join_all();
    return 0;
}
