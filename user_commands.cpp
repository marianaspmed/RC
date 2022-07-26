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
#define PORT "58023"
#define MAX_COMMAND 50
#define ERROR 1
#define OK 0


char DSIP[MAX_COMMAND];
char DSport[MAX_COMMAND];
int fd,errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints,*res;
struct sockaddr_in addr;
char buffer[128];

#define MAX_COMMAND 50
#define TEXT_MAX_SIZE 240
#define ERROR 1
#define OK 0

using namespace std;

char ActiveGID[2];
char ActiveUID[10];
char ActivePass[MAX_COMMAND];
bool loggedin = false, selectedGID = false;

int getNumWords(char* command);
int getFileSize(char* Fname);
char* readFile(char* Fname);

void parseArgs(int argc,char *argv[]){
    char flag1[MAX_COMMAND], flag2[MAX_COMMAND], arg1[MAX_COMMAND], arg2[MAX_COMMAND];
    strcpy(DSport,PORT);
    /* printf("argc: %d\n", argc);
    for(int i = 0; i< argc;i++)
        printf("arg %d: %s\n", i, argv[i]); */
    if(argc == 3){
        if(strcmp(argv[1], "-n") == 0)
            strcpy(DSIP, argv[2]);
        if(strcmp(argv[1], "-p") == 0)
            strcpy(DSport,argv[2]);
    }
    if(argc == 5){
        if(strcmp(argv[1], "-n") == 0)
            strcpy(DSIP, argv[2]);
        if(strcmp(argv[3], "-p") == 0)
            strcpy(DSport,argv[4]);
    }
    printf("Port: %s\n", DSport);
    return;
}

int UDP(char* message, char* reply){
    fd=socket(AF_INET,SOCK_DGRAM,0);
    if(fd==-1) /*error*/return(ERROR);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;

    errcode=getaddrinfo(NULL,DSport,&hints,&res) ;
    if(errcode!=0) /*error*/ return(ERROR);

    n = sendto(fd,message,strlen(message),0,res->ai_addr,res->ai_addrlen);
    if(n==-1) /*error*/ return(ERROR);

    addrlen=sizeof(addr);
    n = recvfrom (fd,reply,MAX_COMMAND*10,0, (struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ return(ERROR);

    //write(1,"echo: ",6); write(1,buffer,n);
    freeaddrinfo(res);
    close (fd);
    return OK;
}

int TCP(char* message, char* reply){
    ssize_t nleft,nwritten,nread, nbytes;
    char*ptr;
    int fd, n;
    fd=socket(AF_INET,SOCK_STREAM,0);   //TCP socket
    if (fd==-1) return(ERROR); //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM;      //TCP socket

    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt",DSport,&hints,&res);
    if(errcode!=0)/*error*/return(ERROR);

    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        /*error*/return(ERROR);
    }

    ptr=strcpy(buffer, message);

    nbytes = strlen(message);

    nleft = nbytes;
    while (nleft > 0)
    {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0) 
            exit(1);
        nleft -= nwritten;
        ptr += nwritten;
    }
    nbytes = 128;
    nleft = nbytes;
    ptr = &reply[0];
    while ((nread = read(fd, ptr, nleft)) != 0)
    {
        if (nread == -1) 
            exit(1);
        nleft -= nread;
        ptr += nread;
    }
    
    /* printf("Reply in tcp: %s", reply); */

    /* write(1,"echo: ",6);  */
    write(1,reply,nread);
    freeaddrinfo(res);
    close(fd);
    return OK;
}

int parseCommand(char* command){
    return OK;
}
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
int parseMID(char* MID);

