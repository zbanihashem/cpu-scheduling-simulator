#include "scheduler.h"

#include <algorithm>
#include <limits>
#include <queue>


std::vector<Process> Scheduler::fcfs(std::vector<Process> processes)
{
    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.arrivalTime < b.arrivalTime;
              });

    int currentTime = 0;

    for (Process& p : processes) {

        if (currentTime < p.arrivalTime) {
            currentTime = p.arrivalTime;
        }

        p.startTime = currentTime;
        currentTime += p.burstTime;
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


std::vector<Process> Scheduler::sjf(std::vector<Process> processes)
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

                if (processes[i].burstTime < shortestBurst) {

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

            currentTime = nextArrival;
            continue;
        }

        Process& p = processes[selected];

        p.startTime = currentTime;
        currentTime += p.burstTime;
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


std::vector<Process> Scheduler::srtf(std::vector<Process> processes)
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
            currentTime++;
            continue;
        }

        Process& p = processes[selected];

        if (p.startTime == -1) {
            p.startTime = currentTime;
        }

        p.remainingTime--;
        currentTime++;

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
    int quantum)
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
            std::min(quantum, p.remainingTime);

        p.remainingTime -= executionTime;
        currentTime += executionTime;

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
    std::vector<Process> processes)
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

        // No process is ready: CPU is idle.
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

            currentTime = nextArrival;
            continue;
        }

        Process& p = processes[selected];

        p.startTime = currentTime;

        currentTime += p.burstTime;

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