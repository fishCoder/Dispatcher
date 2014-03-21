#include "INIReader.h"
//端口

#define GeneratorServerPort  9010
#define GameServerPort  9091
#define NodeJsPort 9099

//Json 协议

#define OPERATE_TYEP "type"
#define SENCE_ID "sence"
#define MAP_ID   "map"
#define _H_SOCKET "id"
#define CFG_SCHEME "scheme"
#define AMOUNT "num"

//static bool isoutput = true;

//void outputmsg(std::string msg){
//    if(isoutput)
//       std::cout << msg << std::endl;
//}
#ifndef _DISPATCHER_SET_PARAM_
#define _DISPATCHER_SET_PARAM_


struct _settings_param_{
    std::string redis_address;
    int redis_port;
    int TASK_SIZE;
    int MAX_GET_TIMES;
    int MAX_USE_NUM;
    int EXSIT_TIME;
    int THREAD_POOL_NUM;
    int TIME_BUCKET;
    int is_run_config;
    _settings_param_(){
        INI ini("dispatcher.ini");
        redis_address = ini.get("dispatcher.redisaddress");
        redis_port    = ini.get_int("dispatcher.redisport");
        EXSIT_TIME    = ini.get_int("dispatcher.exsit_time");
        TASK_SIZE     = ini.get_int("dispatcher.task_size");
        MAX_USE_NUM   = ini.get_int("dispatcher.map_max_use");
        THREAD_POOL_NUM = ini.get_int("dispatcher.thread_pool_num");
        TIME_BUCKET   = ini.get_int("dispatcher.time_bucket");
    //    is_run_config = ini.get_int("dispatcher.runconfig");
    };
};
extern struct _settings_param_ param;
#endif // _DISPATCHER_SET_PARAM_
