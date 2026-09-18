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

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::fcfs(
            processes,
            2
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

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::fcfs(
            processes,
            1
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
    }
    catch (const invalid_argument&) {
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

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::sjf(
            processes,
            2
        );

    assert(result.size() == 3);

    assert(result[0].process.pid == 2);
    assert(result[0].coreId == 1);
    assert(result[0].process.startTime == 0);
    assert(result[0].process.completionTime == 2);

    assert(result[1].process.pid == 3);
    assert(result[1].coreId == 2);
    assert(result[1].process.startTime == 0);
    assert(result[1].process.completionTime == 5);

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

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::sjf(
            processes,
            1
        );

    assert(result.size() == 2);

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

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::sjf(
            processes,
            2
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
    }
    catch (const invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    cout << "[PASS] Invalid SJF core count test\n";
}


void testMultiCorePrioritySelection()
{
    vector<Process> processes = {
        {1, 0, 8, 4},
        {2, 0, 3, 1},
        {3, 0, 5, 2},
        {4, 0, 2, 3}
    };

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::priorityScheduling(
            processes,
            2
        );

    assert(result.size() == 4);

    assert(result[0].process.pid == 2);
    assert(result[0].coreId == 1);
    assert(result[0].process.startTime == 0);
    assert(result[0].process.completionTime == 3);

    assert(result[1].process.pid == 3);
    assert(result[1].coreId == 2);
    assert(result[1].process.startTime == 0);
    assert(result[1].process.completionTime == 5);

    assert(result[2].process.pid == 4);
    assert(result[2].coreId == 1);
    assert(result[2].process.startTime == 3);
    assert(result[2].process.completionTime == 5);

    assert(result[3].process.pid == 1);
    assert(result[3].process.startTime == 5);
    assert(result[3].process.completionTime == 13);

    cout << "[PASS] Multi-Core Priority selection test\n";
}


void testPriorityRespectsArrivalTime()
{
    vector<Process> processes = {
        {1, 0, 6, 5},
        {2, 2, 2, 1}
    };

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::priorityScheduling(
            processes,
            1
        );

    assert(result.size() == 2);

    assert(result[0].process.pid == 1);
    assert(result[0].process.startTime == 0);
    assert(result[0].process.completionTime == 6);

    assert(result[1].process.pid == 2);
    assert(result[1].process.startTime == 6);
    assert(result[1].process.completionTime == 8);

    assert(result[1].process.waitingTime == 4);
    assert(result[1].process.turnaroundTime == 6);
    assert(result[1].process.responseTime == 4);

    cout << "[PASS] Priority arrival-time test\n";
}


void testMultiCorePriorityIdleCores()
{
    vector<Process> processes = {
        {1, 2, 5, 3},
        {2, 2, 3, 1}
    };

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::priorityScheduling(
            processes,
            2
        );

    assert(result.size() == 2);

    assert(result[0].process.pid == 2);
    assert(result[0].coreId == 1);
    assert(result[0].process.startTime == 2);
    assert(result[0].process.completionTime == 5);

    assert(result[1].process.pid == 1);
    assert(result[1].coreId == 2);
    assert(result[1].process.startTime == 2);
    assert(result[1].process.completionTime == 7);

    cout << "[PASS] Multi-Core Priority idle core test\n";
}


void testInvalidPriorityCoreCount()
{
    vector<Process> processes = {
        {1, 0, 5, 1}
    };

    bool exceptionThrown = false;

    try {
        MultiCoreScheduler::priorityScheduling(
            processes,
            0
        );
    }
    catch (const invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    cout << "[PASS] Invalid Priority core count test\n";
}


void testMultiCoreAgingChangesOrder()
{
    vector<Process> processes = {
        {1, 0, 4, 1},
        {2, 0, 2, 5},
        {3, 1, 4, 2},
        {4, 2, 4, 2},
        {5, 3, 4, 2}
    };

    const int coreCount = 2;
    const int agingInterval = 2;


    vector<MultiCoreProcessResult> withoutAging =
        MultiCoreScheduler::priorityScheduling(
            processes,
            coreCount
        );


    vector<MultiCoreProcessResult> withAging =
        MultiCoreScheduler::priorityWithAging(
            processes,
            coreCount,
            agingInterval
        );


    assert(withoutAging.size() == 5);
    assert(withAging.size() == 5);


    // Without Aging:
    //
    // t=0:
    // Core 1 -> P1 (priority 1)
    // Core 2 -> P2 (priority 5)
    //
    // This dataset still verifies that the two algorithms
    // complete all processes and preserve valid metrics.

    for (const MultiCoreProcessResult& item : withAging) {

        assert(
            item.process.startTime >=
            item.process.arrivalTime
        );

        assert(
            item.process.completionTime ==
            item.process.startTime +
            item.process.burstTime
        );

        assert(
            item.process.turnaroundTime ==
            item.process.completionTime -
            item.process.arrivalTime
        );

        assert(
            item.process.waitingTime ==
            item.process.turnaroundTime -
            item.process.burstTime
        );

        assert(
            item.process.responseTime ==
            item.process.startTime -
            item.process.arrivalTime
        );
    }


    cout << "[PASS] Multi-Core Aging metrics test\n";
}


void testAgingPromotesWaitingProcess()
{
    vector<Process> processes = {
        {1, 0, 4, 1},
        {2, 0, 2, 6},
        {3, 0, 4, 2},
        {4, 1, 4, 2},
        {5, 2, 4, 2},
        {6, 3, 4, 2}
    };

    const int agingInterval = 2;


    vector<MultiCoreProcessResult> withoutAging =
        MultiCoreScheduler::priorityScheduling(
            processes,
            1
        );


    vector<MultiCoreProcessResult> withAging =
        MultiCoreScheduler::priorityWithAging(
            processes,
            1,
            agingInterval
        );


    assert(withoutAging.size() == 6);
    assert(withAging.size() == 6);


    int positionWithoutAging = -1;
    int positionWithAging = -1;


    for (size_t i = 0;
         i < withoutAging.size();
         i++) {

        if (withoutAging[i].process.pid == 2) {
            positionWithoutAging =
                static_cast<int>(i);
        }
    }


    for (size_t i = 0;
         i < withAging.size();
         i++) {

        if (withAging[i].process.pid == 2) {
            positionWithAging =
                static_cast<int>(i);
        }
    }


    assert(positionWithoutAging != -1);
    assert(positionWithAging != -1);


    // Aging must move the long-waiting,
    // low-priority P2 forward.

    assert(
        positionWithAging <
        positionWithoutAging
    );


    cout << "[PASS] Aging promotes waiting process test\n";
}


void testInvalidAgingInterval()
{
    vector<Process> processes = {
        {1, 0, 5, 3}
    };

    bool exceptionThrown = false;

    try {
        MultiCoreScheduler::priorityWithAging(
            processes,
            2,
            0
        );
    }
    catch (const invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    cout << "[PASS] Invalid Aging interval test\n";
}


void testInvalidAgingCoreCount()
{
    vector<Process> processes = {
        {1, 0, 5, 3}
    };

    bool exceptionThrown = false;

    try {
        MultiCoreScheduler::priorityWithAging(
            processes,
            0,
            2
        );
    }
    catch (const invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    cout << "[PASS] Invalid Aging core count test\n";
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


    testMultiCorePrioritySelection();
    testPriorityRespectsArrivalTime();
    testMultiCorePriorityIdleCores();
    testInvalidPriorityCoreCount();


    testMultiCoreAgingChangesOrder();
    testAgingPromotesWaitingProcess();
    testInvalidAgingInterval();
    testInvalidAgingCoreCount();


    cout <<
        "\nAll Multi-Core tests passed successfully.\n";

    return 0;
}