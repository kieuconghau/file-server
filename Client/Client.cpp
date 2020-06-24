#include "Client.h"

Client::Client()
{
	this->initDatabase();
	this->initWinsock();
}

Client::~Client()
{
}

void Client::run()
{
	this->initConnectSocket();
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
		this->LastError = "WSAStartup() failed with error: " + iResult;
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

void Client::receiveMsg()
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
			this->receiveAFileFromServer();
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

void Client::sendADownloadFileRequest(size_t const& fileIndex)
{
	/* Message structure: FLAG (uint8_t) | MSGLEN (uint64_t) | MSG (uint64_t) */

	int iResult;

	SendMsgFlag flag;
	uint64_t msgLen;
	uint64_t msg;

	flag = SendMsgFlag::DOWNLOAD_FILE;
	msgLen = sizeof(msg);
	msg = fileIndex;

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

	iResult = send(this->UserInfo.ConnectSocket, (char*)&msg, msgLen, 0);
	if (iResult == SOCKET_ERROR) {
		this->LastError = "send() failed with error: " + WSAGetLastError();
		return;
	}
}

void Client::receiveAFileFromServer()
{
	// ...
}

