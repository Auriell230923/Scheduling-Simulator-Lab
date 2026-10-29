// C program for implementation of Simulation
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "process.h"
#include "util.h"


// Function to find the waiting time for all
// processes using Round Robin
void findWaitingTimeRR(ProcessType plist[], int n, int quantum)
{
    int rem_bt[n];

    // Copy burst times into remaining burst time array
    for (int i = 0; i < n; i++)
    {
        rem_bt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    int t = 0;

    // Keep going until all processes are finished
    while (1)
    {
        int done = 1;

        for (int i = 0; i < n; i++)
        {
            if (rem_bt[i] > 0)
            {
                done = 0;

                if (rem_bt[i] > quantum)
                {
                    t += quantum;
                    rem_bt[i] -= quantum;
                }
                else
                {
                    t += rem_bt[i];

                    plist[i].wt = t - plist[i].bt;

                    rem_bt[i] = 0;
                }
            }
        }

        if (done == 1)
            break;
    }
}


// Function to find the waiting time for all
// processes using SJF / SRTF
void findWaitingTimeSJF(ProcessType plist[], int n)
{
    int rem_bt[n];

    for (int i = 0; i < n; i++)
    {
        rem_bt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    int complete = 0;
    int t = 0;
    int minm = INT_MAX;
    int shortest = 0;
    int finish_time;
    int check = 0;

    // Continue until every process is completed
    while (complete != n)
    {
        // Find the process with the shortest remaining time
        // that has already arrived
        for (int j = 0; j < n; j++)
        {
            if ((plist[j].art <= t) &&
                (rem_bt[j] < minm) &&
                (rem_bt[j] > 0))
            {
                minm = rem_bt[j];
                shortest = j;
                check = 1;
            }
        }

        // No process is available yet
        if (check == 0)
        {
            t++;
            continue;
        }

        // Execute selected process for one time unit
        rem_bt[shortest]--;

        minm = rem_bt[shortest];

        if (minm == 0)
            minm = INT_MAX;

        // Process has completed
        if (rem_bt[shortest] == 0)
        {
            complete++;
            check = 0;

            finish_time = t + 1;

            plist[shortest].wt =
                finish_time -
                plist[shortest].art -
                plist[shortest].bt;

            if (plist[shortest].wt < 0)
                plist[shortest].wt = 0;
        }

        t++;
    }
}


// Function to find waiting time for FCFS
void findWaitingTime(ProcessType plist[], int n)
{
    int current_time = 0;

    for (int i = 0; i < n; i++)
    {
        // If the next process has not arrived,
        // CPU remains idle until it arrives
        if (current_time < plist[i].art)
        {
            current_time = plist[i].art;
        }

        // Waiting time = start time - arrival time
        plist[i].wt = current_time - plist[i].art;

        // Move time forward by this process's burst
        current_time += plist[i].bt;
    }
}


// Function to calculate turnaround time
void findTurnAroundTime(ProcessType plist[], int n)
{
    for (int i = 0; i < n; i++)
    {
        plist[i].tat = plist[i].bt + plist[i].wt;
    }
}


// Function used by qsort for priority scheduling
int my_comparer(const void *this, const void *that)
{
    ProcessType *p1 = (ProcessType *)this;
    ProcessType *p2 = (ProcessType *)that;

    // Higher priority number executes first
    if (p1->pri < p2->pri)
        return 1;

    if (p1->pri > p2->pri)
        return -1;

    return 0;
}


// Function to calculate FCFS
void findavgTimeFCFS(ProcessType plist[], int n)
{
    findWaitingTime(plist, n);

    findTurnAroundTime(plist, n);

    printf("\n*********\nFCFS\n");
}


// Function to calculate SJF
void findavgTimeSJF(ProcessType plist[], int n)
{
    findWaitingTimeSJF(plist, n);

    findTurnAroundTime(plist, n);

    printf("\n*********\nSJF\n");
}


// Function to calculate Round Robin
void findavgTimeRR(ProcessType plist[], int n, int quantum)
{
    findWaitingTimeRR(plist, n, quantum);

    findTurnAroundTime(plist, n);

    printf("\n*********\nRR Quantum = %d\n", quantum);
}


// Function to calculate Priority scheduling
void findavgTimePriority(ProcessType plist[], int n)
{
    /*
     * Sort processes according to priority,
     * then apply FCFS.
     */

    qsort(plist, n, sizeof(ProcessType), my_comparer);

    findWaitingTime(plist, n);

    findTurnAroundTime(plist, n);

    printf("\n*********\nPriority\n");
}


void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0;
    int total_tat = 0;

    float awt, att;

    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n");

    for (int i = 0; i < n; i++)
    {
        total_wt += plist[i].wt;
        total_tat += plist[i].tat;

        printf("\t%d\t\t%d\t\t%d\t\t%d\n",
               plist[i].pid,
               plist[i].bt,
               plist[i].wt,
               plist[i].tat);
    }

    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);

    printf("\nAverage waiting time = %.2f", awt);
    printf("\nAverage turn around time = %.2f\n", att);
}


ProcessType *initProc(char *filename, int *n)
{
    FILE *input_file = fopen(filename, "r");

    if (!input_file)
    {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    ProcessType *plist = parse_file(input_file, n);

    fclose(input_file);

    return plist;
}


// Driver code
int main(int argc, char *argv[])
{
    int n;
    int quantum = 2;

    ProcessType *proc_list;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        fflush(stdout);
        return 1;
    }


    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);

    findavgTimeFCFS(proc_list, n);

    printMetrics(proc_list, n);


    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);

    findavgTimeSJF(proc_list, n);

    printMetrics(proc_list, n);


    // Priority
    n = 0;
    proc_list = initProc(argv[1], &n);

    findavgTimePriority(proc_list, n);

    printMetrics(proc_list, n);


    // Round Robin
    n = 0;
    proc_list = initProc(argv[1], &n);

    findavgTimeRR(proc_list, n, quantum);

    printMetrics(proc_list, n);

    return 0;
}