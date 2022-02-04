#include "Display.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>
#include <thread>

#define LINE_NUMBER_SIZE 4
#define NON_CONTENT_LINES 3
#define LINE_OFFSET_STR " << "

void Display::showTopBar(short width) const
{
	std::string timeString = Helper::getTimeString();
	Colorizer timeColor = { 0, timeString.size(), MAGENTA };
	std::string mid(2 * width / 5, ' ');
	std::string fileTitle = std::string(this->wasEdited ? "*" : "") + this->c.getFileName();
	Colorizer fileColor = { timeString.size() + mid.size(), fileTitle.size(), BLUE };

	Colorizers c;
	c.push_back(timeColor);
	c.push_back(fileColor);
	//std::cout << this->padToLine(" ", width);
	std::cout << this->padToLine(Helper::colorize(timeString + mid + fileTitle, c), width);// << std::endl;
}

Display::Display(std::string fname) : c(fname) {
}

void Display::show() const
{
	std::vector<std::string> content = this->c.getLines();
	int count = 0;

	// Getting terminal dimensions
	short width = 0;
	short height = 0;
	Helper::getTerminalSize(&width, &height);
	short effectiveHeight = height - NON_CONTENT_LINES;

	this->showTopBar(width);

	int offset = std::max(0, this->posX - (width / 2));
	Colorizer lineNumberColor = { 0, LINE_NUMBER_SIZE, LINE_NUMBER };
	Colorizer offsetLineColor = { LINE_NUMBER_SIZE, sizeof(LINE_OFFSET_STR) - 1, WHITE };

	// Aligning to top/bottom
	int startIndex = std::max(std::min(this->posY - (effectiveHeight / 2), (int)content.size() - effectiveHeight), 0);

	for (auto i = content.begin() + startIndex; i != content.begin() + std::min(startIndex + height, (int)content.size()); i++) {
		if (count < height - NON_CONTENT_LINES) {
			// For every line in content (that is inside the view)
			std::string lineNumber = std::to_string(count + startIndex);
			lineNumber.insert(lineNumber.end(), LINE_NUMBER_SIZE - Helper::getDisplayLength(lineNumber), ' ');

			// If cursor in line
			if (count + startIndex == this->posY) {
				// If line is offset
				if (this->posX > width - LINE_NUMBER_SIZE - sizeof(LINE_OFFSET_STR) + 1) {
					std::string line = lineNumber + LINE_OFFSET_STR + i->substr(offset) + ' ';

					Colorizer cursorColor = { width / 2 + LINE_NUMBER_SIZE + sizeof(LINE_OFFSET_STR) - 1, 1, CURSOR };
					Colorizers c;
					c.push_back(lineNumberColor);
					c.push_back(offsetLineColor);
					c.push_back(cursorColor);
					std::cout << this->padToLine(Helper::colorize(line, c), width);
				}
				// Not offset
				else {
					std::string line = lineNumber + ' ' + *i + ' ';
					Colorizers c;
					c.push_back(lineNumberColor);
					Colorizer cursorColor = { this->posX + LINE_NUMBER_SIZE + 1, 1, CURSOR };
					c.push_back(cursorColor);
					std::cout << this->padToLine(Helper::colorize(line, c), width);
				}
			}
			// Normal line
			else {
				std::string line = lineNumber + ' ' + *i;
				Colorizers c;
				c.push_back(lineNumberColor);
				std::cout << this->padToLine(Helper::colorize(line, c), width);
			}
		}
		count++;
	}
	// When content is too short
	while (count < height - NON_CONTENT_LINES) {
		std::cout << this->padToLine(" ", width);
		count++;
	}

	// Bottom line:
	std::string commandString = this->state == COMMAND ? "cmd| " : "key| ";
	Colorizer commandStringColor = { 0, commandString.size() - 1, MAGENTA };
	Colorizer commandArgsColor = { commandString.size(), this->lastKeys.size(), BACKGROUND };
	Colorizers c;
	c.push_back(commandStringColor);
	c.push_back(commandArgsColor);
	std::cout << this->padToLine(Helper::colorize(commandString + this->lastKeys, c), width);
	std::cout << this->padToLine(this->commandOutput, width);

	// Reset cursor position
	std::cout << (char)27 << '[' << height << 'A';
	std::cout << (char)27 << '[' << width + 1 << 'D';
}

std::string Display::padToLine(std::string line, short width) const
{
	std::string result = line;
	size_t end = line.size();
	while (Helper::getDisplayLength(result, 0, end) > width) end--;
	result = result.substr(0, end);
	result.insert(result.end(), width - Helper::getDisplayLength(result), ' ');
	return result;
}

void Display::callAction(char x)
{
	if (this->state == COMMAND) { // Ctrl C
		if (this->lastKeys.size() > 0 && this->lastKeys[this->lastKeys.size() - 1] == NEXT_IS_UTILS) {
			this->lastKeys = this->lastKeys.substr(0, this->lastKeys.size() - 1);
		}
		else if (x == ACTION_START_COMMAND) {
			this->lastKeys = "";
		}
		else if (x == ACTION_ENTER) {
			this->state = DEAFULT;
			this->commandOutput = this->c.runCommand(this->lastKeys, this->posX, this->posY, &this->wasEdited);
			this->lastKeys = "";
		}
		else if (x == ACTION_REMOVE && this->lastKeys.size() > 0) {
			this->lastKeys = this->lastKeys.substr(0, this->lastKeys.size() - 1);
		}
		else if (x == NEXT_IS_UTILS || x == NULL) {
			this->lastKeys += NEXT_IS_UTILS;
		}
		else if (Helper::isPrintable(x)) {
			this->lastKeys += x;
		}
	}
	else if (Content::oneClickActions.count(x)) { // One click actions
		auto f = Content::oneClickActions.find(x);
		this->wasEdited = (this->c.*(f->second))(this->posX, this->posY);
	}
	else if (this->lastKeys.size() > 0 && this->lastKeys[0] == NEXT_IS_UTILS) { // Util actions
		this->lastKeys = "";
		if (Content::utilActions.count(x)) {
			auto f = Content::utilActions.find(x);
			this->wasEdited = (this->c.*(f->second))(this->posX, this->posY);
		}
	}
	else if (x == -32 or x == 224 or x == 0) { // Next is utils
		this->lastKeys = "a";
		this->lastKeys[0] = NEXT_IS_UTILS;
	}
	else if (x == ACTION_START_COMMAND) { // Starting command
		this->state = COMMAND;
	}
	else if (Helper::isPrintable(x)) { // A normal character
		this->wasEdited = this->c.actionWrite(this->posX, this->posY, x);
	}
}


