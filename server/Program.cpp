#include "Program.h"

Program::Program()
{
	this->test();
	FixSizeWindow(110, 30);
	FixConsoleWindow();

	/* GUI */
	line_1 = line_2 = line_3 = 2;
	selected = SELECTED::ONLINE;

	//Init something you need
	this->InitDataBaseDirectory();
	this->InitUserList();
	this->InitFileNameList();
}

void Program::InitDataBaseDirectory() {
	if (CreateDirectory(s2ws(DATABASE_PATH).c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
		CreateDirectory(s2ws(DATABASE_PATH + "\\" + SHARED_FILES_FOLDER).c_str(), NULL);
	}
	else return;
}

void Program::InitUserList() {
	fstream f(DATABASE_PATH + "\\" + USER_FILE,
		std::fstream::in | std::fstream::binary);

	while (true) {
		User*    temp = new User;
		
		getline(f, temp->Username, '\0');

		if (f.eof()) {
			delete temp;
			break;
		}

		getline(f, temp->Password, '\0');

		this->UserList.push_back(temp);
	}

	f.close();
}

void Program::InitFileNameList() {
	fstream f(DATABASE_PATH + "\\" + SHARED_FILE_NAMES_FILE,
		std::fstream::in | std::fstream::binary);

	while (true) {
		string str;

		getline(f, str, '\0');

		if (f.eof()) {
			break;
		}

		string* filename = new string;
		*filename = str;
		this->FileNameList.push_back(*filename);
	}

	f.close();
}

unsigned long Program::fileSizeBytes(string filename) {
	ifstream f(DATABASE_PATH + "\\" + SHARED_FILES_FOLDER + "\\" + filename,
		std::fstream::ate | std::fstream::binary);

	if (!f.is_open()) {
		return 0;
	}

	return f.tellg();
}

Program::~Program()
{
	for (int i = 0; i < UserList.size(); i++) {
		delete UserList[i];
	}

	for (int i = 0; i < FileNameList.size(); i++) {
		delete& FileNameList[i];
	}
}

void Program::run()
{
	this->homeScreen(); 
}

void Program::printStatus() {
	if (selected == SELECTED::ONLINE) {
		gotoXY(88, 0);

		setColor(COLOR::BLACK, COLOR::LIGHT_GREEN);
		cout << "        ONLINE        ";
		
	}
	else {
		gotoXY(88, 0);


		setColor(COLOR::BLACK, COLOR::DARK_GRAY);
		cout << " DISCONECT ";

		if (selected == SELECTED::YES) {

			setColor(COLOR::BLACK, COLOR::LIGHT_RED);
			cout << " YES ";

			setColor(COLOR::LIGHT_BLUE, COLOR::BLACK);
			cout << "  NO  ";
		}

		if (selected == SELECTED::NO) {

			setColor(COLOR::LIGHT_RED, COLOR::BLACK);
			cout << " YES ";

			setColor(COLOR::BLACK, COLOR::LIGHT_BLUE);
			cout << "  NO  ";
		}
		
	}
	setColor(COLOR::WHITE, COLOR::BLACK);
}

void Program::navigateStatus() {
	bool esc = false;
	while (true) {

		if (_kbhit()) {
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			// ============= LEFT =============
			if ((GetKeyState(VK_LEFT) & 0x8000) || (GetKeyState(VK_RIGHT) & 0x8000)) {
				if (selected != SELECTED::ONLINE) {
					selected = (selected == SELECTED::YES) ? SELECTED::NO : SELECTED::YES;
					this->printStatus();
				}
			}

			// ============= ESC =============
			if (GetKeyState(VK_ESCAPE) & 0x8000) {
				if (selected == SELECTED::ONLINE) {
					selected = SELECTED::NO;
					this->printStatus();
				}
			}

			// ============= ENTER =============
			if (GetKeyState(VK_RETURN) & 0x8000) {
				if (selected == SELECTED::YES) {
					// SOCKET DISCONECT

					esc = true;
				}
				else {
					selected = SELECTED::ONLINE;
					this->printStatus();
				}
			}
		}
		
		if (esc) break;
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

	printStatus();

	setColor(COLOR::WHITE, COLOR::BLACK);

	for (int i = 0; i < UserList.size(); i++) {
		printClient(UserList[i]->Username, false);
	}

	for (int i = 0; i < FileNameList.size(); i++) {
		printFiles(FileNameList[i]);
	}

	printLog();

	navigateStatus();

}

void Program::printClient(string user, bool login) {
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
	line_1++;
}

void Program::printFiles(string filename) {
	string size = toStringFileSize(filename);
	filename = shortenFileName(filename);

	gotoXY(30, line_2);

	setColor(COLOR::WHITE, COLOR::BLACK);

	cout << " " << filename;
	printSpace(27 - filename.length() - size.length()); 
	cout << size;

	line_2++;
}

string Program::toStringFileSize(string filename) {
	unsigned long size = fileSizeBytes(filename);
	uint8_t d = 0;

	while (size > 1000) {
		size /= 1000;
		d++;
	}

	string parameter[] = { " B", "KB", "MB", "GB" };
	
	return numCommas(size) + " " + parameter[d];
}

string Program::shortenFileName(string filename) {
	if (filename.length() > 20) {
		string str1 = filename.substr(0, 10);
		string str2 = filename.substr(filename.length() - 7, 7);
		filename = str1 + "..." + str2;
	}

	return filename;
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

void Program::test() {
	fstream f(DATABASE_PATH + "\\" + SHARED_FILE_NAMES_FILE, 
		std::fstream::in | std::fstream::out | std::fstream::app | std::fstream::binary);

	string str1 = "thelongfiletestnametest1.txt";
	string str2 = "filetest2.txt";

	f.write(str1.c_str(), str1.size());
	f.write("\0", sizeof(char));
	f.write(str2.c_str(), str2.size());
	f.write("\0", sizeof(char));

	f.close();
}
