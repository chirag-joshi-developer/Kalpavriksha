#include<stdio.h>
#include<stdlib.h>

#define MAXIMUM_PROCESS_NAME_LENGTH 50
#define MAXIMUM_PROCESSES_COUNT 1000
#define HASH_MAP_MAXIMUM_SIZE 1000
#define MAXIMUM_PROCESS_KILL_EVENTS 1000

typedef enum CurrentProcessState{
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} CurrentProcessState;

typedef sturct ProcessControlBlock{
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
    int tatalWaitTime;
    CurrentProcessState processState;
    int processKilledStatus;
    int processKillTime;
}ProcessControlBlock

typedef struct QueueNode{
    ProcessControlBlock* processInfo;
    struct QueueNode* next;
}QueueNode;

typedef sturct Queue{
    QueueNode* front;
    QueueNode* rear;
    int size;
}Queue;

typedef struct HashNode{
    int pid;
    ProcessControlBlock* process;
    struct HashNode* next;
}HashNode;

typedef struct HashMap{
    HashNode* buckets[HASH_MAP_MAXIMUM_SIZE];
}HashMap;

typedef struct Scheduler{
    HashMap* pidToProcessMap;
    Queue* readyQueue;
    Queue* waitingQueue;
    Queue* terminatedQueue;
    ProcessControlBlock* processUnderExecution;
    int currentTime;
    ProcessControlBlock* allProcesses[MAXIMUM_PROCESSES_COUNT];
    int processCount;
    int killEvenCount
}Scheduler;

static int hashFunction(int processId){
    return processId % HASH_MAP_MAXIMUM_SIZE;
}

HashMap* initializeHashmap(){
    HashMap* map = (HashMap*)malloc(sizeof(hashmap));
    if(!map){
        printf("Memory allocation failure \n");
        return NULL;
    }
    for(int i = 0 ; i < HASH_MAP_MAXIMUM_SIZE;i++){
        map->buckets[i] = NULL;
    }
    return map;
}

void insertValueHashMap(HashMap* map, int processId, PCB* process){
    int index = hashFunction(processId);
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    if(!newNode){
        printf("Memory allocation Failure \n");
        exit(1);
    }
    newNode->processId = processId;
    newNode->process = process;
    newNode->next = map->buckets[index];
    map->buckets[index] = newNode;
}

ProcessControlBlock* hashMapGetProcess(HashMap* map,int processId){
    int index = hashFunction(processId);
    HashNode* node = map->buckets[index];

    while(node != NULL){
        if(node->processId == processId){
            return node->process;
        }
        node = node->next;
    }
    return NULL;
}

void freeHashmap(HashMap* map){
    for(int i = 0; i < HASH_MAP_MAXIMUM_SIZE; i++){
        HashNode* node = map->buckets[i];
        while(node != NULL){
            HashNode* tempNode = node;
            node = node->next;
            free(tempNode);
        }
    }
    free(map);
}

Queue* initializeQueue(){
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if(!queue){
        printf("Memory allocation failure \n ");
        return NULL;
    }
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

void enqueue(Queue* queue, ProcessControlBlock* process){
    QueueNode* newNode =  (QueueNode* )malloc(sizeof(QueueNode));
    if(!newNode){
        printf("Memory allocation failure \n");
        exit(1);
    }
    newNode->process = process;
    newNode->next = NULL;
    if(queue->rear == NULL){
        queue->front = queue->rear = newNode;
    }
    else{
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

int queueIsEmpty(Queue* queue){
    return queue->size == 0;
}

ProcessControlBlock* dequeue(Queue* queue){
    if(queueIsEmpty(queue)){
        return NULL;
    }
    QueueNode* tempNode = queue->front;
    ProcessControlBlock* process = tempNode->process;
    queue->front = queue->front->next;
    if(queue->front == NULL){
        queue->rear = NULL;
    }
    free(tempNode);
    queue->size--;
    return process;
}

int removeByPid(Queue* queue, int processId){
    if(queueIsEmpty(queue)){
        return 0;
    }

    QueueNode* currNode = queue->front;
    QueueNode* prevNode = NULL;

    while(currNode != NULL){
        if(currNode->process->processId == processId){
            if(prevNode == NULL){
                queue->front = currNode->next;
                if(queue->front == NULL){
                    queue->rear = NULL;
                }
            }
            else{
                prev->next = currNode->next;
                if(currNode == queue->rear){
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

void freeQueue(Queue* queue){
    while(!isEmpty(queue)){
        dequeue(queue);
    }
    free(queue);
}

int main(){
    printf("hello world ");
    return 0;
}