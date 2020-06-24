#include "Server.h"

Server::Server()
{
	this->initDatabase();
	this->initWinsock();
}

Server::~Server()
{
	closesocket(this->ListenSocket);

	for (size_t i = 0; i < this->UserList.size(); ++i) {
		delete this->UserList[i];
		this->UserList[i] = nullptr;
	}

	// When the client application is completed using the Windows Sockets DLL, the WSACleanup function is called to release resources.
	WSACleanup();
}

void Server::run()
{
	this->initListenSocket();
	this->acceptConnections();
}

void Server::initDatabase()
{
	/*
		Create:
		- .../Database/
		- .../Database/SharedFiles/
		- .../Database/serverlog.txt
		- .../Database/sharedfilenames.txt
		- .../Database/users.bin
	*/
	
	// ...
}

void Server::initWinsock()
{
	WSADATA wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		this->LastError = "WSAStartup() failed with error: " + iResult;
	}
}

void Server::initListenSocket()
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

void Server::acceptConnections()
{
	while (true) {
		SOCKET acceptSocket = INVALID_SOCKET;

		acceptSocket = accept(this->ListenSocket, nullptr, nullptr);

		if (acceptSocket == INVALID_SOCKET) {
			this->LastError = "accept() failed: " + WSAGetLastError();
			continue;
		}

		User* user = new User(acceptSocket);
		
		std::thread dataTranmissionThread(&Server::transmitMsg, this, user);
	}
}

void Server::transmitMsg(User* user)
{
	std::thread sendMsgThread(&Server::sendMsg, this, user);
	std::thread receiveMsgThread(&Server::receiveMsg, this, user);

	// How to pass this while(true)?
	// ...
	while (true);

	// Now, this user is offline so we discard this user from the OnlineUserList
	for (size_t i = 0; i < this->OnlineUserList.size(); ++i) {
		if (this->OnlineUserList[i] == user) {
			this->OnlineUserList.erase(this->OnlineUserList.begin() + i);
			break;
		}
	}

	// Disconnecting gracefully
	// ...
}

void Server::sendMsg(User* user)
{
	// ...
}

void Server::receiveMsg(User* user)
{
	/* Message structure: FLAG (1 byte) | MSGLEN (64 byte) | MSG */

	int iResult;

	RcvMsgFlag flag;
	uint64_t msgLen;
	char* msg = nullptr;

	while (true) {
		// Client shutdown check
		iResult = recv(user->AcceptSocket, nullptr, 0, 0);
		if (iResult == 0)
			break;

		// FLAG
		iResult = recv(user->AcceptSocket, (char*)&flag, sizeof(flag), 0);
		if (iResult == SOCKET_ERROR) {
			this->LastError = "recv() failed with error: " + WSAGetLastError();
			return;
		}

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

void Server::sendAFileToClient(std::string const& indexFile_str, User* user)
{
	size_t indexFile = stoi(indexFile_str);

	SendMsgFlag flag = SendMsgFlag::DOWNLOAD_FILE;

	// ...
}

