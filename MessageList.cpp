

#include "MessageCenter.h"
#include <boost/thread/mutex.hpp>

boost::mutex msg_mtx;

MessageCenter::MessageCenter():msg_center(){

}
void MessageCenter::push_front(std::string msg){
    boost::mutex::scoped_lock  lock(msg_mtx);
    msg_center.push_front(msg);

}

void MessageCenter::push_back(std::string msg){
    boost::mutex::scoped_lock  lock(msg_mtx);
    msg_center.push_messge(msg);

}
std::string MessageCenter::pop_front(){
    boost::mutex::scoped_lock  lock(msg_mtx);
    std::string msg = msg_center.front();
    msg_center.pop_front();
    return msg;
}
bool MessageCenter::empty(){
    return msg_center.empty();
}
