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

void Display::showTopBar(short width, bool wasEdited) const
{
	std::string timeString = Helper::getTimeString();
	Colorizer timeColor = { 0, timeString.size(), MAGENTA };
	std::string mid(2 * width / 5, ' ');
	std::string fileTitle = std::string(wasEdited ? "*" : "") + this->c.getFileName();
	Colorizer fileColor = { timeString.size() + mid.size(), fileTitle.size(), BLUE };

	Colorizers c;
	c.push_back(timeColor);
	c.push_back(fileColor);
	std::cout << this->padToLine(Helper::colorize(timeString + mid + fileTitle, c), width);
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

	this->showTopBar(width, this->c.getEditStatus());

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
					Colorizers c;
					std::string line = lineNumber + LINE_OFFSET_STR + i->substr(offset) + ' ';

					size_t cursorLocation = width / 2 + LINE_NUMBER_SIZE + sizeof(LINE_OFFSET_STR) - 1;
					Colorizer cursorColor = { cursorLocation , 1, CURSOR };

					c.push_back(lineNumberColor);
					c.push_back(offsetLineColor);
					if (this->c.getState() == FIND) {
						cursorColor.style = FIND_HIGHLIGHTING;
						cursorColor.count = this->c.getCommandInfo().size();
					}
					else if (this->c.getState() == VISUAL) {
						Colorizer visualCursorColor = { LINE_NUMBER_SIZE + sizeof(LINE_OFFSET_STR) - 1, width / 2, VISUAL_STYLE }; // First cursor
						if (this->startY == this->posY && this->startX > this->posX - (width / 2)) {
							visualCursorColor.begin = cursorLocation - this->posX + this->startX;
							visualCursorColor.count = this->posX - this->startX;
						}
						c.push_back(visualCursorColor);
					}
					c.push_back(cursorColor);
					std::cout << this->padToLine(Helper::colorize(line, c), width);
				}
				// Not offset
				else {
					std::string line = lineNumber + ' ' + *i + ' ';
					Colorizers c;
					c.push_back(lineNumberColor);
					Colorizer cursorColor = { this->posX + LINE_NUMBER_SIZE + 1, 1, CURSOR };
					if (this->c.isInFindState()) {
						cursorColor.style = FIND_HIGHLIGHTING;
						cursorColor.count = this->c.getCommandInfo().size();
					}
					else if (this->c.getState() == VISUAL) {
						Colorizer visualCursorColor = { LINE_NUMBER_SIZE + 1, this->posX, VISUAL_STYLE }; // First cursor
						if (this->startY == this->posY) {
							visualCursorColor.begin += this->startX;
							visualCursorColor.count -= this->startX;
						}
						c.push_back(visualCursorColor);
					}
					c.push_back(cursorColor);
					std::cout << this->padToLine(Helper::colorize(line, c), width);
				}
			}
			// Normal line
			else {
				std::string line = lineNumber + ' ' + *i;
				Colorizers c;
				c.push_back(lineNumberColor);
				if (this->c.getState() == VISUAL) {
					if (count + startIndex == this->startY) {
						Colorizer visualCursorColor = { this->startX + LINE_NUMBER_SIZE + 1, i->size() - this->startX, VISUAL_STYLE }; // First cursor
						c.push_back(visualCursorColor);
					}
					else if (count + startIndex > this->startY && count + startIndex < this->posY) {
						Colorizer visualCursorColor = { LINE_NUMBER_SIZE + 1, i->size(), VISUAL_STYLE }; // First cursor
						c.push_back(visualCursorColor);
					}
				}
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
	std::string commandString = this->c.getStateString();
	Colorizer commandStringColor = { 0, commandString.size() - 1, MAGENTA };
	std::string commandData = this->c.getCommandArgs(lastKeys);
	Style commandArgsStyle = commandData.size() && commandData[0] == BEGIN_CALLED_COMMAND ? WHITE : BACKGROUND;
	if (this->c.isInFindState()) {
		commandArgsStyle = WHITE;
	}

	Colorizer commandArgsColor = { commandString.size(), commandData.size(),  commandArgsStyle };
	Colorizers c;
	c.push_back(commandStringColor);
	c.push_back(commandArgsColor);
	std::cout << this->padToLine(Helper::colorize(commandString + commandData, c), width);
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
	this->commandOutput = "";

	if (x == -32 or x == 224 or x == 0) { // Next is utils
		this->lastKeys = "a";
		this->lastKeys[0] = NEXT_IS_UTILS;
		return;
	}

	if (x == ACTION_START_FIND) { // Ctrl + F for find
		this->lastKeys = std::string(1, BEGIN_CALLED_COMMAND) + COMMAND_FIND + ' ';
		this->c.setState(COMMAND);
		return;
	}
	else if (x == ACTION_START_FIND_AND_REPLACE) { // Ctrl + R for find and replace 
		this->lastKeys = std::string(1, BEGIN_CALLED_COMMAND) + COMMAND_FIND_AND_REPLACE + ' ';
		this->c.setState(COMMAND);
		return;
	}
	else if (x == ACTION_START_VISUAL) { // Ctrl + V for visual mode
		this->startX = this->posX;
		this->startY = this->posY;
		this->c.actionRightKey(this->posX, this->posY);
		this->lastKeys = "";
		this->c.setState(VISUAL);
		return;
	}
	else if (x == ACTION_START_COMMAND) { // Starting command
		this->c.setState(COMMAND);
		this->lastKeys = "";
		return;
	}
	else if (x == ACTION_START_DEAFULT) { // Starting default
		this->c.setState(DEAFULT);
		this->lastKeys = "";
		return;
	}

	if (this->c.getState() == VISUAL) { // Visual mode
		if (x == EXIT_CMD_MODE || x == ACTION_ENTER) {
			this->c.setState(DEAFULT);
		}
		if (Content::visualCommands.count(x)) { // All visual commands
			auto f = Content::visualCommands.find(x);
			(this->c.*(f->second))(this->posX, this->posY, this->startX, this->startY);
		}
		return;
	}
	else if (this->c.getState() == FIND) { // Find
		if (x == FIND_NEXT || x == ACTION_ENTER) {
			this->commandOutput = this->c.runCommand(COMMAND_FIND, this->posX, this->posY);
		}
		else if (x != NEXT_IS_UTILS && x != NULL) {
			this->c.setState(DEAFULT);
		}
		return;
	}
	if (this->c.isInFindState()) { // Find and replace
		if (x == FIND_NEXT || x == ACTION_ENTER) {
			this->commandOutput = this->c.runCommand(COMMAND_FIND_AND_REPLACE, this->posX, this->posY);
		}
		else if (x == FIND_AND_REPLACE_SKIP) {
			this->c.setState(FIND_AND_REPLACE_F);
			this->commandOutput = this->c.runCommand(COMMAND_FIND_AND_REPLACE, this->posX, this->posY);
		}
		else if (x != NEXT_IS_UTILS && x != NULL) {
			this->c.setState(DEAFULT);
		}
		return;
	}
	if (this->c.getState() == COMMAND) { // Command mode
		// Leave cmd mode
		if (this->lastKeys.size() == 0 && x == EXIT_CMD_MODE) {
			this->c.setState(DEAFULT);
		}
		// let the program know that a weird key was pressed
		else if (this->lastKeys.size() > 0 && this->lastKeys[this->lastKeys.size() - 1] == NEXT_IS_UTILS) {
			this->lastKeys = this->lastKeys.substr(0, this->lastKeys.size() - 1);
		}
		else if (x == ACTION_ENTER) {
			this->c.setState(DEAFULT);
			// No command
			if (!this->lastKeys.size()) return;

			// Called command was... called
			if (this->lastKeys.find_first_not_of("0123456789") == std::string::npos) { // Just a number
				std::stringstream s(this->lastKeys);
				s >> this->posY;
				this->posY = std::min(this->posY, (int)this->c.size() - 1);
				this->posX = std::min(this->posX, (int)this->c.getLine(this->posY).size());
			}
			else if (this->lastKeys[0] == BEGIN_CALLED_COMMAND) {
				this->commandOutput = this->c.runCommand(this->lastKeys.substr(1), this->posX, this->posY);
			}
			// Command does not exist
			else {
				this->commandOutput = "`" + this->lastKeys + "` is not a called command";
			}
			this->lastKeys = "";
		}
		// Remove last character
		else if (x == ACTION_REMOVE && this->lastKeys.size() > 0) {
			this->lastKeys = this->lastKeys.substr(0, this->lastKeys.size() - 1);
		}
		// let the program know that a weird key was pressed 
		else if (x == NEXT_IS_UTILS || x == NULL) {
			this->lastKeys += NEXT_IS_UTILS;
		}
		// Normal character
		else if (Helper::isPrintable(x)) {
			this->lastKeys += x;
			int count = 1; // Number of times to run
			std::string command;
			std::stringstream tmp(this->lastKeys);
			if (isdigit(this->lastKeys[0])) {
				tmp >> count;
				count = std::max(1, count);
			}
			std::getline(tmp, command);

			if (Content::instantCommands.count(command)) {
				for (int i = 0; i < count; i++) {
					this->commandOutput = this->c.runCommand(command, this->posX, this->posY);
					this->lastKeys = "";
				}
			}
		}
		return;
	}
	if (Content::oneClickActions.count(x)) { // One click actions
		auto f = Content::oneClickActions.find(x);
		(this->c.*(f->second))(this->posX, this->posY);
	}
	else if (this->lastKeys.size() > 0 && this->lastKeys[0] == NEXT_IS_UTILS) { // Util actions
		this->lastKeys = "";
		if (Content::utilActions.count(x)) {
			auto f = Content::utilActions.find(x);
			(this->c.*(f->second))(this->posX, this->posY);
		}
	}
	else if (Helper::isPrintable(x)) { // A normal character
		this->c.actionWrite(this->posX, this->posY, x);
	}
}


