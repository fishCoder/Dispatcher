#include "TaskList.h"
#include <boost/thread/mutex.hpp>

boost::mutex task_mtx;

TaskList::TaskList():taskLst(){

}

void TaskList::push_front(std::string task){
    boost::mutex::scoped_lock  lock(task_mtx);
    taskLst.push_front(task);
}
void TaskList::push_back(std::string task){
    boost::mutex::scoped_lock  lock(task_mtx);
    taskLst.push_back(task);
}
std::string TaskList::pop_front(){
    boost::mutex::scoped_lock  lock(task_mtx);
    std::string task = taskLst.front();
    taskLst.pop_front();

    return task;
}
bool TaskList::empty(){
    return taskLst.empty();
}
int TaskList::size(){
    return taskLst.size();
}
