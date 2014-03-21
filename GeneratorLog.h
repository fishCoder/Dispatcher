
#ifndef _COMMON_GENERATOR_LOG_
#define _COMMON_GENERATOR_LOG_

#include "SQLiteHelper.h"
#include <boost/date_time.hpp>

using namespace boost::posix_time;

class GeneratorLog{
public:
    GeneratorLog(int _time_bucket);
    void add_record(int map_type_id,int amount,int map_size);
    std::string get_bucket_log(int map_type_id,int bucket);
    void slow_gen_log(int map_type_id,int duration,std::string ip);
protected:
    void start_record();
    bool is_over_bucket();
private:
    SQLiteHelper sqlite;
    int time_bucket;
    ptime * start_time;
};

#endif // _COMMON_GENERATOR_LOG_
