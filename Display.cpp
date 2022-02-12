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
	std::string mid(2 * width / 5, ' ');
	std::string fileTitle = std::string(wasEdited ? "*" : "") + this->contents[this->activeContent]->getFileName();
	std::string filePadding(max(16 - fileTitle.size(), 0), ' ');

	std::string otherFiles(12, ' ');
	for (size_t i = this->contents.size() - 1; i != 0; i--) {
		size_t index = (this->activeContent + i) % this->contents.size();
		if (index != this->activeContent) {
			otherFiles += (this->contents[index]->getEditStatus() ? "*" : "") + this->contents[index]->getFileName() + '|';
		}
	}

	std::string topBarContents = timeString + mid + fileTitle + otherFiles;
	if (Helper::getDisplayLength(topBarContents) > width) {
		topBarContents = topBarContents.substr(0, width - 3) + "...";
	}

	std::cout << Helper::padToLine(Helper::colorize(StringsVector(timeString, mid, fileTitle, filePadding, otherFiles), StylesVector(MAGENTA, RESET, BLUE, RESET, DIMMED)), width);
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
					std::string line = i->substr(offset);
					size_t cursorLocation = width / 2;
					Style cursorStyle = CURSOR;
					std::string beforeCursor = line.substr(0, cursorLocation);
					std::string visualCursor;
					std::string cursor(1, line[cursorLocation]);
					std::string afterCursor = line.size() > cursorLocation ? line.substr(cursorLocation + 1) : "";
					if (this->contents[this->activeContent]->getState() == FIND) {
						cursorStyle = FIND_HIGHLIGHTING;
						cursor = line.substr(cursorLocation, this->contents[this->activeContent]->getCommandInfo().size());
						afterCursor = afterCursor.substr(cursor.size() - 1);
						std::cout << Helper::padToLine(Helper::colorize(StringsVector(lineNumber, " ", LINE_OFFSET_STR, beforeCursor, cursor, afterCursor), StylesVector(LINE_NUMBER, RESET, WHITE, RESET, cursorStyle, RESET)), width);
					}
					else if (this->contents[this->activeContent]->getState() == VISUAL) {
						std::cout << Helper::padToLine(Helper::colorize(StringsVector(lineNumber, " ", LINE_OFFSET_STR, beforeCursor, cursor, afterCursor), StylesVector(LINE_NUMBER, RESET, WHITE, VISUAL_STYLE, CURSOR, RESET)), width);
					}
					else {
						std::cout << Helper::padToLine(Helper::colorize(StringsVector(lineNumber, " ", LINE_OFFSET_STR, beforeCursor, cursor, afterCursor), StylesVector(LINE_NUMBER, RESET, WHITE, RESET, cursorStyle, RESET)), width);
					}
				}
				// Not offset
				else {
					Style cursorStyle = CURSOR;
					std::string beforeCursor = i->substr(0, posX);
					std::string visualCursor;
					std::string cursor(1, (*i)[posX]);
					std::string afterCursor = i->size() > posX ? i->substr(posX + 1) : "";

					if (this->contents[this->activeContent]->isInFindState()) {
						cursorStyle = FIND_HIGHLIGHTING;
						cursor = i->substr(posX, this->contents[this->activeContent]->getCommandInfo().size());
						afterCursor = afterCursor.substr(cursor.size() - 1);
					}
					else if (this->contents[this->activeContent]->getState() == VISUAL) {
						if (startY == posY) {
							visualCursor = i->substr(startX, posX - startX);
							cursor = std::string(1, (*i)[posX]);
							beforeCursor = beforeCursor.substr(0, startX);
						}
						else {
							beforeCursor = "";
							visualCursor = i->substr(0, posX);
						}
					}
					std::cout << Helper::padToLine(Helper::colorize(StringsVector(lineNumber, " ", beforeCursor, visualCursor, cursor, afterCursor, " "), StylesVector(LINE_NUMBER, RESET, RESET, VISUAL_STYLE, cursorStyle, RESET, RESET)), width);
				}
			}
			// Normal line
			else {
				if (this->contents[this->activeContent]->getState() == VISUAL) {
					if (count + startIndex == startY) {
						std::string beforeCursor = i->substr(0, startX);
						std::string afterCursor = i->substr(startX);
						std::cout << Helper::padToLine(Helper::colorize(StringsVector(lineNumber, " ", beforeCursor, afterCursor), StylesVector(LINE_NUMBER, RESET, RESET, VISUAL_STYLE)), width);
					}
					else if (count + startIndex > startY && count + startIndex < posY) {
						std::cout << Helper::padToLine(Helper::colorize(StringsVector(lineNumber, " ", *i), StylesVector(LINE_NUMBER, RESET, VISUAL_STYLE)), width);
					}
					else {
						std::cout << Helper::padToLine(Helper::colorize(StringsVector(lineNumber, " ", *i), StylesVector(LINE_NUMBER, RESET, RESET)), width);
					}
				}
				else {
					std::cout << Helper::padToLine(Helper::colorize(StringsVector(lineNumber, " ", *i), StylesVector(LINE_NUMBER, RESET, RESET)), width);
				}
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
	std::string commandData = this->contents[this->activeContent]->getCommandArgs(lastKeys);
	Style commandArgsStyle = commandData.size() && commandData[0] == BEGIN_CALLED_COMMAND ? WHITE : BACKGROUND;
	if (this->contents[this->activeContent]->isInFindState()) {
		commandArgsStyle = WHITE;
	}

	std::cout << Helper::padToLine(Helper::colorize(StringsVector(commandString, commandData, " "), StylesVector(MAGENTA, commandArgsStyle, this->contents[this->activeContent]->getState() == COMMAND ? CURSOR : RESET)), width);
	std::cout << Helper::padToLine(this->commandOutput, width);


	// Reset cursor position
	ConsoleUtils::setCursorPosition(0, 0);
}

void Display::callAction(int x)
{
	this->contents[this->activeContent]->callAction(x, this->lastKeys, this->commandOutput);
}


