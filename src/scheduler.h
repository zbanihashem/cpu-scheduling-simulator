#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "process.h"

class Scheduler {
public:
    static std::vector<Process> fcfs(std::vector<Process> processes);

    static std::vector<Process> sjf(std::vector<Process> processes);

    static std::vector<Process> srtf(std::vector<Process> processes);

    static std::vector<Process> roundRobin(
        std::vector<Process> processes,
        int quantum
    );

    static std::vector<Process> priorityScheduling(
        std::vector<Process> processes
    );

    static std::vector<Process> priorityWithAging(
        std::vector<Process> processes,
        int agingInterval
    );
};

#endif