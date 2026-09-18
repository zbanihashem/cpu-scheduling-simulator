#include "multicore_scheduler.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace std;


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


    /*
     * If the new interval continues the same process
     * on the same core, merge it with the previous
     * Gantt entry.
     */
    if (!gantt->empty()) {

        for (auto it = gantt->rbegin();
             it != gantt->rend();
             ++it) {

            if (it->coreId != coreId) {
                continue;
            }


            if (it->pid == pid &&
                it->endTime == startTime) {

                it->endTime = endTime;
                return;
            }


            break;
        }
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


std::vector<MultiCoreProcessResult>
MultiCoreScheduler::srtf(
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


    if (processCount == 0) {
        return {};
    }


    /*
     * SRTF is preemptive, therefore a process may
     * execute on more than one core during its lifetime.
     *
     * coreId in the final result represents the core
     * on which the process completed.
     */


    for (Process& process : processes) {

        process.remainingTime =
            process.burstTime;

        process.startTime = -1;
        process.completionTime = 0;
        process.waitingTime = 0;
        process.turnaroundTime = 0;
        process.responseTime = 0;
    }


    vector<int> coreProcess(
        coreCount,
        -1
    );


    vector<int> completionCore(
        processCount,
        -1
    );


    int completedCount = 0;
    int currentTime = 0;


    while (completedCount < processCount) {

        vector<int> ready;


        for (int i = 0;
             i < processCount;
             i++) {

            if (processes[i].arrivalTime <=
                    currentTime &&
                processes[i].remainingTime > 0) {

                ready.push_back(i);
            }
        }


        /*
         * Sort all ready processes by remaining time.
         *
         * Tie breaking:
         * 1. Shorter remaining time
         * 2. Earlier arrival time
         * 3. Smaller PID
         */
        stable_sort(
            ready.begin(),
            ready.end(),
            [&processes](int a, int b) {

                if (processes[a].remainingTime !=
                    processes[b].remainingTime) {

                    return
                        processes[a].remainingTime <
                        processes[b].remainingTime;
                }


                if (processes[a].arrivalTime !=
                    processes[b].arrivalTime) {

                    return
                        processes[a].arrivalTime <
                        processes[b].arrivalTime;
                }


                return
                    processes[a].pid <
                    processes[b].pid;
            }
        );


        vector<int> selected;


        int numberToRun =
            std::min(
                coreCount,
                static_cast<int>(
                    ready.size()
                )
            );


        for (int i = 0;
             i < numberToRun;
             i++) {

            selected.push_back(
                ready[i]
            );
        }


        /*
         * Keep a selected process on its current core
         * whenever possible.
         *
         * This avoids unnecessary migration when the
         * same process remains selected at the next tick.
         */
        vector<int> newCoreProcess(
            coreCount,
            -1
        );


        vector<bool> assigned(
            processCount,
            false
        );


        for (int core = 0;
             core < coreCount;
             core++) {

            int oldProcess =
                coreProcess[core];


            if (oldProcess == -1) {
                continue;
            }


            bool stillSelected = false;


            for (int index : selected) {

                if (index == oldProcess) {
                    stillSelected = true;
                    break;
                }
            }


            if (stillSelected) {

                newCoreProcess[core] =
                    oldProcess;

                assigned[oldProcess] =
                    true;
            }
        }


        /*
         * Assign newly selected processes to any
         * remaining free cores.
         */
        for (int index : selected) {

            if (assigned[index]) {
                continue;
            }


            for (int core = 0;
                 core < coreCount;
                 core++) {

                if (newCoreProcess[core] == -1) {

                    newCoreProcess[core] =
                        index;

                    assigned[index] =
                        true;

                    break;
                }
            }
        }


        coreProcess =
            newCoreProcess;


        /*
         * Execute exactly one time unit on each core.
         */
        for (int core = 0;
             core < coreCount;
             core++) {

            int index =
                coreProcess[core];


            if (index == -1) {

                addCoreGanttEntry(
                    gantt,
                    core + 1,
                    -1,
                    currentTime,
                    currentTime + 1
                );

                continue;
            }


            Process& process =
                processes[index];


            if (process.startTime == -1) {

                process.startTime =
                    currentTime;
            }


            addCoreGanttEntry(
                gantt,
                core + 1,
                process.pid,
                currentTime,
                currentTime + 1
            );


            process.remainingTime--;


            if (process.remainingTime == 0) {

                process.completionTime =
                    currentTime + 1;

                process.turnaroundTime =
                    process.completionTime -
                    process.arrivalTime;

                process.waitingTime =
                    process.turnaroundTime -
                    process.burstTime;

                process.responseTime =
                    process.startTime -
                    process.arrivalTime;

                completionCore[index] =
                    core + 1;

                completedCount++;
            }
        }


        currentTime++;
    }


    vector<MultiCoreProcessResult> result;


    /*
     * Return results in PID/input order rather than
     * completion order. This makes the metrics table
     * easier to read.
     */
    for (int i = 0;
         i < processCount;
         i++) {

        result.push_back({
            processes[i],
            completionCore[i]
        });
    }


    return result;
}