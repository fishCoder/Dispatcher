#include <iostream>

#include <map>

class TaskList;
class TaskGenerator{
public:
    void repMapToTask(std::map<int,int> &reqMap,TaskList &lst);
    bool hasNext(int num);
};


