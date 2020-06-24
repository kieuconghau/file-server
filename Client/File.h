#pragma once

#include <string>

class File
{
	friend class Server;
public:

private:
	std::string Name;
	uint64_t Size;
};

