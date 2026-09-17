#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>

#include "process.h"
#include "gantt.h"

class Scheduler {
public:
    static std::vector<Process> fcfs(
        std::vector<Process> processes,
        std::vector<GanttEntry>* gantt = nullptr
    );

    static std::vector<Process> sjf(
        std::vector<Process> processes,
        std::vector<GanttEntry>* gantt = nullptr
    );

    static std::vector<Process> srtf(
        std::vector<Process> processes,
        std::vector<GanttEntry>* gantt = nullptr
    );

    static std::vector<Process> roundRobin(
        std::vector<Process> processes,
        int quantum,
        std::vector<GanttEntry>* gantt = nullptr
    );

    static std::vector<Process> priorityScheduling(
        std::vector<Process> processes,
        std::vector<GanttEntry>* gantt = nullptr
    );

    static std::vector<Process> priorityWithAging(
        std::vector<Process> processes,
        int agingInterval,
        std::vector<GanttEntry>* gantt = nullptr
    );
};

#endif