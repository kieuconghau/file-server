#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

struct User {
	string Username;
	string Password;
	SOCKET Socket;
};

class Server {
public:
	Server();
	int run();
private:
	SOCKET AcceptSocketList[10];
	size_t SocketCount = 0;

	SOCKET ListenSocket;
	vector<User*> OnlineUserList;
	vector<User*> UserList;
	vector<string> FileNameList;

	string DatabasePath;            // .../Database/
	string SharedFilesFolder;       // SharedFiles/
	string LogFile;                 // logfile.txt
	string SharedFilesNameFile;     // filename.txt
	string UsersFile;               // user.bin
	// User: USERNAMELEN | USERNAME | PWDLEN | PWD
private:
	void loadUserAccountInfo();
	void verifyUserRegistrationOrLogin(SOCKET socket);
	void addUserAccountInfo(string username, string password, SOCKET socket);
};
