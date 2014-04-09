//
//主要用于读取，写入以及修改地图配置文件
//

#ifndef _COMMON_CONFIGURE_
#define _COMMON_CONFIGURE_
#include "iostream"

//操作类型 修改
#define OPR_MOD 1
//操作类型 增加
#define OPR_ADD 2
//操作类型 删除
#define OPR_DEL 3

class TaskList;

class Configure{
public:
    //将修改后的json配置数据保存到文件
    void modify_configure_scheme(std::string  json_config);
    //修改并生成任务
    void modify_and_config_reids(TaskList & taskLst,std::string json_config);

    void save_config_file(std::string & str_config);
    //读取配置文件
    void read_config_file(std::string & str_config);

    //map_type_id   地图模板id
    //map_amount    地图数量
    //opr_type      操作类型
    //修改配置文件内容
    void change_config_scheme(int map_type_id,int map_amount,int opr_type);
};

#endif
