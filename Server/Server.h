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
    SOCKET static ListenSocket;
    std::string const static DEFAULT_PORT;

    std::mutex static MutexUpload;

    std::vector<User*> static UserList;
    std::vector<User*> static OnlineUserList;
    std::vector<std::string> static FileNameList;

    std::string const static DATABASE_PATH;
    std::string const static  SHARED_FILES_FOLDER;
    std::string const static LOG_FILE;
    std::string const static SHARED_FILE_NAMES_FILE;
    std::string const static USERS_FILE;
    // User structure in file: USERNAME_LEN | USERNAME | PWD_LEN | PWD

    std::string static LastError;

private:
    void static initDatabase();
    void static initWinsock();
    void static initListenSocket();
    
    void static acceptConnections();

    void static transmitMsg(User* user);

    bool static sendMsg(User* user);
    bool static receiveMsg(User* user);

    bool static sendFileToClient(std::string const& indexFile_str, User* user);
};
