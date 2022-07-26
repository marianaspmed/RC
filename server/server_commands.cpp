#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;
#define MAX_COMMAND 50
#define ERROR 1
#define OK 0

typedef struct Message{
    char MID[4];
    char text[240];
/*  char FName[24];
    char FSize[10];    */
}Message;

typedef struct Group{
    int currentMID = 0;
    char GID[3];
    char GName[24];
    vector<Message> Messages;
}Group;

typedef struct User{
    char UID[6];
    char pass[9];
    bool loggedin = false;
    int nGIDS = 0;
    //char GIDS[100][2];
    map<string,Group> myGroups;
}User;

int currentGID = 0;
map<string,User> Users;
map<string,Group> Groups;



int parseUID(char* UID){
    int i;
    if(strlen(UID) != 5){
        return ERROR;
    }
    for(i = 0; i<5; i++){
        if(!isdigit(UID[i])){
            return ERROR;
        }
    }
    return OK;
}
int parsePass(char* password){
    int i;
    if(strlen(password) != 8){
        return ERROR;
    }
    for(i = 0; i<8; i++){
        if(!(isdigit(password[i]) || isalpha(password[i]))){
            return ERROR;
        }
    }
    return OK;
}
int parseGID(char* GID){
    int i;
    if(strlen(GID) != 2){
        return ERROR;
    }
    for(i = 0; i<strlen(GID); i++){
        if(!(isdigit(GID[i]))){
            return ERROR;
        }
    }
    return OK;
}
int parseGName(char* GName){
    int i;
    if(strlen(GName) > 24){
        return ERROR;
    }
    for(i = 0; i<strlen(GName); i++){
        if(!(isdigit(GName[i]) || isalpha(GName[i]) || GName[i] == '-' || GName[i] == '_')){
            return ERROR;
        }
    }
    return OK;
}
int parseFileName(char* filename){
    int i;
    int filesize = strlen(filename);
    if(filesize > 24){
        return ERROR;
    }
    for(i = 0; i<filesize; i++){
        if(i < filesize - 4){
            if(!(isdigit(filename[i]) || isalpha(filename[i]) || filename[i] == '-' || filename[i] == '_')){
            return ERROR;
            }
        }else{
            if((i == filesize - 4) & filename[i] != '.'){
                return ERROR;
            } else if(!isalpha(filename[i])) {
                return ERROR;
            }
        }
        
    }
    return OK;
}
int parseCommand(char* command){
    return OK;
}

bool existingUser(char* UID){
    if(parseUID(UID) == ERROR){
        setvbuf (stdout, NULL, _IONBF, 0);
        return false;
    }

    /* for (auto const &pair: Users) {
        printf("%s", pair.first.c_str());
    } */
    
    map<string,User>::iterator it;
    string uidc(UID);
    for (it = Users.begin(); it != Users.end(); it++){
        /* printf("iterator uid: %s\n", it->first.c_str()); */
        if(it->first.compare(uidc) == 0){
            return true;
        }
    }
    return false;
}

int verifyUser(char* UID, char* pass){
    bool valid = false;
    if(parseUID(UID) == ERROR || parsePass(pass) == ERROR){
        return ERROR;
    }
    map<string,User>::iterator it;
    if(!Users.empty()) {
        string uidc(UID);
        for (it = Users.begin(); it != Users.end(); it++){
            /* printf("iterator uid: %s\n", it->first.c_str()); */
            if(it->first.compare(uidc) == 0){
                valid = true;
            }
        }
    }
    if(valid){
        if(strcmp(Users[UID].pass,pass) != 0){
            return ERROR;
        }
    }
    else{
        return ERROR;
    }
    return OK;
}

void comRegister(char*command, char*message){
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], pass[MAX_COMMAND], reply[MAX_COMMAND], status[MAX_COMMAND];
    sscanf(command,"%s %s %s", commandName, UID, pass);

    strcpy(status,"OK");
    if(parseUID(UID) == ERROR){
        strcpy(status,"NOK");
    }
    else if(existingUser(UID)){
        strcpy(status,"DUP");
    }
    else if(parsePass(pass) == ERROR){
        strcpy(status,"NOK");
    }
    else{
        User user1;
        strcpy(user1.UID, UID);
        strcpy(user1.pass, pass);
        string uidc(UID);
        Users[uidc] = user1;
    }

    sprintf(message, "RRG %s\n", status);
}

void comUnregister(char*command, char* message){
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], pass[MAX_COMMAND], reply[MAX_COMMAND], status[MAX_COMMAND];
    sscanf(command,"%s %s %s", commandName, UID, pass);
    if(verifyUser(UID,pass) == OK){
        strcpy(status,"OK");
        Users.erase(UID);
    }
    else{
        strcpy(status,"NOK");
    }

    sprintf(message, "RUN %s\n", status);
}

void comLogin(char*command, char* message){
    setvbuf (stdout, NULL, _IONBF, 0);
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], pass[MAX_COMMAND], reply[MAX_COMMAND], status[MAX_COMMAND];
    sscanf(command,"%s %s %s", commandName, UID, pass);
    if(!existingUser(UID)) {
        strcpy(status,"NOK");
    }
    if(verifyUser(UID,pass) == OK){
        strcpy(status,"OK");
        string uidc(UID);
        Users[uidc].loggedin = true;
    } else {
        strcpy(status,"NOK");
    }

    sprintf(message, "RLO %s\n", status);
}

