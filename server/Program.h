#pragma once

#include "Console.h"
#include "User.h"

#pragma comment(lib, "Ws2_32.lib")

enum class SELECTED {
	ONLINE = 0,
	YES,
	NO
};

enum class SendMsgFlag : uint8_t
{
	REGISTER_FAIL = 0,
	REGISTER_SUCCESS,
	LOGIN_FAIL_USERNAME,
	LOGIN_FAIL_PASSWORD,
	LOGIN_SUCCESS,
	UPLOAD_FILE_FAIL,
	UPLOAD_FILE_SUCCESS,
	DOWNLOAD_FILE_SUCCESS,
	NEW_USER_LOGIN,	// unconfirmed by Hau
	NEW_FILE_LIST,	// unconfirmed by Hau
	NEW_FILE,		// unconfirmed
	LOGOUT
};

enum class RcvMsgFlag : uint8_t
{
	REGISTER,
	LOGIN,
	PASSWORD,	// unused
	UPLOAD_FILE,
	DOWNLOAD_FILE,
	LOGOUT
};

class Program
{
public:
	Program();
	~Program();

	void run();

private:
	SOCKET ListenSocket;
	LPCSTR const DEFAULT_PORT = "27015";

	std::mutex MutexUpload;

	std::vector<User*> UserList;
	std::vector<User*> OnlineUserList;
	std::vector<std::string> FileNameList;

	uint16_t const BUFFER_LEN = 4096;

	/* ================ GUI ================ */
	SELECTED selected;
	unsigned int line_1; // line of Columm: Client
	unsigned int line_2; // line of Columm: File Uploaded
	unsigned int line_3; // line of Columm: History Log
	unsigned int line_pb;// line of ProgressBar

	/* ================ PATH ================ */
	std::string const DATABASE_PATH = "Sever_Database";			// .../Server_Database/
	std::string const SHARED_FILES_FOLDER = "SharedFiles";		// SharedFiles/
	std::string const LOG_FILE = "logfile.txt";					// logfile.txt
	std::string const SHARED_FILE_NAMES_FILE = "filename.bin";	// filename.bin
	std::string const USER_FILE = "user.bin";					// user.bin
	
	// filename: filename0\0filename1\0
	// user: username\0password\0

	std::string LastError;

private:
	// Init
	void initDataBaseDirectory();
	void initUserList();
	void initFileNameList();
	
	// Set up Server
	void initWinsock();
	void initListenSocket();
	void acceptConnections();
	
	// Transfer
	void receiveMsg(User* user);
	void sendMsg(User* user, SendMsgFlag const& flag, uint64_t const& msgLen, const char* msg);

	int receiveData(User* user, char* buffer, uint64_t const& len);
	int sendData(User* user, const char* buffer, uint64_t const& len);

	// Register

	void verifyUserRegister(User* user);
	void addNewUser(User* user);

	// Login
	void verifyUserLogin(User* user);

	// Download File
	void sendAFileToClient(std::string const& indexFile_str, User* user);
	std::string getPathOfAFile(uint64_t const& indexFile);

	// Upload File
	void receiveAFileFromClient(std::string const& uploadFileName, User* user);

	// Handle error
	void printLastError();

	// ...
	unsigned long fileSizeBytes(string filename);
	string toStringFileSize(string filename);
	string shortenFileName(string filename);
	string shortenFileSize(unsigned long size);

	void homeScreen();
	//void homeNavigate();
	void printStatus();
	void navigateStatus();
	void printFiles(string filename);

	void printLog(string gui, string log);
	void printLog(string user, string gui, string log);

	void printClient(string user, bool login);
	void updateClient(string Username, bool login);
	void printProgressBar(float percentage);

	void test();
};

