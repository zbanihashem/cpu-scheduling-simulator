# CPU Scheduling Simulator

A C++17 CPU scheduling simulator developed as an Operating Systems laboratory project.

The program implements several common CPU scheduling algorithms, calculates scheduling metrics, generates terminal-based Gantt charts, and compares the algorithms on the same workload.

The simulator can run a built-in demonstration workload or accept a custom workload interactively.

## Features

The simulator supports:

- First Come First Served (FCFS)
- Shortest Job First (SJF) - Non-preemptive
- Shortest Remaining Time First (SRTF) - Preemptive SJF
- Round Robin (RR) with user-defined time quantum
- Priority Scheduling - Non-preemptive
- Priority Scheduling with Aging
- Waiting Time calculation
- Turnaround Time calculation
- Response Time calculation
- Terminal-based Gantt charts
- CPU idle period representation
- Average scheduling metrics
- Algorithm comparison
- Interactive custom process input
- Input validation
- Automated scheduler tests

## Scheduling Algorithms

### FCFS

First Come First Served executes processes in the order in which they arrive.

FCFS is non-preemptive. Once a process starts executing, it continues until its CPU burst is completed.

### SJF

Shortest Job First selects the process with the smallest burst time among the processes that have already arrived.

The implementation in this project is non-preemptive.

### SRTF

Shortest Remaining Time First is the preemptive version of SJF.

At every scheduling decision, the ready process with the shortest remaining execution time is selected.

A running process may therefore be preempted when another process with a shorter remaining time becomes ready.

### Round Robin

Round Robin uses a ready queue and assigns each process a limited amount of CPU time called the time quantum.

If a process does not finish within its quantum, it is placed back into the ready queue.

The scheduling function accepts the quantum as a parameter. In custom simulation mode, the user can enter the desired quantum at runtime.

### Priority Scheduling

Priority Scheduling selects the ready process with the highest priority.

In this project:

> A smaller priority number represents a higher priority.

For example:

```text
Priority 1 > Priority 2 > Priority 3
```

The basic Priority Scheduling implementation is non-preemptive.

### Priority Scheduling with Aging

Priority scheduling may cause low-priority processes to wait for a long time when higher-priority processes continue to be selected.

Aging reduces the risk of starvation by gradually improving the effective priority of waiting processes.

The simulator calculates:

```text
priorityBoost = waitingTime / agingInterval

effectivePriority =
    max(1, originalPriority - priorityBoost)
```

The original priority stored in the process is not modified.

Instead, the effective priority is calculated dynamically when the scheduler selects the next process.

The Aging implementation in this project is non-preemptive.

## Scheduling Metrics

The simulator calculates three main scheduling metrics.

### Turnaround Time

```text
Turnaround Time = Completion Time - Arrival Time
```

or:

```text
TAT = CT - AT
```

### Waiting Time

```text
Waiting Time = Turnaround Time - Burst Time
```

or:

```text
WT = TAT - BT
```

### Response Time

```text
Response Time = Start Time - Arrival Time
```

or:

```text
RT = ST - AT
```

Response Time measures how long a process waits before receiving CPU time for the first time.

This is especially important for preemptive scheduling algorithms because response time and total waiting time can be different.

## Process Information

Each process contains the following input information:

```text
PID
Arrival Time
Burst Time
Priority
```

The simulator calculates additional scheduling information:

```text
Remaining Time
Start Time
Completion Time
Waiting Time
Turnaround Time
Response Time
```

## Project Structure

```text
cpu-scheduler/
├── Makefile
├── README.md
├── src/
│   ├── gantt.h
│   ├── main.cpp
│   ├── process.h
│   ├── scheduler.cpp
│   └── scheduler.h
└── tests/
    └── test_scheduler.cpp
```

## Requirements

The project requires:

- A C++17 compatible compiler
- GNU Make

For example, on a Linux system using g++:

```bash
g++ --version
make --version
```

## Building the Project

Build the simulator with:

```bash
make
```

The generated executable is:

```text
scheduler
```

## Running the Simulator

Run the program with:

```bash
make run
```

or:

```bash
./scheduler
```

The main menu is:

```text
CPU Scheduling Simulator
========================
1. Run default demonstration
2. Enter custom processes
0. Exit

Choice:
```

### Default Demonstration

Option 1 runs the built-in workload through all core scheduling algorithms.

The default workload is:

```text
PID    Arrival    Burst    Priority
P1     0          5        2
P2     1          3        1
P3     2          8        4
P4     3          6        2
P5     4          2        3
```

Round Robin uses:

```text
Quantum = 2
```

