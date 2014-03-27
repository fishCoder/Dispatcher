#include "MessageCenter.h"
#include "Output.h"

MessageCenter::MessageCenter():thread_pool(param.THREAD_POOL_NUM){

}
void MessageCenter::push_message(std::string msg){
    //deal_message(msg);
    thread_pool.schedule(boost::bind(deal_message,msg));
}
void MessageCenter::set_handle_func(boost::function<void(std::string)>  const & func){
    this->deal_message = func;
}
