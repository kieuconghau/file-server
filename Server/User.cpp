#include "User.h"

User::User()
{
	this->AcceptSocket = INVALID_SOCKET;
}

User::User(SOCKET const& acceptSocket)
{
	this->AcceptSocket = acceptSocket;
}

User::~User()
{
	// ...

	/*
	// Shutdown the send half of the connection since no more data will be sent from the Server.
	// The server application can still receive data on the socket.
    shutdown(this->AcceptSocket, SD_SEND);

	// When the client application is done receiving data, the closesocket function is called to close the socket.
	closesocket(this->AcceptSocket);
	*/
}
