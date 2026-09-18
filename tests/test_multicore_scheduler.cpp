#include <cassert>
#include <iostream>
#include <vector>

#include "../src/process.h"
#include "../src/multicore_scheduler.h"

using namespace std;


void testMultiCoreFcfs()
{
    vector<Process> processes = {
        {1, 0, 5, 2},
        {2, 1, 3, 1},
        {3, 2, 8, 4},
        {4, 3, 6, 2},
        {5, 4, 2, 3}
    };

    const int coreCount = 2;

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::fcfs(
            processes,
            coreCount,
            &gantt
        );


    assert(result.size() == 5);


    // P1 -> Core 1
    assert(result[0].process.pid == 1);
    assert(result[0].coreId == 1);
    assert(result[0].process.startTime == 0);
    assert(result[0].process.completionTime == 5);
    assert(result[0].process.waitingTime == 0);
    assert(result[0].process.turnaroundTime == 5);
    assert(result[0].process.responseTime == 0);


    // P2 -> Core 2
    assert(result[1].process.pid == 2);
    assert(result[1].coreId == 2);
    assert(result[1].process.startTime == 1);
    assert(result[1].process.completionTime == 4);
    assert(result[1].process.waitingTime == 0);
    assert(result[1].process.turnaroundTime == 3);
    assert(result[1].process.responseTime == 0);


    // P3 -> Core 2
    assert(result[2].process.pid == 3);
    assert(result[2].coreId == 2);
    assert(result[2].process.startTime == 4);
    assert(result[2].process.completionTime == 12);
    assert(result[2].process.waitingTime == 2);
    assert(result[2].process.turnaroundTime == 10);
    assert(result[2].process.responseTime == 2);


    // P4 -> Core 1
    assert(result[3].process.pid == 4);
    assert(result[3].coreId == 1);
    assert(result[3].process.startTime == 5);
    assert(result[3].process.completionTime == 11);
    assert(result[3].process.waitingTime == 2);
    assert(result[3].process.turnaroundTime == 8);
    assert(result[3].process.responseTime == 2);


    // P5 -> Core 1
    assert(result[4].process.pid == 5);
    assert(result[4].coreId == 1);
    assert(result[4].process.startTime == 11);
    assert(result[4].process.completionTime == 13);
    assert(result[4].process.waitingTime == 7);
    assert(result[4].process.turnaroundTime == 9);
    assert(result[4].process.responseTime == 7);


    cout << "[PASS] Multi-Core FCFS scheduling test\n";
}


void testSingleCoreCompatibility()
{
    vector<Process> processes = {
        {1, 0, 5, 2},
        {2, 1, 3, 1},
        {3, 2, 8, 4}
    };

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::fcfs(
            processes,
            1,
            &gantt
        );


    assert(result.size() == 3);

    assert(result[0].coreId == 1);
    assert(result[0].process.startTime == 0);
    assert(result[0].process.completionTime == 5);

    assert(result[1].coreId == 1);
    assert(result[1].process.startTime == 5);
    assert(result[1].process.completionTime == 8);

    assert(result[2].coreId == 1);
    assert(result[2].process.startTime == 8);
    assert(result[2].process.completionTime == 16);


    cout << "[PASS] One-core compatibility test\n";
}


void testEqualArrivalOrder()
{
    vector<Process> processes = {
        {1, 0, 5, 1},
        {2, 0, 3, 1},
        {3, 0, 2, 1}
    };

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::fcfs(
            processes,
            2
        );


    assert(result.size() == 3);

    assert(result[0].process.pid == 1);
    assert(result[1].process.pid == 2);
    assert(result[2].process.pid == 3);


    cout << "[PASS] Equal arrival FCFS order test\n";
}


void testInvalidCoreCount()
{
    vector<Process> processes = {
        {1, 0, 5, 1}
    };

    bool exceptionThrown = false;

    try {

        MultiCoreScheduler::fcfs(
            processes,
            0
        );

    } catch (const invalid_argument&) {

        exceptionThrown = true;
    }


    assert(exceptionThrown);


    cout << "[PASS] Invalid core count test\n";
}


int main()
{
    cout << "Running Multi-Core Scheduler Tests\n";
    cout << "==================================\n";

    testMultiCoreFcfs();
    testSingleCoreCompatibility();
    testEqualArrivalOrder();
    testInvalidCoreCount();

    cout << "\nAll Multi-Core tests passed successfully.\n";

    return 0;
}