#include "Server.h"

SOCKET Server::ListenSocket;
std::string const Server::DEFAULT_PORT = "27015";

std::mutex Server::MutexUpload;

std::vector<User*> Server::UserList;
std::vector<User*> Server::OnlineUserList;
std::vector<std::string> Server::FileNameList;

std::string const Server::DATABASE_PATH = "Database/";
std::string const Server::SHARED_FILES_FOLDER = "SharedFiles/";
std::string const Server::LOG_FILE = "serverlog.txt";
std::string const Server::SHARED_FILE_NAMES_FILE = "sharedfilenames.txt";
std::string const Server::USERS_FILE = "users.bin";

std::string Server::LastError;

Server::Server()
{
	Server::initDatabase();
	Server::initWinsock();
	Server::initListenSocket();

	Server::acceptConnections();
}

Server::~Server()
{
	closesocket(Server::ListenSocket);

	for (size_t i = 0; i < Server::UserList.size(); ++i) {
		delete Server::UserList[i];
		Server::UserList[i] = nullptr;
	}

	// When the client application is completed using the Windows Sockets DLL, the WSACleanup function is called to release resources.
	WSACleanup();
}

void Server::run()
{
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
		Server::LastError = "WSAStartup() failed with error: " + iResult;
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

	iResult = getaddrinfo(nullptr, (LPCSTR)&Server::DEFAULT_PORT, &hints, &result);	// Update 'result' with port, IP address,...
	if (iResult != 0) {
		Server::LastError = "getaddrinfo() failed with error: " + iResult;
		return;
	}

	Server::ListenSocket = INVALID_SOCKET;

	Server::ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (Server::ListenSocket == INVALID_SOCKET) {
		Server::LastError = "socket() failed with error: " + WSAGetLastError();
		freeaddrinfo(result);
		return;
	}

	// Bind the socket which has already been created (ListenSocket) to an IP address and port (result->ai_addr: the first sockadrr in the linked list result).
	// (The IP address and port are retrieved from the variable 'result', which has been updated after the execution of the function 'getaddrinfo'.)
	iResult = bind(Server::ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		Server::LastError = "bind() failed with error: " + WSAGetLastError();
		freeaddrinfo(result);
		return;
	}

	// Now, we don't need 'result' anymore.
	freeaddrinfo(result);

	// Let the socket which has already bind (ListenSocket) listen to all connections from Clients.
	iResult = listen(Server::ListenSocket, SOMAXCONN);	// backlog = SOMAXCONN: the maximum length of the queue of pending connections to accept.
	if (iResult == SOCKET_ERROR) {
		Server::LastError = "listen() failed with error: " + WSAGetLastError();
		return;
	}
}

void Server::acceptConnections()
{
	while (true) {
		SOCKET acceptSocket = INVALID_SOCKET;

		acceptSocket = accept(Server::ListenSocket, nullptr, nullptr);

		if (acceptSocket == INVALID_SOCKET) {
			Server::LastError = "accept() failed: " + WSAGetLastError();
			continue;
		}

		User* user = new User(acceptSocket);
		
		std::thread dataTranmissionThread(Server::transmitMsg, user);
		dataTranmissionThread.join();
	}
}

void Server::transmitMsg(User* user)
{
	std::thread sendMsgThread(Server::sendMsg, user);
	std::thread receiveMsgThread(Server::receiveMsg, user);

	sendMsgThread.join();
	receiveMsgThread.join();

	// How to pass this while(true)?
	// ...
	while (true);

	// Now, this user is offline so we discard this user from the OnlineUserList
	for (size_t i = 0; i < Server::OnlineUserList.size(); ++i) {
		if (Server::OnlineUserList[i] == user) {
			Server::OnlineUserList.erase(Server::OnlineUserList.begin() + i);
			break;
		}
	}

	// Disconnecting gracefully
	// ...
}

bool Server::sendMsg(User* user)
{
	// ...

	return true;
}

bool Server::receiveMsg(User* user)
{
	/* Message structure: FLAG | MSGLEN | MSG */

	int iResult;

	RcvMsgFlag flag;
	size_t msgLen;
	char* msg = nullptr;

	while (true) {
		// Client shutdown check
		iResult = recv(user->AcceptSocket, nullptr, 0, 0);
		if (iResult == 0)
			break;

		// Flag
		iResult = recv(user->AcceptSocket, (char*)&flag, sizeof(flag), 0);
		if (iResult == SOCKET_ERROR)
			return false;

		// MsgLen
		iResult = recv(user->AcceptSocket, (char*)&msgLen, sizeof(msgLen), 0);
		if (iResult == SOCKET_ERROR)
			return false;
		msg = new char[msgLen + 1];

		// Msg
		iResult = recv(user->AcceptSocket, msg, msgLen, 0);
		if (iResult == SOCKET_ERROR)
			return false;
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
			Server::sendFileToClient(msg, user);
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

	
	return true;
}

bool Server::sendFileToClient(std::string const& indexFile_str, User* user)
{
	size_t indexFile = stoi(indexFile_str);

	SendMsgFlag flag = SendMsgFlag::DOWNLOAD_FILE;

	// ...

	return true;
}

