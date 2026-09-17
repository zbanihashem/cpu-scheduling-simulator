#include "scheduler.h"

#include <algorithm>
#include <limits>


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
        int shortestBurst = std::numeric_limits<int>::max();

        for (int i = 0; i < n; i++) {

            if (!finished[i] &&
                processes[i].arrivalTime <= currentTime) {

                if (processes[i].burstTime < shortestBurst) {
                    shortestBurst = processes[i].burstTime;
                    selected = i;
                }
            }
        }

        if (selected == -1) {

            int nextArrival = std::numeric_limits<int>::max();

            for (int i = 0; i < n; i++) {
                if (!finished[i]) {
                    nextArrival =
                        std::min(nextArrival,
                                 processes[i].arrivalTime);
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

    // Initialize runtime fields.
    for (Process& p : processes) {
        p.remainingTime = p.burstTime;
        p.startTime = -1;
    }

    while (completed < n) {

        int selected = -1;
        int shortestRemaining =
            std::numeric_limits<int>::max();

        // Select the arrived process with the
        // shortest remaining CPU time.
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

        // No process is currently ready.
        if (selected == -1) {
            currentTime++;
            continue;
        }

        Process& p = processes[selected];

        // Record start time only on first CPU execution.
        if (p.startTime == -1) {
            p.startTime = currentTime;
        }

        // Execute for one time unit.
        p.remainingTime--;
        currentTime++;

        // Process has finished.
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