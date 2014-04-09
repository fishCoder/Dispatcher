#include <iostream>
#include <map>

#include "Output.h"

class TaskList;
class TaskGenerator{
public:
    void repMapToTask(std::map<int,int> &reqMap,TaskList &lst);
    bool hasNext(int num);
    void gen_task(int map_type_id,int amount,TaskList &lst);
    void urgent_gen_task(int map_type_id,TaskList &lst);
};


