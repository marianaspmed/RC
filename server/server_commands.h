#include <string>

using namespace std;

int parseCommand(char*command);
int parseUID(char*UID);
int parsePass(char*password);
int parseGID(char*GID);
int parseFileName(char* filename);

bool existingUser(char* UID);

int verifyUser(char* UID, char* pass);
void comRegister(char*command, char* message);
void comUnregister(char*command, char* message);
void comLogin(char*command, char* message);
void comLogout(char*command, char* message);
void comExit(char*command, char* message);
void comGroups(char*command, char* message);
void comSubscribe(char*command, char* message);
void comUnsubscribe(char*command, char* message);
void comMyGroups(char*command, char* message);
void comUList(char*command, char* message);
void comPost(char*command, char* message);
void comRetrieve(char*command, char* message);