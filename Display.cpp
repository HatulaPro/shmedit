#include "Display.h"
#include "helpers/Helper.h"
#include "helpers/ConsoleUtils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <map>
#include <thread>
#include "Config.h"

#define LINE_NUMBER_SIZE 4
#define NON_CONTENT_LINES 3
#define LINE_OFFSET_STR " << "

void Display::showTopBar(short width, bool wasEdited) const
{
	std::string timeString = Helper::getTimeString();
	Colorizer timeColor = { 0, timeString.size(), MAGENTA };
	std::string mid(2 * width / 5, ' ');
	std::string fileTitle = Helper::padToLine(std::string(wasEdited ? "*" : "") + this->contents[this->activeContent]->getFileName(), 16);
	Colorizer fileColor = { timeString.size() + mid.size(), fileTitle.size(), BLUE };

	std::string otherFiles(12, ' ');
	//otherFiles += '(' + std::to_string(this->contents.size()) + ')';
	for (size_t i = this->contents.size() - 1; i != 0; i--) {
		size_t index = (this->activeContent + i) % this->contents.size();
		if (index != this->activeContent) {
			otherFiles += (this->contents[index]->getEditStatus() ? "*" : "") + this->contents[index]->getFileName() + '|';
		}
	}
	Colorizer otherFilesColor = { timeString.size() + mid.size() + fileTitle.size(), otherFiles.size(), DIMMED };

	std::string topBarContents = timeString + mid + fileTitle + otherFiles;
	if (Helper::getDisplayLength(topBarContents) > width) {
		topBarContents = topBarContents.substr(0, width - 3) + "...";
		otherFilesColor.count = width - (timeString.size() + mid.size() + fileTitle.size());
	}

	Colorizers c;
	c.push_back(timeColor);
	c.push_back(fileColor);
	c.push_back(otherFilesColor);
	std::cout << Helper::padToLine(Helper::colorize(topBarContents, c), width);
}

Display::Display(std::string fname) {
	this->contents.push_back(new Content(fname, *this));
	this->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	ConsoleUtils::hideCursor();
	ConsoleUtils::getCursorPosition(this->hConsole, this->cursorPosition);
}

Display::~Display()
{
	for (size_t i = 0; i < this->contents.size(); i++) {
		delete this->contents[i];
	}
}

void Display::open(std::string fname)
{
	Content* c = new Content(fname, *this);
	for (size_t i = 0; i < this->contents.size(); i++) {
		if (c->getFileName() == this->contents[i]->getFileName()) {
			this->activeContent = i;
			delete c;
			return;
		}
	}
	this->contents.push_back(c);
	this->activeContent = this->contents.size() - 1;
}

void Display::openNext()
{
	this->activeContent++;
	this->activeContent = this->activeContent % this->contents.size();
}

void Display::openPrev()
{
	this->activeContent += this->contents.size() - 1;
	this->activeContent = this->activeContent % this->contents.size();
}

void Display::closeActiveContent()
{
	if (this->contents.size() == 1) {
		system("cls");
		exit(0);
	}
	// delete this->contents[this->activeContent];
	this->contents.erase(this->contents.begin() + this->activeContent);
	this->activeContent = min(max(this->activeContent - 1, 0), this->contents.size());
}

