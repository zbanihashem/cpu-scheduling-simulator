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

std::vector<MultiCoreProcessResult>
MultiCoreScheduler::roundRobin(
    std::vector<Process> processes,
    int coreCount,
    int quantum,
    std::vector<CoreGanttEntry>* gantt)
{
    if (coreCount <= 0) {
        throw std::invalid_argument(
            "Core count must be greater than zero."
        );
    }


    if (quantum <= 0) {
        throw std::invalid_argument(
            "Quantum must be greater than zero."
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
     * Round Robin is preemptive.
     *
     * A process may execute on different cores during
     * different time slices.
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


    /*
     * Sort process indices by arrival time.
     *
     * We keep the original processes vector unchanged
     * so the final result can still be returned in
     * original input order.
     */
    vector<int> arrivalOrder(
        processCount
    );


    for (int i = 0;
         i < processCount;
         i++) {

        arrivalOrder[i] = i;
    }


    stable_sort(
        arrivalOrder.begin(),
        arrivalOrder.end(),
        [&processes](int a, int b) {

            if (processes[a].arrivalTime !=
                processes[b].arrivalTime) {

                return
                    processes[a].arrivalTime <
                    processes[b].arrivalTime;
            }


            return a < b;
        }
    );


    /*
     * Shared Ready Queue.
     */
    vector<int> readyQueue;


    /*
     * Each core stores the process currently running.
     * -1 means the core is idle.
     */
    vector<int> coreProcess(
        coreCount,
        -1
    );


    /*
     * Time at which the current slice on each core ends.
     */
    vector<int> coreSliceEnd(
        coreCount,
        0
    );


    /*
     * Start time of the current slice.
     */
    vector<int> coreSliceStart(
        coreCount,
        0
    );


    /*
     * Amount of CPU time assigned to the current slice.
     */
    vector<int> coreSliceLength(
        coreCount,
        0
    );


    /*
     * Core on which each process finally completes.
     */
    vector<int> completionCore(
        processCount,
        -1
    );


    int nextArrival = 0;
    int completedCount = 0;
    int currentTime = 0;


    /*
     * Helper lambda:
     * add every process that has arrived by time.
     */
    auto addArrivals =
        [&](int time) {

            while (
                nextArrival < processCount &&
                processes[
                    arrivalOrder[nextArrival]
                ].arrivalTime <= time) {

                readyQueue.push_back(
                    arrivalOrder[nextArrival]
                );

                nextArrival++;
            }
        };


    /*
     * Start from the first arrival rather than
     * simulating unnecessary empty time.
     */
    if (nextArrival < processCount) {

        currentTime =
            processes[
                arrivalOrder[nextArrival]
            ].arrivalTime;
    }


    /*
     * Record initial idle time if the first process
     * arrives after time zero.
     */
    if (currentTime > 0) {

        for (int core = 0;
             core < coreCount;
             core++) {

            addCoreGanttEntry(
                gantt,
                core + 1,
                -1,
                0,
                currentTime
            );
        }
    }


    addArrivals(
        currentTime
    );


    while (completedCount < processCount) {

        /*
         * Assign ready processes to every free core.
         */
        for (int core = 0;
             core < coreCount;
             core++) {

            if (coreProcess[core] != -1) {
                continue;
            }


            if (readyQueue.empty()) {
                continue;
            }


            int processIndex =
                readyQueue.front();


            readyQueue.erase(
                readyQueue.begin()
            );


            /*
             * If this core has been idle since its previous
             * Gantt entry, record that gap before assigning
             * the next process.
             */
            if (gantt != nullptr) {

                int lastEndTime = 0;

                for (auto it = gantt->rbegin();
                     it != gantt->rend();
                     ++it) {

                    if (it->coreId == core + 1) {
                        lastEndTime = it->endTime;
                        break;
                    }
                }

                if (lastEndTime < currentTime) {

                    addCoreGanttEntry(
                        gantt,
                        core + 1,
                        -1,
                        lastEndTime,
                        currentTime
                    );
                }
            }


            Process& process =
                processes[processIndex];


            int sliceLength =
                std::min(
                    quantum,
                    process.remainingTime
                );


            coreProcess[core] =
                processIndex;


            coreSliceStart[core] =
                currentTime;


            coreSliceLength[core] =
                sliceLength;


            coreSliceEnd[core] =
                currentTime +
                sliceLength;


            if (process.startTime == -1) {

                process.startTime =
                    currentTime;
            }
        }


        /*
         * Find the next event.
         *
         * An event can be:
         * 1. a process arrival
         * 2. a quantum ending
         * 3. a process completing
         */
        int nextEventTime =
            std::numeric_limits<int>::max();


        if (nextArrival < processCount) {

            nextEventTime =
                processes[
                    arrivalOrder[nextArrival]
                ].arrivalTime;
        }


        for (int core = 0;
             core < coreCount;
             core++) {

            if (coreProcess[core] != -1) {

                nextEventTime =
                    std::min(
                        nextEventTime,
                        coreSliceEnd[core]
                    );
            }
        }


        if (nextEventTime ==
            std::numeric_limits<int>::max()) {

            break;
        }


        /*
         * If all cores are idle and the next event is
         * a future arrival, record the idle interval.
         */
        bool anyCoreBusy = false;


        for (int core = 0;
             core < coreCount;
             core++) {

            if (coreProcess[core] != -1) {

                anyCoreBusy = true;
                break;
            }
        }


        if (!anyCoreBusy &&
            nextEventTime > currentTime) {

            for (int core = 0;
                 core < coreCount;
                 core++) {

                addCoreGanttEntry(
                    gantt,
                    core + 1,
                    -1,
                    currentTime,
                    nextEventTime
                );
            }
        }


        currentTime =
            nextEventTime;


        /*
         * First handle all slices that end exactly
         * at this timestamp.
         *
         * Their executed CPU time is deducted here.
         */
        vector<int> expiredProcesses;


        for (int core = 0;
             core < coreCount;
             core++) {

            if (coreProcess[core] == -1) {
                continue;
            }


            if (coreSliceEnd[core] !=
                currentTime) {

                continue;
            }


            int processIndex =
                coreProcess[core];


            Process& process =
                processes[processIndex];


            addCoreGanttEntry(
                gantt,
                core + 1,
                process.pid,
                coreSliceStart[core],
                coreSliceEnd[core]
            );


            process.remainingTime -=
                coreSliceLength[core];


            /*
             * The core becomes free at this timestamp.
             */
            coreProcess[core] = -1;


            if (process.remainingTime == 0) {

                process.completionTime =
                    currentTime;


                process.turnaroundTime =
                    process.completionTime -
                    process.arrivalTime;


                process.waitingTime =
                    process.turnaroundTime -
                    process.burstTime;


                process.responseTime =
                    process.startTime -
                    process.arrivalTime;


                completionCore[processIndex] =
                    core + 1;


                completedCount++;
            }
            else {

                /*
                 * Do not immediately requeue it.
                 *
                 * We first insert processes that have
                 * arrived by this timestamp.
                 */
                expiredProcesses.push_back(
                    processIndex
                );
            }
        }


        /*
         * New arrivals at this timestamp enter the
         * Ready Queue before expired processes are
         * placed back at the end.
         *
         * This prevents a process whose quantum just
         * expired from jumping ahead of a process that
         * was already waiting to enter the queue.
         */
        addArrivals(
            currentTime
        );


        /*
         * Requeue unfinished processes whose quantum
         * ended at this timestamp.
         */
        for (int processIndex :
             expiredProcesses) {

            readyQueue.push_back(
                processIndex
            );
        }
    }


    /*
     * Build final result in original input order.
     */
    vector<MultiCoreProcessResult> result;


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