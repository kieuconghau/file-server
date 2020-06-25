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

	LPCSTR ServerIP;
	LPCSTR const SERVER_PORT = "27015";

	std::vector<File> FileList;

	uint16_t const BUFFER_LEN = 4096;

	std::string const DATABASE_PATH = "Client_Database";
	std::string const DOWNLOAD_FOLDER = "Download";			// default
	std::string const LOG_FILE = "logfile.txt";

	std::string LastError;

	/* ================ GUI ================ */
	int          line_2;	// line of Columm : File Uploaded
	int          line_3;	// line of Columm : History Log
	SELECTED     selected;	// selected STATE

private:
	// Init
	void initDataBaseDirectory();
	void initFileList();

	// Set up Client
	void initWinsock();
	void initConnectSocket();
	
	// Transfer
	void receiveMsg();
	void sendMsg(SendMsgFlag const& flag, const char* msg, uint64_t const& msgLen);
	
	int receiveData(char* buffer, uint64_t const& len);
	int sendData(const char* buffer, uint64_t const& len);

	// Register
	void registerAccount();

	// Download File
	void sendADownloadFileRequest(uint64_t const& fileIndex);
	void receiveADownloadFileReply(std::string const& downloadedFilePath);

	// Upload File
	void uploadFile(std::string const& uploadedFilePath);
	std::string getFileNameFromPath(std::string const& path);

	// Handle error
	void printLastError();

	// ...
	void homeScreen();

	void printTitle();
	void printFile(string name, string size, bool selected);

	string shortenFileName(string filename);
	string shortenFileSize(unsigned long size);
	void printLog(string gui, string log);
	void printLog(SendMsgFlag flag);
	void printLog(RcvMsgFlag flag);

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
	void printProgressBar(float percentage);
};

