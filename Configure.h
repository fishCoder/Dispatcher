#ifndef _COMMON_CONFIGURE_
#define _COMMON_CONFIGURE_
#include "iostream"

#define OPR_MOD 1
#define OPR_ADD 2
#define OPR_DEL 3

class TaskList;

class Configure{
public:
    void init_configure();
    void configure_redis_map(TaskList & taskLst);
    void modify_configure_scheme(std::string json_config);
    void modify_and_config_reids(TaskList & taskLst,std::string json_config);
    void save_config_file(std::string & str_config);
    void read_config_file(std::string & str_config);
    void change_config_scheme(int map_type_id,int map_amount,int opr_type);
};

#endif
