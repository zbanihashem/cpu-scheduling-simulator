#include "multicore_scheduler.h"

#include <algorithm>
#include <stdexcept>


std::vector<MultiCoreProcessResult>
MultiCoreScheduler::fcfs(
    std::vector<Process> processes,
    int coreCount,
    std::vector<CoreGanttEntry>* gantt)
{
    if (coreCount <= 0) {
        throw std::invalid_argument(
            "Core count must be greater than zero."
        );
    }

    std::stable_sort(
        processes.begin(),
        processes.end(),
        [](const Process& a, const Process& b) {
            return a.arrivalTime < b.arrivalTime;
        }
    );


    std::vector<int> coreAvailableTime(
        coreCount,
        0
    );

    std::vector<MultiCoreProcessResult> result;


    for (Process& p : processes) {

        int selectedCore = 0;

        for (int core = 1;
             core < coreCount;
             core++) {

            if (coreAvailableTime[core] <
                coreAvailableTime[selectedCore]) {

                selectedCore = core;
            }
        }


        int startTime =
            std::max(
                p.arrivalTime,
                coreAvailableTime[selectedCore]
            );


        if (gantt != nullptr &&
            coreAvailableTime[selectedCore] <
                p.arrivalTime) {

            gantt->push_back({
                selectedCore + 1,
                -1,
                coreAvailableTime[selectedCore],
                p.arrivalTime
            });
        }


        p.startTime = startTime;

        p.completionTime =
            p.startTime +
            p.burstTime;

        p.turnaroundTime =
            p.completionTime -
            p.arrivalTime;

        p.waitingTime =
            p.turnaroundTime -
            p.burstTime;

        p.responseTime =
            p.startTime -
            p.arrivalTime;


        if (gantt != nullptr) {

            gantt->push_back({
                selectedCore + 1,
                p.pid,
                p.startTime,
                p.completionTime
            });
        }


        coreAvailableTime[selectedCore] =
            p.completionTime;


        result.push_back({
            p,
            selectedCore + 1
        });
    }


    return result;
}