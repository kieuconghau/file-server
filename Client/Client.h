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

enum class SendMsgFlag : uint8_t
{
    REGISTER,
    LOGIN,
    PASSWORD,
    UPLOAD_FILE,
    DOWNLOAD_FILE,
    LOGOUT
};

enum class RcvMsgFlag : uint8_t
{
    FAIL,
    SUCCESS,
    DOWNLOAD_FILE,
    LOGOUT
};

class Client
{
public:
    Client();
    ~Client();

    void run();

private:
    string DatabasePath;            // .../Database/
    string LogFile;                 // logfile.txt
private:

};

