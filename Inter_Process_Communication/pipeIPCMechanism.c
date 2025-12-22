#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#define MAXIMUM_SIZE 1000

typedef struct PipePacket{
    int arraySize;
    int array[MAXIMUM_SIZE];
}PipePacket;

int readArray(PipePacket* packet){
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

void displayArray(PipePacket* packet){
    for(int i = 0;i < packet->arraySize ; i++){
        printf("%d ",packet->array[i]);
    }
    printf("\n");
}

void sortArray(PipePacket* packet){
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

void parentRoutine(int toChild, int fromChild, PipePacket* packet){
    close(toChild[0]);
    close(fromChild[1]);

    write(toChild[1], &packet->arraySize, sizeof(int));
    write(toChild[1], packet->array, packet->arraySize* sizeof(int));
    close(toChild[1]);

    read(fromChild[0], &packet->arraySize,sizeof(int));
    read(fromChild[0],packet->array,packet->arraySize*sizeof(int));
    close(fromChild[0]);
}

void childRoutine(int fromParent[], int toParent[]){
    PipePacket packet;

    close(fromParent[1]);
    close(toParent[0]);

    read(fromParent[0], &packet.arraySize, sizeof(int));
    read(fromParent[0], packet.array, packet.arraySize * sizeof(int));
    close(fromParent[0]);

    printf("Sorting array\n");
    sortArray(&packet);

    write(toParent[1], &packet.arraySize, sizeof(int));
    write(toParent[1], packet.array,packet.arraySize * sizeof(int));
    close(toParent[1]);
}

void executePipeIPC(){
    PipePacket packet;

    if(readInput(&packet) == -1){
        return ;
    }

    printf("\n Parent Process \n");
    printf("Array before sorting : \n");
    displayArray(&packet);

    int parentToChild[2];
    int childToParent[2];

    if(pipe(parentToChild) == -1 || pipe(childToParent) == -1){
        printf("Pipe creation failed \n ");
        return;
    }

    pid_t pid = fork();

    if(pid == 0){
        printf("\n Child process \n");
        childRoutine(parentToChild, childToParent);
        exit(0);
    }
    else if(pid > 0){
        parentRoutine(parentToChild, childToParent, &packet);
        wait(NULL);

        printf("\n parent Process \n");
        printf("Array after sorting: \n");
        displayArray(&packet);
    }
    else{
        printf("Fork failure \n");
    }
}

int main(){
    executePipeIPC();
    return 0;
}