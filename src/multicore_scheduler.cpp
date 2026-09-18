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


int findNextEventTime(
    const std::vector<Process>& processes,
    const std::vector<bool>& scheduled,
    const std::vector<int>& coreAvailableTime,
    int currentTime)
{
    int nextTime =
        std::numeric_limits<int>::max();


    for (int availableTime : coreAvailableTime) {

        if (availableTime > currentTime) {

            nextTime =
                std::min(
                    nextTime,
                    availableTime
                );
        }
    }


    for (std::size_t i = 0;
         i < processes.size();
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


    return nextTime;
}


void calculateMetrics(
    Process& process,
    int startTime)
{
    process.startTime =
        startTime;

    process.completionTime =
        process.startTime +
        process.burstTime;

    process.turnaroundTime =
        process.completionTime -
        process.arrivalTime;

    process.waitingTime =
        process.turnaroundTime -
        process.burstTime;

    process.responseTime =
        process.startTime -
        process.arrivalTime;
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
            return a.arrivalTime <
                   b.arrivalTime;
        }
    );


    std::vector<int> coreAvailableTime(
        coreCount,
        0
    );

    std::vector<MultiCoreProcessResult> result;


    for (Process& process : processes) {

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
                process.arrivalTime,
                coreAvailableTime[selectedCore]
            );


        if (coreAvailableTime[selectedCore] <
            process.arrivalTime) {

            addCoreGanttEntry(
                gantt,
                selectedCore + 1,
                -1,
                coreAvailableTime[selectedCore],
                process.arrivalTime
            );
        }


        calculateMetrics(
            process,
            startTime
        );


        addCoreGanttEntry(
            gantt,
            selectedCore + 1,
            process.pid,
            process.startTime,
            process.completionTime
        );


        coreAvailableTime[selectedCore] =
            process.completionTime;


        result.push_back({
            process,
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

                if (scheduled[i] ||
                    processes[i].arrivalTime >
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


            Process& process =
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


            calculateMetrics(
                process,
                currentTime
            );


            addCoreGanttEntry(
                gantt,
                core + 1,
                process.pid,
                process.startTime,
                process.completionTime
            );


            coreAvailableTime[core] =
                process.completionTime;

            scheduled[selected] = true;
            scheduledCount++;


            result.push_back({
                process,
                core + 1
            });
        }


        if (scheduledCount ==
            processCount) {

            break;
        }


        int nextTime =
            findNextEventTime(
                processes,
                scheduled,
                coreAvailableTime,
                currentTime
            );


        if (nextTime ==
            std::numeric_limits<int>::max()) {

            break;
        }


        currentTime = nextTime;
    }


    return result;
}


std::vector<MultiCoreProcessResult>
MultiCoreScheduler::priorityScheduling(
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

        for (int core = 0;
             core < coreCount;
             core++) {

            if (coreAvailableTime[core] >
                currentTime) {

                continue;
            }


            int selected = -1;
            int bestPriority =
                std::numeric_limits<int>::max();


            for (int i = 0;
                 i < processCount;
                 i++) {

                if (scheduled[i] ||
                    processes[i].arrivalTime >
                        currentTime) {

                    continue;
                }


                if (processes[i].priority <
                    bestPriority) {

                    bestPriority =
                        processes[i].priority;

                    selected = i;
                }
            }


            if (selected == -1) {
                continue;
            }


            Process& process =
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


            calculateMetrics(
                process,
                currentTime
            );


            addCoreGanttEntry(
                gantt,
                core + 1,
                process.pid,
                process.startTime,
                process.completionTime
            );


            coreAvailableTime[core] =
                process.completionTime;

            scheduled[selected] = true;
            scheduledCount++;


            result.push_back({
                process,
                core + 1
            });
        }


        if (scheduledCount ==
            processCount) {

            break;
        }


        int nextTime =
            findNextEventTime(
                processes,
                scheduled,
                coreAvailableTime,
                currentTime
            );


        if (nextTime ==
            std::numeric_limits<int>::max()) {

            break;
        }


        currentTime = nextTime;
    }


    return result;
}


std::vector<MultiCoreProcessResult>
MultiCoreScheduler::priorityWithAging(
    std::vector<Process> processes,
    int coreCount,
    int agingInterval,
    std::vector<CoreGanttEntry>* gantt)
{
    if (coreCount <= 0) {
        throw std::invalid_argument(
            "Core count must be greater than zero."
        );
    }


    if (agingInterval <= 0) {
        throw std::invalid_argument(
            "Aging interval must be greater than zero."
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

        for (int core = 0;
             core < coreCount;
             core++) {

            if (coreAvailableTime[core] >
                currentTime) {

                continue;
            }


            int selected = -1;

            int bestEffectivePriority =
                std::numeric_limits<int>::max();


            for (int i = 0;
                 i < processCount;
                 i++) {

                if (scheduled[i] ||
                    processes[i].arrivalTime >
                        currentTime) {

                    continue;
                }


                int waitingSoFar =
                    currentTime -
                    processes[i].arrivalTime;


                int priorityImprovement =
                    waitingSoFar /
                    agingInterval;


                int effectivePriority =
                    std::max(
                        1,
                        processes[i].priority -
                        priorityImprovement
                    );


                if (effectivePriority <
                    bestEffectivePriority) {

                    bestEffectivePriority =
                        effectivePriority;

                    selected = i;
                }
            }


            if (selected == -1) {
                continue;
            }


            Process& process =
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


            calculateMetrics(
                process,
                currentTime
            );


            addCoreGanttEntry(
                gantt,
                core + 1,
                process.pid,
                process.startTime,
                process.completionTime
            );


            coreAvailableTime[core] =
                process.completionTime;

            scheduled[selected] = true;
            scheduledCount++;


            result.push_back({
                process,
                core + 1
            });
        }


        if (scheduledCount ==
            processCount) {

            break;
        }


        int nextTime =
            findNextEventTime(
                processes,
                scheduled,
                coreAvailableTime,
                currentTime
            );


        if (nextTime ==
            std::numeric_limits<int>::max()) {

            break;
        }


        currentTime = nextTime;
    }


    return result;
}