void Display::show() const
{
	int posX, posY, startX, startY;
	this->contents[this->activeContent]->getCursorPositions(posX, posY, startX, startY);


	std::vector<std::string> content = this->contents[this->activeContent]->getLines();
	int count = 0;

	// Getting terminal dimensions
	short width = 0;
	short height = 0;
	ConsoleUtils::getTerminalSize(this->hConsole, &width, &height);
	short effectiveHeight = height - NON_CONTENT_LINES;

	this->showTopBar(width, this->contents[this->activeContent]->getEditStatus());

	int offset = max(0, posX - (width / 2));
	Colorizer lineNumberColor = { 0, LINE_NUMBER_SIZE, LINE_NUMBER };
	Colorizer offsetLineColor = { LINE_NUMBER_SIZE, sizeof(LINE_OFFSET_STR) - 1, WHITE };

	// Aligning to top/bottom
	int startIndex = max(min(posY - (effectiveHeight / 2), (int)content.size() - effectiveHeight), 0);

	for (auto i = content.begin() + startIndex; i != content.begin() + min(startIndex + height, (int)content.size()); i++) {
		if (count < height - NON_CONTENT_LINES) {
			// For every line in content (that is inside the view)
			std::string lineNumber = std::to_string(count + startIndex);
			lineNumber.insert(lineNumber.end(), LINE_NUMBER_SIZE - Helper::getDisplayLength(lineNumber), ' ');

			// If cursor in line
			if (count + startIndex == posY) {
				// If line is offset
				if (posX > width - LINE_NUMBER_SIZE - sizeof(LINE_OFFSET_STR) + 1) {
					Colorizers c;
					std::string line = lineNumber + LINE_OFFSET_STR + i->substr(offset) + ' ';

					size_t cursorLocation = width / 2 + LINE_NUMBER_SIZE + sizeof(LINE_OFFSET_STR) - 1;
					Colorizer cursorColor = { cursorLocation , 1, CURSOR };

					c.push_back(lineNumberColor);
					c.push_back(offsetLineColor);
					if (this->contents[this->activeContent]->getState() == FIND) {
						cursorColor.style = FIND_HIGHLIGHTING;
						cursorColor.count = this->contents[this->activeContent]->getCommandInfo().size();
					}
					else if (this->contents[this->activeContent]->getState() == VISUAL) {
						Colorizer visualCursorColor = { LINE_NUMBER_SIZE + sizeof(LINE_OFFSET_STR) - 1, width / 2, VISUAL_STYLE }; // First cursor
						if (startY == posY && startX > posX - (width / 2)) {
							visualCursorColor.begin = cursorLocation - posX + startX;
							visualCursorColor.count = posX - startX;
						}
						c.push_back(visualCursorColor);
					}
					c.push_back(cursorColor);
					std::cout << Helper::padToLine(Helper::colorize(line, c), width);
				}
				// Not offset
				else {
					std::string line = lineNumber + ' ' + *i + ' ';
					Colorizers c;
					c.push_back(lineNumberColor);
					Colorizer cursorColor = { posX + LINE_NUMBER_SIZE + 1, 1, CURSOR };
					if (this->contents[this->activeContent]->isInFindState()) {
						cursorColor.style = FIND_HIGHLIGHTING;
						cursorColor.count = this->contents[this->activeContent]->getCommandInfo().size();
					}
					else if (this->contents[this->activeContent]->getState() == VISUAL) {
						Colorizer visualCursorColor = { LINE_NUMBER_SIZE + 1, posX, VISUAL_STYLE }; // First cursor
						if (startY == posY) {
							visualCursorColor.begin += startX;
							visualCursorColor.count -= startX;
						}
						c.push_back(visualCursorColor);
					}
					c.push_back(cursorColor);
					std::cout << Helper::padToLine(Helper::colorize(line, c), width);
				}
			}
			// Normal line
			else {
				std::string line = lineNumber + ' ' + *i;
				Colorizers c;
				c.push_back(lineNumberColor);
				if (this->contents[this->activeContent]->getState() == VISUAL) {
					if (count + startIndex == startY) {
						Colorizer visualCursorColor = { startX + LINE_NUMBER_SIZE + 1, i->size() - startX, VISUAL_STYLE }; // First cursor
						c.push_back(visualCursorColor);
					}
					else if (count + startIndex > startY && count + startIndex < posY) {
						Colorizer visualCursorColor = { LINE_NUMBER_SIZE + 1, i->size(), VISUAL_STYLE }; // First cursor
						c.push_back(visualCursorColor);
					}
				}
				std::cout << Helper::padToLine(Helper::colorize(line, c), width);
			}
		}
		count++;
	}
	// When content is too short
	while (count < height - NON_CONTENT_LINES) {
		std::cout << Helper::padToLine(" ", width);
		count++;
	}

	// Bottom line:
	std::string commandString = this->contents[this->activeContent]->getStateString();
	Colorizer commandStringColor = { 0, commandString.size() - 1, MAGENTA };
	std::string commandData = this->contents[this->activeContent]->getCommandArgs(lastKeys);
	Style commandArgsStyle = commandData.size() && commandData[0] == BEGIN_CALLED_COMMAND ? WHITE : BACKGROUND;
	if (this->contents[this->activeContent]->isInFindState()) {
		commandArgsStyle = WHITE;
	}

	Colorizer commandArgsColor = { commandString.size(), commandData.size(),  commandArgsStyle };
	Colorizer commandCursor = { commandString.size() + commandData.size(), 1,  CURSOR };
	Colorizers c;
	c.push_back(commandStringColor);
	c.push_back(commandArgsColor);
	if (this->contents[this->activeContent]->getState() == COMMAND) {
		c.push_back(commandCursor);
	}
	std::cout << Helper::padToLine(Helper::colorize(commandString + commandData + ' ', c), width);
	std::cout << Helper::padToLine(this->commandOutput, width);

	// Reset cursor position

	ConsoleUtils::setCursorPosition(0, 0);
}

void Display::callAction(int x)
{
	this->contents[this->activeContent]->callAction(x, this->lastKeys, this->commandOutput);
}


