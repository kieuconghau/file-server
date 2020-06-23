#pragma once

#include "Console.h"

struct File {
	string name;
	int size;
};

class Program
{
public:
	unsigned int line_1; // line of Columm: Client
	unsigned int line_2; // line of Columm : File Uploaded
	unsigned int line_3; // line of Columm : History Log
	Program();
	~Program();

	void run();

private:
	

	void homeScreen();
	//void homeNavigate();
	void printFiles();
	void printLog();
	void printClient();
};
