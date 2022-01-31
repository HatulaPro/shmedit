#include "Display.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctype.h>
#include <chrono>

Display::Display(std::string fname) : c(fname) {
	this->fileName = fname;
}

void Display::show() const
{
	short x = 0;
	short y = 0;

	Helper::getTerminalSize(&x, &y);
	time_t t = time(nullptr);
	tm localTime;
	localtime_s(&localTime, &t);

	std::cout << this->padToLine(Helper::colorize((localTime.tm_hour < 10 ? "0" + std::to_string(localTime.tm_hour) : std::to_string(localTime.tm_hour)) + ':' + (localTime.tm_min < 10 ? "0" + std::to_string(localTime.tm_min) : std::to_string(localTime.tm_min)) + std::string(x / 3, ' ') + std::string(this->wasEdited ? "*" : "") + this->fileName, MAGENTA), x) << std::endl << std::endl;
	int count = 0;

	std::vector<std::string> content = this->c.getLines();

	int startIndex = std::max(this->posY - (y / 2), 0);
	if (this->posY > (int)content.size() - y + 15) {
		startIndex = std::max((int)content.size() - y + 5, 0);
	}
	for (auto i = content.begin() + startIndex; i != content.begin() + std::min(startIndex + y, (int)content.size()); i++) {
		if (count < y - 5) {
			std::string lineNumber = std::to_string(count + startIndex);
			lineNumber.insert(lineNumber.end(), 4 - Helper::getDisplayLength(lineNumber), ' ');
			if (count + startIndex == this->posY) {
				int offset = std::max(0, this->posX - (x / 2));
				if (offset > 0) {
					std::cout << Helper::colorize(lineNumber, LINE_NUMBER) << Helper::colorize(" << ", WHITE) << this->padToLine(Helper::setCursor(i->substr(offset), this->posX - offset), x - 8);
				}
				else {
					std::cout << this->padToLine(Helper::colorize(lineNumber, LINE_NUMBER) + ' ' + Helper::setCursor(*i, this->posX), x);
				}
			}
			else {
				std::cout << this->padToLine(Helper::colorize(lineNumber, LINE_NUMBER) + ' ' + *i, x);
			}
		}
		count++;
	}
	while (count < y - 5) {
		std::cout << this->padToLine(" ", x);
		count++;
	}

	std::cout << this->padToLine(Helper::colorize("Command: " + this->lastKeys, MAGENTA), x);

	std::cout << (char)27 << '[' << y << 'A';
	std::cout << (char)27 << '[' << x - 1 << 'D';
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

	//if (this->state == FIND) {
	//	if (x == '\n' || x == '\t' || x == '\r') {
	//		std::copy(this->content.begin(), this->content.end(), this->lastContent);
	//		for (int i = 0; i < this->content.size(); i++) {
	//			this->content[i] = Helper::replace(this->content[i], this->lastKeys, Helper::colorize(this->lastKeys, Style::BACKGROUND));
	//			//this->lastKeys.replace(this->lastKeys.begin(), this->lastKeys.end(), Helper::colorize(this->lastKeys, Style::BLUE));
	//		}
	//		//std::cout << "FINDING " << this->lastKeys;
	//		this->lastKeys = "";
	//		this->state = DEAFULT;
	//	}
	//	else {
	//		this->lastKeys += x;
	//	}
	//	return;
	//}
	//std::cout << x;
	if (this->lastKeys.size() > 0 && this->lastKeys[0] == -32) {
		this->lastKeys = "";

		if (x == 'S') { // Delete
			this->wasEdited = this->c.actionDelete(&this->posX, &this->posY);
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
			this->wasEdited = this->c.actionMoveLineUp(&this->posX, &this->posY);
		}
		else if (x == -96) { // Alt Down
			this->wasEdited = this->c.actionMoveLineDown(&this->posX, &this->posY);
		}
	}
	else if (x == 19) { // Ctrl + S
		this->wasEdited = false;
		Helper::writeFile(this->fileName, this->c.getContent());
	}
	else if (x == '\r' || x == '\n') { // Enter
		this->wasEdited = this->c.actionEnter(&this->posX, &this->posY);
	}
	else if (x == 8) { // Remove
		this->wasEdited = this->c.actionRemove(&this->posX, &this->posY);
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
		this->wasEdited = this->c.actionWrite(&this->posX, &this->posY, x);
	}
}


