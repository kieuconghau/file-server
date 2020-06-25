#include "Program.h"

Program::Program()
{
	//this->test();
	FixSizeWindow(110, 30);
	FixConsoleWindow();

	/* GUI */
	line_1 = line_2 = line_3 = line_pb = 2;
	selected = SELECTED::ONLINE;

	//Init something you need
	this->initDataBaseDirectory();
	this->initUserList();
	this->initFileNameList();

	/* LOG */
	fstream f(DATABASE_PATH + "\\" + LOG_FILE, std::fstream::out | std::fstream::app);
	time_t now = time(0);
	tm* ltm = localtime(&now);
	f << "================ Date: " << ltm->tm_mday << " - " << 1 + ltm->tm_mon << " - " << (1900 + ltm->tm_year) << " ================" << endl;
	f.close();
}

Program::~Program()
{
	closesocket(this->ListenSocket);

	for (uint64_t i = 0; i < this->UserList.size(); ++i) {
		delete this->UserList[i];
		this->UserList[i] = nullptr;
	}

	for (int i = 0; i < FileNameList.size(); i++) {		// ???
		delete& this->FileNameList[i];
	}

	// When the client application is completed using the Windows Sockets DLL, the WSACleanup function is called to release resources.
	WSACleanup();
}

void Program::run()
{
	std::thread userInteractThread(&Program::homeScreen, this);
	userInteractThread.detach();

	this->initWinsock();
	this->initListenSocket();
	this->acceptConnections();
}

void Program::initDataBaseDirectory() {
	if (CreateDirectory(s2ws(DATABASE_PATH).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		CreateDirectory(s2ws(DATABASE_PATH + "\\" + SHARED_FILES_FOLDER).c_str(), NULL);
	}
	else return;
}

void Program::initUserList() {
	fstream f(DATABASE_PATH + "\\" + USER_FILE,
		std::fstream::in | std::fstream::binary);

	if (!f.is_open())
		return;

	while (true) {
		User*    temp = new User;
		
		getline(f, temp->Username, '\0');

		if (f.eof()) {
			delete temp;
			break;
		}

		getline(f, temp->Password, '\0');

		this->UserList.push_back(temp);
	}

	f.close();
}

void Program::initFileNameList() {
	fstream f(DATABASE_PATH + "\\" + SHARED_FILE_NAMES_FILE,
		std::fstream::in | std::fstream::binary);

	if (!f.is_open())
		return;

	while (true) {
		string str;

		getline(f, str, '\0');

		if (f.eof()) {
			break;
		}

		string* filename = new string;
		*filename = str;
		this->FileNameList.push_back(*filename);
	}

	f.close();
}

void Program::initWinsock()
{
	WSADATA wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		this->LastError = "WSAStartup() failed with error: " + std::to_string(iResult);
		this->printLastError();
	}
}

void Program::initListenSocket()
{
	// Create a socket for listening to all connections from Clients.
	struct addrinfo* result = nullptr;	// A pointer to a linked list of one or more addrinfo structures that contains response information about the host.
	struct addrinfo hints;
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			// The Internet Protocol version 4 (IPv4) address family.
	hints.ai_socktype = SOCK_STREAM;	// Provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism.
	hints.ai_protocol = IPPROTO_TCP;	// The Transmission Control Protocol (TCP).
	hints.ai_flags = AI_PASSIVE;		// The socket address will be used in a call to the bindfunction.

	iResult = getaddrinfo(nullptr, this->DEFAULT_PORT, &hints, &result);	// Update 'result' with port, IP address,...
	if (iResult != 0) {
		this->LastError = "getaddrinfo() failed with error: " + std::to_string(iResult);
		this->printLastError();
		return;
	}

	this->ListenSocket = INVALID_SOCKET;

	this->ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (this->ListenSocket == INVALID_SOCKET) {
		this->LastError = "socket() failed with error: " + std::to_string(WSAGetLastError());
		this->printLastError();
		freeaddrinfo(result);
		return;
	}

	// Bind the socket which has already been created (ListenSocket) to an IP address and port (result->ai_addr: the first sockadrr in the linked list result).
	// (The IP address and port are retrieved from the variable 'result', which has been updated after the execution of the function 'getaddrinfo'.)
	iResult = bind(this->ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		this->LastError = "bind() failed with error: " + std::to_string(WSAGetLastError());
		this->printLastError();
		freeaddrinfo(result);
		return;
	}

	// Now, we don't need 'result' anymore.
	freeaddrinfo(result);

	// Let the socket which has already bind (ListenSocket) listen to all connections from Clients.
	iResult = listen(this->ListenSocket, SOMAXCONN);	// backlog = SOMAXCONN: the maximum length of the queue of pending connections to accept.
	if (iResult == SOCKET_ERROR) {
		this->LastError = "listen() failed with error: " + std::to_string(WSAGetLastError());
		this->printLastError();
		return;
	}
}

