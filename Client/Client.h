#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "User.h"
#include "File.h"

#include <vector>
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
    User UserInfo;

    std::string ServerIP;
    std::string ServerPort;

    std::vector<File> FileList;

    std::string const DATABASE_PATH = "Client_Database";
    std::string const LOG_FILE = "logfile.txt";

    std::string LastError;

private:
    void initDatabase();
    void initWinsock();
    
    void initConnectSocket();

    void receiveMsg();

    void sendADownloadFileRequest(size_t const& fileIndex);
    void receiveAFileFromServer();
};

