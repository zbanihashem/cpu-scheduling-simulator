#include "../src/scheduler.h"

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

namespace {

bool expect(bool condition, const string& name)
{
    if (condition) {
        cout << "[PASS] " << name << "\n";
        return true;
    }

    cout << "[FAIL] " << name << "\n";
    return false;
}

const Process* findProcess(
    const vector<Process>& processes,
    int pid)
{
    for (const Process& p : processes) {
        if (p.pid == pid) {
            return &p;
        }
    }

    return nullptr;
}

bool testBasicDemotion()
{
    vector<Process> processes = {
        {1, 0, 10, 1}
    };

    vector<GanttEntry> gantt;

    vector<Process> result =
        Scheduler::mlfq(processes, 2, 4, &gantt);

    const Process* p1 = findProcess(result, 1);

    return expect(
        p1 != nullptr &&
        p1->startTime == 0 &&
        p1->completionTime == 10 &&
        p1->waitingTime == 0 &&
        p1->turnaroundTime == 10 &&
        p1->responseTime == 0,
        "MLFQ basic demotion test"
    );
}

bool testQueueOrdering()
{
    vector<Process> processes = {
        {1, 0, 8, 1},
        {2, 0, 3, 1}
    };

    vector<GanttEntry> gantt;

    vector<Process> result =
        Scheduler::mlfq(processes, 2, 4, &gantt);

    const Process* p1 = findProcess(result, 1);
    const Process* p2 = findProcess(result, 2);

    return expect(
        p1 != nullptr &&
        p2 != nullptr &&
        p1->startTime == 0 &&
        p2->startTime == 2 &&
        p2->completionTime == 9 &&
        p1->completionTime == 11,
        "MLFQ queue ordering and demotion test"
    );
}

bool testHigherQueuePreemption()
{
    vector<Process> processes = {
        {1, 0, 10, 1},
        {2, 3, 1, 1}
    };

    vector<GanttEntry> gantt;

    vector<Process> result =
        Scheduler::mlfq(processes, 2, 4, &gantt);

    const Process* p1 = findProcess(result, 1);
    const Process* p2 = findProcess(result, 2);

    return expect(
        p1 != nullptr &&
        p2 != nullptr &&
        p2->startTime == 3 &&
        p2->completionTime == 4 &&
        p2->responseTime == 0 &&
        p1->completionTime == 11,
        "MLFQ higher-priority queue preemption test"
    );
}

bool testIdlePeriod()
{
    vector<Process> processes = {
        {1, 3, 2, 1}
    };

    vector<GanttEntry> gantt;

    vector<Process> result =
        Scheduler::mlfq(processes, 2, 4, &gantt);

    const Process* p1 = findProcess(result, 1);

    bool idleCorrect =
        !gantt.empty() &&
        gantt[0].pid == -1 &&
        gantt[0].startTime == 0 &&
        gantt[0].endTime == 3;

    return expect(
        p1 != nullptr &&
        idleCorrect &&
        p1->startTime == 3 &&
        p1->completionTime == 5,
        "MLFQ idle period test"
    );
}

bool testMetrics()
{
    vector<Process> processes = {
        {1, 0, 5, 1},
        {2, 0, 2, 1}
    };

    vector<Process> result =
        Scheduler::mlfq(processes, 2, 4);

    const Process* p1 = findProcess(result, 1);
    const Process* p2 = findProcess(result, 2);

    return expect(
        p1 != nullptr &&
        p2 != nullptr &&
        p1->startTime == 0 &&
        p1->completionTime == 7 &&
        p1->turnaroundTime == 7 &&
        p1->waitingTime == 2 &&
        p1->responseTime == 0 &&
        p2->startTime == 2 &&
        p2->completionTime == 4 &&
        p2->turnaroundTime == 4 &&
        p2->waitingTime == 2 &&
        p2->responseTime == 2,
        "MLFQ metrics test"
    );
}

bool testInvalidQuantum()
{
    vector<Process> processes = {
        {1, 0, 5, 1}
    };

    bool q0Rejected = false;
    bool q1Rejected = false;

    try {
        Scheduler::mlfq(processes, 0, 4);
    } catch (const invalid_argument&) {
        q0Rejected = true;
    }

    try {
        Scheduler::mlfq(processes, 2, 0);
    } catch (const invalid_argument&) {
        q1Rejected = true;
    }

    return expect(
        q0Rejected && q1Rejected,
        "Invalid MLFQ quantum test"
    );
}

}

int main()
{
    cout << "Running MLFQ Scheduler Tests\n";
    cout << "============================\n";

    int passed = 0;
    int total = 6;

    passed += testBasicDemotion();
    passed += testQueueOrdering();
    passed += testHigherQueuePreemption();
    passed += testIdlePeriod();
    passed += testMetrics();
    passed += testInvalidQuantum();

    cout << "\n";

    if (passed == total) {
        cout << "All MLFQ tests passed successfully.\n";
        return 0;
    }

    cout << passed << " of "
         << total
         << " MLFQ tests passed.\n";

    return 1;
}
