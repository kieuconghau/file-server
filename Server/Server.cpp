#include "Server.h"

Server::Server() {
	DatabasePath = "";
	UsersFile = "user.bin";
}

int Server::run() {
	WSAData wsaData;

	int iResult;

	// Initialize Winsock
	WORD ver = MAKEWORD(2, 2);					// version 2.2
	iResult = WSAStartup(ver, &wsaData);		// initiate use of WS2_32.dll

	if (iResult != 0) {
		printf("WSAStartup() failed: %d\n", iResult);
		return 1;
	}

	// Create a socket for the server
	ADDRINFOA* result = NULL;
	ADDRINFOA hints;

	ZeroMemory(&hints, sizeof(hints));		// Fill a block of memory (hints) with zeros
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);		// result is the pointer pointing to the first ADDRINFO in the linked list

	if (iResult != 0) {
		printf("getaddrinfo() failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Bind the socket which has already been created (ListenSocket) to an IP address and port
	iResult = ::bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	// Listen on a socket
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("listen() failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Load user account info
	this->loadUserAccountInfo();

	vector<thread> registerUserThreadList;
	int threadCount = 0;

	// Accept connections from clients
	while (true) {
		SOCKET AcceptSocket = INVALID_SOCKET;

		AcceptSocket = accept(ListenSocket, NULL, NULL);

		if (AcceptSocket == INVALID_SOCKET) {
			printf("accept() failed: %d\n", WSAGetLastError());
			continue;
		}

		registerUserThreadList.push_back(thread(&Server::verifyUserRegistrationOrLogin, this, AcceptSocket));
		threadCount++;
		//HANDLE sendAndRcvMsgThread = CreateThread(NULL, 0, sendAndRcvMsg, &AcceptSocketList[SocketCount], 0, NULL);
	}

	// Close the Listen socket
	closesocket(ListenSocket);

	// Clean up
	WSACleanup();

	return 0;
}

void Server::loadUserAccountInfo() {
	ifstream fin;
	fin.open(DatabasePath + UsersFile, ios::binary);
	if (fin.is_open() == false) {
		cout << "Failed to open user info file to read" << endl;
		return;
	}

	fin.seekg(0, ios::end);
	int fileLength = fin.tellg();
	fin.seekg(0, ios::beg);

	uint8_t fieldLength;
	string field;
	while (fileLength > 0) {
		User* pUser = new User;

		getline(fin, field, '\0');
		pUser->Username = field;

		getline(fin, field, '\0');
		pUser->Password = field;

		UserList.push_back(pUser);

		fileLength -= fin.tellg();
	}

	fin.close();
}

void Server::verifyUserRegistrationOrLogin(SOCKET socket) {
	string username;
	string password;
	uint8_t flag; // Placeholder
	uint64_t msgLen;
	char* msg;
	uint8_t mode;
	bool result = true;

	recv(socket, (char*)&flag, sizeof(flag), 0);
	mode = flag;
	recv(socket, (char*)&msgLen, sizeof(msgLen), 0);
	msg = new char[msgLen + 1];
	memset(msg, 0, msgLen + 1);
	recv(socket, (char*)msg, msgLen, 0);
	username = msg;
	delete[] msg;

	recv(socket, (char*)&flag, sizeof(flag), 0);
	if (flag != 2) return;
	recv(socket, (char*)&msgLen, sizeof(msgLen), 0);
	msg = new char[msgLen + 1];
	memset(msg, 0, msgLen + 1);
	recv(socket, (char*)msg, msgLen, 0);
	password = msg;
	delete[] msg;

	switch (mode) {
	case 0:	// Placeholder	// Registration
	{
		result = true;
		for (size_t i = 0; i < UserList.size(); i++) {
			if (username == UserList[i]->Username) {
				result = false;
			}
		}

		if (result == true) {	// Registration succeeded
			addUserAccountInfo(username, password, socket);
			this->AcceptSocketList[SocketCount] = socket;
			++this->SocketCount;

			flag = 1;
			send(socket, (char*)&flag, sizeof(flag), 0);

			cout << "Socket " << socket << " registered succeessfully!" << endl;
			cout << "Username: " << username << endl;
		}
		else {	// Registration failed
			flag = 0;
			send(socket, (char*)&flag, sizeof(flag), 0);
			cout << "Socket " << socket << " failed to register" << endl;
		}

		break;
	}
	case 1:	// Placeholder	// Login
		break;
	default:
		cout << "Illegal flag. Something went wrong..." << endl;
	}
}

void Server::addUserAccountInfo(string username, string password, SOCKET socket) {
	ofstream fout;
	fout.open(DatabasePath + UsersFile, ios::app | ios::binary);
	if (fout.is_open() == false) {
		cout << "Failed to open user info file to write" << endl;
		return;
	}

	User* pUser = new User;
	pUser->Username = username;
	pUser->Password = password;
	pUser->Socket = socket;
	UserList.push_back(pUser);
	OnlineUserList.push_back(pUser);

	fout.seekp(0, ios::end);
	cout << "File size before writing: " << fout.tellp() << endl;

	fout.write((char*)username.c_str(), username.length() + 1);
	fout.write((char*)password.c_str(), password.length() + 1);

	fout.close();
}
