#include "Helper.h"
#include "../Content.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <ctype.h>
#include <conio.h>

#define STD_OUTPUT_HANDLE -11




std::string Helper::colorize(std::string text, Style style)
{
	switch (style) {
	case Style::RESET:
		return "\033[0m" + text + "\033[0m";
	case Style::UNDERLINE:
		return "\033[4m" + text + "\033[0m";
	case Style::BLACK:
		return "\033[30m" + text + "\033[0m";
	case Style::WHITE:
		return "\033[37m" + text + "\033[0m";
	case Style::RED:
		return "\033[31m" + text + "\033[0m";
	case Style::BLUE:
		return "\033[34;1m" + text + "\033[0m";
	case Style::MAGENTA:
		return "\033[35m" + text + "\033[0m";
	case Style::CURSOR:
		return "\033[42m" + text + "\033[0m";
	case Style::BACKGROUND:
		return "\033[104m" + text + "\033[0m";
	case Style::VISUAL_STYLE:
		return "\033[100m" + text + "\033[0m";
	case Style::FIND_HIGHLIGHTING:
		return "\033[101m" + text + "\033[0m";
	case Style::LINE_NUMBER:
		return "\033[46m\033[34m" + text + "\033[0m";
	case Style::DIMMED:
		return "\033[38;5;242m" + text + "\033[0m";
	case Style::FILE_EXPLORER:
		return "\033[48;2;6;6;46m\033[37m" + text + "\033[0m";
	case Style::FILE_EXPLORER_ACTIVE:
		return "\033[48;2;68;30;66m\033[37m" + text + "\033[0m";
	}
}

std::string Helper::colorize(std::vector<std::string> text, std::vector<Style> styles, short width)
{
	if (text.size() != styles.size()) throw std::exception("Invalid colorizers lengths.");
	std::string result;
	size_t totalSize = 0;
	for (size_t i = 0; i < text.size(); i++) {
		text[i] = Helper::replace(text[i], "\t", std::string(Config::settings["TAB_SIZE"], ' '));
		totalSize += text[i].size();
	}

	if (width > totalSize) {
		for (size_t i = 0; i < text.size(); i++) {
			result += Helper::colorize(text[i], styles[i]);
		}
		result += std::string(width - totalSize, ' ');
	}
	else {
		// XXXXXXXXXXX
		// AAAAABBBBBCCCCCCCCC
		size_t index = 1;
		do {
			text[text.size() - index] = text[text.size() - index].substr(0, text[text.size() - index].size() - totalSize + width);
			totalSize = 0;
			for (size_t i = 0; i < text.size(); i++) {
				totalSize += text[i].size();
			}
			index++;
		} while (totalSize > width);

		for (size_t i = 0; i < text.size(); i++) {
			result += Helper::colorize(text[i], styles[i]);
		}

	}

	return result;
}

std::string Helper::setCursor(std::string line, int x)
{
	if (x > line.size()) {
		return line;
	}
	if (line.size() == 0) {
		return Helper::colorize(" ", Style::CURSOR);
	}
	if (x == 0) {
		return Helper::colorize(line.substr(0, 1), Style::CURSOR) + line.substr(1);
	}
	if (x == line.size()) {
		return line + Helper::colorize(" ", Style::CURSOR);
	}
	return line.substr(0, x) + Helper::colorize(line.substr(x, 1), Style::CURSOR) + line.substr(x + 1);
}


size_t Helper::getDisplayLength(std::string str, size_t begin, size_t end)
{
	size_t s = 0;
	size_t inStyleCount = 0;
	bool inStyle = false;
	size_t finishLine = min(end, str.size());
	for (size_t i = begin; i < finishLine; i++) {
		if (str[i] == '\033') {
			inStyle = true;
			inStyleCount = 1;
		}
		else if (inStyle && isalpha(str[i])) {
			inStyle = false;
			inStyleCount = 0;
		}
		else if (!inStyle) {
			s += 1;
		}
		else {
			inStyleCount++;
		}
	}
	if (inStyle) s += inStyleCount;
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
		else if (inStyle && isalpha(str[i])) {
			inStyle = false;
		}
		else if (!inStyle) {
			s += 1;
		}
	}
	return s;
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
	return c >= 32 && c <= 126 || c == '\t';
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

int Helper::getNextKey()
{
	char key = _getch();
	if (key == NEXT_IS_UTILS || key == NULL) {
		return ((int)_getch() << 8) + key;
	}
	return key;
}
