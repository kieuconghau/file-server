#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_IP "127.0.0.1"

class Client {
public:
	int run();
private:
	SOCKET Socket;

	string DatabasePath;            // .../Database/
	string LogFile;                 // logfile.txt
private:
	void registerAccount();
};
