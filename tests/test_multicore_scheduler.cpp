#include <cassert>
#include <iostream>
#include <stdexcept>
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


    assert(result[0].process.pid == 1);
    assert(result[0].coreId == 1);
    assert(result[0].process.startTime == 0);
    assert(result[0].process.completionTime == 5);

    assert(result[1].process.pid == 2);
    assert(result[1].coreId == 2);
    assert(result[1].process.startTime == 1);
    assert(result[1].process.completionTime == 4);

    assert(result[2].process.pid == 3);
    assert(result[2].coreId == 2);
    assert(result[2].process.startTime == 4);
    assert(result[2].process.completionTime == 12);

    assert(result[3].process.pid == 4);
    assert(result[3].coreId == 1);
    assert(result[3].process.startTime == 5);
    assert(result[3].process.completionTime == 11);

    assert(result[4].process.pid == 5);
    assert(result[4].coreId == 1);
    assert(result[4].process.startTime == 11);
    assert(result[4].process.completionTime == 13);


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


    cout << "[PASS] One-core FCFS compatibility test\n";
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


void testMultiCoreSjfShortestJobsFirst()
{
    vector<Process> processes = {
        {1, 0, 8, 1},
        {2, 0, 2, 1},
        {3, 0, 5, 1}
    };

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::sjf(
            processes,
            2,
            &gantt
        );


    assert(result.size() == 3);


    // At time 0 both cores are free.
    // SJF must select the two shortest ready jobs.

    assert(result[0].process.pid == 2);
    assert(result[0].coreId == 1);
    assert(result[0].process.startTime == 0);
    assert(result[0].process.completionTime == 2);

    assert(result[1].process.pid == 3);
    assert(result[1].coreId == 2);
    assert(result[1].process.startTime == 0);
    assert(result[1].process.completionTime == 5);


    // Core 1 becomes free at time 2.
    // P1 is the only remaining process.

    assert(result[2].process.pid == 1);
    assert(result[2].coreId == 1);
    assert(result[2].process.startTime == 2);
    assert(result[2].process.completionTime == 10);


    cout << "[PASS] Multi-Core SJF shortest jobs test\n";
}


void testSjfDoesNotWaitForFutureShortJob()
{
    vector<Process> processes = {
        {1, 0, 8, 1},
        {2, 3, 1, 1}
    };

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::sjf(
            processes,
            1,
            &gantt
        );


    assert(result.size() == 2);


    // P2 is shorter, but it has not arrived at time 0.
    // Non-preemptive SJF must start P1 immediately.

    assert(result[0].process.pid == 1);
    assert(result[0].process.startTime == 0);
    assert(result[0].process.completionTime == 8);

    assert(result[1].process.pid == 2);
    assert(result[1].process.startTime == 8);
    assert(result[1].process.completionTime == 9);


    cout << "[PASS] SJF future arrival test\n";
}


void testMultiCoreSjfIdleCores()
{
    vector<Process> processes = {
        {1, 2, 4, 1},
        {2, 3, 2, 1}
    };

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::sjf(
            processes,
            2,
            &gantt
        );


    assert(result.size() == 2);

    assert(result[0].process.pid == 1);
    assert(result[0].coreId == 1);
    assert(result[0].process.startTime == 2);
    assert(result[0].process.completionTime == 6);

    assert(result[1].process.pid == 2);
    assert(result[1].coreId == 2);
    assert(result[1].process.startTime == 3);
    assert(result[1].process.completionTime == 5);


    cout << "[PASS] Multi-Core SJF idle core test\n";
}


void testInvalidSjfCoreCount()
{
    vector<Process> processes = {
        {1, 0, 5, 1}
    };

    bool exceptionThrown = false;

    try {

        MultiCoreScheduler::sjf(
            processes,
            0
        );

    } catch (const invalid_argument&) {

        exceptionThrown = true;
    }


    assert(exceptionThrown);


    cout << "[PASS] Invalid SJF core count test\n";
}


int main()
{
    cout << "Running Multi-Core Scheduler Tests\n";
    cout << "==================================\n";


    testMultiCoreFcfs();
    testSingleCoreCompatibility();
    testEqualArrivalOrder();
    testInvalidCoreCount();


    testMultiCoreSjfShortestJobsFirst();
    testSjfDoesNotWaitForFutureShortJob();
    testMultiCoreSjfIdleCores();
    testInvalidSjfCoreCount();


    cout <<
        "\nAll Multi-Core tests passed successfully.\n";

    return 0;
}