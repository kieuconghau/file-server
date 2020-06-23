#pragma once

#include "Console.h"

enum class SELECTED {
	ONLINE = 0,
	YES,
	NO
};

struct File {
	string name;
	int size;
};

class Program
{
public:
	SELECTED selected;
	unsigned int line_1; // line of Columm: Client
	unsigned int line_2; // line of Columm : File Uploaded
	unsigned int line_3; // line of Columm : History Log
	Program();
	~Program();

	void run();

private:
	

	void homeScreen();
	//void homeNavigate();
	void printStatus();
	void navigateStatus();
	void printFiles();
	void printLog();
	void printClient();
};
