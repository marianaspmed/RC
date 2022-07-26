#include <string>

using namespace std;
void parseArgs(int argc, char *argv[]);
int parseCommand(char*command);
int parseUID(char*UID);
int parsePass(char*password);
int parseGID(char*GID);
int parseFileName(char* filename);
int parseMID(char*MID);

char*getRequest(char*command);

int comRegister(char*command);
int comUnregister(char*command);
int comLogin(char*command);
int comLogout(char*command);
int comExit(char*command);
int comGroups(char*command);
int comSubscribe(char*command);
int comUnsubscribe(char*command);
int comMyGroups(char*command);
int comSelect(char*command);
int comShowGid(char*command);
int comUList(char*command);
int comPost(char*command);
int comRetrieve(char*command);
