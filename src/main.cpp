#include <iostream>
#include <vector>

#include "process.h"
#include "scheduler.h"

using namespace std;


void printResult(
    const string& title,
    const vector<Process>& result)
{
    cout << "\n" << title << "\n";

    cout <<
        "-------------------------------------------------------------\n";

    cout <<
        "PID\tAT\tBT\tST\tCT\tWT\tTAT\tRT\n";

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


void printInput(const vector<Process>& processes)
{
    cout << "PID\tArrival\tBurst\tPriority\n";

    for (const Process& p : processes) {

        cout << "P" << p.pid << "\t"
             << p.arrivalTime << "\t"
             << p.burstTime << "\t"
             << p.priority << "\n";
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

    const int quantum = 2;

    cout << "CPU Scheduling Simulator\n";
    cout << "========================\n\n";

    cout << "Main Test Dataset\n";
    cout << "---------------------------------\n";

    printInput(processes);

    vector<Process> fcfsResult =
        Scheduler::fcfs(processes);

    vector<Process> sjfResult =
        Scheduler::sjf(processes);

    vector<Process> srtfResult =
        Scheduler::srtf(processes);

    vector<Process> rrResult =
        Scheduler::roundRobin(
            processes,
            quantum
        );

    vector<Process> priorityResult =
        Scheduler::priorityScheduling(
            processes
        );

    printResult(
        "FCFS Result",
        fcfsResult
    );

    printResult(
        "SJF (Non-preemptive) Result",
        sjfResult
    );

    printResult(
        "SRTF (Preemptive SJF) Result",
        srtfResult
    );

    printResult(
        "Round Robin Result (Quantum = 2)",
        rrResult
    );

    printResult(
        "Priority Scheduling Result",
        priorityResult
    );


    // Separate dataset designed to demonstrate Aging.
    vector<Process> agingTest = {
        {1, 0, 4, 1},
        {2, 0, 3, 6},
        {3, 1, 4, 1},
        {4, 2, 4, 2},
        {5, 3, 4, 2},
        {6, 4, 4, 3}
    };

    const int agingInterval = 3;

    cout << "\n\nAging Demonstration Dataset\n";
    cout << "=================================\n";

    printInput(agingTest);

    vector<Process> withoutAging =
        Scheduler::priorityScheduling(
            agingTest
        );

    vector<Process> withAging =
        Scheduler::priorityWithAging(
            agingTest,
            agingInterval
        );

    printResult(
        "Priority Scheduling WITHOUT Aging",
        withoutAging
    );

    printResult(
        "Priority Scheduling WITH Aging (Interval = 3)",
        withAging
    );

    return 0;
}