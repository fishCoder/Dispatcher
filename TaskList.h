
#ifndef _COMMON_TASK_LIST_
#define _COMMON_TASK_LIST_

#include <list>
#include <iostream>


class TaskList{
public:
    TaskList();
    void push_front(std::string task);
    void push_back(std::string task);
    std::string pop_front();
    bool empty();
    int size();
private:
    std::list<std::string> taskLst;
};

#endif
