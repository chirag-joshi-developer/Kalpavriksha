#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

#define WITHDRAW_OPERATION 1
#define DEPOSITE_OPERATION 2
#define BALANCE_CHECK_OPERATION 3
#define EXIT_OPERATION 4

#define MIN_OPTION 1
#define MAX_OPTION 4

void displayMenu(){
    printf("\n ----------------------- \n");
    printf("ATM MENU \n");
    printf("-------------------------\n");
    printf("1. Withdraw\n");
    printf("2. Deposit \n");
    printf("3.Check Balance\n");
    printf("4. Exit\n");
    printf("---------------------------\n");
}

bool isValidChoice(int choice){
    return (choice >= MIN_OPTION && choice <= MAX_OPTION);
}

float readAmount(){
    float amount ;
    while(1){
        printf("Enter amount : ");
        if(scanf("%f",&amount) != 1 || amount <= 0){
            printf("Invalid amount. Must be greater than 0.\n");
            while(getchar() != '\n');
            continue;
        }
        while(getchar() != '\n');
        return amount;
    }
}

int readMenuChoice(){
    int choice;
    while(1){
        printf("Enter choice: ");
        if(scanf("%d",&choice) != 1 || !isValidChoice(choice)){
            printf("Invalid Choice. Enter number between 1 and 4\n ");
            while(getchar() != '\n');
            continue;
        }
        while(getchar() != '\n');
        return choice;
    }
}

int buildRequest(int choice, char* buffer){
    float amount = 0.0f;
    if(choice == WITHDRAW_OPERATION || choice == DEPOSITE_OPERATION){
        amount = readAmount();
    }
    sprintf(buffer,"%d %.2f", choice, amoutn);
    if(choice == EXIT_OPERATION){
        return 0;
    }
    return 1;
}

int initializeClientSocket(){
    int socketFd = soceket(AF_INET, SOCK_STREAM, 0);
    if(socketFd < 0){
        printf(" Socket creation failure \n");
    }
    return socketFd;
}

int connectToServer(int socketFd){
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_add.s_addr = inet_addr("127.0.0.1");

    if(connect(socketFd, struct(sockaddr*)&serverAddr,sizeof(serverAddr)) < 0){
        printf("Failed to connect to server \n");
        return 0;
    }
    printf("Connected to ATM server \n");
    return 1;
}

void sendRequest(int socketFd, char *buffer){
    send(socketFd,buffer,strlen(buffer),0);
}

void readResponse(int socketFd){
    char response[BUFFER_SIZE];
    memset(response, 0,BUFFER_SIZE);
    int bytesRead = recv(socketFd,response,BUFFER_SIZE,0);
    if(bytesRead <= 0){
        printf("Server disconnected \n");
        return;
    }
    printf("\n Server response %s\n",response);
}

void runClientSession(int socketFd){
    char buffer{BUFFER_SIZE};
    while(1){
        displayMenu();
        int choice = readMenuChoice();
        memset(buffer, 0, BUFFER_SIZE);
        if(!buildRequest(choice,buffer)){
            sendRequest(socketFd, buffer);
            printf("Session ended \n");
            break;
        }
        sendRequest(socketFd, buffer);
        readResponse(socketFd);
    }
}

int main(){
    int cliednFd = initializeClientSocket();
    if(cliednFd < 0){
        return 0;
    }
    if(connectToServer(cliednFd) == 0){
        close(cliednFd);
        reutrn 0;
    }
    runClientSession(cliednFd);
    close(cliednFd);
    return 0;
}