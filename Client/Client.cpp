#include "Client.h"

SOCKET Client::ConnectSocket;

std::string Client::ServerIP;
std::string Client::ServerPort;

std::string const Client::DATABASE_PATH = "Database/";
std::string const Client::LOG_FILE = "logfile.txt";

std::string Client::LastError;

Client::Client()
{
	Client::initDatabase();
	Client::initWinsock();
}

Client::~Client()
{
}

void Client::run()
{
	Client::initConnectSocket();
}

void Client::initDatabase()
{
	// ...
}

void Client::initWinsock()
{
	WSADATA wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		Client::LastError = "WSAStartup() failed with error: " + iResult;
	}
}

void Client::initConnectSocket()
{
	struct addrinfo* result = nullptr;	// A pointer to a linked list of one or more addrinfo structures that contains response information about the host.
	struct addrinfo hints;
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo((PCSTR)&Client::ServerIP, (PCSTR)&Client::ServerPort, &hints, &result);	// Update 'result' with port, IP address,...

	if (iResult != 0) {
		Client::LastError = "getaddrinfo() failed: " + iResult;
		return;
	}

	Client::ConnectSocket = INVALID_SOCKET;

	for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		Client::ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		
		if (Client::ConnectSocket == INVALID_SOCKET) {
			Client::LastError = "socket() failed with error: " + WSAGetLastError();
			return;
		}

		iResult = connect(Client::ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		
		if (iResult == SOCKET_ERROR) {
			closesocket(Client::ConnectSocket);
			Client::ConnectSocket = INVALID_SOCKET;
			continue;
		}

		break;
	}

	if (Client::ConnectSocket == INVALID_SOCKET) {
		Client::LastError = "Error at socket(): " + WSAGetLastError();
	}

	freeaddrinfo(result);
}

void Client::transmitMsg()
{
	std::thread sendMsgThread(Client::sendMsg);
	std::thread receiveMsgThread(Client::receiveMsg);

	sendMsgThread.join();
	receiveMsgThread.join();
	
	// How to pass this while(true)?
	// ...
	while (true);
}

void Client::sendMsg()
{
	// ...
}

void Client::receiveMsg()
{
	/* Message structure: FLAG (1 byte) | MSGLEN (64 byte) | MSG */

	int iResult;

	RcvMsgFlag flag;
	uint64_t msgLen;
	char* msg;

	while (true) {
		// Client shutdown check
		iResult = recv(Client::ConnectSocket, nullptr, 0, 0);
		if (iResult == 0)
			break;

		// FLAG
		iResult = recv(Client::ConnectSocket, (char*)&flag, sizeof(flag), 0);
		if (iResult == SOCKET_ERROR) {
			Client::LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}

		// MSGLEN
		iResult = recv(Client::ConnectSocket, (char*)&msgLen, sizeof(msgLen), 0);
		if (iResult == SOCKET_ERROR) {
			Client::LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}
		msg = new char[msgLen + 1];

		// MSG
		iResult = recv(Client::ConnectSocket, msg, msgLen, 0);
		if (iResult == SOCKET_ERROR) {
			Client::LastError = "recv() failed with error: " + WSAGetLastError();
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
			Client::receiveAFileFromServer();
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

void Client::receiveAFileFromServer()
{
	// ...
}

