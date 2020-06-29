#pragma once

#include "Console.h"
#include "User.h"
#include "File.h"
#include <mutex>

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
	PASSWORD,	// unused
	UPLOAD_FILE,
	DOWNLOAD_FILE,
	LOGOUT_CLIENT,
	LOGOUT_SERVER
};

enum class RcvMsgFlag : uint8_t
{
	REGISTER_FAIL = 0,
	REGISTER_SUCCESS,
	LOGIN_FAIL_USERNAME,
	LOGIN_FAIL_PASSWORD,
	LOGIN_FAIL_ONLINE,
	LOGIN_SUCCESS,
	UPLOAD_FILE_FAIL,
	UPLOAD_FILE_SUCCESS,
	DOWNLOAD_FILE_SUCCESS,
	NEW_USER_LOGIN,
	NEW_FILE_LIST,
	NEW_FILE,
	LOGOUT_CLIENT,
	LOGOUT_SERVER,
	CLIENT_LOGOUT_NOTIF
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
	std::string LastUploadedFilePath;
	std::string dirDownFolder;

	bool ExitFlag;

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
	bool initConnectSocket();
	
	// Transfer
	void receiveMsg();
	void sendMsg(SendMsgFlag const& flag, const char* msg, uint64_t const& msgLen);
	
	int receiveData(char* buffer, uint64_t const& len);
	int sendData(const char* buffer, uint64_t const& len);

	// Register
	void tryRegister();

	// Login
	void tryLogin();

	// Write log for new user's login
	void writeLogNewLogin();

	// Download File
	void sendADownloadFileRequest(uint64_t const& fileIndex);
	void receiveADownloadFileReply(std::string const& downloadedFilePath);

	// Upload File
	void sendAnUploadFileRequest(std::string const& uploadedFilePath);
	void uploadFile(std::string const& uploadedFilePath);
	void updateSharedFileList(std::string const& newFileContent);
	std::string getFileNameFromPath(std::string const& path);

	// Init File List
	void initSharedFileList(std::string const& initFileContent);

	// Logout - Client
	void sendALogoutRequest();
	void receiveALogoutReply();

	// Logout - Server
	void sendALogoutReply();

	// Handle error
	void printLastError();

	// GUI
	void homeScreen();

	void printTitle();
	void printFile(string name, string size, bool selected);

	string shortenFileName(string filename);
	string shortenFileSize(unsigned long size);
	void printLog(string gui, string log);
	void printLog(string gui_1, string gui_2, string log);

	/* ==== Client Register/Login ==== */
	void buttonClient();	// no mutex
	void printIP();
	void printClient();
	bool navigateClient();
	bool inputIP();
	void loginClient();
	/* =============================== */

	void printMode();
	void navigateMode();
	string enterPath();
	bool isFilePathExist(const std::string& name);
	bool dirExists(const std::string& dirName_in);

	void printStatus();
	void printProgressBar(float percentage);	// no mutex
};