void comLogout(char*command, char* message){
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], pass[MAX_COMMAND], reply[MAX_COMMAND], status[MAX_COMMAND];
    sscanf(command,"%s %s %s", commandName, UID, pass);
    if(verifyUser(UID,pass) == OK){
        strcpy(status,"OK");
        string uidc(UID);
        Users[uidc].loggedin = false;
    }
    else{
        strcpy(status,"NOK");
    }

    sprintf(message, "ROU %s\n", status);
}

void comGroups(char*command, char* message){
    int n = Groups.size();
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], reply[MAX_COMMAND], status[MAX_COMMAND*n];
    sscanf(command,"%s %s", commandName, UID);
    string uidc(UID);
    if (n == 0){
        strcpy(status, "0");
    }
    else {
        sprintf(status, "%ld", Groups.size());

        map<string,Group>::iterator it;
        string uidc(UID);
        char message_aux[MAX_COMMAND], mid[5];
        for (it = Groups.begin(); it != Groups.end(); it++){
            /* printf("iterator gid: %s\n", it->first.c_str()); */
            if(it->second.currentMID == 0){
                sprintf(mid, "%s", "0000");
            } else {
                sprintf(mid, "%d", Groups[it->first].currentMID);
            }
            sprintf(message_aux, " %s %s %s", Groups[it->first].GID, Groups[it->first].GName, mid);
            strcat(status, message_aux);
        }
    }


    sprintf(message, "RGL %s\n", status);
}

void comSubscribe(char*command, char* message){
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], GID[3], GName[MAX_COMMAND], reply[MAX_COMMAND], status[MAX_COMMAND];
    sscanf(command,"%s %s %s %s", commandName, UID, GID, GName);
    if(parseUID(UID) == ERROR){
        strcpy(status,"E_USR");
    }
    else if(parseGID(GID) == ERROR){
        strcpy(status,"E_GRP");
    }
    else if(parseGName(GName) == ERROR){
        strcpy(status,"E_GNAME");
    }
    else if(strcmp(GID,"00") == 0){
        if(currentGID == 99){
            strcpy(status,"E_FULL");
        }
        else{
            // new group
            Group group1;
            int n = Groups.size();
            n++;
            if(n < 10){ //has 1 digit
                sprintf(GID,"0%d", n);
            }
            strcpy(group1.GID, GID);
            strcpy(group1.GName, GName);
            string gidc(GID);
            Groups[gidc] = group1;
            string uidc(UID);
            Users[uidc].myGroups[gidc] = group1;
            sprintf(status,"NEW %s", GID);
            //strncpy(Users[uidc].GIDS[n++], GID, 2);
        }
    }
    else{
        strcpy(status,"OK");
        string uidc(UID);
        int n = Users[uidc].nGIDS++;
        string gidc(GID);
        Group group1 = Groups[gidc];
        Users[uidc].myGroups[gidc] = group1;
    }

    sprintf(message, "RGS %s\n", status);
}

void comUnsubscribe(char*command, char* message){
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], GID[MAX_COMMAND], reply[MAX_COMMAND], status[MAX_COMMAND];
    sscanf(command,"%s %s %s", commandName, UID, GID);
    if(parseUID(UID) == ERROR){
        strcpy(status,"E_USR");
    }
    else if(parseGID(GID) == ERROR){
        strcpy(status,"E_GRP");
    }
    
    else{
        strcpy(status,"OK");
        string uidc(UID);
        Users[uidc].nGIDS--;
        string gidc(GID);
        Users[uidc].myGroups.erase(gidc);
    }

    sprintf(message, "RGU %s\n", status);
}

void comMyGroups(char*command, char* message){
    
    char commandName[MAX_COMMAND], UID[MAX_COMMAND];
    sscanf(command,"%s %s", commandName, UID);
    string uidc(UID);
    int n = Users[uidc].myGroups.size();

    char reply[MAX_COMMAND], status[MAX_COMMAND*n*5];
    if(parseUID(UID) == ERROR || !Users[UID].loggedin){
        strcpy(status,"E_USR");
    } else if (n == 0){
        strcpy(status, "0");
    } else {
        sprintf(status, "%d", n);

        map<string,Group>::iterator it;
        string uidc(UID);
        char message_aux[MAX_COMMAND], mid[5];
        for (it = Users[uidc].myGroups.begin(); it != Users[uidc].myGroups.end(); it++){
            /* printf("iterator gid: %s\n", it->first.c_str()); */
            if(it->second.currentMID == 0){
                sprintf(mid, "%s", "0000");
            } else {
                sprintf(mid, "%d",Users[uidc].myGroups[it->first].currentMID);
            }
            sprintf(message_aux, " %s %s %s", Users[uidc].myGroups[it->first].GID, Users[uidc].myGroups[it->first].GName, mid);
            strcat(status, message_aux);
        }
    }

    sprintf(message, "RGM %s\n", status);
}

// ulist
void comUList(char*command, char* message) {
    ;
}

// post
void comPost(char*command, char* message) {
    ;
}

// retrieve
void comRetrieve(char*command, char* message) {
    ;
}