#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <sstream>
#include <ctime>
#include <conio.h>
#include <vector>

#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS

#define maxWIDTH 110
#define maxHEIGHT 30

using namespace std;

enum class COLOR {
	BLACK = 0,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHT_GRAY,
	DARK_GRAY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_MAGENTA,
	YELLOW,
	WHITE
};

void printSpace(int n);
string numCommas(uint64_t value);

// Console
void clrscr();
void gotoXY(const unsigned int& x, const unsigned int& y);
unsigned int whereX();
unsigned int whereY();
void sleep(int x);
void FixConsoleWindow();
void FixSizeWindow(int width, int height);
void setColor(COLOR textColor, COLOR bgColor);
void printConsole(COLOR textColor, COLOR bgColor, const unsigned int& x, const unsigned int& y, string content);
void printTextAtMid(string const& text, size_t const& left, size_t const& right);
string hidePassword();
wstring s2ws(const std::string& s);