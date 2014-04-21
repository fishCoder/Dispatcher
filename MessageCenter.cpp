#include "MessageCenter.h"




MessageCenter::MessageCenter(boost::asio::io_service & io_server):io_ser(io_server){

}

void MessageCenter::push_message(std::string msg){
    boost::mutex::scoped_lock  lock(mc_mtx);
    //deal_message(msg);
    //thread_pool.schedule(boost::bind(deal_message,msg));
    //thread_pool.schedule(prio_task_func(5,boost::bind(deal_message,msg)));
    io_ser.post(boost::bind(deal_message,msg));
}

void MessageCenter::push_message_high_prio(std::string msg){
    boost::mutex::scoped_lock  lock(mc_mtx);
    //thread_pool.schedule(boost::bind(deal_message,msg));
    //thread_pool += boost::threadpool::prio_task_func(50,boost::bind(deal_message,msg));
    //thread_pool.schedule(prio_task_func(50,boost::bind(deal_message,msg)));
    //deal_message(msg);
    io_ser.post(boost::bind(deal_message,msg));
}


void MessageCenter::set_handle_func(boost::function<void(std::string)>  const & func){
    this->deal_message = func;
}
