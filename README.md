# CPU Scheduling Simulator

A C++17 CPU scheduling simulator developed as an Operating Systems laboratory project.

The project simulates and compares CPU scheduling algorithms in both **single-core** and **multi-core** environments. It calculates common scheduling metrics, generates terminal-based Gantt charts, supports interactive workloads, and includes automated tests.

## Features

The simulator supports:

- First Come First Served (FCFS)
- Shortest Job First (SJF) - Non-preemptive
- Shortest Remaining Time First (SRTF) - Preemptive SJF
- Round Robin (RR) with user-defined time quantum
- Priority Scheduling - Non-preemptive
- Priority Scheduling with Aging
- Single-core scheduling
- Multi-core scheduling with a configurable number of CPU cores
- Waiting Time calculation
- Turnaround Time calculation
- Response Time calculation
- Terminal-based Gantt charts for each CPU core
- CPU idle period representation
- Average scheduling metrics
- Algorithm comparison
- Interactive custom process input
- Input validation
- Automated tests

## Scheduling Algorithms

### FCFS

First Come First Served executes processes in the order in which they arrive.

FCFS is non-preemptive. Once a process starts executing, it continues until its CPU burst is completed.

### SJF

Shortest Job First selects the process with the smallest burst time among the processes that have already arrived.

The implementation in this project is non-preemptive.

### SRTF

Shortest Remaining Time First is the preemptive version of SJF.

At each scheduling decision, the ready processes with the shortest remaining execution times are selected.

A running process may therefore be preempted when another process with a shorter remaining time becomes ready.

### Round Robin

Round Robin uses a shared ready queue and assigns each process a limited amount of CPU time called the time quantum.

If a process does not finish within its quantum, it is placed back into the ready queue.

The quantum is configurable at runtime in custom simulation mode.

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

The original priority stored in the process is not modified. The effective priority is calculated dynamically whenever the scheduler selects another process.

The Aging implementation in this project is non-preemptive.

## Single-Core and Multi-Core Scheduling

The simulator contains separate scheduling implementations for single-core and multi-core execution.

In single-core mode, only one process can execute at a time.

In multi-core mode, multiple processes may execute simultaneously. The number of CPU cores is configurable in custom simulation mode.

The multi-core simulator uses a simplified **global scheduling model**:

- All processes belong to a common workload.
- Ready processes are selected from a shared scheduling state or ready queue.
- A free core can execute an eligible ready process.
- Preemptive algorithms may execute different time slices of the same process on different cores.
- Processor affinity, migration cost, cache effects, NUMA behavior, and operating-system-specific scheduling details are outside the scope of this simulator.

For preemptive multi-core algorithms such as SRTF and Round Robin, the `Core` value in the result table represents the core on which the process **completed**. The per-core Gantt chart shows the complete execution history and should be used to see migrations between cores.

## Scheduling Metrics

The simulator calculates three main scheduling metrics.

### Turnaround Time

```text
Turnaround Time = Completion Time - Arrival Time
TAT = CT - AT
```

### Waiting Time

```text
Waiting Time = Turnaround Time - Burst Time
WT = TAT - BT
```

### Response Time

```text
Response Time = Start Time - Arrival Time
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
│   ├── main.cpp
│   ├── process.h
│   ├── scheduler.cpp
│   ├── scheduler.h
│   ├── gantt.h
│   ├── multicore_scheduler.cpp
│   └── multicore_scheduler.h
└── tests/
    ├── test_scheduler.cpp
    ├── test_multicore_scheduler.cpp
    ├── test_multicore_srtf.cpp
    └── test_multicore_rr.cpp
```

## Requirements

The project requires:

- A C++17-compatible compiler
- GNU Make

For example:

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

Option 1 runs a built-in workload through both the single-core and multi-core schedulers.

The default workload is:

```text
PID    Arrival    Burst    Priority
P1     0          5        2
P2     1          3        1
P3     2          8        4
P4     3          6        2
P5     4          2        3
```

The default configuration uses:

```text
Round Robin Quantum = 2
CPU Cores           = 2
Aging Interval      = 3
```

The demonstration also runs a separate single-core Priority Scheduling workload specifically designed to show the effect of Aging.

### Custom Simulation

Option 2 allows the user to define a workload interactively.

The program asks for:

```text
Number of processes
Arrival time
Burst time
Priority
Round Robin quantum
Number of CPU cores
```

The same workload is then executed by the single-core and multi-core scheduling implementations.

