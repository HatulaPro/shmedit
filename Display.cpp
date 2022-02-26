#include "Display.h"
#include "addons/FileExplorer.h"
#include "helpers/Helper.h"
#include "helpers/FilesUtil.h"
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
#define NON_CONTENT_LINES 4
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

	std::cout << Helper::colorize(StringsVector(timeString, mid, fileTitle, filePadding, otherFiles), StylesVector(Style::MAGENTA, Style::RESET, Style::BLUE, Style::RESET, Style::DIMMED), width);

}

Display::Display(std::string fname) : addon(nullptr) {
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
	this->closeFileExplorer();
}

void Display::openFileExplorer()
{
	this->lastKeys = "";
	this->addon = new FileExplorer(*this, FilesUtil::getDirectoryName(this->contents[this->activeContent]->getFileName()));
}

void Display::closeFileExplorer()
{
	this->lastKeys = "";
	if (this->addon != nullptr) {
		delete this->addon;
		this->addon = nullptr;
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
	if (this->contents.size() == 1) this->closeAll();
	// delete this->contents[this->activeContent];
	this->contents.erase(this->contents.begin() + this->activeContent);
	this->activeContent = min(max(this->activeContent - 1, 0), this->contents.size() - 1);
}

void Display::closeAll()
{
	while (true) {
		system("cls");
		exit(0);
	}
}

void Display::saveAll()
{
	for (auto c : this->contents) c->actionSaveFile();
	this->closeAll();
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

	if (this->addon != nullptr) {
		this->addon->show(10, 5, width - 20, effectiveHeight - 5);
	}
	else {



		this->showTopBar(width, this->contents[this->activeContent]->getEditStatus());

		int offset = max(0, posX - (width / 2));
		int state = this->contents[this->activeContent]->getState();
		bool isInFindState = this->contents[this->activeContent]->isInFindState();

		// Aligning to top/bottom
		int startIndex = max(min(posY - (effectiveHeight / 2), (int)content.size() - effectiveHeight), 0);

		size_t commandInfoSize = this->contents[this->activeContent]->getCommandInfo().size();
		if (commandInfoSize && this->contents[this->activeContent]->getCommandInfo()[0] == '^') commandInfoSize--;

		for (auto i = content.begin() + startIndex; i != content.begin() + min(startIndex + height, (int)content.size()); i++) {
			std::vector<std::string> strs;
			std::vector<Style> styles;
			int lineIndex = count + startIndex;
			if (count < effectiveHeight) {
				// For every line in content (that is inside the view)
				std::string lineNumber = std::to_string(lineIndex);
				lineNumber += std::string(LINE_NUMBER_SIZE - Helper::getDisplayLength(lineNumber), ' ');

				// If cursor in line
				if (lineIndex == posY) {
					// If line is offset
					if (posX > width - LINE_NUMBER_SIZE - sizeof(LINE_OFFSET_STR) + 1) {
						std::string line = i->substr(offset);
						size_t cursorLocation = width / 2;
						Style cursorStyle = Style::CURSOR;
						std::string beforeCursor = line.substr(0, cursorLocation);
						std::string cursor(1, line[cursorLocation]);
						std::string afterCursor = line.size() > cursorLocation ? line.substr(cursorLocation + 1) : "";

						styles = StylesVector(Style::LINE_NUMBER, Style::RESET, Style::WHITE, Style::RESET, cursorStyle, Style::RESET);
						strs = StringsVector(lineNumber, " ", LINE_OFFSET_STR, beforeCursor, cursor, afterCursor);
						if (isInFindState) {
							styles[4] = Style::FIND_HIGHLIGHTING;
							cursor = line.substr(cursorLocation, commandInfoSize);
							afterCursor = afterCursor.substr(cursor.size() - 1);
							strs = StringsVector(lineNumber, " ", LINE_OFFSET_STR, beforeCursor, cursor, afterCursor);
						}
						else if (state == VISUAL) {
							if (startX < offset) {
								styles[3] = Style::VISUAL_STYLE;
							}
							else {
								std::string visualCursor = line.substr(cursorLocation - posX + startX, posX - startX);
								beforeCursor = line.substr(0, cursorLocation - posX + startX);

								styles = StylesVector(Style::LINE_NUMBER, Style::RESET, Style::WHITE, Style::RESET, Style::VISUAL_STYLE, cursorStyle, Style::RESET);
								strs = StringsVector(lineNumber, " ", LINE_OFFSET_STR, beforeCursor, visualCursor, cursor, afterCursor + " ");
							}
						}
					}
					// Not offset
					else {
						Style cursorStyle = Style::CURSOR;
						std::string beforeCursor = i->substr(0, posX);
						std::string visualCursor;
						std::string cursor(1, (*i)[posX]);
						std::string afterCursor = i->size() > posX ? i->substr(posX + 1) : "";

						if (isInFindState) {
							cursorStyle = Style::FIND_HIGHLIGHTING;
							cursor = i->substr(posX, commandInfoSize);
							afterCursor = afterCursor.substr(cursor.size() - 1);
						}
						else if (state == VISUAL) {
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
						strs = StringsVector(lineNumber, " ", beforeCursor, visualCursor, cursor, afterCursor, " ");
						styles = StylesVector(Style::LINE_NUMBER, Style::RESET, Style::RESET, Style::VISUAL_STYLE, cursorStyle, Style::RESET, Style::RESET);
					}
				}
				// Cursor not in line
				else {
					strs = StringsVector(lineNumber, " ", *i);
					styles = StylesVector(Style::LINE_NUMBER, Style::RESET, Style::RESET);
					if (state == VISUAL) {
						if (lineIndex == startY) {
							strs[2] = i->substr(0, startX);
							strs.push_back(i->substr(startX));
							styles.push_back(Style::VISUAL_STYLE);
						}
						else if (lineIndex > startY && lineIndex < posY) {
							styles[2] = Style::VISUAL_STYLE;
						}
					}
				}
				std::cout << Helper::colorize(strs, styles, width);
			}
			count++;
		}
		// When content is too short
		while (count < effectiveHeight) {
			std::cout << Helper::colorize(StringsVector(" "), StylesVector(Style::RESET), width);
			count++;
		}

		// Bottom line:
		std::string commandString = this->contents[this->activeContent]->getStateString();
		std::string commandData = this->contents[this->activeContent]->getCommandArgs(lastKeys);
		Style commandArgsStyle = commandData.size() && commandData[0] == BEGIN_CALLED_COMMAND ? Style::WHITE : Style::BACKGROUND;
		if (this->contents[this->activeContent]->isInFindState()) {
			commandArgsStyle = Style::WHITE;
		}

		std::cout << Helper::colorize(StringsVector(commandString, commandData, " "), StylesVector(Style::MAGENTA, commandArgsStyle, this->contents[this->activeContent]->getState() == COMMAND ? Style::CURSOR : Style::RESET), width);
		std::cout << Helper::colorize(StringsVector(this->commandOutput), StylesVector(Style::RESET), width)<< std::endl;
	}
	// Reset cursor position
	ConsoleUtils::setCursorPosition(1, 1);
}

void Display::callAction(int x)
{
	if (this->addon != nullptr) {
		this->addon->callAction(x, this->lastKeys, this->commandOutput);
	}
	else {
		this->contents[this->activeContent]->callAction(x, this->lastKeys, this->commandOutput);
	}
}


