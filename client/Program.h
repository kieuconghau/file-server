#pragma once

#include "Console.h"
#include "User.h"
#include "File.h"

enum class SELECTED {
	REGISTER = 0,
	LOGIN,
	UPLOAD,
	DOWNLOAD,
	ESC,
	YES,
	NO
};

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

class Program
{
public:
	Program();
	~Program();

	void run();

private:
	User UserInfo;

	std::string ServerIP;
	std::string ServerPort;

	std::vector<File> FileList;

	uint16_t const BUFFER_LEN = 4096;

	std::string const DATABASE_PATH = "Client_Database";
	std::string const LOG_FILE = "logfile.txt";

	std::string LastError;

	/* ================ GUI ================ */
	int          line_2;	// line of Columm : File Uploaded
	int          line_3;	// line of Columm : History Log
	SELECTED     selected;	// selected STATE

private:
	/* ================ Init ================ */
	void initDataBaseDirectory();
	void initFileList();

	// Set up Client
	void initWinsock();
	void initConnectSocket();
	
	// Transfer
	void receiveMsg();
	void sendMsg(SendMsgFlag const& flag, char* msg, uint64_t const& msgLen);
	
	int receiveData(char* buffer, size_t const& len);
	int sendData(char* buffer, size_t const& len);

	// Register
	void registerAccount();

	// Download File
	void downloadFile(size_t const& fileIndex);
	void receiveAFileFromServer(std::string const& downloadPath);

	// Handle error
	void printError();

	// ...
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
