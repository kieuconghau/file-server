#pragma once

#include "Console.h"

enum class SELECTED {
	ONLINE = 0,
	YES,
	NO
};

struct User {
	string Username;
	string Password;
	SOCKET Socket;
};

class Program
{
public:
	

	Program();
	~Program();

	void run();

private:
	SOCKET ListenSocket;
	std::string const DEFAULT_PORT = "27015";

	//std::mutex MutexUpload;

	std::vector<User*> UserList;
	std::vector<User*> OnlineUserList;
	std::vector<std::string> FileNameList;

	/* ================ GUI ================ */
	SELECTED selected;
	unsigned int line_1; // line of Columm: Client
	unsigned int line_2; // line of Columm : File Uploaded
	unsigned int line_3; // line of Columm : History Log

	/* ================ PATH ================ */
	std::string const DATABASE_PATH = "Sever_Database";    // .../Database/
	std::string const SHARED_FILES_FOLDER = "SharedFiles";       // SharedFiles/
	std::string const LOG_FILE = "logfile.txt";       // logfile.txt
	std::string const SHARED_FILE_NAMES_FILE = "filename.bin";      // filename.bin
	std::string const USER_FILE = "user.bin";          // user.bin
	
	std::string LastError;
private:
	void InitDataBaseDirectory();
	void InitUserList();
	void InitFileNameList();
	unsigned long fileSizeBytes(string filename);
	string toStringFileSize(string filename);
	string shortenFileName(string filename);

	void homeScreen();
	//void homeNavigate();
	void printStatus();
	void navigateStatus();
	void printFiles(string filename);
	void printLog();
	void printClient(string user, bool login);

	void test();
};
