#ifndef _COMMON_CONFIGURE_
#define _COMMON_CONFIGURE_
#include "iostream"
class TaskList;

class Configure{
public:
    void init_configure();
    void configure_redis_map(TaskList & taskLst);
    void modify_configure_scheme(std::string json_config);
    void modify_and_config_reids(TaskList & taskLst,std::string json_config);
    void save_config_file(std::string & str_config);
    void read_config_file(std::string & str_config);
};

#endif