void Program::acceptConnections()
{
	while (true) {
		SOCKET acceptSocket = INVALID_SOCKET;

		acceptSocket = accept(this->ListenSocket, nullptr, nullptr);

		if (acceptSocket == INVALID_SOCKET) {
			this->LastError = "accept() failed: " + std::to_string(WSAGetLastError());
			this->printLastError();
			continue;
		}

		User* user = new User(acceptSocket);

		// The Server is always ready to receive messages from Clients
		std::thread receiveMsgThread(&Program::receiveMsg, this, user);
		receiveMsgThread.detach();
	}
}

void Program::receiveMsg(User* user)
{
	/* Message structure: FLAG (uint8_t) | MSGLEN (uint64_t) | MSG */
	
	int shutdownFlag;

	RcvMsgFlag flag;
	uint64_t msgLen;
	char* msg;

	while (true) {
		shutdownFlag = this->receiveData(user, (char*)&flag, sizeof(flag));
		if (shutdownFlag == 0)	// Check if the Client (user) shutdowns
			break;

		this->receiveData(user, (char*)&msgLen, sizeof(msgLen));
		msg = new char[msgLen + 1];

		this->receiveData(user, msg, msgLen);
		msg[msgLen] = '\0';

		switch (flag)
		{
		case RcvMsgFlag::REGISTER:
			this->verifyUserRegister(user);
			break;
		case RcvMsgFlag::LOGIN:
			this->verifyUserLogin(user);
			break;
		case RcvMsgFlag::PASSWORD:
			// ...
			break;
		case RcvMsgFlag::UPLOAD_FILE: {
			std::string uploadedFileName(msg);

			// Log
			string content = user->Username + string(": ") + string("Request to upload ") + shortenFileName(uploadedFileName) + string(".");
			printLog(content, content);

			std::thread uploadFileThread(&Program::receiveAFileFromClient, this, uploadedFileName, user);
			uploadFileThread.join();	// ...
			break;
		}
		case RcvMsgFlag::DOWNLOAD_FILE: {
			std::string indexFile_str(msg);

			// Log
			string content = user->Username + string(": ") + string("Request to download ") + shortenFileName(FileNameList[stoi(indexFile_str)]) + string(".");
			printLog(content, content);

			std::thread sendFileThread(&Program::sendAFileToClient, this, indexFile_str, user);
			sendFileThread.detach();
			break;
		}
		case RcvMsgFlag::LOGOUT:
			// ...
			break;
		default:
			break;
		}

		delete[] msg;
		msg = nullptr;
	}
}

void Program::sendMsg(User* user, SendMsgFlag const& flag, uint64_t const& msgLen, const char* msg)
{
	this->sendData(user, (char*)&flag, sizeof(flag));
	this->sendData(user, (char*)&msgLen, sizeof(msgLen));
	this->sendData(user, msg, msgLen);
}

int Program::receiveData(User* user, char* buffer, uint64_t const& len)
{
	int iResult;

	iResult = recv(user->AcceptSocket, buffer, len, 0);
	if (iResult == SOCKET_ERROR) {
		this->LastError = "recv() failed with error: " + std::to_string(WSAGetLastError());
		this->printLastError();
	}

	return iResult;
}

int Program::sendData(User* user, const char* buffer, uint64_t const& len)
{
	int iResult;

	iResult = send(user->AcceptSocket, buffer, len, 0);
	if (iResult == SOCKET_ERROR) {
		this->LastError = "send() failed with error: " + std::to_string(WSAGetLastError());
		this->printLastError();
	}
	
	return iResult;
}

