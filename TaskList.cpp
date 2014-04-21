#include "TaskList.h"


TaskList::TaskList(Generator &generator):taskLst(),generator(generator){

}

void TaskList::push_front(std::string task){
    boost::mutex::scoped_lock  lock(task_mtx);
    if(generator.has_free_generator()){
        int _h_socket = generator.find_free_generator();
        generator.send_task(_h_socket,task);
        generator.set_generator_busy(_h_socket);
        return;
    }
    taskLst.push_front(task);
}
void TaskList::push_back(std::string task){
    boost::mutex::scoped_lock  lock(task_mtx);
    if(generator.has_free_generator()){
        int _h_socket = generator.find_free_generator();
        generator.send_task(_h_socket,task);
        generator.set_generator_busy(_h_socket);
        return;
    }
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