The default demonstration also runs a separate Priority Scheduling example designed specifically to demonstrate Aging.

### Custom Simulation

Option 2 allows the user to define a workload interactively.

The program asks for:

```text
Number of processes
Arrival time
Burst time
Priority
Round Robin quantum
```

For example:

```text
Number of processes: 3

Process P1
Arrival time: 0
Burst time: 5
Priority: 2

Process P2
Arrival time: 1
Burst time: 2
Priority: 1

Process P3
Arrival time: 4
Burst time: 3
Priority: 3

Round Robin quantum: 3
```

The custom workload is then executed using FCFS, SJF, SRTF, Round Robin, and Priority Scheduling.

The program prints individual process metrics, Gantt charts, and the final algorithm comparison.

## Input Validation

Interactive input is validated before it is accepted.

The current rules are:

```text
Number of processes >= 1
Arrival time        >= 0
Burst time          >= 1
Priority            >= 1
Round Robin quantum >= 1
```

Invalid values are rejected and the program asks for the value again.

## Gantt Chart

Each scheduling algorithm can generate a terminal-based execution timeline.

For example:

```text
SRTF Gantt Chart
[0-P1-1] [1-P2-4] [4-P5-6] [6-P1-10] [10-P4-16] [16-P3-24]
```

Each segment contains:

```text
[start time - process - end time]
```

CPU idle periods are represented using:

```text
IDLE
```

For example:

```text
[0-IDLE-2] [2-P1-5] [5-IDLE-8] [8-P2-10]
```

Consecutive execution units belonging to the same process are merged into a single Gantt segment.

## Algorithm Comparison

The simulator calculates average Waiting Time, Turnaround Time, and Response Time for each core scheduling algorithm.

For the default workload:

```text
Algorithm                   Avg WT       Avg TAT      Avg RT
------------------------------------------------------------
FCFS                          8.20         13.00        8.20
SJF                           5.60         10.40        5.60
SRTF                          5.20         10.00        4.20
Round Robin                  10.60         15.40        2.80
Priority                      6.60         11.40        6.60
```

The results depend on the workload and on the performance metric being considered.

No single scheduling algorithm is optimal for every workload and every metric.

For example, in the default workload SRTF produces a lower average Waiting Time than Round Robin, while Round Robin produces a lower average Response Time.

## Aging Demonstration

A separate workload is included to demonstrate the effect of Aging.

The demonstration contains a low-priority process:

```text
P2
Arrival Time = 0
Burst Time   = 3
Priority     = 6
```

Without Aging, this process executes last and starts at time 20.

With:

```text
Aging Interval = 3
```

its effective priority improves while it waits.

It therefore starts at time 16 instead of time 20.

The resulting Gantt chart is:

```text
Priority with Aging Gantt Chart
[0-P1-4] [4-P3-8] [8-P4-12] [12-P5-16] [16-P2-19] [19-P6-23]
```

This example demonstrates how Aging can reduce long waiting times and reduce the risk of starvation.

## Automated Tests

The project includes an automated test program located in:

```text
tests/test_scheduler.cpp
```

Run the tests with:

```bash
make test
```

The current test suite verifies:

- CPU idle periods
- Processes with identical arrival times
- SJF scheduling order
- Round Robin using a different quantum
- Gantt chart execution intervals

A successful test run produces:

```text
Running Scheduler Tests
=======================
[PASS] CPU idle test
[PASS] Same arrival time test
[PASS] Round Robin quantum test

All tests passed successfully.
```

## Cleaning the Project

Remove generated executables and object files with:

```bash
make clean
```

Perform a complete clean build with:

```bash
make rebuild
```

## Implementation Notes

The simulator uses integer time units.

FCFS, SJF, Priority Scheduling, and Priority Scheduling with Aging are non-preemptive.

SRTF is preemptive and evaluates scheduling decisions as simulated time advances.

Round Robin is preemptive and uses a configurable time quantum.

For equal scheduling criteria, the current implementation selects the first eligible process encountered according to the stored process order.

The project simulates CPU scheduling behavior. It does not modify or interact with the real operating system CPU scheduler.

## Portability

The project uses standard C++17 and does not depend on platform-specific scheduling APIs.

The primary development environment is Ubuntu Linux under WSL2 using GNU g++ and GNU Make.

The project is intended to compile on other Linux distributions with a C++17-compatible compiler.

## Possible Future Extension

A possible extension is Multilevel Feedback Queue (MLFQ) scheduling.

MLFQ is intentionally outside the core implementation so that the required scheduling algorithms, metrics, comparison, testing, and documentation remain the primary focus of the project.