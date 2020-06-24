#include "Program.h"

void Program::InitFileList() {
	
}

void Program::InitDataBaseDirectory() {
	if (CreateDirectory(s2ws(DATABASE_PATH).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		
	}
	else return;
}

Program::Program()
{
	FixSizeWindow(110, 30);
	FixConsoleWindow();

	/* GUI */
	this->selected = SELECTED::REGISTER;
	this->line_2 = 0;
	this->line_3 = 2;

	//Init something you need
	this->InitDataBaseDirectory();
	this->InitFileList();
	this->initWinsock();
}

Program::~Program()
{
	//Del things you init
}

void Program::run()
{
	this->homeScreen();
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

void Program::initConnectSocket()
{
	struct addrinfo* result = nullptr;	// A pointer to a linked list of one or more addrinfo structures that contains response information about the host.
	struct addrinfo hints;
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo((PCSTR)&this->ServerIP, (PCSTR)&this->ServerPort, &hints, &result);	// Update 'result' with port, IP address,...

	if (iResult != 0) {
		this->LastError = "getaddrinfo() failed: " + iResult;
		return;
	}

	this->UserInfo.ConnectSocket = INVALID_SOCKET;

	for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		this->UserInfo.ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (this->UserInfo.ConnectSocket == INVALID_SOCKET) {
			this->LastError = "socket() failed with error: " + WSAGetLastError();
			return;
		}

		iResult = connect(this->UserInfo.ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR) {
			closesocket(this->UserInfo.ConnectSocket);
			this->UserInfo.ConnectSocket = INVALID_SOCKET;
			continue;
		}

		break;
	}

	if (this->UserInfo.ConnectSocket == INVALID_SOCKET) {
		this->LastError = "Error at socket(): " + WSAGetLastError();
	}

	freeaddrinfo(result);
}

void Program::receiveMsg()
{
	/* Message structure: FLAG (uint8_t) | MSGLEN (uint64_t) | MSG (uint8_t*) */

	int iResult;

	RcvMsgFlag flag;
	uint64_t msgLen;
	uint8_t* msg;

	while (true) {
		// FLAG
		iResult = recv(this->UserInfo.ConnectSocket, (char*)&flag, sizeof(flag), 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}

		// Check if the Server shutdowns
		if (iResult == 0)
			break;

		// MSGLEN
		iResult = recv(this->UserInfo.ConnectSocket, (char*)&msgLen, sizeof(msgLen), 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}
		msg = new uint8_t[msgLen + 1];

		// MSG
		iResult = recv(this->UserInfo.ConnectSocket, (char*)msg, msgLen, 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}
		msg[msgLen] = '\0';

		switch (flag)
		{
		case RcvMsgFlag::FAIL:
			// ...
			break;
		case RcvMsgFlag::SUCCESS:
			// ...
			break;
		case RcvMsgFlag::DOWNLOAD_FILE:
			
			this->receiveAFileFromServer("G:/KCH/");
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

void Program::sendADownloadFileRequest(size_t const& fileIndex)
{
	/* Message structure: FLAG (uint8_t) | MSGLEN (uint64_t) | MSG (string) */

	int iResult;

	SendMsgFlag flag;
	uint64_t msgLen;
	std::string msg;

	flag = SendMsgFlag::DOWNLOAD_FILE;
	msgLen = sizeof(msg);
	msg = std::to_string(fileIndex);

	iResult = send(this->UserInfo.ConnectSocket, (char*)&flag, sizeof(flag), 0);
	if (iResult == SOCKET_ERROR) {
		this->LastError = "send() failed with error: " + WSAGetLastError();
		return;
	}

	iResult = send(this->UserInfo.ConnectSocket, (char*)&msgLen, sizeof(msgLen), 0);
	if (iResult == SOCKET_ERROR) {
		this->LastError = "send() failed with error: " + WSAGetLastError();
		return;
	}

	iResult = send(this->UserInfo.ConnectSocket, msg.c_str(), msgLen, 0);
	if (iResult == SOCKET_ERROR) {
		this->LastError = "send() failed with error: " + WSAGetLastError();
		return;
	}
}

void Program::receiveAFileFromServer(std::string const& downloadPath)
{
	std::ofstream fout(downloadPath, std::ios_base::binary);

	if (fout.is_open()) {
		int iResult;
		
		size_t fileSize;
		char* buffer = new char[this->BUFFER_LEN];

		// Receive file's size
		iResult = recv(this->UserInfo.ConnectSocket, (char*)&fileSize, sizeof(fileSize), 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}
		
		// Receive file's data
		for (size_t i = 0; i < fileSize / this->BUFFER_LEN; ++i) {
			fout.write(buffer, this->BUFFER_LEN);
			iResult = send(this->UserInfo.ConnectSocket, buffer, this->BUFFER_LEN, 0);
			if (iResult == SOCKET_ERROR) {
				this->LastError = "recv() failed with error: " + WSAGetLastError();
				return;
			}
		}

		fout.write(buffer, fileSize % this->BUFFER_LEN);
		iResult = send(this->UserInfo.ConnectSocket, buffer, fileSize % this->BUFFER_LEN, 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}

		delete[] buffer;
		fout.close();
	}
	else {
		this->LastError = "Unable to open file " + downloadPath;
	}
}


void Program::homeScreen() {
	this->printTitle();
	this->printMode();
	this->printClient();
	this->printStatus();
	this->navigateClient();

	for (int i = 0; i < FileList.size(); i++) {
		printFile(FileList[i].fileName, FileList[i].fileSize, false);
		line_2++;
	}
	line_2 = 2;
	
	
	
	this->navigateMode();
}

void Program::printMode() {
	if (selected == SELECTED::DOWNLOAD) {
		setColor(COLOR::BLACK, COLOR::DARK_GRAY);
		gotoXY(30, 0);
		cout << " DOWNLOAD ";
	}
	else {
		setColor(COLOR::BLACK, COLOR::LIGHT_CYAN);
		gotoXY(30, 0);
		cout << "  UPLOAD  ";
	}
}

void Program::navigateMode() {
	selected = SELECTED::UPLOAD;
	bool esc = false;

	while (true) {

		if (_kbhit()) {
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			// ============= LEFT/RIGHT =============
			if ((GetKeyState(VK_LEFT) & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000)) {

				if ((selected == SELECTED::UPLOAD) || (selected == SELECTED::DOWNLOAD)) {
					selected = (selected == SELECTED::UPLOAD) ? SELECTED::DOWNLOAD : SELECTED::UPLOAD;
					this->printMode();

					if (FileList.size() > 0) {
						printFile(FileList[0].fileName, FileList[0].fileSize, false); // reset the previous line back to normal
						line_2 = 0;									  // reset line back to the top of FileList
						if (selected == SELECTED::DOWNLOAD) {
							printFile(FileList[0].fileName, FileList[0].fileSize, true);
						}
					}
				}
				

				if ((selected == SELECTED::YES) || (selected == SELECTED::NO)) {
					selected = (selected == SELECTED::YES) ? SELECTED::NO : SELECTED::YES;
					this->printStatus();
				}
			}

			// ============= UP =============
			if (GetKeyState(VK_UP) & 0x8000) {
				if (selected == SELECTED::DOWNLOAD) {
					if (FileList.size() > 0) {
						printFile(FileList[line_2].fileName, FileList[line_2].fileSize, false); // reset the previous line back to normal
						line_2--;
						line_2 += (line_2 < 0) ? FileList.size() : 0;
						printFile(FileList[line_2].fileName, FileList[line_2].fileSize, true);
					}
				}
			}

			// ============= DOWN =============
			if (GetKeyState(VK_DOWN) & 0x8000) {
				if (selected == SELECTED::DOWNLOAD) {
					if (FileList.size() > 0) {
						printFile(FileList[line_2].fileName, FileList[line_2].fileSize, false); // reset the previous line back to normal
						line_2++;
						line_2 %= FileList.size();
						printFile(FileList[line_2].fileName, FileList[line_2].fileSize, true);
					}
				}
			}

			// ============= ENTER =============
			if (GetKeyState(VK_RETURN) & 0x8000) {

				if (selected == SELECTED::UPLOAD) {
					this->enterPath();
					// CSocket UPLOAD file

					// If success or fail, do something
				}

				if (selected == SELECTED::DOWNLOAD) {
					if (FileList.size() > 0) {
						this->enterPath();
						//CSocket DOWNLOAD file
					}
				}

				if (selected == SELECTED::YES) {
					esc = true;
				}

				if (selected == SELECTED::NO) {
					selected = SELECTED::UPLOAD;
					printStatus();
				}
			}

			// ============= ESC =============
			if (GetKeyState(VK_ESCAPE) & 0x8000) {
				if (FileList.size() > 0) {
					printFile(FileList[0].fileName, FileList[0].fileSize, false); // reset the previous line back to normal
					line_2 = 0;									  // reset line back to the top of FileList
				}
				selected = SELECTED::UPLOAD;
				this->printMode();

				selected = SELECTED::NO;
				this->printStatus();
			}
		}

		if (esc) break;
	}
}

string Program::enterPath() {
	string path;

	gotoXY(41, 0);
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	cin >> path;

	gotoXY(41, 0); printSpace(69);
	return path;
}

void Program::printTitle() {
	setColor(COLOR::BLACK, COLOR::LIGHT_GRAY);
	gotoXY(30, 1); printSpace(8); cout << "File Uploaded"; printSpace(8); 
	
	setColor(COLOR::BLACK, COLOR::LIGHT_GRAY);
	cout << "|"; 
	
	setColor(COLOR::BLACK, COLOR::LIGHT_GRAY);
	printSpace(20); cout << "History Log"; printSpace(19);


	setColor(COLOR::WHITE, COLOR::BLACK);
}

void Program::printFile(string name, string size, bool selected) {
	gotoXY(30, line_2 + 2);
	 
	if (selected) {
		setColor(COLOR::BLACK, COLOR::LIGHT_BLUE);
	}
	else setColor(COLOR::WHITE, COLOR::BLACK);
	cout << " " << name; printSpace(27 - name.length() - size.length()); cout << size << " ";
}

void Program::printLog(string content) {
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
	cout << content;

	line_3++;
}

void Program::buttonClient() {
	gotoXY(1, 10);
	if (selected == SELECTED::REGISTER) {
		setColor(COLOR::BLACK, COLOR::LIGHT_RED);
	}
	else setColor(COLOR::BLACK, COLOR::DARK_GRAY);
	cout << "  REGISTER  ";

	setColor(COLOR::WHITE, COLOR::BLACK);
	cout << " ";

	if (selected == SELECTED::LOGIN) {
		setColor(COLOR::BLACK, COLOR::LIGHT_RED);
	}
	else setColor(COLOR::BLACK, COLOR::DARK_GRAY);
	cout << "     LOGIN     ";

	setColor(COLOR::BLACK, COLOR::BLACK);
}

void Program::printClient() {
	gotoXY(0, 9); 
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	printTextAtMid("[CLIENT]", 0, 30);

	buttonClient();

	setColor(COLOR::DARK_GRAY, COLOR::BLACK);
	gotoXY(0, 11); cout << "|                            |";
	gotoXY(0, 12); cout << "|                            |";
	gotoXY(0, 13); cout << "|                            |";
	gotoXY(0, 14); cout << "'============================'";

	setColor(COLOR::DARK_GRAY, COLOR::BLACK);
	gotoXY(1, 11); cout << "ServerIP:";
	gotoXY(1, 12); cout << "Username:";
	gotoXY(1, 13); cout << "Password:";

}

void Program::navigateClient() {
	while (true) {

		if (_kbhit()) {
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			// ============= LEFT =============
			if (GetKeyState(VK_LEFT) & 0x8000) {
				selected = (selected == SELECTED::REGISTER) ? SELECTED::LOGIN : SELECTED::REGISTER;
				this->buttonClient();

			}

			// ============= RIGHT =============
			if (GetKeyState(VK_RIGHT) & 0x8000) {
				selected = (selected == SELECTED::REGISTER) ? SELECTED::LOGIN : SELECTED::REGISTER;
				this->buttonClient();
			}

			// ============= ENTER =============
			if (GetKeyState(VK_RETURN) & 0x8000) {

				this->loginClient();

				if (selected == SELECTED::REGISTER) {
					// CSOCKET register
					printLog("Registration success.");
					
					// After Register -> Login
					selected = SELECTED::LOGIN;
					this->buttonClient();
					this->printClient();
					this->loginClient();
				}
				
				if(selected == SELECTED::LOGIN) { 
					// CSOCKET login
					printLog("Login success.");
				}

				printLog("Conected to server.");
				break;
			}
		}

	}
}

void Program::loginClient() {
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK); gotoXY(1, 11); cout << "ServerIP: ";
	setColor(COLOR::WHITE, COLOR::BLACK);					  cin >> this->ServerIP;
	setColor(COLOR::DARK_GRAY, COLOR::BLACK);  gotoXY(1, 11); cout << "ServerIP: ";


	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK); gotoXY(1, 12); cout << "Username: ";
	setColor(COLOR::WHITE, COLOR::BLACK);					  cin >> UserInfo.Username;
	setColor(COLOR::DARK_GRAY, COLOR::BLACK);  gotoXY(1, 12); cout << "Username: ";

	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK); gotoXY(1, 13); cout << "Password: ";
	setColor(COLOR::WHITE, COLOR::BLACK);	                  UserInfo.Password = hidePassword();
	setColor(COLOR::DARK_GRAY, COLOR::BLACK);  gotoXY(1, 13); cout << "Password: ";

	setColor(COLOR::DARK_GRAY, COLOR::BLACK);  gotoXY(0, 14); cout << "'============================'";
}

void Program::printStatus() {
	if ((selected != SELECTED::YES) && (selected != SELECTED::NO)) {
		gotoXY(4, 16);

		setColor(COLOR::BLACK, COLOR::LIGHT_GREEN);
		cout << "        ONLINE        ";

	}
	else {
		gotoXY(4, 16);

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