#include <iostream>

#include <map>

class TaskList;
class TaskGenerator{
public:
    void repMapToTask(std::map<int,int> &reqMap,TaskList &lst);
    bool hasNext(int num);

    void urgent_gen_task(int map_type_id,TaskList &lst);
};


