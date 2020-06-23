#include "Client.h"

int Client::run() {
	WSAData wsaData;

	int iResult;

	// Initialize Winsock
	WORD ver = MAKEWORD(2, 2);		// version 2.2
	iResult = WSAStartup(ver, &wsaData);		// initiate use of WS2_32.dll

	if (iResult != 0) {
		printf("WSAStartup() failed: %d\n", iResult);
		return 1;
	}

	// Create a socket for the server
	ADDRINFOA* result = NULL;
	ADDRINFOA hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);		// result is the pointer pointing to the first ADDRINFO in the linked list

	if (iResult != 0) {
		printf("getaddrinfo() failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to an address until one succeeds
	for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to the Server!\n");
		WSACleanup();
		return 1;
	}

	this->Socket = ConnectSocket;

	this->registerAccount();

	//HANDLE sendMsgThread;
	//HANDLE rcvMsgThread;

	//sendMsgThread = CreateThread(NULL, 0, sendMsg, &ConnectSocket, 0, NULL);
	//rcvMsgThread = CreateThread(NULL, 0, rcvMsg, &ConnectSocket, 0, NULL);

	while (true);

	// Close the Connect socket
	iResult = shutdown(ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ConnectSocket);

	// Clean up
	WSACleanup();

	return 0;
}

void Client::registerAccount() {
	string username;
	string password;
	uint8_t flag; // Placeholder
	uint64_t msgLen;
	char* msg;
	bool result = false;

	while (result == false) {
		cout << "Username: ";	getline(cin, username);
		cout << "Password: ";	getline(cin, password);

		// Send account info for registration
		flag = 0; // Placeholder
		send(this->Socket, (char*)&flag, sizeof(flag), 0);
		msgLen = username.length();
		send(this->Socket, (char*)&msgLen, sizeof(msgLen), 0);
		send(this->Socket, (char*)username.c_str(), msgLen, 0);
		flag = 2; // Placeholder
		send(this->Socket, (char*)&flag, sizeof(flag), 0);
		msgLen = password.length();
		send(this->Socket, (char*)&msgLen, sizeof(msgLen), 0);
		send(this->Socket, (char*)password.c_str(), msgLen, 0);

		// Receive server's response
		recv(this->Socket, (char*)&flag, sizeof(flag), 0);

		switch (flag) {
		case 0:	// Placeholder	// Registration failed
		{
			recv(this->Socket, (char*)&msgLen, sizeof(msgLen), 0);
			msg = new char[msgLen + 1];
			memset(msg, 0, msgLen + 1);
			recv(this->Socket, (char*)msg, msgLen, 0);
			cout << "Registration failed." << endl;
			cout << msg << endl;
			delete[] msg;
			break;
		}
		case 1:	// Placeholder	// Registration succeeded
		{
			cout << "Registration succeeded." << endl;
			result = true;
			break;
		}
		default:
			cout << "Illegal flag. Something went wrong..." << endl;
		}
	}
}
