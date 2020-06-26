#pragma once

#include <string>

class File
{
	friend class Program;
public:

private:
	std::string fileName;
	std::string fileSize;
};
