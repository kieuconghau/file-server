#include "User.h"

User::User()
{
	this->ConnectSocket = INVALID_SOCKET;
}

User::User(SOCKET const& acceptSocket)
{
	this->ConnectSocket = acceptSocket;
}

User::~User()
{
	// ...
}