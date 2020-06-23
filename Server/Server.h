#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

enum class SendMsgFlag : uint8_t
{
    FAIL,
    SUCCESS,
    DOWNLOAD_FILE,
    LOGOUT
};

enum class RcvMsgFlag : uint8_t
{
    REGISTER,
    LOGIN,
    PASSWORD,
    UPLOAD_FILE,
    DOWNLOAD_FILE,
    LOGOUT
};

struct User
{
    string Username;
    string Password;
    SOCKET AcceptSocket;
};

class Server
{
public:
    
    void run();

private:
    SOCKET ListenSocket;
    
    mutex MutexUpload;

    vector<User*> OnlineUserList;
    vector<User*> UserList;
    vector<string> FileNameList;

    static string const DatabasePath;            // .../Database/
    static string const SharedFilesFolder;       // SharedFiles/
    static string const LogFile;                 // serverlog.txt
    static string const SharedFilesNameFile;     // sharedfilenames.txt
    static string const UsersFile;               // users.bin
                                                 // User: USERNAMELEN | USERNAME | PWDLEN | PWD
private:
    void initialize();
    
    bool receiveMsg(User const& user);

    bool sendFileToClient(string const& indexFile_str, User const& user);
};
