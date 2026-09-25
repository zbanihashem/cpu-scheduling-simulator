#include "scheduler.h"

#include <algorithm>
#include <limits>
#include <queue>
#include <stdexcept>


namespace {

void addGanttEntry(
    std::vector<GanttEntry>* gantt,
    int pid,
    int startTime,
    int endTime)
{
    if (gantt == nullptr || startTime == endTime) {
        return;
    }

    // Merge consecutive execution segments
    // belonging to the same process.
    if (!gantt->empty() &&
        gantt->back().pid == pid &&
        gantt->back().endTime == startTime) {

        gantt->back().endTime = endTime;
        return;
    }

    gantt->push_back({
        pid,
        startTime,
        endTime
    });
}

}


std::vector<Process> Scheduler::fcfs(
    std::vector<Process> processes,
    std::vector<GanttEntry>* gantt)
{
    std::sort(
        processes.begin(),
        processes.end(),
        [](const Process& a, const Process& b) {
            return a.arrivalTime < b.arrivalTime;
        }
    );

    int currentTime = 0;

    for (Process& p : processes) {

        if (currentTime < p.arrivalTime) {

            addGanttEntry(
                gantt,
                -1,
                currentTime,
                p.arrivalTime
            );

            currentTime = p.arrivalTime;
        }

        p.startTime = currentTime;

        int executionStart = currentTime;

        currentTime += p.burstTime;

        addGanttEntry(
            gantt,
            p.pid,
            executionStart,
            currentTime
        );

        p.completionTime = currentTime;

        p.turnaroundTime =
            p.completionTime - p.arrivalTime;

        p.waitingTime =
            p.turnaroundTime - p.burstTime;

        p.responseTime =
            p.startTime - p.arrivalTime;
    }

    return processes;
}


std::vector<Process> Scheduler::sjf(
    std::vector<Process> processes,
    std::vector<GanttEntry>* gantt)
{
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;

    std::vector<bool> finished(n, false);
    std::vector<Process> result;

    while (completed < n) {

        int selected = -1;

        int shortestBurst =
            std::numeric_limits<int>::max();

        for (int i = 0; i < n; i++) {

            if (!finished[i] &&
                processes[i].arrivalTime <= currentTime) {

                if (processes[i].burstTime <
                    shortestBurst) {

                    shortestBurst =
                        processes[i].burstTime;

                    selected = i;
                }
            }
        }

        if (selected == -1) {

            int nextArrival =
                std::numeric_limits<int>::max();

            for (int i = 0; i < n; i++) {

                if (!finished[i]) {

                    nextArrival =
                        std::min(
                            nextArrival,
                            processes[i].arrivalTime
                        );
                }
            }

            addGanttEntry(
                gantt,
                -1,
                currentTime,
                nextArrival
            );

            currentTime = nextArrival;
            continue;
        }

        Process& p = processes[selected];

        p.startTime = currentTime;

        int executionStart = currentTime;

        currentTime += p.burstTime;

        addGanttEntry(
            gantt,
            p.pid,
            executionStart,
            currentTime
        );

        p.completionTime = currentTime;

        p.turnaroundTime =
            p.completionTime - p.arrivalTime;

        p.waitingTime =
            p.turnaroundTime - p.burstTime;

        p.responseTime =
            p.startTime - p.arrivalTime;

        finished[selected] = true;
        completed++;

        result.push_back(p);
    }

    return result;
}


std::vector<Process> Scheduler::srtf(
    std::vector<Process> processes,
    std::vector<GanttEntry>* gantt)
{
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;

    std::vector<Process> result;

    for (Process& p : processes) {
        p.remainingTime = p.burstTime;
        p.startTime = -1;
    }

    while (completed < n) {

        int selected = -1;

        int shortestRemaining =
            std::numeric_limits<int>::max();

        for (int i = 0; i < n; i++) {

            if (processes[i].arrivalTime <= currentTime &&
                processes[i].remainingTime > 0) {

                if (processes[i].remainingTime <
                    shortestRemaining) {

                    shortestRemaining =
                        processes[i].remainingTime;

                    selected = i;
                }
            }
        }

        if (selected == -1) {

            addGanttEntry(
                gantt,
                -1,
                currentTime,
                currentTime + 1
            );

            currentTime++;
            continue;
        }

        Process& p = processes[selected];

        if (p.startTime == -1) {
            p.startTime = currentTime;
        }

        int executionStart = currentTime;

        p.remainingTime--;
        currentTime++;

        addGanttEntry(
            gantt,
            p.pid,
            executionStart,
            currentTime
        );

        if (p.remainingTime == 0) {

            p.completionTime = currentTime;

            p.turnaroundTime =
                p.completionTime - p.arrivalTime;

            p.waitingTime =
                p.turnaroundTime - p.burstTime;

            p.responseTime =
                p.startTime - p.arrivalTime;

            completed++;

            result.push_back(p);
        }
    }

    return result;
}


