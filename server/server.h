#include <vector>
#include <string>
#include <map>

#define MAX_NAME 1000
#define MAX_PASS 8

using namespace std;

typedef struct Message {
    string _text;
    string _filename;
    bool _read = false;
} Message;

typedef struct Group {
    int _key;
	string _name;
    map<int,int> _usersIds;
    vector<Message> _messages;
} Group;

typedef struct User {
    int _id;
    string _pass[MAX_PASS];
    map<int,int> _groupIds;
} User;