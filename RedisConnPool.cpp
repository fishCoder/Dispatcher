#include "RedisConnPool.h"


#include <boost/thread/mutex.hpp>

boost::mutex rc_mtx;
boost::mutex get_mtx;

RedisConnPool::RedisConnPool(){
    for(int i=0 ; i<param.THREAD_POOL_NUM+1 ; i++){
        redis_vect.push_back(new redis_struct());
    }
}
int RedisConnPool::get_redis_index(){
    boost::mutex::scoped_lock  lock(get_mtx);
    while(true){
        for(int i=0 ; i<param.THREAD_POOL_NUM+1 ;i++){
            if(!redis_vect[i]->isusing){
                redis_vect[i]->isusing = true;
                //std::cout << "[RedisConnPool] redis_vect[" << i << "] isusing: " << redis_vect[i]->isusing <<std::endl;
                setRedisClientUsed(i);
                return i;
            }
        }
    }
}
void RedisConnPool::setRedisClientUsed(int index){
    boost::mutex::scoped_lock  lock(rc_mtx);
    redis_vect[index]->isusing = true;
}
void RedisConnPool::set_redis_free(int index){
    boost::mutex::scoped_lock  lock(rc_mtx);
    redis_vect[index]->isusing=false;
}
shared_ptr_redis RedisConnPool::operator[](int index){
    return redis_vect[index]->ptr_redis;
}
