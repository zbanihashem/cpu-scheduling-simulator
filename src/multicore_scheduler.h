#ifndef MULTICORE_SCHEDULER_H
#define MULTICORE_SCHEDULER_H

#include <vector>

#include "process.h"


struct CoreGanttEntry {
    int coreId;
    int pid;
    int startTime;
    int endTime;
};


struct MultiCoreProcessResult {
    Process process;
    int coreId;
};


class MultiCoreScheduler {
public:
    static std::vector<MultiCoreProcessResult> fcfs(
        std::vector<Process> processes,
        int coreCount,
        std::vector<CoreGanttEntry>* gantt = nullptr
    );
};


#endif