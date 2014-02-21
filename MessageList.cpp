

#include "MessageList.h"
#include <boost/thread/mutex.hpp>

boost::mutex msg_mtx;

MessageList::MessageList():msgLst(){

}
void MessageList::push_front(std::string msg){
    boost::mutex::scoped_lock  lock(msg_mtx);
    msgLst.push_front(msg);

}

void MessageList::push_back(std::string msg){
    boost::mutex::scoped_lock  lock(msg_mtx);
    msgLst.push_back(msg);

}
std::string MessageList::pop_front(){
    boost::mutex::scoped_lock  lock(msg_mtx);
    std::string msg = msgLst.front();
    msgLst.pop_front();
    return msg;
}
bool MessageList::empty(){
    return msgLst.empty();
}
