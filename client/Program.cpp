#include "Program.h"

void Program::InitFileList() {
	
	File a;
	a.name = "abcefg.txt"; 
	a.size = 4096;
	list.push_back(a);

	File b;
	b.name = "1233444.docx";
	b.size = 1128040;
	list.push_back(b);

	File c;
	c.name = "testingtesting.txt";
	c.size = 280400;
	list.push_back(c);

	File d;
	d.name = "1230493111.bin";
	d.size = 12333123;
	list.push_back(d);

	File e;
	e.name = "boobie.jpg";
	e.size = 113333123;
	list.push_back(e);
}

Program::Program()
{
	FixSizeWindow(110, 30);
	FixConsoleWindow();
	selected = SELECTED::REGISTER;
	line_2 = 0;
	line_3 = 2;

	//Init something you need
	InitFileList();
}

Program::~Program()
{
	//Del things you init
}

void Program::run()
{
	this->homeScreen();
}

void Program::homeScreen() {
	this->printTitle();
	this->printMode();
	this->printClient();
	this->navigateClient();

	for (int i = 0; i < list.size(); i++) {
		printFile(list[i].name, list[i].size, false);
		line_2++;
	}
	line_2 = 2;
	
	
	
	this->navigateMode();
}

void Program::printMode() {
	if (selected == SELECTED::DOWNLOAD) {
		setColor(COLOR::BLACK, COLOR::DARK_GRAY);
		gotoXY(30, 0);
		cout << " DOWNLOAD ";
	}
	else {
		setColor(COLOR::BLACK, COLOR::LIGHT_CYAN);
		gotoXY(30, 0);
		cout << "  UPLOAD  ";
	}
}

void Program::navigateMode() {
	bool esc = false;
	while (true) {

		if (_kbhit()) {
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			// ============= LEFT/RIGHT =============
			if ((GetKeyState(VK_LEFT) & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000)) {
				selected = (selected == SELECTED::UPLOAD) ? SELECTED::DOWNLOAD : SELECTED::UPLOAD;
				this->printMode();
				
				printFile(list[0].name, list[0].size, false); // reset the previous line back to normal
				line_2 = 0;									  // reset line back to the top of list
				if (selected == SELECTED::DOWNLOAD) {
					printFile(list[0].name, list[0].size, true);
				}
			}

			// ============= UP =============
			if (GetKeyState(VK_UP) & 0x8000) {
				if (selected == SELECTED::DOWNLOAD) {
					printFile(list[line_2].name, list[line_2].size, false); // reset the previous line back to normal
					line_2--;
					line_2 += (line_2 < 0) ? list.size() : 0;
					printFile(list[line_2].name, list[line_2].size, true);
				}
			}

			// ============= DOWN =============
			if (GetKeyState(VK_DOWN) & 0x8000) {
				if (selected == SELECTED::DOWNLOAD) {
					printFile(list[line_2].name, list[line_2].size, false); // reset the previous line back to normal
					line_2++;
					line_2 %= list.size();
					printFile(list[line_2].name, list[line_2].size, true);
				}

			}

			// ============= ENTER =============
			if (GetKeyState(VK_RETURN) & 0x8000) {

				if (selected == SELECTED::UPLOAD) {
					this->getUploadPath();
					// CSocket UPLOAD file

					// If success or fail, do something
				}

				if (selected == SELECTED::DOWNLOAD) {
					//CSocket DOWNLOAD file
				}
			}

			// ============= ESC =============
			if (GetKeyState(VK_ESCAPE) & 0x8000) {
				esc = true;
			}
		}

		if (esc) break;
	}
}

string Program::getUploadPath() {
	string path;

	gotoXY(41, 0);
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	cin >> path;

	gotoXY(41, 0); printSpace(69);
	return path;
}

void Program::printTitle() {
	setColor(COLOR::BLACK, COLOR::LIGHT_GRAY);
	gotoXY(30, 1); printSpace(8); cout << "File Uploaded"; printSpace(8); 
	
	setColor(COLOR::BLACK, COLOR::LIGHT_GRAY);
	cout << "|"; 
	
	setColor(COLOR::BLACK, COLOR::LIGHT_GRAY);
	printSpace(20); cout << "History Log"; printSpace(19);


	setColor(COLOR::WHITE, COLOR::BLACK);
}