int comRegister(char*command){
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], pass[MAX_COMMAND], message[MAX_COMMAND * 3], reply[MAX_COMMAND];
    sscanf(command,"%s %s %s", commandName, UID, pass);
    if(parseUID(UID) == ERROR){
        return ERROR;
    }
    if(parsePass(pass) == ERROR){
        return ERROR;
    }
    sprintf(message, "REG %s %s\n", UID, pass);
    printf("%s", message);

    UDP(message,reply);
    printf("Reply: %s", reply);

    return 0;
}
int comUnregister(char* command){
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], pass[MAX_COMMAND], message[MAX_COMMAND * 3], reply[MAX_COMMAND*10];
    sscanf(command,"%s %s %s", commandName, UID, pass);
    if(parseUID(UID) == ERROR){
        return ERROR;
    }
    if(parsePass(pass) == ERROR){
        return ERROR;
    }
    sprintf(message, "UNR %s %s\n", UID, pass);
    printf("%s", message);

    UDP(message,reply);
    printf("Reply: %s", reply);
    return OK;
}
int comLogin(char* command){
    char commandName[MAX_COMMAND], UID[MAX_COMMAND], pass[MAX_COMMAND], message[MAX_COMMAND * 3], reply[MAX_COMMAND];
    sscanf(command,"%s %s %s", commandName, UID, pass);
    if(parseUID(UID) == ERROR){
        return ERROR;
    }
    if(parsePass(pass) == ERROR){
        return ERROR;
    }
    sprintf(message, "LOG %s %s\n", UID, pass);
    printf("%s", message);

    UDP(message,reply);
    printf("Reply: %s", reply);
    strcpy(ActiveUID,UID);
    strcpy(ActivePass,pass);
    loggedin = true;
    return OK;
}
int comLogout(char* command){
    char commandName[MAX_COMMAND], message[MAX_COMMAND * 3], reply[MAX_COMMAND];
    sscanf(command,"%s", commandName);
    if(!loggedin){
        return ERROR;
    }

    sprintf(message, "OUT %s %s\n", ActiveUID, ActivePass);
    printf("%s", message);

    UDP(message,reply);
    printf("Reply: %s", reply);
    loggedin = false;
    return OK;
}
int comExit(char* command){
    exit(0);
}
int comGroups(char* command){
    char message[MAX_COMMAND * 3], reply[MAX_COMMAND*10];
    sprintf(message, "GLS\n");
    printf("%s", message);

    UDP(message,reply);
    printf("Reply: %s", reply);
    return OK;
}
int comSubscribe(char* command){
    char commandName[MAX_COMMAND], GID[MAX_COMMAND], 
    GName[MAX_COMMAND], message[MAX_COMMAND * 3], reply[MAX_COMMAND*10];
    sscanf(command,"%s %s %s", commandName, GID, GName);
    if(!loggedin){
        return ERROR;
    }
    if(parseGID(GID) == ERROR){
        return ERROR;
    }
    if(parseGName(GName) == ERROR){
        return ERROR;
    }
    sprintf(message, "GSR %s %s %s\n", ActiveUID, GID, GName);
    printf("%s", message);

    UDP(message,reply);
    printf("Reply: %s", reply);
    return OK;
}
int comUnsubscribe(char* command){
    char commandName[MAX_COMMAND], GID[MAX_COMMAND], 
    message[MAX_COMMAND * 3], reply[MAX_COMMAND*10];
    sscanf(command,"%s %s", commandName, GID);
    if(!loggedin){
        return ERROR;
    }
    if(parseGID(GID) == ERROR){
        return ERROR;
    }
    sprintf(message, "GUR %s %s\n", ActiveUID, GID);
    printf("%s", message);

    UDP(message,reply);
    printf("Reply: %s", reply);
    return OK;
}
int comMyGroups(char* command){
    char commandName[MAX_COMMAND], message[MAX_COMMAND * 3], reply[MAX_COMMAND*20];
    if(!loggedin){
        return ERROR;
    }
    sprintf(message, "GLM %s\n", ActiveUID);
    printf("%s", message);

    UDP(message,reply);
    printf("Reply: %s", reply);
    return OK;
}
int comSelect(char* command){
    char commandName[MAX_COMMAND], GID[MAX_COMMAND];
    sscanf(command,"%s %s", commandName, GID);
    if(!loggedin){
        return ERROR;
    }
    if(parseGID(GID) == ERROR){
        return ERROR;
    }
    strcpy(ActiveGID, GID);
    
    printf("ActiveGID = %s\n", ActiveGID);
    selectedGID = true;
    return OK;
}
int comShowGid(char*command){
    if(!loggedin){
        return ERROR;
    }
    printf("GID = %s", ActiveGID);
    return OK;
}
int comUList(char* command){
    char commandName[MAX_COMMAND],  message[MAX_COMMAND*3], reply[MAX_COMMAND*3];
    if(!loggedin){
        return ERROR;
    }
    sprintf(message, "ULS %s\n", ActiveGID);
    printf("%s", message);

    TCP(message,reply);
    printf("Reply: %s", reply);
    return OK;
}
int comPost(char* command){
    char commandName[MAX_COMMAND], text[TEXT_MAX_SIZE], message[MAX_COMMAND], 
    reply[MAX_COMMAND], Fname[24];
    int Tsize = 0, count = 0;

    count = getNumWords(command);
    printf("Number of arguments = %d\n", count);
    
    if(!loggedin){
        return ERROR;   
    }
    if(!selectedGID){
        return ERROR;
    }

    char *messageToPost = text;
    if(count == 2){
        /* Remove "" from text message */
        sscanf(command,"%s \"%240[^\"]\"", commandName, text);
        Tsize = strlen(text);
        sprintf(message, "PST %s %s %d %s\n", ActiveUID, ActiveGID, Tsize, messageToPost);
    } else if(count == 3){
        sscanf(command,"%s \"%240[^\"]\" %s", commandName, text, Fname);
        Tsize = strlen(text);
        if(parseFileName(Fname) == ERROR){
            return ERROR;
        }
        sprintf(message, "PST %s %s %d %s %s %d %s\n", ActiveUID, ActiveGID, Tsize, messageToPost, Fname, getFileSize(Fname), readFile(Fname));
    } else {
        return -1;
    }
    
    printf("%s", message);
    TCP(message,reply);
    printf("Reply: %s", reply);
    return 0;
}

int getNumWords(char* command){
    /* Get number of arguments */
    int count = 0;
    for (int i = 0;command[i] != '\0';i++){
        if (command[i] == ' ' && command[i+1] != ' '){
            count++;  
        } else if (command[i] == '\"' && command[i+1] != '\"'){
            for (i = i + 1; command[i] != '\"'; i++){
                ;
            }
            count++;
        }                         
    }
    return count;
}

char* readFile(char* Fname){
    FILE *fp;
    fp = fopen(Fname, "rb");
    if (fp == NULL){
        return NULL;
    }
    int Fsize = getFileSize(Fname);
    /* Read from file */
    char * buffer = (char*)malloc(sizeof(char)*(Fsize + 128));
    fread(buffer,Fsize,1,fp);
    /* printf("Buffer: %s\n",buffer); */
    fclose(fp);
    buffer[Fsize] = '\n';
    return buffer;
}

int getFileSize(char* Fname){
    FILE *fp;
    fp = fopen(Fname, "rb");
    fseek(fp, 0, SEEK_END);
    int Fsize = ftell(fp);
    fclose(fp);
    return Fsize;
}

int comRetrieve(char* command){
    if(!loggedin){
        return ERROR;
    }
    if(!selectedGID){
        return ERROR;
    }
    return 0;
}
