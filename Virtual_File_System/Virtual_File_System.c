#include<stdio.h>
#include<stdlib.h>

#define BLOCK_SIZE 512
#define NUMBER_OF_BLOCKS 1024
#define FILE_NAME_MAXIMUM_SIZE 51
#define FILE_NAME_MINIMUM_SIZE 1
#define MAXIMUM_COMMAND_LENGTH 51

typedef struct freeBlock{
    int index;
    struct freeBlock* next;
    struct freeBlock* prev;
}freeBlock;

typedef struct fileOrDirectoryNode{
    int isFile;
    char name[FILE_NAME_MAXIMUM_SIZE];
    struct fileOrDirectoryNode* parent;
    struct fileOrDirectoryNode* child;
    struct fileOrDirectoryNode* next;
    struct fileOrDirectoryNode* prev;
    int blockPointer[NUMBER_OF_BLOCKS];
    int totalBlockCOUNT;
}fileOrDirectoryNode;

freeBlock* freeBlocksHead = NULL;
freeBlock* freeBlocksTail = NULL;

fileOrDirectoryNode* root = NULL;
fileOrDirectoryNode* cwd = NULL;

char* readInput(char * command){
    printf("Enter command \n");
    while(fgets(command,MAXIMUM_COMMAND_LENGTH,stdin) == NULL){
        printf("Error reading input \n");
        printf("Enter command \n");
    }
    return command;
}

void initializeSystem(){

}

void mkdir(){

}

void changeDirectory(){

}

void createFile(){

}

void writeFile(){

}

void readFile(){

}

void DeleteFile(){

}

void RemoveDirectory(){

}

void ShowCurrentDirectory(){

}

void ShowDiskImage(){

}

void TerminateProgram(){

}

int main(){
    printf("helloe");
    char** diskSpace = (char**) malloc(NUMBER_OF_BLOCKS * sizeof(char*));

    for(int i=0;i<NUMBER_OF_BLOCKS;i++){
        *(diskSpace+i) = (char*) malloc(BLOCK_SIZE * sizeof(char));
    }
    return 0;
}