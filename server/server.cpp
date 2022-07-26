#include <iostream>
#include <map>
#include "server.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "server_commands.h"
#define PORT "58023"
#define ERROR 1
#define OK 0

#define MAX_COMMAND 50

using namespace std;

string DSIP;
char DSport[MAX_COMMAND];
int fd, errcode;
ssize_t n; socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[MAX_COMMAND], message[MAX_COMMAND * 3];

int sendUDP(char* message);

int main(int argc, char* argv[]){
    char command[MAX_COMMAND], commandName[MAX_COMMAND], message[MAX_COMMAND*5], args[MAX_COMMAND];
    strcpy(DSport, PORT);
    if(argc == 3 || argc == 4){
        if(strcmp(argv[1],"-p") == 0){
            strcpy(DSport, argv[2]);
        }
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd==-1) {
        return -1; /*error*/
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family=AF_INET; //IPV4
    hints.ai_socktype = SOCK_DGRAM; //UDP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, DSport, &hints, &res);
    if(errcode != 0) {
        return -1; /*error*/
    }
    
    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if(n==-1) {
        return -1; /*error*/
    }
    printf("Port: %s\n", DSport);
    while(1) {
        addrlen = sizeof(addr);
        n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*) &addr, &addrlen);
        if (n==-1){
            return -1; /*error*/
        }
        write(1, "User request: ", 14);
        write(1, buffer, n);

        if(sscanf(buffer, "%50[^\n]", command) != 0){
            sscanf(buffer, "%s %s", commandName, args);
            if(parseCommand(command) == 0){
                if(strcmp(commandName,"REG") == 0) {
                    comRegister(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"UNR") == 0){
                    comUnregister(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"LOG") == 0) {
                    comLogin(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"OUT") == 0) {
                    comLogout(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"GLS") == 0) {
                    comGroups(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"GSR") == 0) {
                    comSubscribe(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"GUR") == 0) {
                    comUnsubscribe(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"GLM") == 0) {
                    comMyGroups(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"ULS") == 0) {
                    comUList(command, message);
                sendUDP(message);
                } else if(strcmp(commandName,"PST") == 0){
                    comPost(command, message);
                    sendUDP(message);
                } else if(strcmp(commandName,"RTV") == 0) {
                    comRetrieve(command, message);
                    sendUDP(message);
                }
            }
            else{
                //error
                continue;
            }
        }
    }
    freeaddrinfo(res);
    close(fd);
}
int sendUDP(char* message) {
    printf("Server reply: %s\n", message);
    n = sendto(fd, message, strlen(message), 0, (struct sockaddr*)&addr, addrlen);
    if (n==-1) {
        return -1; /*error*/
    }
}

int sendUDP1(char* message) {
    while(1) {
        n = sendto(fd, message, strlen(message), 0, (struct sockaddr*)&addr, addrlen);
        if (n==-1) {
            return -1; /*error*/
        }
    }

    freeaddrinfo(res);
    close(fd);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd==-1) {
        return -1; /*error*/
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family=AF_INET; //IPV4
    hints.ai_socktype = SOCK_DGRAM; //UDP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, DSport, &hints, &res);
    if(errcode != 0) {
        return -1; /*error*/
    }
    
    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if(n==-1) {
        return -1; /*error*/
    }
}