void Program::printFile(string name, int size, bool selected) {
	gotoXY(30, line_2 + 2);
	
	if (selected) {
		setColor(COLOR::BLACK, COLOR::LIGHT_BLUE);
	}
	else setColor(COLOR::WHITE, COLOR::BLACK);
	cout << " " << name; printSpace(27 - name.length() - numCommas(size).length()); cout << numCommas(size) << " ";
}

void Program::printLog(string content) {
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
	cout << content;

	line_3++;
}

void Program::buttonClient() {
	gotoXY(1, 10);
	if (selected == SELECTED::REGISTER) {
		setColor(COLOR::BLACK, COLOR::LIGHT_RED);
	}
	else setColor(COLOR::BLACK, COLOR::DARK_GRAY);
	cout << "  REGISTER  ";

	setColor(COLOR::WHITE, COLOR::BLACK);
	cout << " ";

	if (selected == SELECTED::LOGIN) {
		setColor(COLOR::BLACK, COLOR::LIGHT_RED);
	}
	else setColor(COLOR::BLACK, COLOR::DARK_GRAY);
	cout << "     LOGIN     ";

	setColor(COLOR::BLACK, COLOR::BLACK);
}

void Program::printClient() {
	gotoXY(0, 9); 
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK);
	printTextAtMid("[CLIENT]", 0, 30);

	buttonClient();

	setColor(COLOR::DARK_GRAY, COLOR::BLACK);
	gotoXY(0, 11); cout << "|                            |";
	gotoXY(0, 12); cout << "|                            |";
	gotoXY(0, 13); cout << "|                            |";
	gotoXY(0, 14); cout << "'============================'";

	setColor(COLOR::DARK_GRAY, COLOR::BLACK);
	gotoXY(1, 11); cout << "ServerIP:";
	gotoXY(1, 12); cout << "Username:";
	gotoXY(1, 13); cout << "Password:";

}

void Program::navigateClient() {
	while (true) {

		if (_kbhit()) {
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			// ============= LEFT =============
			if (GetKeyState(VK_LEFT) & 0x8000) {
				selected = (selected == SELECTED::REGISTER) ? SELECTED::LOGIN : SELECTED::REGISTER;
				this->buttonClient();

			}

			// ============= RIGHT =============
			if (GetKeyState(VK_RIGHT) & 0x8000) {
				selected = (selected == SELECTED::REGISTER) ? SELECTED::LOGIN : SELECTED::REGISTER;
				this->buttonClient();
			}

			// ============= ENTER =============
			if (GetKeyState(VK_RETURN) & 0x8000) {

				this->loginClient();

				if (selected == SELECTED::REGISTER) {
					// CSOCKET register
					printLog("Registration success.");
				}
				else {
					// CSOCKET login
					printLog("Login success.");
				}

				printLog("Conected to server.");
				break;
			}
		}

	}
	selected = SELECTED::UPLOAD;
}

void Program::loginClient() {
	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK); gotoXY(1, 11); cout << "ServerIP: ";
	setColor(COLOR::WHITE, COLOR::BLACK);					  cin >> user.ip;
	setColor(COLOR::DARK_GRAY, COLOR::BLACK);  gotoXY(1, 11); cout << "ServerIP: ";


	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK); gotoXY(1, 12); cout << "Username: ";
	setColor(COLOR::WHITE, COLOR::BLACK);					  cin >> user.username;
	setColor(COLOR::DARK_GRAY, COLOR::BLACK);  gotoXY(1, 12); cout << "Username: ";

	setColor(COLOR::LIGHT_CYAN, COLOR::BLACK); gotoXY(1, 13); cout << "Password: ";
	setColor(COLOR::WHITE, COLOR::BLACK);	                  user.password = hidePassword();
	setColor(COLOR::DARK_GRAY, COLOR::BLACK);  gotoXY(1, 13); cout << "Password: ";

	setColor(COLOR::DARK_GRAY, COLOR::BLACK);  gotoXY(0, 14); cout << "'============================'";
}