void Program::verifyUserRegister(User* user) {
	char* username;
	char* password;
	size_t usernameLen;
	size_t passwordLen;

	receiveData(user, (char*)&usernameLen, sizeof(usernameLen));
	username = new char[usernameLen + 1];
	receiveData(user, username, usernameLen + 1);
	receiveData(user, (char*)&passwordLen, sizeof(passwordLen));
	password = new char[passwordLen + 1];
	receiveData(user, password, passwordLen + 1);

	bool username_existed = false;
	for (size_t i = 0; i < this->UserList.size(); i++) {
		if (username == this->UserList[i]->Username) {
			username_existed = true;
		}
	}

	if (username_existed == true) {
		sendMsg(user, SendMsgFlag::REGISTER_FAIL, 0, NULL);
	}

	sendMsg(user, SendMsgFlag::REGISTER_SUCCESS, 0, NULL);
	this->addNewUser(user);

	cout << "User <" << user->Username << "> registered an account." << endl;	// PRINT LOG

	delete[] username;
	delete[] password;
}

void Program::addNewUser(User* user) {
	// Update UserList
	UserList.push_back(user);

	// Write new user to Server Database
	ofstream serverDatabase;
	serverDatabase.open(this->DATABASE_PATH + "\\" + this->USER_FILE, ios::app | ios::binary);
	if (serverDatabase.is_open() == false) {
		this->LastError = "Failed to open server database file to write";
		this->printLastError();
		return;
	}

	serverDatabase.seekp(0, ios::end);
	serverDatabase.write((char*)user->Username.c_str(), user->Username.length() + 1);
	serverDatabase.write((char*)user->Password.c_str(), user->Password.length() + 1);

	serverDatabase.close();
}

void Program::verifyUserLogin(User* user) {
	char* username;
	char* password;
	uint8_t usernameLen;
	uint8_t passwordLen;

	receiveData(user, (char*)&usernameLen, sizeof(usernameLen));
	username = new char[usernameLen + 1];
	receiveData(user, username, usernameLen + 1);
	receiveData(user, (char*)&passwordLen, sizeof(passwordLen));
	password = new char[passwordLen + 1];
	receiveData(user, password, passwordLen + 1);

	int pos;
	for (size_t i = 0; i < this->UserList.size(); i++) {
		if (this->UserList[i]->Username == username) {
			pos = i;
			user->Username = username;
			if (this->UserList[i]->Username == password) {
				user->Password = password;
			}
		}
	}

	if (user->Username == "") {
		sendMsg(user, SendMsgFlag::LOGIN_FAIL_USERNAME, 0, NULL);
		return;
	}
	if (user->Password == "") {
		sendMsg(user, SendMsgFlag::LOGIN_FAIL_PASSWORD, 0, NULL);
		return;
	}

	delete this->UserList[pos];
	this->UserList[pos] = user;
	OnlineUserList.push_back(user);

	sendMsg(user, SendMsgFlag::LOGIN_SUCCESS, 0, NULL);

	// Send the list of shared files to current user...

	// Send new user's info to other clients for them to print log...
	// Send to every online users, except the newly-logged in user,
	// who's at the back of the OnlineUserList vector
	for (size_t i = 0; i < this->OnlineUserList.size() - 1; i++) {
		sendMsg(user, SendMsgFlag::NEW_USER_LOGIN, 0, NULL);

		sendData(OnlineUserList[i], (char*)&usernameLen, sizeof(usernameLen));
		sendData(OnlineUserList[i], username, usernameLen + 1);
	}

	cout << "User <" << user->Username << "> logged in." << endl;	// PRINT LOG

	delete[] username;
	delete[] password;
}