The program prints process metrics, Gantt charts, and algorithm comparison tables.

## Input Validation

Interactive input is validated before it is accepted.

The current rules are:

```text
Number of processes  >= 1
Arrival time         >= 0
Burst time           >= 1
Priority             >= 1
Round Robin quantum  >= 1
Number of CPU cores  >= 1
```

Invalid values are rejected and the program asks for the value again.

The multi-core scheduler functions also reject invalid core counts, and Round Robin and Aging reject invalid quantum and aging interval values.

## Gantt Charts

Each scheduling algorithm generates a terminal-based execution timeline.

A single-core example:

```text
SRTF Gantt Chart
[0-P1-1] [1-P2-4] [4-P5-6] [6-P1-10] [10-P4-16] [16-P3-24]
```

A multi-core example:

```text
Multi-Core SJF Gantt Chart
======================
Core 1: [0-P1-5] [5-P4-11]
Core 2: [0-IDLE-1] [1-P2-4] [4-P5-6] [6-P3-14]
```

Each segment contains:

```text
[start time - process - end time]
```

CPU idle periods are represented using:

```text
IDLE
```

Consecutive execution units belonging to the same process on the same core are merged into a single Gantt segment when appropriate.

## Algorithm Comparison

The simulator calculates average Waiting Time, Turnaround Time, and Response Time for the scheduling algorithms.

For the default single-core workload:

```text
Algorithm                   Avg WT       Avg TAT      Avg RT
------------------------------------------------------------
FCFS                          8.20         13.00        8.20
SJF                           5.60         10.40        5.60
SRTF                          5.20         10.00        4.20
Round Robin                  10.60         15.40        2.80
Priority                      6.60         11.40        6.60
```

For the default two-core workload:

```text
Algorithm                     Avg WT       Avg TAT      Avg RT
--------------------------------------------------------------
FCFS (2 cores)                  2.20          7.00        2.20
SJF (2 cores)                   1.20          6.00        1.20
SRTF (2 cores)                  1.20          6.00        1.20
Round Robin (2 cores)           2.80          7.60        0.60
Priority (2 cores)              1.40          6.20        1.40
```

The results depend on the workload and on the performance metric being considered. No single scheduling algorithm is optimal for every workload and every metric.

For example, in the default single-core workload SRTF produces a lower average Waiting Time than Round Robin, while Round Robin produces a lower average Response Time.

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

its effective priority improves while it waits, and it starts at time 16 instead of time 20.

This demonstrates how Aging can reduce long waits and reduce the risk of starvation. It does not imply that Aging always improves every average scheduling metric.

## Automated Tests

Run all automated tests with:

```bash
make test
```

The test suite contains four executables:

```text
scheduler_tests
multicore_tests
multicore_srtf_tests
multicore_rr_tests
```

The tests cover:

- Single-core idle behavior
- Processes with identical arrival times
- Round Robin quantum behavior
- Multi-core FCFS
- Multi-core SJF
- Multi-core Priority Scheduling
- Multi-core Priority Scheduling with Aging
- Multi-core SRTF
- SRTF preemption and metrics
- Multi-core Round Robin
- Round Robin ready-queue behavior
- Invalid core counts
- Invalid Round Robin quantum
- Invalid Aging interval
- Multi-core idle periods

A successful test run ends with all test groups reporting that their tests passed.

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

- The simulator uses integer time units.
- FCFS, SJF, Priority Scheduling, and Priority Scheduling with Aging are non-preemptive.
- SRTF and Round Robin are preemptive.
- Multi-core scheduling uses a simplified global scheduling model.
- Multi-core SRTF makes scheduling decisions as simulated time advances.
- Multi-core Round Robin uses a shared ready queue and configurable time quantum.
- Aging changes effective scheduling priority without modifying the original process priority.
- The simulator models scheduling behavior only; it does not modify or interact with the real operating-system CPU scheduler.

## Portability

The project uses standard C++17 and does not depend on platform-specific scheduling APIs.

The primary development environment is Ubuntu Linux under WSL2 using GNU g++ and GNU Make.

The code is intended to compile on other Linux distributions with a C++17-compatible compiler.

## Possible Future Extension

Multilevel Feedback Queue (MLFQ) is a possible bonus extension.

MLFQ is intentionally outside the current core implementation. The required single-core and multi-core scheduling algorithms, scheduling metrics, Gantt charts, comparisons, testing, and documentation are implemented first so the project remains focused and testable.
