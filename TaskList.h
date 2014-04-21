
#ifndef _COMMON_TASK_LIST_
#define _COMMON_TASK_LIST_

#include <list>
#include <iostream>
#include <boost/thread/mutex.hpp>


#include "GeneratorClass.h"

class TaskList{
public:
    TaskList(Generator &generator);
    void set_generator();
    void push_front(std::string task);
    void push_back(std::string task);
    std::string pop_front();
    bool empty();
    int size();
private:
    boost::mutex task_mtx;
    std::list<std::string> taskLst;
    Generator &generator;
};

#endif
