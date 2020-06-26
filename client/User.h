#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class User
{
    friend class Program;
public:
    User();
    User(SOCKET const& connectSocket);
    ~User();

private:
    std::string Username;
    std::string Password;
    SOCKET ConnectSocket;
};

