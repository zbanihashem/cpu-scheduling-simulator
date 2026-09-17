#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "process.h"

class Scheduler {
public:
    static std::vector<Process> fcfs(std::vector<Process> processes);
};

#endif