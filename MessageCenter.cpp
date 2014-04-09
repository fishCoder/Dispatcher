#include "MessageCenter.h"


MessageCenter::MessageCenter(boost::asio::io_service & io_server):io_ser(io_server),thread_pool(param.THREAD_POOL_NUM){

}

void MessageCenter::push_message(std::string msg){
    //deal_message(msg);
    //thread_pool.schedule(boost::bind(deal_message,msg));
    thread_pool.schedule(prio_task_func(5,boost::bind(deal_message,msg)));
    //io_ser.post(boost::bind(deal_message,msg));
}

void MessageCenter::push_message_high_prio(std::string msg){
    //thread_pool.schedule(boost::bind(deal_message,msg));
    //thread_pool += boost::threadpool::prio_task_func(50,boost::bind(deal_message,msg));
    thread_pool.schedule(prio_task_func(50,boost::bind(deal_message,msg)));
    //deal_message(msg);
    //io_ser.post(boost::bind(deal_message,msg));
}

void MessageCenter::push_task(boost::function<void()>  const & func){
    thread_pool.schedule(prio_task_func(5,func));
}

void MessageCenter::set_handle_func(boost::function<void(std::string)>  const & func){
    this->deal_message = func;
}