std::vector<Process> Scheduler::roundRobin(
    std::vector<Process> processes,
    int quantum,
    std::vector<GanttEntry>* gantt)
{
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;

    std::vector<Process> result;
    std::queue<int> readyQueue;
    std::vector<bool> added(n, false);

    for (Process& p : processes) {
        p.remainingTime = p.burstTime;
        p.startTime = -1;
    }

    while (completed < n) {

        for (int i = 0; i < n; i++) {

            if (!added[i] &&
                processes[i].arrivalTime <= currentTime) {

                readyQueue.push(i);
                added[i] = true;
            }
        }

        if (readyQueue.empty()) {

            addGanttEntry(
                gantt,
                -1,
                currentTime,
                currentTime + 1
            );

            currentTime++;
            continue;
        }

        int index = readyQueue.front();
        readyQueue.pop();

        Process& p = processes[index];

        if (p.startTime == -1) {
            p.startTime = currentTime;
        }

        int executionTime =
            std::min(
                quantum,
                p.remainingTime
            );

        int executionStart = currentTime;

        p.remainingTime -= executionTime;
        currentTime += executionTime;

        addGanttEntry(
            gantt,
            p.pid,
            executionStart,
            currentTime
        );

        for (int i = 0; i < n; i++) {

            if (!added[i] &&
                processes[i].arrivalTime <= currentTime) {

                readyQueue.push(i);
                added[i] = true;
            }
        }

        if (p.remainingTime > 0) {

            readyQueue.push(index);

        } else {

            p.completionTime = currentTime;

            p.turnaroundTime =
                p.completionTime - p.arrivalTime;

            p.waitingTime =
                p.turnaroundTime - p.burstTime;

            p.responseTime =
                p.startTime - p.arrivalTime;

            completed++;

            result.push_back(p);
        }
    }

    return result;
}


std::vector<Process> Scheduler::priorityScheduling(
    std::vector<Process> processes,
    std::vector<GanttEntry>* gantt)
{
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;

    std::vector<bool> finished(n, false);
    std::vector<Process> result;

    while (completed < n) {

        int selected = -1;

        int highestPriority =
            std::numeric_limits<int>::max();

        for (int i = 0; i < n; i++) {

            if (!finished[i] &&
                processes[i].arrivalTime <= currentTime) {

                if (processes[i].priority <
                    highestPriority) {

                    highestPriority =
                        processes[i].priority;

                    selected = i;
                }
            }
        }

        if (selected == -1) {

            int nextArrival =
                std::numeric_limits<int>::max();

            for (int i = 0; i < n; i++) {

                if (!finished[i]) {

                    nextArrival =
                        std::min(
                            nextArrival,
                            processes[i].arrivalTime
                        );
                }
            }

            addGanttEntry(
                gantt,
                -1,
                currentTime,
                nextArrival
            );

            currentTime = nextArrival;
            continue;
        }

        Process& p = processes[selected];

        p.startTime = currentTime;

        int executionStart = currentTime;

        currentTime += p.burstTime;

        addGanttEntry(
            gantt,
            p.pid,
            executionStart,
            currentTime
        );

        p.completionTime = currentTime;

        p.turnaroundTime =
            p.completionTime - p.arrivalTime;

        p.waitingTime =
            p.turnaroundTime - p.burstTime;

        p.responseTime =
            p.startTime - p.arrivalTime;

        finished[selected] = true;
        completed++;

        result.push_back(p);
    }

    return result;
}


std::vector<Process> Scheduler::priorityWithAging(
    std::vector<Process> processes,
    int agingInterval,
    std::vector<GanttEntry>* gantt)
{
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;

    std::vector<bool> finished(n, false);
    std::vector<Process> result;

    while (completed < n) {

        int selected = -1;

        int bestEffectivePriority =
            std::numeric_limits<int>::max();

        for (int i = 0; i < n; i++) {

            if (!finished[i] &&
                processes[i].arrivalTime <= currentTime) {

                int waitingSoFar =
                    currentTime -
                    processes[i].arrivalTime;

                int priorityBoost =
                    waitingSoFar / agingInterval;

                int effectivePriority =
                    std::max(
                        1,
                        processes[i].priority -
                        priorityBoost
                    );

                if (effectivePriority <
                    bestEffectivePriority) {

                    bestEffectivePriority =
                        effectivePriority;

                    selected = i;
                }
            }
        }

        if (selected == -1) {

            int nextArrival =
                std::numeric_limits<int>::max();

            for (int i = 0; i < n; i++) {

                if (!finished[i]) {

                    nextArrival =
                        std::min(
                            nextArrival,
                            processes[i].arrivalTime
                        );
                }
            }

            addGanttEntry(
                gantt,
                -1,
                currentTime,
                nextArrival
            );

            currentTime = nextArrival;
            continue;
        }

        Process& p = processes[selected];

        p.startTime = currentTime;

        int executionStart = currentTime;

        currentTime += p.burstTime;

        addGanttEntry(
            gantt,
            p.pid,
            executionStart,
            currentTime
        );

        p.completionTime = currentTime;

        p.turnaroundTime =
            p.completionTime - p.arrivalTime;

        p.waitingTime =
            p.turnaroundTime - p.burstTime;

        p.responseTime =
            p.startTime - p.arrivalTime;

        finished[selected] = true;
        completed++;

        result.push_back(p);
    }

    return result;
}

