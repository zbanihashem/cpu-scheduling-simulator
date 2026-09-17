#include <iostream>
#include <vector>

#include "process.h"
#include "scheduler.h"

using namespace std;


void printResult(const string& title,
                 const vector<Process>& result)
{
    cout << "\n" << title << "\n";
    cout << "-------------------------------------------------------------\n";
    cout << "PID\tAT\tBT\tST\tCT\tWT\tTAT\tRT\n";

    for (const Process& p : result) {
        cout << "P" << p.pid << "\t"
             << p.arrivalTime << "\t"
             << p.burstTime << "\t"
             << p.startTime << "\t"
             << p.completionTime << "\t"
             << p.waitingTime << "\t"
             << p.turnaroundTime << "\t"
             << p.responseTime << "\n";
    }
}


int main()
{
    vector<Process> processes = {
        {1, 0, 5, 2},
        {2, 1, 3, 1},
        {3, 2, 8, 4},
        {4, 3, 6, 2},
        {5, 4, 2, 3}
    };

    cout << "CPU Scheduling Simulator\n";
    cout << "========================\n\n";

    cout << "Input Processes\n";
    cout << "---------------------------------\n";
    cout << "PID\tArrival\tBurst\tPriority\n";

    for (const Process& p : processes) {
        cout << "P" << p.pid << "\t"
             << p.arrivalTime << "\t"
             << p.burstTime << "\t"
             << p.priority << "\n";
    }

    vector<Process> fcfsResult =
        Scheduler::fcfs(processes);

    vector<Process> sjfResult =
        Scheduler::sjf(processes);

    vector<Process> srtfResult =
        Scheduler::srtf(processes);

    printResult("FCFS Result", fcfsResult);

    printResult(
        "SJF (Non-preemptive) Result",
        sjfResult
    );

    printResult(
        "SRTF (Preemptive SJF) Result",
        srtfResult
    );

    return 0;
}