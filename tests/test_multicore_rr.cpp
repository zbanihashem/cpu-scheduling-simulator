#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../src/process.h"
#include "../src/multicore_scheduler.h"

using namespace std;


const MultiCoreProcessResult& findProcess(
    const vector<MultiCoreProcessResult>& result,
    int pid)
{
    for (const MultiCoreProcessResult& item : result) {

        if (item.process.pid == pid) {
            return item;
        }
    }

    throw runtime_error("Process not found.");
}


void testBasicMultiCoreRoundRobin()
{
    vector<Process> processes = {
        {1, 0, 5, 1},
        {2, 0, 4, 1},
        {3, 1, 2, 1}
    };

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::roundRobin(
            processes,
            2,
            2,
            &gantt
        );


    assert(result.size() == 3);


    const Process& p1 =
        findProcess(result, 1).process;

    const Process& p2 =
        findProcess(result, 2).process;

    const Process& p3 =
        findProcess(result, 3).process;


    assert(p1.startTime == 0);
    assert(p2.startTime == 0);


    /*
     * Expected scheduling:
     *
     * Quantum = 2
     *
     * Core 1:
     * P1  0-2
     * P3  2-4
     * P2  4-6
     *
     * Core 2:
     * P2  0-2
     * P1  2-4
     * P1  4-5
     *
     * P3 arrives at t=1 and therefore waits
     * in the Ready Queue before P1 and P2
     * are requeued at t=2.
     */


    assert(p3.startTime == 2);


    assert(p1.completionTime == 5);
    assert(p2.completionTime == 6);
    assert(p3.completionTime == 4);


    /*
     * Metrics:
     *
     * P1:
     * TAT = 5 - 0 = 5
     * WT  = 5 - 5 = 0
     * RT  = 0 - 0 = 0
     *
     * P2:
     * TAT = 6 - 0 = 6
     * WT  = 6 - 4 = 2
     * RT  = 0 - 0 = 0
     *
     * P3:
     * TAT = 4 - 1 = 3
     * WT  = 3 - 2 = 1
     * RT  = 2 - 1 = 1
     */


    assert(p1.turnaroundTime == 5);
    assert(p1.waitingTime == 0);
    assert(p1.responseTime == 0);


    assert(p2.turnaroundTime == 6);
    assert(p2.waitingTime == 2);
    assert(p2.responseTime == 0);


    assert(p3.turnaroundTime == 3);
    assert(p3.waitingTime == 1);
    assert(p3.responseTime == 1);


    cout <<
        "[PASS] Basic Multi-Core Round Robin test\n";
}


void testRoundRobinReadyQueueOrder()
{
    vector<Process> processes = {
        {1, 0, 5, 1},
        {2, 0, 5, 1},
        {3, 1, 1, 1}
    };


    vector<CoreGanttEntry> gantt;


    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::roundRobin(
            processes,
            2,
            2,
            &gantt
        );


    const Process& p3 =
        findProcess(result, 3).process;


    /*
     * P3 arrives at t=1.
     *
     * At t=2 both P1 and P2 reach the end
     * of their first quantum.
     *
     * The Ready Queue must therefore be:
     *
     * P3 -> P1 -> P2
     *
     * P3 must immediately receive a core.
     */


    assert(p3.startTime == 2);
    assert(p3.completionTime == 3);


    bool p3StartsAtTwo = false;


    for (const CoreGanttEntry& entry : gantt) {

        if (entry.pid == 3 &&
            entry.startTime == 2 &&
            entry.endTime == 3) {

            p3StartsAtTwo = true;
        }
    }


    assert(p3StartsAtTwo);


    cout <<
        "[PASS] Round Robin Ready Queue order test\n";
}


