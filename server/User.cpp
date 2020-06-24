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
}