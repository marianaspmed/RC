#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "user_commands.h"

#define MAX_COMMAND 50

using namespace std;

int main (int argc, char *argv[]){
    char command[MAX_COMMAND], commandName[MAX_COMMAND], args[MAX_COMMAND];
    parseArgs(argc,argv);
    while(true){
        if(fgets(command,MAX_COMMAND,stdin) != NULL){
            sscanf(command,"%s %s", commandName, args);
            if(parseCommand(command) == 0){
                if(strcmp(commandName,"reg") == 0){
                    comRegister(command);
                }
                else if(strcmp(commandName,"unregister") == 0 || strcmp(commandName,"unr") == 0)
                    comUnregister(command);
                else if(strcmp(commandName,"login") == 0)
                    comLogin(command);
                else if(strcmp(commandName,"logout") == 0)
                    comLogout(command); 
                else if(strcmp(commandName,"exit") == 0)
                    comExit(command); 
                else if(strcmp(commandName,"groups") == 0 || strcmp(commandName,"gl") == 0)
                    comGroups(command);
                else if(strcmp(commandName,"subscribe") == 0 || strcmp(commandName,"s") == 0)
                    comSubscribe(command);
                else if(strcmp(commandName,"unsubscribe") == 0 || strcmp(commandName,"u") == 0)
                    comUnsubscribe(command);
                else if(strcmp(commandName,"my_groups") == 0 || strcmp(commandName,"mgl") == 0)
                    comMyGroups(command);
                else if(strcmp(commandName,"select") == 0 || strcmp(commandName,"sag") == 0)
                    comSelect(command);
                else if(strcmp(commandName,"ulist") == 0 || strcmp(commandName,"ul") == 0)
                    comUList(command);
                else if(strcmp(commandName,"post") == 0){
                    comPost(command);
                }
                else if(strcmp(commandName,"retrieve") == 0)
                    comRetrieve(command);
            }
            else{
                continue;
            }
        }
    }

}
