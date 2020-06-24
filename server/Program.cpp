#include "Program.h"

Program::Program()
{
	//this->test();
	FixSizeWindow(110, 30);
	FixConsoleWindow();

	/* GUI */
	line_1 = line_2 = line_3 = 2;
	selected = SELECTED::ONLINE;

	//Init something you need
	this->InitDataBaseDirectory();
	this->InitUserList();
	this->InitFileNameList();
	this->initWinsock();
}

Program::~Program()
{
	closesocket(this->ListenSocket);

	for (size_t i = 0; i < this->UserList.size(); ++i) {
		delete this->UserList[i];
		this->UserList[i] = nullptr;
	}

	for (int i = 0; i < FileNameList.size(); i++) {
		delete& this->FileNameList[i];
	}

	// When the client application is completed using the Windows Sockets DLL, the WSACleanup function is called to release resources.
	WSACleanup();
}

void Program::InitDataBaseDirectory() {
	if (CreateDirectory(s2ws(DATABASE_PATH).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		CreateDirectory(s2ws(DATABASE_PATH + "\\" + SHARED_FILES_FOLDER).c_str(), NULL);
	}
	else return;
}

void Program::InitUserList() {
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

void Program::InitFileNameList() {
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
		this->LastError = "WSAStartup() failed with error: " + iResult;
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

	iResult = getaddrinfo(nullptr, (LPCSTR)&this->DEFAULT_PORT, &hints, &result);	// Update 'result' with port, IP address,...
	if (iResult != 0) {
		this->LastError = "getaddrinfo() failed with error: " + iResult;
		return;
	}

	this->ListenSocket = INVALID_SOCKET;

	this->ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (this->ListenSocket == INVALID_SOCKET) {
		this->LastError = "socket() failed with error: " + WSAGetLastError();
		freeaddrinfo(result);
		return;
	}

	// Bind the socket which has already been created (ListenSocket) to an IP address and port (result->ai_addr: the first sockadrr in the linked list result).
	// (The IP address and port are retrieved from the variable 'result', which has been updated after the execution of the function 'getaddrinfo'.)
	iResult = bind(this->ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		this->LastError = "bind() failed with error: " + WSAGetLastError();
		freeaddrinfo(result);
		return;
	}

	// Now, we don't need 'result' anymore.
	freeaddrinfo(result);

	// Let the socket which has already bind (ListenSocket) listen to all connections from Clients.
	iResult = listen(this->ListenSocket, SOMAXCONN);	// backlog = SOMAXCONN: the maximum length of the queue of pending connections to accept.
	if (iResult == SOCKET_ERROR) {
		this->LastError = "listen() failed with error: " + WSAGetLastError();
		return;
	}
}

void Program::acceptConnections()
{
	while (true) {
		SOCKET acceptSocket = INVALID_SOCKET;

		acceptSocket = accept(this->ListenSocket, nullptr, nullptr);

		if (acceptSocket == INVALID_SOCKET) {
			this->LastError = "accept() failed: " + WSAGetLastError();
			continue;
		}

		User* user = new User(acceptSocket);

		std::thread receiveMsgThread(&Program::receiveMsg, this, user);
	}
}

void Program::receiveMsg(User* user)
{
	/* Message structure: FLAG (1 byte) | MSGLEN (64 byte) | MSG */

	int iResult;

	RcvMsgFlag flag;
	uint64_t msgLen;
	char* msg;

	while (true) {
		// FLAG
		iResult = recv(user->AcceptSocket, (char*)&flag, sizeof(flag), 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}

		// Check if the Client (user) shutdowns
		if (iResult == 0)
			break;

		// MSGLEN
		iResult = recv(user->AcceptSocket, (char*)&msgLen, sizeof(msgLen), 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}
		msg = new char[msgLen + 1];

		// MSG
		iResult = recv(user->AcceptSocket, msg, msgLen, 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}
		msg[msgLen] = '\0';

		switch (flag)
		{
		case RcvMsgFlag::REGISTER:
			// ...
			break;
		case RcvMsgFlag::LOGIN:
			// ...
			break;
		case RcvMsgFlag::PASSWORD:
			// ...
			break;
		case RcvMsgFlag::UPLOAD_FILE:
			// ...
			break;
		case RcvMsgFlag::DOWNLOAD_FILE:
			this->sendAFileToClient(msg, user);
			break;
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

void Program::sendAFileToClient(std::string const& indexFile_str, User* user)
{
	size_t indexFile = stoi(indexFile_str);

	SendMsgFlag flag = SendMsgFlag::DOWNLOAD_FILE;

	std::ifstream fin(this->getPathOfAFile(indexFile), std::ios_base::binary);

	if (fin.is_open()) {
		int iResult;
		size_t fileSize;
		char* buffer = new char[this->BUFFER_LEN];

		fin.seekg(std::ios_base::end);
		fileSize = fin.gcount();

		// Send file's size
		iResult = send(user->AcceptSocket, (char*)&fileSize, sizeof(fileSize), 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "send() failed with error: " + WSAGetLastError();
			return;
		}

		// Send file's data
		for (size_t i = 0; i < fileSize / this->BUFFER_LEN; ++i) {
			fin.read(buffer, this->BUFFER_LEN);
			iResult = send(user->AcceptSocket, buffer, this->BUFFER_LEN, 0);
			if (iResult == SOCKET_ERROR) {
				this->LastError = "send() failed with error: " + WSAGetLastError();
				return;
			}
		}

		fin.read(buffer, fileSize % this->BUFFER_LEN);
		iResult = send(user->AcceptSocket, buffer, fileSize % this->BUFFER_LEN, 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "send() failed with error: " + WSAGetLastError();
			return;
		}

		delete[] buffer;
		fin.close();
	}
}

std::string Program::getPathOfAFile(size_t const& indexFile)
{
	return this->DATABASE_PATH + "\\" + this->SHARED_FILES_FOLDER + "\\" + this->FileNameList[indexFile];
}

unsigned long Program::fileSizeBytes(string filename) {
	ifstream f(DATABASE_PATH + "\\" + SHARED_FILES_FOLDER + "\\" + filename,
		std::fstream::ate | std::fstream::binary);

	if (!f.is_open()) {
		return 0;
	}

	return f.tellg();
}

void Program::run()
{
	this->homeScreen(); 
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
	printSpace(11); cout << "Clients"; printSpace(11); cout << "|"; printSpace(8); cout << "File Uploaded"; printSpace(8); cout << "|"; printSpace(20); cout << "History Log"; printSpace(19);

	printStatus();

	setColor(COLOR::WHITE, COLOR::BLACK);

	for (int i = 0; i < UserList.size(); i++) {
		printClient(UserList[i]->Username, false);
	}

	for (int i = 0; i < FileNameList.size(); i++) {
		printFiles(FileNameList[i]);
	}

	printLog();

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
	if (filename.length() > 20) {
		string str1 = filename.substr(0, 10);
		string str2 = filename.substr(filename.length() - 7, 7);
		filename = str1 + "..." + str2;
	}

	return filename;
}

void Program::printLog() {
	time_t now = time(0);
	tm* ltm = localtime(&now);

	gotoXY(61, line_3);

	// [hh:mm]
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	if (ltm->tm_hour < 10) cout << "[0" << ltm->tm_hour;
	else cout << "[" << ltm->tm_hour;
	if (ltm->tm_min < 10) cout << ":0" << ltm->tm_min << "] ";
	else cout << ":" << ltm->tm_min << "] ";

	// Content
	setColor(COLOR::WHITE, COLOR::BLACK);
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
