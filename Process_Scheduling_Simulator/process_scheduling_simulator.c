#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAXIMUM_PROCESS_NAME_LENGTH 50
#define MAXIMUM_PROCESSES_COUNT 1000
#define HASH_MAP_MAXIMUM_SIZE 1000
#define MAXIMUM_PROCESS_KILL_EVENTS 1000
#define MAXIMUM_INPUT_LENGTH 100

typedef enum CurrentProcessState
{
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} CurrentProcessState;

typedef struct ProcessControlBlock
{
    int processId;
    char name[MAXIMUM_PROCESS_NAME_LENGTH];
    int processBurstTime;
    int processArrivalTime;
    int ioStartTime;
    int ioDuration;
    int executionTime;
    int ioRemaining;
    int processCompletionTime;
    int totalTurnAroundTime;
    int totalWaitTime;
    CurrentProcessState processState;
    int processKilledStatus;
    int processKillTime;
} ProcessControlBlock;

typedef struct QueueNode
{
    ProcessControlBlock *processInfo;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

typedef struct HashNode
{
    int pid;
    ProcessControlBlock *process;
    struct HashNode *next;
} HashNode;

typedef struct HashMap
{
    HashNode *buckets[HASH_MAP_MAXIMUM_SIZE];
} HashMap;

typedef struct Scheduler
{
    HashMap *pidToProcessMap;
    Queue *readyQueue;
    Queue *waitingQueue;
    Queue *terminatedQueue;
    ProcessControlBlock *processUnderExecution;
    int currentTime;
    ProcessControlBlock *allProcesses[MAXIMUM_PROCESSES_COUNT];
    int processCount;
    int killProcessId[MAXIMUM_PROCESS_KILL_EVENTS];
    int processKillTimes[MAXIMUM_PROCESS_KILL_EVENTS];
    int killEventCount;
    int terminatedProcessCount;
} Scheduler;

static int hashFunction(int processId)
{
    return processId % HASH_MAP_MAXIMUM_SIZE;
}

HashMap *initializeHashmap()
{
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (!map)
    {
        printf("Memory allocation failure \n");
        return NULL;
    }
    for (int i = 0; i < HASH_MAP_MAXIMUM_SIZE; i++)
    {
        map->buckets[i] = NULL;
    }
    return map;
}

void insertValueHashMap(HashMap *map, int processId, ProcessControlBlock *process)
{
    int index = hashFunction(processId);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    if (!newNode)
    {
        printf("Memory allocation Failure \n");
        exit(1);
    }
    newNode->process = process;
    newNode->process->processId = processId;
    newNode->next = map->buckets[index];
    map->buckets[index] = newNode;
}

ProcessControlBlock *hashMapGetProcess(HashMap *map, int processId)
{
    int index = hashFunction(processId);
    HashNode *node = map->buckets[index];

    while (node != NULL)
    {
        if (node->process->processId == processId)
        {
            return node->process;
        }
        node = node->next;
    }
    return NULL;
}

void freeHashmap(HashMap *map)
{
    for (int i = 0; i < HASH_MAP_MAXIMUM_SIZE; i++)
    {
        HashNode *node = map->buckets[i];
        while (node != NULL)
        {
            HashNode *tempNode = node;
            node = node->next;
            free(tempNode);
        }
    }
    free(map);
}

Queue *initializeQueue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (!queue)
    {
        printf("Memory allocation failure \n ");
        return NULL;
    }
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

void enqueue(Queue *queue, ProcessControlBlock *process)
{
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (!newNode)
    {
        printf("Memory allocation failure \n");
        exit(1);
    }
    newNode->processInfo = process;
    newNode->next = NULL;
    if (queue->rear == NULL)
    {
        queue->front = queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

int queueIsEmpty(Queue *queue)
{
    return queue->size == 0;
}

ProcessControlBlock *dequeue(Queue *queue)
{
    if (queueIsEmpty(queue))
    {
        return NULL;
    }
    QueueNode *tempNode = queue->front;
    ProcessControlBlock *process = tempNode->processInfo;
    queue->front = queue->front->next;
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    free(tempNode);
    queue->size--;
    return process;
}

int removeByPid(Queue *queue, int processId)
{
    if (queueIsEmpty(queue))
    {
        return 0;
    }

    QueueNode *currNode = queue->front;
    QueueNode *prevNode = NULL;

    while (currNode != NULL)
    {
        if (currNode->processInfo->processId == processId)
        {
            if (prevNode == NULL)
            {
                queue->front = currNode->next;
                if (queue->front == NULL)
                {
                    queue->rear = NULL;
                }
            }
            else
            {
                prevNode->next = currNode->next;
                if (currNode == queue->rear)
                {
                    queue->rear = prevNode;
                }
            }
            free(currNode);
            queue->size--;
            return 1;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }
    return 0;
}

void freeQueue(Queue *queue)
{
    while (!queueIsEmpty(queue))
    {
        dequeue(queue);
    }
    free(queue);
}

ProcessControlBlock *initializeProcess(char *processName, int processId, int processBurstTime, int ioStart, int ioDuration)
{
    ProcessControlBlock *newProcess = (ProcessControlBlock *)malloc(sizeof(ProcessControlBlock));
    if (!newProcess)
    {
        printf("Memory allocation failure \n");
        return NULL;
    }
    strncpy(newProcess->name, processName, MAXIMUM_PROCESS_NAME_LENGTH - 1);
    newProcess->name[MAXIMUM_PROCESS_NAME_LENGTH - 1] = '\0';
    newProcess->processId = processId;
    newProcess->processBurstTime = processBurstTime;
    newProcess->processArrivalTime = 0;
    newProcess->ioStartTime = ioStart;
    newProcess->ioDuration = ioDuration;
    newProcess->ioRemaining = 0;
    newProcess->executionTime = 0;
    newProcess->processCompletionTime = 0;
    newProcess->totalTurnAroundTime = 0;
    newProcess->totalWaitTime = 0;
    newProcess->processState = READY;
    newProcess->processKilledStatus = 0;
    newProcess->processKillTime = -1;
    return newProcess;
}

void freeProcessControlBlock(ProcessControlBlock *process)
{
    if (process)
    {
        free(process);
    }
}

Scheduler *initializeScheduler()
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    if (!scheduler)
    {
        printf("Memory allocation failure \n");
        return NULL;
    }
    scheduler->pidToProcessMap = initializeHashmap();
    scheduler->readyQueue = initializeQueue();
    scheduler->waitingQueue = initializeQueue();
    scheduler->terminatedQueue = initializeQueue();
    scheduler->processUnderExecution = NULL;
    scheduler->currentTime = 0;
    scheduler->processCount = 0;
    scheduler->killEventCount = 0;
    scheduler->terminatedProcessCount = 0;
    return scheduler;
}

void addProcess(Scheduler *scheduler, char *name, int processId, int processBurstTime, int ioStart, int ioDuration)
{
    if (scheduler->processCount >= MAXIMUM_PROCESSES_COUNT)
    {
        printf("Max processes reached\n");
        return;
    }
    ProcessControlBlock *newProcess = initializeProcess(name, processId, processBurstTime, ioStart, ioDuration);
    scheduler->allProcesses[scheduler->processCount++] = newProcess;
    insertValueHashMap(scheduler->pidToProcessMap, processId, newProcess);
    enqueue(scheduler->readyQueue, newProcess);
}

void addKillEvent(Scheduler *scheduler, int processId, int killTime)
{
    if (scheduler->killEventCount >= MAXIMUM_PROCESS_KILL_EVENTS)
    {
        printf("Max kill events reached\n");
        return;
    }
    scheduler->killProcessId[scheduler->killEventCount] = processId;
    scheduler->processKillTimes[scheduler->killEventCount] = killTime;
    scheduler->killEventCount++;
}

static void processKillEvents(Scheduler *scheduler)
{
    int time = scheduler->currentTime;
    for (int i = 0; i < scheduler->killEventCount; ++i)
    {
        if (scheduler->processKillTimes[i] == time)
        {
            int processId = scheduler->killProcessId[i];
            ProcessControlBlock *process = hashMapGetProcess(scheduler->pidToProcessMap, processId);
            if (!process)
            {
                continue;
            }
            if (process->processKilledStatus)
            {
                continue;
            }
            removeByPid(scheduler->readyQueue, processId);
            removeByPid(scheduler->waitingQueue, processId);
            if (scheduler->processUnderExecution && scheduler->processUnderExecution->processId == processId)
            {
                process->processKilledStatus = 1;
                process->processKillTime = time;
                process->processCompletionTime = time;
                process->processState = TERMINATED;
                enqueue(scheduler->terminatedQueue, process);
                scheduler->terminatedProcessCount++;
                scheduler->processUnderExecution = NULL;
            }
            else
            {
                if (process->processState != TERMINATED)
                {
                    process->processKilledStatus = 1;
                    process->processKillTime = time;
                    process->processCompletionTime = time;
                    process->processState = TERMINATED;
                    enqueue(scheduler->terminatedQueue, process);
                    scheduler->terminatedProcessCount++;
                }
            }
        }
    }
}

static void updateWaitingQueue(Scheduler *scheduler)
{
    if (queueIsEmpty(scheduler->waitingQueue))
    {
        return;
    }
    QueueNode *prevNode = NULL;
    QueueNode *currNode = scheduler->waitingQueue->front;
    while (currNode != NULL)
    {
        ProcessControlBlock *process = currNode->processInfo;
        if (process->ioRemaining > 0)
        {
            process->ioRemaining -= 1;
        }
        QueueNode *nextNode = currNode->next;
        if (process->ioRemaining <= 0 && process->processState != TERMINATED && !process->processKilledStatus)
        {
            if (prevNode == NULL)
            {
                scheduler->waitingQueue->front = nextNode;
                if (scheduler->waitingQueue->front == NULL)
                {
                    scheduler->waitingQueue->rear = NULL;
                }
            }
            else
            {
                prevNode->next = nextNode;
                if (currNode == scheduler->waitingQueue->rear)
                {
                    scheduler->waitingQueue->rear = prevNode;
                }
            }
            scheduler->waitingQueue->size--;
            process->processState = READY;
            enqueue(scheduler->readyQueue, process);
            free(currNode);
        }
        else
        {
            prevNode = currNode;
        }
        currNode = nextNode; 
    }
}

void displaySchedulerResult(Scheduler *scheduler)
{
    printf("\nFinal Results\n");
    printf("%-5s %-15s %-5s %-5s %-10s %-10s %-15s\n",
           "PID", "Name", "CPU", "IO", "Turnaround", "Waiting", "Status");
    for (int i = 0; i < scheduler->processCount; i++)
    {
        ProcessControlBlock *process = scheduler->allProcesses[i];
        if (process->processKilledStatus)
        {
            printf("%-5d %-15s %-5d %-5d %-10s %-10s KILLED at %-3d\n",
                   process->processId,
                   process->name,
                   process->processBurstTime,
                   process->ioDuration,
                   "-",
                   "-",
                   process->processKillTime);
        }
        else
        {
            process->totalTurnAroundTime =
                process->processCompletionTime - process->processArrivalTime;
            process->totalWaitTime =
                process->totalTurnAroundTime - process->processBurstTime;

            printf("%-5d %-15s %-5d %-5d %-10d %-10d OK\n",
                   process->processId,
                   process->name,
                   process->processBurstTime,
                   process->ioDuration,
                   process->totalTurnAroundTime,
                   process->totalWaitTime);
        }
    }
}

void freeScheduler(Scheduler *scheduler)
{
    for (int i = 0; i < scheduler->processCount; i++)
    {
        freeProcessControlBlock(scheduler->allProcesses[i]);
    }
    freeHashmap(scheduler->pidToProcessMap);
    freeQueue(scheduler->waitingQueue);
    freeQueue(scheduler->readyQueue);
    freeQueue(scheduler->terminatedQueue);
    free(scheduler);
}

int main()
{
    Scheduler *scheduler = initializeScheduler();
    if (!scheduler)
    {
        return 1;
    }

    char inputLine[MAXIMUM_INPUT_LENGTH];
    while (fgets(inputLine, MAXIMUM_INPUT_LENGTH, stdin))
    {
        char *tempLine = inputLine;
        while (isspace((unsigned char)*tempLine))
        {
            tempLine++;
        }
        if (*tempLine == '\0' || *tempLine == '\n')
        {
            continue;
        }
        if (strcmp(tempLine,"RUN\n") == 0 || strcmp(tempLine,"RUN") == 0){
            break;
        }
        if (strncmp(tempLine, "KILL", 4) == 0)
        {
            int processId = 0, time = 0;
            if (sscanf(tempLine, "KILL %d %d", &processId, &time) == 2)
            {
                addKillEvent(scheduler, processId, time);
            }
            else
            {
                fprintf(stderr, "Incorrect KILL line: %s\n", tempLine);
            }
        }
        else
        {
            char name[MAXIMUM_PROCESS_NAME_LENGTH];
            int processId, processBurstTime;
            int ioStart;
            int ioDuration;
            int items = sscanf(tempLine, "%49s %d %d %d %d", name, &processId, &processBurstTime, &ioStart, &ioDuration);
            if (items == 5)
            {
                addProcess(scheduler, name, processId, processBurstTime, ioStart, ioDuration);
            }
            else
            {
                fprintf(stderr, "Incorrect process line %s\n", tempLine);
            }
        }
    }

    if (scheduler->processCount == 0)
    {
        printf("No processes \n");
        freeScheduler(scheduler);
        return 0;
    }

    int totalProcesses = scheduler->processCount;
    for (int i = 0; i < totalProcesses; ++i)
    {
        ProcessControlBlock *newProcess = scheduler->allProcesses[i];
        newProcess->ioRemaining = 0;
    }

    while (scheduler->terminatedProcessCount < totalProcesses)
    {
        processKillEvents(scheduler);

        if (scheduler->processUnderExecution == NULL)
        {
            ProcessControlBlock *nextNode = dequeue(scheduler->readyQueue);
            if (nextNode != NULL && !nextNode->processKilledStatus && nextNode->processState != TERMINATED)
            {
                nextNode->processState = RUNNING;
                scheduler->processUnderExecution = nextNode;
            }
        }
        if (scheduler->processUnderExecution != NULL)
        {
            ProcessControlBlock *currNode = scheduler->processUnderExecution;
            if (currNode->processKilledStatus)
            {
                currNode->processState = TERMINATED;
                currNode->processCompletionTime = scheduler->currentTime;
                enqueue(scheduler->terminatedQueue, currNode);
                scheduler->processUnderExecution = NULL;
                scheduler->terminatedProcessCount++;
            }
            else
            {
                sleep(1);
                currNode->executionTime += 1;

                if (currNode->ioStartTime > 0 && currNode->executionTime == currNode->ioStartTime)
                {
                    currNode->ioRemaining = currNode->ioDuration;
                    currNode->processState = WAITING;
                    enqueue(scheduler->waitingQueue, currNode);
                    scheduler->processUnderExecution = NULL;
                }
                else if (currNode->executionTime >= currNode->processBurstTime)
                {
                    currNode->processState = TERMINATED;
                    currNode->processCompletionTime = scheduler->currentTime + 1;
                    enqueue(scheduler->terminatedQueue, currNode);
                    scheduler->processUnderExecution = NULL;
                    scheduler->terminatedProcessCount++;
                }
            }
        }
        else
        {
            sleep(1);
        }

        updateWaitingQueue(scheduler);

        scheduler->currentTime += 1;
    }
    displaySchedulerResult(scheduler);
    freeScheduler(scheduler);

    return 0;
}