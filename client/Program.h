#pragma once

#include "Console.h"



struct File {
	string name;
	int size;
};

struct List {
	File* files;
	int n;
};

struct Info {
	string ip, username, password;
};

enum class SELECTED {
	REGISTER = 0,
	LOGIN,
	UPLOAD,
	DOWNLOAD,
	ESC,
	YES,
	NO
};

class Program
{
public:
	Program();
	~Program();
	void run();

	/* Init for test */
	void InitFileList();

private:
	int          line_2;	// line of Columm : File Uploaded
	int          line_3;	// line of Columm : History Log

	SELECTED     selected;	// selected STATE
	Info         user;		// Client info
	vector<File> list;		// List file


	void homeScreen();

	void printTitle();
	void printFile(string name, int size, bool selected);
	void printLog(string content);

	/* ==== Client Register/Login ==== */
	void buttonClient();
	void printClient();
	void navigateClient();
	void loginClient();
	/* =============================== */

	void printMode();
	void navigateMode();
	string enterPath();

	void printStatus();
};