void Program::sendAFileToClient(std::string const& indexFile_str, User* user)
{
	// Send a reply to the Client (user) first
	SendMsgFlag flag = SendMsgFlag::DOWNLOAD_FILE;
	uint64_t msgLen = 0;
	char* msg = nullptr;

	this->sendMsg(user, flag, msgLen, msg);

	// Log
	string content = string("Accept ") + user->Username + string("'s download request.");
	printLog(content, content);

	// Then, send a file to that Client (user)
	std::string filePath = this->getPathOfAFile(stoi(indexFile_str));

	std::ifstream fin(filePath, std::ios_base::binary);

	if (fin.is_open()) {
		int iResult;

		uint64_t fileSize;
		char* buffer = new char[this->BUFFER_LEN];

		// Get file's size
		fin.seekg(0, std::ios_base::end);
		fileSize = fin.tellg();
		fin.seekg(0, std::ios_base::beg);

		// Send file's size
		this->sendData(user, (char*)&fileSize, sizeof(fileSize));

		// Log
		string gui = string("Start sending ") + shortenFileName(FileNameList[stoi(indexFile_str)]) + string(" (") + shortenFileSize(fileSize) + string(").");
		string log = string("Start sending ") + shortenFileName(FileNameList[stoi(indexFile_str)]) + string(" (") + shortenFileSize(fileSize) + string(").");
		printLog(string("To ") + user->Username + string(":"), gui, log);

		ShowConsoleCursor(false);
		line_pb = line_3;
		line_3++;

		printProgressBar(0);

		// Send file's data
		for (uint64_t i = 0; i < fileSize / this->BUFFER_LEN; ++i) {
			fin.read(buffer, this->BUFFER_LEN);
			this->sendData(user, buffer, this->BUFFER_LEN);

			// Progress
			if (i % 10 == 0)
				printProgressBar((i + 1) * this->BUFFER_LEN * 1.0 / fileSize);
		}
		fin.read(buffer, fileSize % this->BUFFER_LEN);
		this->sendData(user, buffer, fileSize % this->BUFFER_LEN);

		printProgressBar(1); // Complete 100%

		ShowConsoleCursor(true);

		// Log
		string gui = string("Send ") + shortenFileName(FileNameList[stoi(indexFile_str)]) + string(" (") + shortenFileSize(fileSize) + string(") succeed.");
		string log = string("Send ") + shortenFileName(FileNameList[stoi(indexFile_str)]) + string(" (") + shortenFileSize(fileSize) + string(") succeed.");
		printLog(string("To ") + user->Username + string(":"), gui, log);

		// Release resources
		delete[] buffer;
		fin.close();
	}
	else {
		this->LastError = "Failed to open file " + filePath;
		this->printLastError();
	}
}

std::string Program::getPathOfAFile(uint64_t const& indexFile)
{
	if (indexFile >= this->FileNameList.size())
		throw "Out of subscript";

	return this->DATABASE_PATH + "\\" + this->SHARED_FILES_FOLDER + "\\" + this->FileNameList[indexFile];
}

void Program::receiveAFileFromClient(std::string const& uploadFileName, User* user)
{
	this->MutexUpload.lock();

	std::ofstream fout(this->DATABASE_PATH + "\\" + this->SHARED_FILES_FOLDER + "\\" + uploadFileName, std::ios_base::binary);

	if (fout.is_open()) {
		int iResult;

		uint64_t fileSize;
		char* buffer = new char[this->BUFFER_LEN];

		// Receive file's size
		this->receiveData(user, (char*)&fileSize, sizeof(fileSize));

		// Log
		string gui = string("Start recieving ") + shortenFileName(uploadFileName) + string(" (") + shortenFileSize(fileSize) + string(").");
		string log = string("Start recieving ") + uploadFileName + string(" (") + shortenFileSize(fileSize) + string(").");
		printLog(string("From ") + user->Username + string(":"), gui, log);

		ShowConsoleCursor(false);
		line_pb = line_3;
		line_3++;

		printProgressBar(0);

		// Receive file's data
		for (uint64_t i = 0; i < fileSize / this->BUFFER_LEN; ++i) {
			this->receiveData(user, buffer, this->BUFFER_LEN);
			fout.write(buffer, this->BUFFER_LEN);

			// Progress
			if (i % 10 == 0)
				printProgressBar((i + 1) * this->BUFFER_LEN * 1.0 / fileSize);
		}

		this->receiveData(user, buffer, fileSize % this->BUFFER_LEN);
		fout.write(buffer, fileSize % this->BUFFER_LEN);

		printProgressBar(1); // Complete 100%

		ShowConsoleCursor(true);

		// Log
		gui = string("Recieve ") + shortenFileName(uploadFileName) + string(" (") + shortenFileSize(fileSize) + string(") succeed.");
		log = string("Recieve") + uploadFileName + string(" (") + shortenFileSize(fileSize) + string(") succeed.");
		printLog(string("From ") + user->Username + string(":"), gui, log);

		// Release resources
		delete[] buffer;
		fout.close();
	}
	else {
		this->LastError = "Unable to create file " + uploadFileName;
		this->printLastError();
	}

	this->MutexUpload.unlock();
}

