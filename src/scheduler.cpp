#include "scheduler.h"

#include <algorithm>

std::vector<Process> Scheduler::fcfs(std::vector<Process> processes)
{
    // FCFS executes processes in order of arrival.
    std::sort(processes.begin(), processes.end(),
              [](const Process& a, const Process& b) {
                  return a.arrivalTime < b.arrivalTime;
              });

    int currentTime = 0;

    for (Process& p : processes) {

        // CPU may be idle before this process arrives.
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