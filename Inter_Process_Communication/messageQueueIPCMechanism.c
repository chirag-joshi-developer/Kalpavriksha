#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/wait.h>

#define MAXIMUM_SIZE 1000
#define TO_CHILD 1
#define TO_PARENT 2

typedef struct  MessagePacket
{
    long messageType;
    int arraySize;
    int array[MAXIMUM_SIZE];
}MessagePacket;

void displayArray(MessagePacket* packet){
    for(int i = 0;i < packet->arraySize ; i++){
        printf("%d ",packet->array[i]);
    }
    printf("\n");
}

void sortArray(MessagePacket* packet){
    for(int i = 1; i < packet->arraySize; i++){
        int key = packet->array[i];
        int j = i-1;
        while(j >= 0 && packet->array[i] > key){
            packet->array[j+1] = packet->array[j];
            j--;
        }
        packet->array[j+1] = key;
    }
}

int initializeMessageQueue(){
    int messageId = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    if(messageId == -1){
        printf("Message Queue creation failure \n");
        return 0;
    }
    return 1;
}

void destroyMessageQueue(){
    msgctl(msgId,IPC_RMID,NULL);
}

int pushMessage(int messageId, MessagePacket* packet){
    if(msgsnd(msgId, packet, sizeof(MessagePacket) - sizeof(long),0) == -1){
        printf("Message send failure \n");
        return 0;
    }
    return 1;
}

int pullMessage(int messageId, MessagePacket* packet, long type){
    if(msgrcv(msgId, packet, sizeof(MessagePacket) - sizeof(long), type,0) == -1){
        printf("Message receive failure \n");
        return 0;
    }
    return 1;
}

int readArray(MessagePacket* packet){
    printf("Enter number of elements :\n");
    scanf("%d",&packet->arraySize);

    if(packet->arraySize <= 0 || packet->arraySize > MAXIMUM_SIZE){
        printf("Array size must be between 1 and 1000 \n");
        return -1;
    }

    printf("Enter %d elements:\n ",packet->arraySize);
    for(int i = 0; i < packet->arraySize;i++){
        if(scanf("%d", &packet->array[i]) != 1){
            printf("Array element must be an integer value.\n");
            return -1;
        }
    }
    return 1;
}

void messageQueueIPCSorting(){
    
    int messageId = initializeMessageQueue();
    if(messageId == 0){
        return ;
    }
    
    MessagePacket packet;

    if(readArray(&packet) == -1){
        destroyMessageQueue(messageId);
        return;
    }

    printf("\n Parent Process \n");
    printf("Array before sorting \n");

    displayArray(&packet);
    packet.messageType = TO_CHILD;

    if(pushMessage(messageId, &packet) == 0){
        destroyMessageQueue(messageId);
        return;
    }

    pid_t pid = fork();

    if(pid == 0){
        if(pullMessage(messageId, &packet, TO_CHILD) == 0){
            exit(1);
        }

        printf("\n Child Process \n");
        printf("Sorting Array\n");
        sortArray(&packet);
        packet.messageType = TO_PARENT;

        pushMessage(messageId, &packet);
        exit(0);
    }
    else if(pid > 0){
        wait(NULL);

        if(pullMessage(messageId, &packet, TO_PARENT) == 0){
            destroyMessageQueue(messageId);
            return;
        }

        printf("\n Parent Process \n");
        printf("Array after sorting:\n");
        displayArray(&packet);
    }
    else{
        printf("Fork Failed \n");
    }
    destroyMessageQueue(messageId);
}

int main(){
    messageQueueIPCSorting();
    return 0;
}