void Program::printLastError()
{
	cout << this->LastError << "\n";
}

unsigned long Program::fileSizeBytes(string filename) {
	ifstream f(DATABASE_PATH + "\\" + SHARED_FILES_FOLDER + "\\" + filename,
		std::fstream::ate | std::fstream::binary);

	if (!f.is_open()) {
		return 0;
	}

	uint64_t fileSize = f.tellg();
	f.close();

	return fileSize;
}

void Program::printStatus() {
	if (selected == SELECTED::ONLINE) {
		gotoXY(88, 0);

		setColor(COLOR::BLACK, COLOR::LIGHT_GREEN);
		cout << "        ONLINE        ";
		
	}
	else {
		gotoXY(88, 0);


		setColor(COLOR::BLACK, COLOR::DARK_GRAY);
		cout << " DISCONECT ";

		if (selected == SELECTED::YES) {

			setColor(COLOR::BLACK, COLOR::LIGHT_RED);
			cout << " YES ";

			setColor(COLOR::LIGHT_BLUE, COLOR::BLACK);
			cout << "  NO  ";
		}

		if (selected == SELECTED::NO) {

			setColor(COLOR::LIGHT_RED, COLOR::BLACK);
			cout << " YES ";

			setColor(COLOR::BLACK, COLOR::LIGHT_BLUE);
			cout << "  NO  ";
		}
		
	}
	setColor(COLOR::WHITE, COLOR::BLACK);
}

void Program::navigateStatus() {
	bool esc = false;
	while (true) {

		if (_kbhit()) {
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			// ============= LEFT =============
			if ((GetKeyState(VK_LEFT) & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000)) {
				if (selected != SELECTED::ONLINE) {
					selected = (selected == SELECTED::YES) ? SELECTED::NO : SELECTED::YES;
					this->printStatus();
				}
			}

			// ============= ESC =============
			if (GetKeyState(VK_ESCAPE) & 0x8000) {
				if (selected == SELECTED::ONLINE) {
					selected = SELECTED::NO;
					this->printStatus();
				}
			}

			// ============= ENTER =============
			if (GetKeyState(VK_RETURN) & 0x8000) {
				if (selected == SELECTED::YES) {
					// SOCKET DISCONECT

					esc = true;
				}
				else {
					selected = SELECTED::ONLINE;
					this->printStatus();
				}
			}
		}
		
		if (esc) break;
	}
}

void Program::homeScreen() {
	string name = "abc.xyz";
	int size = 4096;
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	gotoXY(0, 0);
	printTextAtMid("[SERVER]", 0, maxWIDTH);

	gotoXY(0, 1);
	setColor(COLOR::BLACK, COLOR::LIGHT_GRAY);
	printSpace(11); cout << "Clients"; printSpace(11); cout << "|"; printSpace(8); cout << " File Shared "; printSpace(8); cout << "|"; printSpace(20); cout << "History Log"; printSpace(19);

	printStatus();

	setColor(COLOR::WHITE, COLOR::BLACK);

	for (int i = 0; i < UserList.size(); i++) {
		printClient(UserList[i]->Username, false);
	}

	for (int i = 0; i < FileNameList.size(); i++) {
		printFiles(FileNameList[i]);
	}

	navigateStatus();
}

void Program::printClient(string user, bool login) {
	if (login) {
		gotoXY(1, line_1);
		setColor(COLOR::WHITE, COLOR::BLACK);
		cout << user; printSpace(27 - user.length() - 5);
		setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
		cout << "[ONL]";
	}
	else {
		gotoXY(1, line_1);
		setColor(COLOR::DARK_GRAY, COLOR::BLACK);
		cout << user; printSpace(27 - user.length() - 5);
		setColor(COLOR::RED, COLOR::BLACK);
		cout << "[OFF]";
	}
	line_1++;
}

void Program::printFiles(string filename) {
	string size = toStringFileSize(filename);
	filename = shortenFileName(filename);

	gotoXY(30, line_2);

	setColor(COLOR::WHITE, COLOR::BLACK);

	cout << " " << filename;
	printSpace(27 - filename.length() - size.length()); 
	cout << size;

	line_2++;
}