void testRoundRobinQuantumOne()
{
    vector<Process> processes = {
        {1, 0, 3, 1},
        {2, 0, 3, 1},
        {3, 0, 2, 1}
    };


    vector<CoreGanttEntry> gantt;


    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::roundRobin(
            processes,
            2,
            1,
            &gantt
        );


    assert(result.size() == 3);


    const Process& p1 =
        findProcess(result, 1).process;

    const Process& p2 =
        findProcess(result, 2).process;

    const Process& p3 =
        findProcess(result, 3).process;


    /*
     * Initial Ready Queue:
     *
     * P1 -> P2 -> P3
     *
     * t=0:
     * Core 1 -> P1
     * Core 2 -> P2
     *
     * At t=1:
     *
     * P3 is already waiting.
     * P1 and P2 return to the end.
     *
     * Queue:
     *
     * P3 -> P1 -> P2
     */


    assert(p1.startTime == 0);
    assert(p2.startTime == 0);
    assert(p3.startTime == 1);


    assert(p1.completionTime > 0);
    assert(p2.completionTime > 0);
    assert(p3.completionTime > 0);


    cout <<
        "[PASS] Round Robin quantum = 1 test\n";
}


void testRoundRobinMetrics()
{
    vector<Process> processes = {
        {1, 0, 5, 1},
        {2, 0, 4, 1},
        {3, 1, 2, 1}
    };


    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::roundRobin(
            processes,
            2,
            2
        );


    for (const MultiCoreProcessResult& item :
         result) {

        const Process& p =
            item.process;


        assert(
            p.turnaroundTime ==
            p.completionTime -
            p.arrivalTime
        );


        assert(
            p.waitingTime ==
            p.turnaroundTime -
            p.burstTime
        );


        assert(
            p.responseTime ==
            p.startTime -
            p.arrivalTime
        );


        assert(p.waitingTime >= 0);
        assert(p.responseTime >= 0);
    }


    cout <<
        "[PASS] Multi-Core Round Robin metrics test\n";
}


void testRoundRobinIdlePeriod()
{
    vector<Process> processes = {
        {1, 3, 3, 1}
    };


    vector<CoreGanttEntry> gantt;


    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::roundRobin(
            processes,
            2,
            2,
            &gantt
        );


    const Process& p1 =
        findProcess(result, 1).process;


    assert(p1.startTime == 3);
    assert(p1.completionTime == 6);

    assert(p1.waitingTime == 0);
    assert(p1.turnaroundTime == 3);
    assert(p1.responseTime == 0);


    /*
     * Verify that initial idle time is
     * represented in the Gantt chart.
     */

    bool core1Idle = false;
    bool core2Idle = false;


    for (const CoreGanttEntry& entry : gantt) {

        if (entry.pid != -1 ||
            entry.startTime != 0 ||
            entry.endTime != 3) {

            continue;
        }


        if (entry.coreId == 1) {
            core1Idle = true;
        }


        if (entry.coreId == 2) {
            core2Idle = true;
        }
    }


    assert(core1Idle);
    assert(core2Idle);


    cout <<
        "[PASS] Multi-Core Round Robin idle period test\n";
}


void testInvalidRoundRobinQuantum()
{
    vector<Process> processes = {
        {1, 0, 5, 1}
    };


    bool exceptionThrown = false;


    try {

        MultiCoreScheduler::roundRobin(
            processes,
            2,
            0
        );
    }
    catch (const invalid_argument&) {

        exceptionThrown = true;
    }


    assert(exceptionThrown);


    cout <<
        "[PASS] Invalid Round Robin quantum test\n";
}


void testInvalidRoundRobinCoreCount()
{
    vector<Process> processes = {
        {1, 0, 5, 1}
    };


    bool exceptionThrown = false;


    try {

        MultiCoreScheduler::roundRobin(
            processes,
            0,
            2
        );
    }
    catch (const invalid_argument&) {

        exceptionThrown = true;
    }


    assert(exceptionThrown);


    cout <<
        "[PASS] Invalid Round Robin core count test\n";
}


int main()
{
    cout <<
        "Running Multi-Core Round Robin Tests\n";

    cout <<
        "====================================\n";


    testBasicMultiCoreRoundRobin();

    testRoundRobinReadyQueueOrder();

    testRoundRobinQuantumOne();

    testRoundRobinMetrics();

    testRoundRobinIdlePeriod();

    testInvalidRoundRobinQuantum();

    testInvalidRoundRobinCoreCount();


    cout <<
        "\nAll Multi-Core Round Robin tests "
        "passed successfully.\n";


    return 0;
}