std::vector<Process> Scheduler::mlfq(
    std::vector<Process> processes,
    int q0Quantum,
    int q1Quantum,
    std::vector<GanttEntry>* gantt)
{
    if (q0Quantum <= 0 || q1Quantum <= 0) {
        throw std::invalid_argument("MLFQ quantum values must be greater than zero");
    }

    int n = static_cast<int>(processes.size());
    int completed = 0;
    int currentTime = 0;
    std::vector<Process> result;
    std::queue<int> q0, q1, q2;
    std::vector<bool> added(n, false);

    for (Process& p : processes) {
        p.remainingTime = p.burstTime;
        p.startTime = -1;
    }

    auto addArrivals = [&]() {
        for (int i = 0; i < n; ++i) {
            if (!added[i] && processes[i].arrivalTime <= currentTime) {
                q0.push(i);
                added[i] = true;
            }
        }
    };

    auto completeProcess = [&](Process& p) {
        p.completionTime = currentTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        p.responseTime = p.startTime - p.arrivalTime;
        ++completed;
        result.push_back(p);
    };

    while (completed < n) {
        addArrivals();

        if (q0.empty() && q1.empty() && q2.empty()) {
            int nextArrival = std::numeric_limits<int>::max();
            for (int i = 0; i < n; ++i) {
                if (!added[i]) {
                    nextArrival = std::min(nextArrival, processes[i].arrivalTime);
                }
            }

            if (nextArrival == std::numeric_limits<int>::max()) {
                break;
            }

            addGanttEntry(gantt, -1, currentTime, nextArrival);
            currentTime = nextArrival;
            continue;
        }

        // Q0: highest priority, Round Robin.
        if (!q0.empty()) {
            int index = q0.front();
            q0.pop();
            Process& p = processes[index];

            if (p.startTime == -1) {
                p.startTime = currentTime;
            }

            int executionTime = std::min(q0Quantum, p.remainingTime);
            int executionStart = currentTime;
            p.remainingTime -= executionTime;
            currentTime += executionTime;

            addGanttEntry(gantt, p.pid, executionStart, currentTime);
            addArrivals();

            if (p.remainingTime == 0) {
                completeProcess(p);
            } else {
                q1.push(index);
            }
            continue;
        }

        // Q1: Round Robin. A new Q0 arrival preempts it immediately.
        if (!q1.empty()) {
            int index = q1.front();
            q1.pop();
            Process& p = processes[index];

            if (p.startTime == -1) {
                p.startTime = currentTime;
            }

            int usedQuantum = 0;
            bool preemptedByQ0 = false;

            while (p.remainingTime > 0 && usedQuantum < q1Quantum) {
                int executionStart = currentTime;
                --p.remainingTime;
                ++usedQuantum;
                ++currentTime;

                addGanttEntry(gantt, p.pid, executionStart, currentTime);
                addArrivals();

                if (!q0.empty() && p.remainingTime > 0) {
                    preemptedByQ0 = true;
                    break;
                }
            }

            if (p.remainingTime == 0) {
                completeProcess(p);
            } else if (preemptedByQ0) {
                q1.push(index);
            } else {
                q2.push(index);
            }
            continue;
        }

        // Q2: FCFS. A new Q0 arrival preempts it.
        int index = q2.front();
        q2.pop();
        Process& p = processes[index];

        if (p.startTime == -1) {
            p.startTime = currentTime;
        }

        bool preemptedByQ0 = false;

        while (p.remainingTime > 0) {
            int executionStart = currentTime;
            --p.remainingTime;
            ++currentTime;

            addGanttEntry(gantt, p.pid, executionStart, currentTime);
            addArrivals();

            if (!q0.empty() && p.remainingTime > 0) {
                preemptedByQ0 = true;
                break;
            }
        }

        if (p.remainingTime == 0) {
            completeProcess(p);
        } else if (preemptedByQ0) {
            q2.push(index);
        }
    }

    return result;
}
