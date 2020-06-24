#pragma once

#include "User.h"

#include <vector>
#include <thread>
#include <mutex>

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

class Server
{
public:
    Server();
    ~Server();

    void run();

private:
    SOCKET ListenSocket;
    std::string const DEFAULT_PORT = "27015";   // ...

    std::mutex MutexUpload;

    std::vector<User*> UserList;
    std::vector<User*> OnlineUserList;
    std::vector<std::string> FileNameList;

    std::string const DATABASE_PATH = "Database/";
    std::string const  SHARED_FILES_FOLDER = "SharedFiles/";
    std::string const LOG_FILE = "serverlog.txt";
    std::string const SHARED_FILE_NAMES_FILE = "sharedfilenames.txt";
    std::string const USERS_FILE = "users.bin";
    // User structure in file: USERNAME_LEN | USERNAME | PWD_LEN | PWD

    std::string LastError;

private:
    void initDatabase();
    void initWinsock();

    void initListenSocket();
    void acceptConnections();

    void transmitMsg(User* user);

    void sendMsg(User* user);
    void receiveMsg(User* user);

    void sendAFileToClient(std::string const& indexFile_str, User* user);
};
