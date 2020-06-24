#include "Console.h"


void printTextAtMid(string const& text, uint64_t const& left, uint64_t const& right)
{
    gotoXY((right - left - text.length()) / 2, whereY());
    cout << text;
}

void setColor(COLOR textColor, COLOR bgColor)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ((uint64_t)textColor + ((uint64_t)bgColor * 16)));
}

void clrscr()
{
    CONSOLE_SCREEN_BUFFER_INFO	csbiInfo;
    HANDLE	hConsoleOut;
    COORD	Home = { 0,0 };
    DWORD	dummy;

    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsoleOut, &csbiInfo);

    FillConsoleOutputCharacter(hConsoleOut, ' ', csbiInfo.dwSize.X * csbiInfo.dwSize.Y, Home, &dummy);
    csbiInfo.dwCursorPosition.X = 0;
    csbiInfo.dwCursorPosition.Y = 0;
    SetConsoleCursorPosition(hConsoleOut, csbiInfo.dwCursorPosition);
}

void gotoXY(const unsigned int& x, const unsigned int& y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void printSpace(int n) {
    for (int i = 0; i < n / 10; i++) {
        cout << "          ";
    }

    int r = n / 10;
    n = n - r * 10;

    for (int i = 0; i < n; i++) {
        cout << " ";
    }
}

string numCommas(uint64_t value) {
    string numWithCommas = to_string(value);
    int insertPosition = numWithCommas.length() - 3;
    while (insertPosition > 0) {
        numWithCommas.insert(insertPosition, ",");
        insertPosition -= 3;
    }
    return numWithCommas;
}

unsigned int whereX() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.X;
}

unsigned int whereY() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.Y;
}

void sleep(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(x));
}

void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void FixSizeWindow(int x, int y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (h == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Unable to get stdout handle.");

    // If either dimension is greater than the largest console window we can have,
    // there is no point in attempting the change.
    {
        COORD largestSize = GetLargestConsoleWindowSize(h);
        if (x > largestSize.X)
            throw std::invalid_argument("The x dimension is too large.");
        if (y > largestSize.Y)
            throw std::invalid_argument("The y dimension is too large.");
    }


    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    if (!GetConsoleScreenBufferInfo(h, &bufferInfo))
        throw std::runtime_error("Unable to retrieve screen buffer info.");

    SMALL_RECT& winInfo = bufferInfo.srWindow;
    COORD windowSize = { winInfo.Right - winInfo.Left + 1, winInfo.Bottom - winInfo.Top + 1 };

    if (windowSize.X > x || windowSize.Y > y)
    {
        // window size needs to be adjusted before the buffer size can be reduced.
        SMALL_RECT info =
        {
            0,
            0,
            x < windowSize.X ? x - 1 : windowSize.X - 1,
            y < windowSize.Y ? y - 1 : windowSize.Y - 1
        };

        if (!SetConsoleWindowInfo(h, TRUE, &info))
            throw std::runtime_error("Unable to resize window before resizing buffer.");
    }

    COORD size = { x, y + 9001 }; // 9001: max Scroll y
    if (!SetConsoleScreenBufferSize(h, size))
        throw std::runtime_error("Unable to resize screen buffer.");


    SMALL_RECT info = { 0, 0, x - 1, y - 1 };
    if (!SetConsoleWindowInfo(h, TRUE, &info))
        throw std::runtime_error("Unable to resize window after resizing buffer.");
}

void printConsole(COLOR textColor, COLOR bgColor, const unsigned int& x, const unsigned int& y, string content) {
    gotoXY(x, y);
    setColor(textColor, bgColor);
    cout << content;
}

string hidePassword() {
    setColor(COLOR::WHITE, COLOR::BLACK);
    string input;
    char temp;
    while (true) {
        temp = _getch();
        if (GetKeyState(VK_ESCAPE) & 0x8000) {
            while (GetKeyState(VK_ESCAPE) & 0x8000) {};
            continue;
        }
        else if (temp == '\0') {
            continue;
        }
        else if (temp == '\r') {
            break;
        }
        else if (input.length() > 0 && temp == 8) {
            input.pop_back();
            std::cout << "\b \b";
        }
        else if (input.length() == 0 && temp == 8) {
            continue;
        }
        else {
            std::cout << temp;
            Sleep(50);
            std::cout << "\b*";
            input.push_back(temp);
        }
    }
    return input;
}

wstring s2ws(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}