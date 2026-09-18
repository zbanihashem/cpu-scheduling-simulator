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


    static std::vector<MultiCoreProcessResult> sjf(
        std::vector<Process> processes,
        int coreCount,
        std::vector<CoreGanttEntry>* gantt = nullptr
    );


    static std::vector<MultiCoreProcessResult> priorityScheduling(
        std::vector<Process> processes,
        int coreCount,
        std::vector<CoreGanttEntry>* gantt = nullptr
    );


    static std::vector<MultiCoreProcessResult> priorityWithAging(
        std::vector<Process> processes,
        int coreCount,
        int agingInterval,
        std::vector<CoreGanttEntry>* gantt = nullptr
    );


    static std::vector<MultiCoreProcessResult> srtf(
        std::vector<Process> processes,
        int coreCount,
        std::vector<CoreGanttEntry>* gantt = nullptr
    );
};


#endif