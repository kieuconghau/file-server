#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

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
    static SOCKET ConnectSocket;

    static std::string ServerIP;
    static std::string ServerPort;

    static std::string const DATABASE_PATH;            // .../Database/
    static std::string const LOG_FILE;                 // logfile.txt

    static std::string LastError;

private:
    static void initDatabase();
    static void initWinsock();
    
    static void initConnectSocket();

    static void transmitMsg();

    static void sendMsg();
    static void receiveMsg();

    static void receiveAFileFromServer();
};