string Program::toStringFileSize(string filename) {
	unsigned long size = fileSizeBytes(filename);
	uint8_t d = 0;

	while (size > 1000) {
		size /= 1000;
		d++;
	}

	string parameter[] = { " B", "KB", "MB", "GB" };
	
	return numCommas(size) + " " + parameter[d];
}

string Program::shortenFileName(string filename) {
	if (filename.length() > 18) {
		string str1 = filename.substr(0, 9);
		string str2 = filename.substr(filename.length() - 6, 6);
		filename = str1 + "..." + str2;
	}

	return filename;
}

string Program::shortenFileSize(unsigned long size) {
	uint8_t d = 0;

	while (size > 1000) {
		size /= 1000;
		d++;
	}

	string parameter[] = { " B", "KB", "MB", "GB" };

	return numCommas(size) + " " + parameter[d];
}

void Program::printLog(string gui, string log) {
	fstream f(DATABASE_PATH + "\\" + LOG_FILE, std::fstream::out | std::fstream::app);
	time_t now = time(0);
	tm* ltm = localtime(&now);
	string timeline;

	gotoXY(61, line_3);

	// [hh:mm]
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	if (ltm->tm_hour < 10) timeline = string("[0") + numCommas(ltm->tm_hour);
	else timeline = string("[") + numCommas(ltm->tm_hour);
	if (ltm->tm_min < 10) timeline += string(":0") + numCommas(ltm->tm_min) + string("] ");
	else timeline += string(":") + numCommas(ltm->tm_min) + string("] ");

	cout << timeline;
	f << timeline;

	// Content
	setColor(COLOR::WHITE, COLOR::BLACK);
	cout << gui;
	f << log << endl;

	line_3++;
	f.close();
}

void Program::printLog(string user, string gui, string log) {
	fstream f(DATABASE_PATH + "\\" + LOG_FILE, std::fstream::out | std::fstream::app);
	time_t now = time(0);
	tm* ltm = localtime(&now);
	string timeline;

	gotoXY(61, line_3);

	// [hh:mm]
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	if (ltm->tm_hour < 10) timeline = string("[0") + numCommas(ltm->tm_hour);
	else timeline = string("[") + numCommas(ltm->tm_hour);
	if (ltm->tm_min < 10) timeline += string(":0") + numCommas(ltm->tm_min) + string("] ");
	else timeline += string(":") + numCommas(ltm->tm_min) + string("] ");

	cout << timeline;
	f << timeline;

	// Content
	setColor(COLOR::WHITE, COLOR::BLACK);
	cout << user; line_3++;
	gotoXY(69, line_3);
	cout << gui;

	f << user << endl << log << endl;

	line_3++;
	f.close();
}

void Program::test() {
	fstream f(DATABASE_PATH + "\\" + SHARED_FILE_NAMES_FILE, 
		std::fstream::in | std::fstream::out | std::fstream::app | std::fstream::binary);

	string str1 = "thelongfiletestnametest1.txt";
	string str2 = "filetest2.txt";

	f.write(str1.c_str(), str1.size());
	f.write("\0", sizeof(char));
	f.write(str2.c_str(), str2.size());
	f.write("\0", sizeof(char));

	f.close();
}

void Program::updateClient(string Username, bool login) {
	unsigned int idx;

	for (int i = 0; i < UserList.size(); i++) {
		if (Username.compare(UserList[i]->Username) == 0) {
			idx = i;
			break;
		}
	}

	line_1 = idx + 2;
	printClient(Username, login);
}

void Program::printProgressBar(float percentage) {
	int val = (int)(percentage * 100);
	int width = (int)(percentage * 29);

	gotoXY(69, line_pb);
	cout << "Progress ";

	gotoXY(78, line_pb);
	cout << "[                             ]";

	setColor(COLOR::DARK_GRAY, COLOR::BLACK);
	gotoXY(79, line_pb);
	for (int i = 0; i < width; i++) {
		cout << "|";
	}

	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	string per = numCommas(val) + "%";
	gotoXY((int)((79 + 108 - per.length()) / 2), line_pb);
	cout << per;

	setColor(COLOR::WHITE, COLOR::BLACK);
}