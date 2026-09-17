#include <cassert>
#include <iostream>
#include <vector>

#include "../src/process.h"
#include "../src/scheduler.h"
#include "../src/gantt.h"

using namespace std;


void testCpuIdle()
{
    vector<Process> processes = {
        {1, 2, 3, 1},
        {2, 8, 2, 2}
    };

    vector<GanttEntry> gantt;

    vector<Process> result =
        Scheduler::fcfs(
            processes,
            &gantt
        );

    assert(result.size() == 2);

    assert(result[0].startTime == 2);
    assert(result[0].completionTime == 5);

    assert(result[1].startTime == 8);
    assert(result[1].completionTime == 10);

    assert(gantt.size() == 4);

    // CPU idle from 0 to 2
    assert(gantt[0].pid == -1);
    assert(gantt[0].startTime == 0);
    assert(gantt[0].endTime == 2);

    // P1 runs from 2 to 5
    assert(gantt[1].pid == 1);
    assert(gantt[1].startTime == 2);
    assert(gantt[1].endTime == 5);

    // CPU idle again from 5 to 8
    assert(gantt[2].pid == -1);
    assert(gantt[2].startTime == 5);
    assert(gantt[2].endTime == 8);

    // P2 runs from 8 to 10
    assert(gantt[3].pid == 2);
    assert(gantt[3].startTime == 8);
    assert(gantt[3].endTime == 10);

    cout << "[PASS] CPU idle test\n";
}


void testSameArrivalTime()
{
    vector<Process> processes = {
        {1, 0, 5, 3},
        {2, 0, 2, 2},
        {3, 0, 1, 1}
    };

    vector<GanttEntry> gantt;

    vector<Process> result =
        Scheduler::sjf(
            processes,
            &gantt
        );

    assert(result.size() == 3);

    // All processes arrive at time 0.
    // SJF must choose the shortest burst first.
    assert(result[0].pid == 3);
    assert(result[1].pid == 2);
    assert(result[2].pid == 1);

    assert(gantt.size() == 3);

    assert(gantt[0].pid == 3);
    assert(gantt[0].startTime == 0);
    assert(gantt[0].endTime == 1);

    assert(gantt[1].pid == 2);
    assert(gantt[1].startTime == 1);
    assert(gantt[1].endTime == 3);

    assert(gantt[2].pid == 1);
    assert(gantt[2].startTime == 3);
    assert(gantt[2].endTime == 8);

    cout << "[PASS] Same arrival time test\n";
}


void testRoundRobinDifferentQuantum()
{
    vector<Process> processes = {
        {1, 0, 5, 1},
        {2, 0, 4, 1}
    };

    const int quantum = 3;

    vector<GanttEntry> gantt;

    vector<Process> result =
        Scheduler::roundRobin(
            processes,
            quantum,
            &gantt
        );

    assert(result.size() == 2);

    assert(gantt.size() == 4);

    // Expected timeline:
    // P1: 0-3
    // P2: 3-6
    // P1: 6-8
    // P2: 8-9

    assert(gantt[0].pid == 1);
    assert(gantt[0].startTime == 0);
    assert(gantt[0].endTime == 3);

    assert(gantt[1].pid == 2);
    assert(gantt[1].startTime == 3);
    assert(gantt[1].endTime == 6);

    assert(gantt[2].pid == 1);
    assert(gantt[2].startTime == 6);
    assert(gantt[2].endTime == 8);

    assert(gantt[3].pid == 2);
    assert(gantt[3].startTime == 8);
    assert(gantt[3].endTime == 9);

    cout << "[PASS] Round Robin quantum test\n";
}


int main()
{
    cout << "Running Scheduler Tests\n";
    cout << "=======================\n";

    testCpuIdle();
    testSameArrivalTime();
    testRoundRobinDifferentQuantum();

    cout << "\nAll tests passed successfully.\n";

    return 0;
}