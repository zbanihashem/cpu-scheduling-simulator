#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "process.h"
#include "scheduler.h"
#include "gantt.h"

using namespace std;


struct AverageMetrics {
    double waitingTime;
    double turnaroundTime;
    double responseTime;
};


void printInput(
    const vector<Process>& processes)
{
    cout << "\nPID\tArrival\tBurst\tPriority\n";

    for (const Process& p : processes) {
        cout << "P" << p.pid << "\t"
             << p.arrivalTime << "\t"
             << p.burstTime << "\t"
             << p.priority << "\n";
    }
}


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


void printGantt(
    const string& title,
    const vector<GanttEntry>& gantt)
{
    cout << "\n" << title << " Gantt Chart\n";

    for (const GanttEntry& entry : gantt) {
        cout << "[" << entry.startTime
             << "-";

        if (entry.pid == -1) {
            cout << "IDLE";
        } else {
            cout << "P" << entry.pid;
        }

        cout << "-"
             << entry.endTime
             << "] ";
    }

    cout << "\n";
}


AverageMetrics calculateAverageMetrics(
    const vector<Process>& processes)
{
    double totalWaiting = 0.0;
    double totalTurnaround = 0.0;
    double totalResponse = 0.0;

    for (const Process& p : processes) {
        totalWaiting += p.waitingTime;
        totalTurnaround += p.turnaroundTime;
        totalResponse += p.responseTime;
    }

    int count = processes.size();

    return {
        totalWaiting / count,
        totalTurnaround / count,
        totalResponse / count
    };
}


void printComparisonRow(
    const string& algorithm,
    const vector<Process>& result)
{
    AverageMetrics metrics =
        calculateAverageMetrics(result);

    cout << left
         << setw(22) << algorithm
         << right
         << setw(12)
         << fixed
         << setprecision(2)
         << metrics.waitingTime
         << setw(14)
         << metrics.turnaroundTime
         << setw(12)
         << metrics.responseTime
         << "\n";
}


void printComparison(
    const vector<Process>& fcfsResult,
    const vector<Process>& sjfResult,
    const vector<Process>& srtfResult,
    const vector<Process>& rrResult,
    const vector<Process>& priorityResult)
{
    cout << "\n\nAlgorithm Comparison\n";

    cout <<
        "============================================================\n";

    cout << left
         << setw(22) << "Algorithm"
         << right
         << setw(12) << "Avg WT"
         << setw(14) << "Avg TAT"
         << setw(12) << "Avg RT"
         << "\n";

    cout <<
        "------------------------------------------------------------\n";

    printComparisonRow(
        "FCFS",
        fcfsResult
    );

    printComparisonRow(
        "SJF",
        sjfResult
    );

    printComparisonRow(
        "SRTF",
        srtfResult
    );

    printComparisonRow(
        "Round Robin",
        rrResult
    );

    printComparisonRow(
        "Priority",
        priorityResult
    );
}


int readInteger(
    const string& prompt,
    int minimum)
{
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value &&
            value >= minimum) {
            return value;
        }

        cout << "Invalid input. Enter an integer >= "
             << minimum
             << ".\n";

        cin.clear();

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );
    }
}


vector<Process> readProcesses()
{
    int count =
        readInteger(
            "Number of processes: ",
            1
        );

    vector<Process> processes;

    cout << "\nPriority rule: "
         << "smaller number = higher priority.\n";

    for (int i = 1; i <= count; i++) {
        cout << "\nProcess P" << i << "\n";

        int arrivalTime =
            readInteger(
                "Arrival time: ",
                0
            );

        int burstTime =
            readInteger(
                "Burst time: ",
                1
            );

        int priority =
            readInteger(
                "Priority: ",
                1
            );

        processes.push_back({
            i,
            arrivalTime,
            burstTime,
            priority
        });
    }

    return processes;
}


