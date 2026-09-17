#ifndef PROCESS_H
#define PROCESS_H

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;

    int remainingTime;
    int startTime;
    int completionTime;

    int waitingTime;
    int turnaroundTime;
    int responseTime;
};

#endif