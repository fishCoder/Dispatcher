
#ifndef _COMMON_MESSAGE_LIST_
#define _COMMON_MESSAGE_LIST_

#include <list>
#include <iostream>
class MessageCenter{
public:
    MessageCenter();
    void push_front(std::string msg);
    void push_back(std::string msg);
    std::string pop_front();
    bool empty();
private:
    std::list<std::string> msg_center;
};

#endif
