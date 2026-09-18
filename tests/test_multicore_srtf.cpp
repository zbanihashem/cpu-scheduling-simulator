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


void testBasicMultiCoreSrtf()
{
    vector<Process> processes = {
        {1, 0, 5, 1},
        {2, 0, 3, 1},
        {3, 0, 1, 1}
    };

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::srtf(
            processes,
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


    assert(p3.startTime == 0);
    assert(p3.completionTime == 1);

    assert(p2.startTime == 0);
    assert(p2.completionTime == 3);

    assert(p1.startTime == 1);
    assert(p1.completionTime == 6);

    cout << "[PASS] Basic Multi-Core SRTF test\n";
}


void testSrtfPreemption()
{
    vector<Process> processes = {
        {1, 0, 10, 1},
        {2, 0, 8, 1},
        {3, 2, 1, 1}
    };

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::srtf(
            processes,
            2,
            &gantt
        );


    const Process& p1 =
        findProcess(result, 1).process;

    const Process& p2 =
        findProcess(result, 2).process;

    const Process& p3 =
        findProcess(result, 3).process;


    /*
     * At t=2:
     *
     * P1 remaining = 8
     * P2 remaining = 6
     * P3 remaining = 1
     *
     * Therefore P3 must immediately receive a CPU.
     */

    assert(p3.startTime == 2);
    assert(p3.completionTime == 3);

    assert(p2.completionTime == 8);
    assert(p1.completionTime == 11);


    /*
     * P3 should appear in the Gantt chart
     * exactly at the preemption point.
     */

    bool foundP3AtTime2 = false;

    for (const CoreGanttEntry& entry : gantt) {

        if (entry.pid == 3 &&
            entry.startTime == 2 &&
            entry.endTime == 3) {

            foundP3AtTime2 = true;
        }
    }

    assert(foundP3AtTime2);

    cout << "[PASS] Multi-Core SRTF preemption test\n";
}


void testSrtfMetrics()
{
    vector<Process> processes = {
        {1, 0, 10, 1},
        {2, 0, 8, 1},
        {3, 2, 1, 1}
    };

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::srtf(
            processes,
            2
        );


    for (const MultiCoreProcessResult& item : result) {

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

    cout << "[PASS] Multi-Core SRTF metrics test\n";
}


void testSrtfIdlePeriod()
{
    vector<Process> processes = {
        {1, 3, 2, 1}
    };

    vector<CoreGanttEntry> gantt;

    vector<MultiCoreProcessResult> result =
        MultiCoreScheduler::srtf(
            processes,
            2,
            &gantt
        );


    const Process& p1 =
        findProcess(result, 1).process;

    assert(p1.startTime == 3);
    assert(p1.completionTime == 5);
    assert(p1.waitingTime == 0);
    assert(p1.responseTime == 0);

    cout << "[PASS] Multi-Core SRTF idle period test\n";
}


void testInvalidSrtfCoreCount()
{
    vector<Process> processes = {
        {1, 0, 5, 1}
    };

    bool exceptionThrown = false;

    try {
        MultiCoreScheduler::srtf(
            processes,
            0
        );
    }
    catch (const invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    cout << "[PASS] Invalid SRTF core count test\n";
}


int main()
{
    cout << "Running Multi-Core SRTF Tests\n";
    cout << "=============================\n";


    testBasicMultiCoreSrtf();
    testSrtfPreemption();
    testSrtfMetrics();
    testSrtfIdlePeriod();
    testInvalidSrtfCoreCount();


    cout <<
        "\nAll Multi-Core SRTF tests passed successfully.\n";

    return 0;
}