#include "Display.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>

#define LINE_NUMBER_SIZE 4
#define NON_CONTENT_LINES 2
#define LINE_OFFSET_STR " << "

void Display::showTopBar(short width) const
{
	std::string timeString = Helper::getTimeString();
	Colorizer timeColor = { 0, timeString.size(), MAGENTA };
	std::string mid(2 * width / 5, ' ');
	std::string fileTitle = std::string(this->wasEdited ? "*" : "") + this->fileName;
	Colorizer fileColor = { timeString.size() + mid.size(), fileTitle.size(), BLUE };

	Colorizers c;
	c.push_back(timeColor);
	c.push_back(fileColor);
	std::cout << this->padToLine(Helper::colorize(timeString + mid + fileTitle, c), width) << std::endl;
}

Display::Display(std::string fname) : c(fname) {
	this->fileName = fname;
}

void Display::show() const
{
	std::vector<std::string> content = this->c.getLines();
	int count = 0;

	// Getting terminal dimensions
	short width = 0;
	short height = 0;
	Helper::getTerminalSize(&width, &height);

	// Top bar
	this->showTopBar(width);


	int offset = std::max(0, this->posX - (width / 2));
	Colorizer lineNumberColor = { 0, LINE_NUMBER_SIZE, LINE_NUMBER };
	Colorizer offsetLineColor = { LINE_NUMBER_SIZE, sizeof(LINE_OFFSET_STR) - 1, WHITE };

	// Aligning to top/bottom
	int startIndex = std::max(std::min(this->posY - ((height - NON_CONTENT_LINES) / 2), (int)content.size() - (height - NON_CONTENT_LINES)), 0);
	
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
	std::string commandString = "Command: ";
	Colorizer commandStringColor = { 0, commandString.size() - 1, MAGENTA };
	Colorizer commandArgsColor = { commandString.size(), this->lastKeys.size(), WHITE };
	Colorizers c;
	c.push_back(commandStringColor);
	c.push_back(commandArgsColor);
	std::cout << this->padToLine(Helper::colorize(commandString + this->lastKeys, c), width);

	// Reset cursor position
	std::cout << (char)27 << '[' << height << 'A';
	std::cout << (char)27 << '[' << width - 1 << 'D';
}

std::string Display::padToLine(std::string line, short width) const
{
	std::string result = line;
	while (Helper::getDisplayLength(result) > width) result = result.substr(0, result.size() - 1);
	result.insert(result.end(), width - Helper::getDisplayLength(result), ' ');
	return result;
}

void Display::callAction(char x)
{
	if (this->lastKeys.size() > 0 && this->lastKeys[0] == -32) {
		this->lastKeys = "";
		

		if (x == 'S') { // Delete
			this->wasEdited = this->c.actionDelete(this->posX, this->posY);
		}
		else if (x == -109) { // Ctrl Delete
			this->wasEdited = this->c.actionDeleteWord(this->posX, this->posY);
		}
		else if (x == 'K') { // Left key
			if (this->posX > 0) {
				this->posX -= 1;
			}
			else if (this->posY > 0) {
				this->posY -= 1;
				this->posX = this->c.getLine(this->posY).size();
			}
		}
		else if (x == 'M') { // Right key
			if (this->posX < this->c.getLine(this->posY).size()) {
				this->posX += 1;
			}
			else if (this->posY < this->c.size() - 1) {
				this->posY += 1;
				this->posX = 0;
			}
		}
		else if (x == 'H') { // Up key
			if (this->posY > 0) {
				this->posY -= 1;
				if (this->c.getLine(this->posY).size() < this->posX) {
					this->posX = this->c.getLine(this->posY).size();
				}
			}
			else {
				this->posX = 0;
			}
		}
		else if (x == 'P') { // Down key
			if (this->posY < this->c.size() - 1) {
				this->posY += 1;
				if (this->c.getLine(this->posY).size() < this->posX) {
					this->posX = this->c.getLine(this->posY).size();
				}
			}
			else {
				this->posX = this->c.getLine(this->posY).size();
			}
		}
		else if (x == 't') { // Ctrl + Right
			if (this->posX == this->c.getLine(this->posY).size() && this->posY < this->c.size() - 1) {
				this->posX = 0;
				this->posY++;
			}
			else {
				int firstType = isalnum(this->c.getLine(this->posY)[this->posX]);
				while (isalnum(this->c.getLine(this->posY)[this->posX]) == firstType) {
					if (this->posX == this->c.getLine(this->posY).size()) break;
					this->posX++;
				}
			}
		}
		else if (x == 's') { // Ctrl + Left
			if (this->posX == 0) {
				if (this->posY > 0) {
					this->posY--;
					this->posX = this->c.getLine(this->posY).size();
				}
			}
			else {
				int firstType = isalnum(this->c.getLine(this->posY)[this->posX - 1]);
				do {
					this->posX--;
				} while (this->posX > 1 && isalnum(this->c.getLine(this->posY)[this->posX - 1]) == firstType);
			}
		}
	}
	else if (this->lastKeys.size() > 0 && this->lastKeys[0] == 0) {
		this->lastKeys = "";
		if (x == -104) { // Alt Up
			this->wasEdited = this->c.actionMoveLineUp(this->posX, this->posY);
		}
		else if (x == -96) { // Alt Down
			this->wasEdited = this->c.actionMoveLineDown(this->posX, this->posY);
		}
	}
	else if (x == 19) { // Ctrl + S
		this->wasEdited = false;
		Helper::writeFile(this->fileName, this->c.getContent());
	}
	else if (x == '\r') { // Enter
		this->wasEdited = this->c.actionEnter(this->posX, this->posY);
	}
	else if (x == '\n') {
		this->wasEdited = this->c.actionEnterNewline(this->posX, this->posY);
	}
	else if (x == 8) { // Remove
		this->wasEdited = this->c.actionRemove(this->posX, this->posY);
	}
	else if (x == 127) { // Ctrl Remove
		this->wasEdited = this->c.actionRemoveWord(this->posX, this->posY);
	}
	else if (x == -32 or x == 224 or x == 0) {
		this->lastKeys = "a";
		this->lastKeys[0] = x;
	}
	else if (x == 6) {
		this->lastKeys = "";
		this->state = FIND;
	}
	else {
		this->wasEdited = this->c.actionWrite(this->posX, this->posY, x);
	}
}


