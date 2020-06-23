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
    static SOCKET ListenSocket;
    static std::string const DEFAULT_PORT;

    static std::mutex MutexUpload;

    static std::vector<User*> UserList;
    static std::vector<User*> OnlineUserList;
    static std::vector<std::string> FileNameList;

    static std::string const DATABASE_PATH;
    static std::string const  SHARED_FILES_FOLDER;
    static std::string const LOG_FILE;
    static std::string const SHARED_FILE_NAMES_FILE;
    static std::string const USERS_FILE;
    // User structure in file: USERNAME_LEN | USERNAME | PWD_LEN | PWD

    static std::string LastError;

private:
    static void initDatabase();
    static void initWinsock();

    static void initListenSocket();
    static void acceptConnections();

    static void transmitMsg(User* user);

    static void sendMsg(User* user);
    static void receiveMsg(User* user);

    static void sendAFileToClient(std::string const& indexFile_str, User* user);
};
