#pragma once

#include "Console.h"

struct File {
	string fileName;
	string fileSize;
};

struct User {
	string Username;
	string Password;
};

enum class SELECTED {
	REGISTER = 0,
	LOGIN,
	UPLOAD,
	DOWNLOAD,
	ESC,
	YES,
	NO
};

class Program
{
public:
	Program();
	~Program();
	void run();

	/* Init for test */
	void InitFileList();

private:
	SOCKET ConnectSocket;

	std::string ServerIP;
	std::string ServerPort;

	std::vector<File> FileList;

	std::string const DATABASE_PATH = "Client_Database";
	std::string const LOG_FILE = "logfile.txt";

	std::string LastError;

	/* ================ GUI ================ */
	int          line_2;	// line of Columm : File Uploaded
	int          line_3;	// line of Columm : History Log
	SELECTED     selected;	// selected STATE



	User         UserInfo;		// Client info
	vector<File> list;		// List file

private:
	/* ================ Init ================ */
	void InitDataBaseDirectory();



	void homeScreen();

	void printTitle();
	void printFile(string name, string size, bool selected);
	void printLog(string content);

	/* ==== Client Register/Login ==== */
	void buttonClient();
	void printClient();
	void navigateClient();
	void loginClient();
	/* =============================== */

	void printMode();
	void navigateMode();
	string enterPath();

	void printStatus();
};
