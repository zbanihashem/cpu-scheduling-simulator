#include "multicore_scheduler.h"

#include <algorithm>
#include <limits>
#include <stdexcept>


namespace {

void addCoreGanttEntry(
    std::vector<CoreGanttEntry>* gantt,
    int coreId,
    int pid,
    int startTime,
    int endTime)
{
    if (gantt == nullptr ||
        startTime == endTime) {
        return;
    }

    gantt->push_back({
        coreId,
        pid,
        startTime,
        endTime
    });
}

}


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


        if (coreAvailableTime[selectedCore] <
            p.arrivalTime) {

            addCoreGanttEntry(
                gantt,
                selectedCore + 1,
                -1,
                coreAvailableTime[selectedCore],
                p.arrivalTime
            );
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


        addCoreGanttEntry(
            gantt,
            selectedCore + 1,
            p.pid,
            p.startTime,
            p.completionTime
        );


        coreAvailableTime[selectedCore] =
            p.completionTime;


        result.push_back({
            p,
            selectedCore + 1
        });
    }


    return result;
}


std::vector<MultiCoreProcessResult>
MultiCoreScheduler::sjf(
    std::vector<Process> processes,
    int coreCount,
    std::vector<CoreGanttEntry>* gantt)
{
    if (coreCount <= 0) {
        throw std::invalid_argument(
            "Core count must be greater than zero."
        );
    }


    const int processCount =
        static_cast<int>(
            processes.size()
        );


    std::vector<bool> scheduled(
        processCount,
        false
    );

    std::vector<int> coreAvailableTime(
        coreCount,
        0
    );

    std::vector<MultiCoreProcessResult> result;

    int scheduledCount = 0;
    int currentTime = 0;


    while (scheduledCount < processCount) {

        bool scheduledSomething = false;


        for (int core = 0;
             core < coreCount;
             core++) {

            if (coreAvailableTime[core] >
                currentTime) {

                continue;
            }


            int selected = -1;
            int shortestBurst =
                std::numeric_limits<int>::max();


            for (int i = 0;
                 i < processCount;
                 i++) {

                if (scheduled[i]) {
                    continue;
                }

                if (processes[i].arrivalTime >
                    currentTime) {

                    continue;
                }


                if (processes[i].burstTime <
                    shortestBurst) {

                    shortestBurst =
                        processes[i].burstTime;

                    selected = i;
                }
            }


            if (selected == -1) {
                continue;
            }


            Process& p =
                processes[selected];


            if (coreAvailableTime[core] <
                currentTime) {

                addCoreGanttEntry(
                    gantt,
                    core + 1,
                    -1,
                    coreAvailableTime[core],
                    currentTime
                );
            }


            p.startTime = currentTime;

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


            addCoreGanttEntry(
                gantt,
                core + 1,
                p.pid,
                p.startTime,
                p.completionTime
            );


            coreAvailableTime[core] =
                p.completionTime;

            scheduled[selected] = true;
            scheduledCount++;

            result.push_back({
                p,
                core + 1
            });

            scheduledSomething = true;
        }


        if (scheduledCount ==
            processCount) {

            break;
        }


        int nextTime =
            std::numeric_limits<int>::max();


        for (int core = 0;
             core < coreCount;
             core++) {

            if (coreAvailableTime[core] >
                currentTime) {

                nextTime =
                    std::min(
                        nextTime,
                        coreAvailableTime[core]
                    );
            }
        }


        for (int i = 0;
             i < processCount;
             i++) {

            if (!scheduled[i] &&
                processes[i].arrivalTime >
                    currentTime) {

                nextTime =
                    std::min(
                        nextTime,
                        processes[i].arrivalTime
                    );
            }
        }


        if (nextTime ==
            std::numeric_limits<int>::max()) {

            if (!scheduledSomething) {
                break;
            }
        } else {

            currentTime = nextTime;
        }
    }


    return result;
}