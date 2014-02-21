#ifndef _COMMON_TASK_MAKDER_
#define _COMMON_TASK_MAKDER_

#include <list>

class TaskMaker{
public:
    void push_task(int type,int num);
protected:
private:
    list<std::string> &taskLst;
};
#endif
