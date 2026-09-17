# CPU Scheduling Simulator

A C++17 CPU scheduling simulator developed as an Operating Systems laboratory project.

The program implements several common CPU scheduling algorithms, calculates scheduling metrics, generates terminal-based Gantt charts, and compares the performance of the algorithms on the same workload.

## Features

The simulator currently supports:

- First Come First Served (FCFS)
- Shortest Job First (SJF) - Non-preemptive
- Shortest Remaining Time First (SRTF) - Preemptive SJF
- Round Robin (RR) with configurable time quantum
- Priority Scheduling - Non-preemptive
- Priority Scheduling with Aging
- Waiting Time calculation
- Turnaround Time calculation
- Response Time calculation
- Terminal-based Gantt charts
- Average scheduling metrics
- Algorithm comparison
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

At every scheduling decision, the ready process with the shortest remaining execution time is selected. A running process may therefore be preempted when a shorter process becomes ready.

### Round Robin

Round Robin uses a ready queue and assigns each process a limited amount of CPU time called the time quantum.

If the process does not finish within its quantum, it is placed back into the ready queue.

The quantum is passed to the scheduler as a parameter and is not hard-coded into the scheduling algorithm.

### Priority Scheduling

Priority Scheduling selects the ready process with the highest priority.

In this project:

> A smaller priority number represents a higher priority.

For example, priority 1 has higher priority than priority 4.

The basic Priority Scheduling implementation is non-preemptive.

### Priority Scheduling with Aging

Priority scheduling can cause low-priority processes to wait for a long time if higher-priority processes continue to be selected.

Aging reduces the risk of starvation by gradually improving the effective priority of processes while they wait.

The simulator uses:

```text
priorityBoost = waitingTime / agingInterval

effectivePriority =
    max(1, originalPriority - priorityBoost)
```

The original process priority is not modified. The effective priority is calculated dynamically during scheduling.

## Scheduling Metrics

For each process, the simulator calculates the following metrics.

### Turnaround Time

```text
Turnaround Time = Completion Time - Arrival Time
```

### Waiting Time

```text
Waiting Time = Turnaround Time - Burst Time
```

### Response Time

```text
Response Time = Start Time - Arrival Time
```

Response Time measures how long a process waits before receiving CPU time for the first time.

This is especially important for preemptive algorithms such as Round Robin and SRTF because response time and total waiting time may be significantly different.

## Process Information

Each process contains the following input information:

```text
PID
Arrival Time
Burst Time
Priority
```

The scheduler calculates:

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

## Building the Project

The project requires:

- A C++17 compatible compiler
- GNU Make

Build the simulator with:

```bash
make
```

The executable will be created as:

```text
scheduler
```

## Running the Simulator

Run the simulator with:

```bash
make run
```

or directly:

```bash
./scheduler
```

## Running Tests

The project contains automated scheduler tests.

Run them with:

```bash
make test
```

The current test suite verifies:

- CPU idle periods
- Processes with identical arrival times
- SJF selection behavior
- Round Robin with a different time quantum
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

## Cleaning Build Files

Remove generated executables and object files with:

```bash
make clean
```

To perform a complete clean build:

```bash
make rebuild
```

## Gantt Chart

Each scheduling algorithm can generate a terminal-based execution timeline.

For example:

```text
SRTF Gantt Chart
[0-P1-1] [1-P2-4] [4-P5-6] [6-P1-10] [10-P4-16] [16-P3-24]
```

This representation makes preemption and process execution order directly visible.

CPU idle periods are represented as `IDLE`.

## Algorithm Comparison

The simulator calculates the average Waiting Time, Turnaround Time, and Response Time for each scheduling algorithm.

Example output:

```text
Algorithm Comparison
============================================================
Algorithm                   Avg WT       Avg TAT      Avg RT
------------------------------------------------------------
FCFS                          8.20         13.00        8.20
SJF                           5.60         10.40        5.60
SRTF                          5.20         10.00        4.20
Round Robin                  10.60         15.40        2.80
Priority                      6.60         11.40        6.60
```

The results depend on the workload. No single scheduling algorithm is optimal for every workload or every performance metric.

For example, an algorithm may provide a low average response time while producing a higher average waiting time.

## Aging Demonstration

A separate workload is included to demonstrate the effect of Aging.

Without Aging, a low-priority process in the demonstration waits until time 20 before receiving the CPU.

With an aging interval of 3, its effective priority gradually improves and it begins execution at time 16.

Example:

```text
Priority with Aging Gantt Chart
[0-P1-4] [4-P3-8] [8-P4-12] [12-P5-16] [16-P2-19] [19-P6-23]
```

This demonstrates how Aging can reduce long waiting times and reduce the risk of starvation.

## Development Environment

The project is written in standard C++17 and is intended to remain portable across Linux environments.

It has been developed and tested using:

- Ubuntu Linux under WSL2
- GNU g++
- GNU Make

The code avoids platform-specific scheduling APIs because the purpose of the project is to simulate scheduling algorithms rather than modify the operating system scheduler itself.

## Possible Future Extension

A possible future extension is a Multilevel Feedback Queue (MLFQ) scheduler.

MLFQ is intentionally kept outside the core project requirements so that the primary scheduling algorithms remain the focus of the simulator.