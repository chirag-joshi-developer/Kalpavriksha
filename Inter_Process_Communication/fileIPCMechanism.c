#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

#define MAXIMUM_SIZE 1000
#define FILE_NAME "data.txt"

typedef struct FileData{
    int arraySize;
    int array[MAXIMUM_SIZE];
} FileData;

int readArray(FileData *data){
    printf("Enter number of elements :\n");
    scanf("%d",&data->arraySize);

    if(data->arraySize <= 0 || data->arraySize >MAXIMUM_SIZE){
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

void displayArray(FileData* data){
    for(int i = 0;i < data->arraySize ; i++){
        printf("%d ",data->array[i]);
    }
    printf("\n");
}
void sortArray(FileData* data){
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

void writeToFile(FileData *data){
    FILE* filePointer = fopen(FILE_NAME, "w");
    if(!filePointer){
        printf("File access failure \n");
        return;
    }
    fprintf(filePointer, "%d\n", data->arraySize);
    for(int i = 0; i < data->arraySize; i++){
        fprintf(filePointer, "%d ", data->array[i]);
    }
    fclose(filePointer);
}

void readFromFile(FileData* data){
    FILE* filePointer = fopen(FILE_NAME,"r");
    if(!filePointer){
        printf("File access failure \n");
        return;
    }
    fscanf(filePointer, "%d", &data->arraySize);
    for(int i = 0; i < data->arraySize; i++){
        fscanf(filePointer, "%d", &data->array[i]);
    }
    fclose(filePointer);
}

void sortingFileIPC(){
    FileData data;
    if(readArray(&data) == -1){
        return;
    }

    printf("\nProcess 1: Parent process\n");
    printf("Array before sorting \n");
    displayArray(&data);

    writeToFile(&data);
    printf("Data is written to file: %s \n",FILE_NAME);
    fflush(stdout); 
    pid_t processId = fork();

    if(processId == 0){
        printf("\nProcess 2: child process \n");
        readFromFile(&data);
        sortArray(&data);
        writeToFile(&data);
        printf("Data is sorted in the file: %s \n",FILE_NAME);
        exit(0);
    }
    else if(processId > 0){
        wait(NULL);
        printf("\nProcess 1: parent process \n");
        readFromFile(&data);

        printf("Array after sorting :\n");
        displayArray(&data);
    }
    else{
        printf("Process fork Failure \n");
    }
}

int main(){
    sortingFileIPC();
    return 0;
}