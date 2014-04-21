
#ifndef _REDIS_CONNECTION_POOL
#define _REDIS_CONNECTION_POOL

#include <redisclient.h>
#include <boost/shared_ptr.hpp>
#include <vector>

#include <boost/thread/mutex.hpp>



#include "Output.h"

typedef boost::shared_ptr<redis::client> shared_ptr_redis;

typedef struct _redis_client{
    bool isusing;
    shared_ptr_redis ptr_redis;
    _redis_client():ptr_redis(){
        while(!connect());
        isusing = false;
    }
    bool connect(){
        try{
            ptr_redis.reset(new redis::client(param.redis_address,param.redis_port));
            return true;
        }catch(redis::connection_error &e){
            std::cout << e.what() << std::endl;
            return false;
        }
    }
} redis_struct;
/**
*redis连接池 用vector管理redisclient
*当从连接池取redis连接时，查看是否有空闲的连接池
*有则取出，一个并标记该连接为忙
*
*使用辅助类ScopeRedis可以当不用连接时 自动释放该连接
*
*如何获得一个redisclient
*1.get_redis_index()返回一个vector的下标
*2.operator[] 运算符重载 传入一个下标 ，返回一个redisclient指针
*/

class RedisConnPool{
public:
    RedisConnPool();
    int get_redis_index();
    void set_redis_free(int index);
    shared_ptr_redis operator[](int index);
protected:
    void setRedisClientUsed(int index);
private:
    boost::mutex rc_mtx;
    boost::mutex get_mtx;
    vector<redis_struct *> redis_vect;
};

/**
*redis连接使用范围内 构造这个对象
*当退出这个范围时 这个对象调用析构函数释放资源
*/

class ScopeRedis{
public:
    ScopeRedis(int _index,RedisConnPool & _redis_pool):redis_pool(_redis_pool){
        index = _index;
    }
    ~ScopeRedis(){
        redis_pool.set_redis_free(index);
    }
private:
    int index;
    RedisConnPool & redis_pool;
};
#endif // _REDIS_CONNECTION_POOL




