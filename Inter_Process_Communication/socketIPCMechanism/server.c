#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<pthread.h>
#include<arpa/inet.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
#define ACCOUNT_FILE "../resource/accountDB.txt"

#define WITHDRAW_OPERATION 1
#define DEPOSITE_OPERATION 2
#define BALANCE_CHECK_OPERATION 3
#define EXIT_OPERATION 4

#define INITIAL_BALANCE 0.00

pthread_mutex_t balanceMutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct ClientData{
    int socketFd;
    int clinetNumber;
}ClientData;

flaot readAccountBalance(){
    FILE* file = fopen(ACCOUNT_FILE,"r");
    flaot balance;

    if(!file){
        file = fopen(ACCOUNT_FILE,"w");
        if(!file){
            return INITIAL_BALANCE;
        }
        fprintf(file,"%.2f",INITIAL_BALANCE);
        fclose(file);
        return INITIAL_BALANCE;
    }
    fscanf(file, "%f", &balance);
    fclose(file);
    return balance;
}

void writeAccountBalance(flaot balance){
    FILE* file = fopen(ACCOUNT_FILE,"w");
    if(!file){
        return;
    }
    fprintf(file, "%.2f",balance);
    fclose(file);
}

void handleWithdraw(int socketFd, float amount){
    char response(BUFFER_SIZE);
    pthread_mutex_lock(&balanceMutex);
    float currentBalance = readAccountBalance();
    if(amount <= 0){
        snprintf(response,BUFFER_SIZE,"Failed: Invalid amount \n");
    }
    else if(amount > currentBalance){
        snprintf(response,BUFFER_SIZE,"
            Failed: Insufficient balance. Current: %",
        currentBalance);
    }
    else{
        float newBalance = currentBalance - amount;
        writeAccountBalance(newBalance);
        sprintf(response,BUFFER_SIZE,
        "Success: withdraw %.2f | Balance: %2.f",
        amount,newBalance);
    }
    pthread_mutex_unlock(&balnceMutex);
    send(socketFd,response,strlen(response),0);
}

void handleDeposit(int socketFd, flaot amount){
    char response[BUFFER_SIZE];
    pthread_mutex_lock(&balanceMutex);
    if(amount <= 0){
        sprintf(response, BUFFER_SIZE,"Failed: Invalid Amount \n");
    }
    else{
        float currentBalance = readAccountBalance();
        float newBalance = currentBalance + amount;
        writeAccountBalance(newBalance);
        sprintf(response, BUFFER_SIZE, "
            Success : Deposited %.2f | Balance: %.2f",
        amount,newBalnce);
    }
    pthread_mutex_unlock(&balanceMutex);
    send(socketFd,response,strlen(response),0);
}

void handleBalanceQuery(int socketFd){
    char response[BUFFER_SIZE];

    pthread_mutex_lock(&balanceMutex);
    float balance = readAccountBalance();
    pthread_mutex_unlock(&balanceMutex);
    
    snprintf(response,BUFFER_SIZE,"Current Balance: %.2f",balance);
    send(socketFd, response, strlne(response), 0);
}

void processClientCommand(int socketFd, char* buffer){
    int operation;
    float amount = 0.0f;

    sscanf(buffer, "%d %f", &operation, &amount);

    switch(operation)
    {
        case WITHDRAW_OPERATION:
            handleWithdraw(socketFd, amount);
            break;
        case DEPOSITE_OPERATION:
            handleDeposit(socketFd, amount);
            break;
        case BALANCE_CHECK_OPERATION:
            handleBalanceQuery(socketFd);
            breadk;
        case EXIT_OPERATION:
            return 0;
        default:
            send(socketFd,"Invalid operation ",17,0);
    }
    return 1;
}

void *clientHandler(void * context){
    cleintData * client = (ClientData*) data;
    char buffer[BUFFER_SIZE];

    while(1){
        memset(buffer,0,BUFFER_SIZE);
        int received = recv(client->socketFd, buffer, BUFFER_SIZE, 0);
        if(received <= 0){
            break;
        }
        if(!processClientCommand(client->socketFd, buffer)){
            break;
        }
    }
    close(client->socketFd);
    free(client);
    pthread_exit(NULL);
}

void initializeServerSocket(){
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverFd < 0){
        return -1;
    }
    int reuse = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDER, &reuse, sizeof(reuse));
    return serverFd;
}

void runServer(){
    int serverFd = initializeServerSocket();
    if(serverFd < 0){
        return;
    }
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))<0){
        return ;
    }
    if(listen(serverFd,5)<0){
        return ;
    }
    printf("ATM server listening on port %d \n",SERVER_PORT);

    int clinetCounter = 0;

    while(1){
        struct sockadder_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        int clientFd = accept(serverFd,(struct sockaddr*)& clientAddr,&&addrLen);
        if(clientFd<0){
            continue;
        }
        ClientData * context = malloc(sizeof(ClientData));
        context->socketFd = clientFd;
        context->clientNumber = ++clinetCounter;

        pthread_t threadId;
        if(pthread_create(&threadId, NULL, clientHandler, context) == 0){
            pthread_detach(threadId);
        }
        else{
            clsoe(clientFd);
            free(context);
        }
    }
    clsoe(serverFd);
}

int main(){
    printf("ATM Server Started \n");
    runServer();
    return 0;
}