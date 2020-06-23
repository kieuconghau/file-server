#include "Program.h"

Program::Program()
{
	FixSizeWindow(110, 30);
	FixConsoleWindow();
	line_1 = line_2 = line_3 = 2;
	//Init something you need
}

Program::~Program()
{
	//Del things you init
}

void Program::run()
{
	while (true) { 
		this->homeScreen(); 
		sleep(5000);
	}
	
}

void Program::homeScreen() {
	string name = "abc.xyz";
	int size = 4096;
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	gotoXY(0, 0);
	printTextAtMid("[SERVER]", 0, maxWIDTH);

	gotoXY(0, 1);
	setColor(COLOR::BLACK, COLOR::LIGHT_GRAY);
	printSpace(11); cout << "Clients"; printSpace(11); cout << "|"; printSpace(8); cout << "File Uploaded"; printSpace(8); cout << "|"; printSpace(20); cout << "History Log"; printSpace(19);

	setColor(COLOR::WHITE, COLOR::BLACK);
	
	printClient();
	printFiles();
	printLog();
}

void Program::printClient() {
	string user = "tambuu2804";
	bool login = 0;

	if (login) {
		gotoXY(1, line_1);
		setColor(COLOR::WHITE, COLOR::BLACK);
		cout << user; printSpace(27 - user.length() - 5);
		setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
		cout << "[ONL]";
	}
	else {
		gotoXY(1, line_1);
		setColor(COLOR::DARK_GRAY, COLOR::BLACK);
		cout << user; printSpace(27 - user.length() - 5);
		setColor(COLOR::RED, COLOR::BLACK);
		cout << "[OFF]";
	}
}

void Program::printFiles() {
	string name = "eqweqw.xyz";
	int size = 4096;

	gotoXY(30, line_2);

	setColor(COLOR::WHITE, COLOR::BLACK);
	cout << " " << name; printSpace(27 - name.length() - numCommas(size).length()); cout << numCommas(size);
}

void Program::printLog() {
	time_t now = time(0);
	tm* ltm = localtime(&now);

	gotoXY(61, line_3);

	// [hh:mm]
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	if (ltm->tm_hour < 10) cout << "[0" << ltm->tm_hour;
	else cout << "[" << ltm->tm_hour;
	if (ltm->tm_min < 10) cout << ":0" << ltm->tm_min << "] ";
	else cout << ":" << ltm->tm_min << "] ";

	// Content
	setColor(COLOR::WHITE, COLOR::BLACK);
}

