#include "Helper.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <ctype.h>
#include <Windows.h>
#include <filesystem>
#include <iostream>


void Helper::hideCursor()
{
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = false;
	lpCursor.dwSize = 50;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &lpCursor);
}

std::string Helper::readFile(std::string fileName)
{
	std::string content;
	std::string tmp;
	std::ifstream f(fileName);
	while (getline(f, tmp)) {
		content += tmp + '\n';
	}
	f.close();
	return content;
}

void Helper::writeFile(std::string fileName, std::string content)
{
	std::ofstream f(fileName);
	f << content;
	f.close();
}

std::string Helper::colorize(std::string text, int style)
{
	switch (style) {
	case RESET:
		return "\033[0m" + text + "\033[0m";
	case UNDERLINE:
		return "\033[4m" + text + "\033[0m";
	case BLACK:
		return "\033[30m" + text + "\033[0m";
	case WHITE:
		return "\033[37m" + text + "\033[0m";
	case RED:
		return "\033[31m" + text + "\033[0m";
	case BLUE:
		return "\033[34m" + text + "\033[0m";
	case MAGENTA:
		return "\033[35m" + text + "\033[0m";
	case CURSOR:
		return "\033[42m" + text + "\033[0m";
	case BACKGROUND:
		return "\033[104m" + text + "\033[0m";
	case VISUAL_STYLE:
		return "\033[100m" + text + "\033[0m";
	case FIND_HIGHLIGHTING:
		return "\033[101m" + text + "\033[0m";
	case LINE_NUMBER:
		return "\033[46m\033[34m" + text + "\033[0m";
	}
}

std::string Helper::colorize(std::string text, Colorizers colorizers)
{
	if (colorizers.size() == 0) {
		return text;
	}

	std::string result = text.substr(0, colorizers[0].begin);
	int minus = colorizers[0].begin;
	if (colorizers[0].begin == 0) {
		minus = 0;
	}
	result += Helper::colorize(text.substr(colorizers[0].begin, colorizers[0].count), colorizers[0].style);
	
	Colorizers copy;
	for (size_t i = 1; i < colorizers.size(); i++) {
		Colorizer c = { colorizers[i].begin - colorizers[0].count - minus, colorizers[i].count, colorizers[i].style };
		copy.push_back(c);
	}

	return result + Helper::colorize(text.substr(colorizers[0].begin + colorizers[0].count), copy);
}

std::string Helper::setCursor(std::string line, int x)
{
	if (x > line.size()) {
		return line;
	}
	if (line.size() == 0) {
		return Helper::colorize(" ", CURSOR);
	}
	if (x == 0) {
		return Helper::colorize(line.substr(0, 1), CURSOR) + line.substr(1);
	}
	if (x == line.size()) {
		return line + Helper::colorize(" ", CURSOR);
	}
	return line.substr(0, x) + Helper::colorize(line.substr(x, 1), CURSOR) + line.substr(x + 1);
}


size_t Helper::getDisplayLength(std::string str, size_t begin, size_t end)
{
	size_t s = 0;
	bool inStyle = false;
	size_t finishLine = min(end, str.size());
	for (size_t i = begin; i < finishLine; i++) {
		if (str[i] == '\033') {
			inStyle = true;
		}
		else if (inStyle && str[i] == 'm') {
			inStyle = false;
		}
		else if (!inStyle) {
			s += 1;
		}
	}
	return s;
}

size_t Helper::getDisplayIndex(std::string str, size_t index)
{
	if (index >= str.size()) {
		throw std::exception("Invalid index");
	}
	size_t s = 0;
	bool inStyle = false;
	for (size_t i = 0; i < str.size(); i++) {
		if (i == index) {
			return s;
		}
		if (str[i] == '\033') {
			inStyle = true;
		}
		else if (inStyle && str[i] == 'm') {
			inStyle = false;
		}
		else if (!inStyle) {
			s += 1;
		}
	}
	return s;
}

void Helper::getTerminalSize(short* x, short* y)
{
	// Only works on windows!
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	*x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

std::string Helper::replace(std::string subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::string Helper::getTimeString()
{
	time_t t = time(nullptr);
	tm localTime;
	localtime_s(&localTime, &t);

	return (localTime.tm_hour < 10 ? "0" + std::to_string(localTime.tm_hour) : std::to_string(localTime.tm_hour)) + ':' + (localTime.tm_min < 10 ? "0" + std::to_string(localTime.tm_min) : std::to_string(localTime.tm_min));
}

bool Helper::isPrintable(char c)
{
	return c >= 32 && c <= 126;
}

bool Helper::isAlphanumeric(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

std::string Helper::trim(std::string s)
{
	std::string copy = s;
	copy.erase(copy.find_last_not_of(" \t\n\r\f\v") + 1);
	copy.erase(0, copy.find_first_not_of(" \t\n\r\f\v"));
	return copy;
}

std::string Helper::getFileName(std::string fileName)
{
	return "";
}

std::vector<std::string> Helper::getFilesInDirectory(std::string dir)
{
	std::vector<std::wstring> names;
	std::wstring search_path(dir.begin(), dir.end());
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			//if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			//}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	std::vector<std::string> res;
	for (auto i : names) {
		res.push_back(std::string(i.begin(), i.end()));
	}
	return res;
}

bool Helper::insStrCompare(std::string a, std::string b)
{
	if (b.size() != a.size()) return false;
	std::for_each(a.begin(), a.end(), [](char& c) {
		c = ::tolower(c);
	});
	std::for_each(b.begin(), b.end(), [](char& c) {
		c = ::tolower(c);
	});
	return a == b;
}
