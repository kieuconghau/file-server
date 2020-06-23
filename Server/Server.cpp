#include "Server.h"

string const Server::DatabasePath = "Database/";
string const Server::SharedFilesFolder = "SharedFiles/";
string const Server::LogFile = "serverlog.txt";
string const Server::SharedFilesNameFile = "sharedfilenames.txt";
string const Server::UsersFile = "users.bin";



void Server::run()
{

}

void Server::initialize()
{
	/*
		Create:
		- .../Database/
		- .../Database/SharedFiles/
		- .../Database/serverlog.txt
		- .../Database/sharedfilenames.txt
		- .../Database/users.bin
	*/
}

bool Server::receiveMsg(User const& user)
{
	int iResult;

	RcvMsgFlag flag;
	size_t msgLen;
	char* msg = nullptr;

	while (true) {
		iResult = recv(user.AcceptSocket, (char*)&flag, sizeof(flag), 0);
		if (iResult == SOCKET_ERROR)
			return false;

		iResult = recv(user.AcceptSocket, (char*)&msgLen, sizeof(msgLen), 0);
		if (iResult == SOCKET_ERROR)
			return false;
		msg = new char[msgLen + 1];

		iResult = recv(user.AcceptSocket, msg, msgLen, 0);
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
			this->sendFileToClient(msg, user);
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

bool Server::sendFileToClient(string const& indexFile_str, User const& user)
{
	size_t indexFile = stoi(indexFile_str);

	SendMsgFlag flag = SendMsgFlag::DOWNLOAD_FILE;

	// ...

	return true;
}

