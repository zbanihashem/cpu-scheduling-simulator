#ifndef PROCESS_H
#define PROCESS_H

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;

    int remainingTime = 0;
    int startTime = 0;
    int completionTime = 0;

    int waitingTime = 0;
    int turnaroundTime = 0;
    int responseTime = 0;
};

#endif