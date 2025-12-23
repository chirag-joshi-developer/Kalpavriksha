#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define MAXIMUM_SIZE 1000

typedef struct SharedData{
    int arraySize;
    int array[MAXIMUM_SIZE];
} SharedData;

void sortArray(SharedData* data){
    for(int i = 1; i < data->arraySize; i++){
        int key = data->array[i];
        int j = i-1;
        while(j >= 0 && data->array[j] > key){
            data->array[j+1] = data->array[j];
            j--;
        }
        data->array[j+1] = key;
    }
}

void displayArray(SharedData* data){
    for(int i = 0;i < data->arraySize ; i++){
        printf("%d ",data->array[i]);
    }
    printf("\n");
}

int readArray(SharedData* data){
    printf("Enter number of elements :\n");
    scanf("%d",&data->arraySize);

    if(data->arraySize <= 0 || data->arraySize > MAXIMUM_SIZE){
        printf("Array size must be between 1 and 1000 \n");
        return -1;
    }

    printf("Enter %d elements:\n ",data->arraySize);
    for(int i = 0; i < data->arraySize;i++){
        if(scanf("%d", &data->array[i]) != 1){
            printf("Array element must be an integer value.\n");
            return -1;
        }
    }
    return 1;
}

void childTask(SharedData* data){
    printf("Sorting array in shared memory \n");
    sortArray(data);
}

void releaseSharedData(SharedData* data, int sharedMemoryIdentifier){
    shmdt(data);
    shmctl(sharedMemoryIdentifier, IPC_RMID, NULL);
}

SharedData* initializeSharedData(int *sharedMemoryIdentifier){
    *sharedMemoryIdentifier = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    if(*sharedMemoryIdentifier == -1){
        printf("Shared memory creation failed \n");
        return NULL;
    }
    SharedData* data = (SharedData*)shmat(*sharedMemoryIdentifier,NULL,0);
    if(data == (void*)-1){
        printf("Shared memory attach failure \n");
        return NULL;
    }
    return data;
}

void executeSharedMemoryIPC(){
    int sharedMemoryIdentifier;
    SharedData * data = initializeSharedData(&sharedMemoryIdentifier);
    if(!data){
        return;
    }

    if(readArray(data) == -1){
        releaseSharedData(data, sharedMemoryIdentifier);
        return;
    }
    
    printf("\nParent Process \n");
    printf("Array before sorting :\n");
    displayArray(data);

    pid_t pid = fork();

    if(pid == 0){
        printf("\nChild Process \n");
        childTask(data);
        exit(0);
    }
    else if(pid > 0){
        wait(NULL);
        printf("\nParent Process\n");
        printf("Array after sorting \n");
        displayArray(data);
    }
    else{
        printf("Fork failure \n");
    }
    releaseSharedData(data, sharedMemoryIdentifier);
}
int main(){
    executeSharedMemoryIPC();
    return 0;
}