void runSchedulers(
    const vector<Process>& processes,
    int quantum)
{
    vector<GanttEntry> fcfsGantt;
    vector<GanttEntry> sjfGantt;
    vector<GanttEntry> srtfGantt;
    vector<GanttEntry> rrGantt;
    vector<GanttEntry> priorityGantt;


    vector<Process> fcfsResult =
        Scheduler::fcfs(
            processes,
            &fcfsGantt
        );

    vector<Process> sjfResult =
        Scheduler::sjf(
            processes,
            &sjfGantt
        );

    vector<Process> srtfResult =
        Scheduler::srtf(
            processes,
            &srtfGantt
        );

    vector<Process> rrResult =
        Scheduler::roundRobin(
            processes,
            quantum,
            &rrGantt
        );

    vector<Process> priorityResult =
        Scheduler::priorityScheduling(
            processes,
            &priorityGantt
        );


    printResult(
        "FCFS Result",
        fcfsResult
    );

    printGantt(
        "FCFS",
        fcfsGantt
    );


    printResult(
        "SJF (Non-preemptive) Result",
        sjfResult
    );

    printGantt(
        "SJF",
        sjfGantt
    );


    printResult(
        "SRTF (Preemptive SJF) Result",
        srtfResult
    );

    printGantt(
        "SRTF",
        srtfGantt
    );


    string rrTitle =
        "Round Robin Result (Quantum = " +
        to_string(quantum) +
        ")";

    printResult(
        rrTitle,
        rrResult
    );

    printGantt(
        "Round Robin",
        rrGantt
    );


    printResult(
        "Priority Scheduling Result",
        priorityResult
    );

    printGantt(
        "Priority",
        priorityGantt
    );


    printComparison(
        fcfsResult,
        sjfResult,
        srtfResult,
        rrResult,
        priorityResult
    );
}


void runAgingDemo()
{
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


    vector<GanttEntry> withoutAgingGantt;
    vector<GanttEntry> agingGantt;


    vector<Process> withoutAging =
        Scheduler::priorityScheduling(
            agingTest,
            &withoutAgingGantt
        );

    vector<Process> withAging =
        Scheduler::priorityWithAging(
            agingTest,
            agingInterval,
            &agingGantt
        );


    printResult(
        "Priority Scheduling WITHOUT Aging",
        withoutAging
    );

    printGantt(
        "Priority without Aging",
        withoutAgingGantt
    );


    printResult(
        "Priority Scheduling WITH Aging (Interval = 3)",
        withAging
    );

    printGantt(
        "Priority with Aging",
        agingGantt
    );
}


void runDefaultDemo()
{
    vector<Process> processes = {
        {1, 0, 5, 2},
        {2, 1, 3, 1},
        {3, 2, 8, 4},
        {4, 3, 6, 2},
        {5, 4, 2, 3}
    };

    const int quantum = 2;

    cout << "\nDefault Demonstration\n";
    cout << "=====================\n";

    printInput(processes);

    runSchedulers(
        processes,
        quantum
    );

    runAgingDemo();
}


void runCustomSimulation()
{
    cout << "\nCustom Simulation\n";
    cout << "=================\n\n";

    vector<Process> processes =
        readProcesses();

    int quantum =
        readInteger(
            "\nRound Robin quantum: ",
            1
        );

    cout << "\nCustom Dataset\n";
    cout << "--------------\n";

    printInput(processes);

    runSchedulers(
        processes,
        quantum
    );
}


void printMenu()
{
    cout << "\nCPU Scheduling Simulator\n";
    cout << "========================\n";

    cout << "1. Run default demonstration\n";
    cout << "2. Enter custom processes\n";
    cout << "0. Exit\n";
}


int main()
{
    while (true) {
        printMenu();

        int choice =
            readInteger(
                "\nChoice: ",
                0
            );

        switch (choice) {
            case 1:
                runDefaultDemo();
                break;

            case 2:
                runCustomSimulation();
                break;

            case 0:
                cout << "\nGoodbye.\n";
                return 0;

            default:
                cout << "Invalid choice. "
                     << "Please select 0, 1, or 2.\n";
        }
    }
}