#include "Content.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctype.h>
#include <chrono>

const std::map<char, bool (Content::*)(int&, int&)> Content::utilActions = {
	{ACTION_DELETE, &Content::actionDelete},
	{ ACTION_CTRL_DELETE, &Content::actionDeleteWord },
	{ ACTION_LEFT_KEY, &Content::actionLeftKey },
	{ ACTION_RIGHT_KEY, &Content::actionRightKey },
	{ ACTION_UP_KEY, &Content::actionUpKey },
	{ ACTION_DOWN_KEY, &Content::actionDownKey },
	{ ACTION_CTRL_RIGHT_KEY, &Content::actionWordRight },
	{ ACTION_CTRL_LEFT_KEY, &Content::actionWordLeft },
	{ ACTION_ALT_UP, &Content::actionMoveLineUp },
	{ ACTION_ALT_DOWN, &Content::actionMoveLineDown },
};

const std::map<char, bool (Content::*)(int&, int&)> Content::oneClickActions = {
	{ ACTION_ENTER, &Content::actionEnter },
	{ ACTION_CTRL_ENTER, &Content::actionEnterNewline },
	{ ACTION_REMOVE, &Content::actionRemove },
	{ ACTION_CTRL_REMOVE, &Content::actionRemoveWord },
	{ ACTION_CTRL_S, &Content::actionSaveFile },
};

Content::Content(std::string c)
{
	this->fileName = c;
	setContent(c);
}

void Content::setContent(std::string c)
{
	std::string tmp;
	std::stringstream cc(Helper::readFile(c).c_str());
	this->content = std::vector<std::string>();
	while (std::getline(cc, tmp, '\n')) {
		this->content.push_back(Helper::replace(tmp, "\t", "  "));
	}

	if (this->content.size() == 0) {
		this->content.push_back("");
	}
}

std::vector<std::string> Content::getLines() const
{
	return this->content;
}

std::string Content::getLine(size_t index) const
{
	return this->content[index];
}

size_t Content::size() const
{
	return this->content.size();
}

std::string Content::getContent() const
{
	std::string result;
	for (auto i = this->content.begin(); i != this->content.end(); i++) {
		result += *i + '\n';
	}
	return result;
}

std::string Content::getFileName() const
{
	return this->fileName;
}

bool Content::actionDelete(int& posX, int& posY)
{
	if (posX < this->content[posY].size()) {
		this->content[posY] = this->content[posY].substr(0, posX) + this->content[posY].substr(posX + 1);
		return true;
	}
	else if (posY + 1 < this->content.size()) {
		this->content[posY] += this->content[posY + 1];
		this->content.erase(this->content.begin() + (posY)+1);
		return true;
	}
	return false;
}

bool Content::actionDeleteWord(int& posX, int& posY)
{
	if (posX == this->content[posY].size()) {
		return this->actionDelete(posX, posY);
	}
	int firstType = isalnum(this->content[posY][posX]);
	int count = 0;
	while (isalnum(this->content[posY][posX + count]) == firstType) {
		if (posX + count == this->content[posY].size()) break;
		count++;
	}
	this->content[posY].erase(posX, count);
	return true;
}

bool Content::actionMoveLineUp(int& posX, int& posY)
{
	if (posY > 0) {
		std::string tmp = this->content[posY];
		this->content.erase(this->content.begin() + (posY));
		this->content.insert(this->content.begin() + (posY)-1, tmp);
		(posY)--;
		return true;
	}
	return false;
}

bool Content::actionMoveLineDown(int& posX, int& posY)
{
	if (posY < this->content.size() - 1) {
		std::string tmp = this->content[posY];
		this->content.erase(this->content.begin() + (posY));
		this->content.insert(this->content.begin() + (posY)+1, tmp);
		(posY)++;
		return true;
	}
}

bool Content::actionEnter(int& posX, int& posY)
{
	std::string beforeEnter = this->content[posY].substr(0, posX);
	int spaceCount = 0;
	while (spaceCount < beforeEnter.size() && beforeEnter[spaceCount] == ' ') spaceCount += 2;
	std::string afterEnter = this->content[posY].substr(posX);
	this->content[posY] = beforeEnter;
	posY += 1;
	this->content.insert(this->content.begin() + posY, std::string(spaceCount, ' ') + afterEnter);
	posX = spaceCount;
	return true;
}

bool Content::actionEnterNewline(int& posX, int& posY)
{
	int spaceCount = 0;
	while (spaceCount < this->content[posY].size() && this->content[posY][spaceCount] == ' ') spaceCount += 2;
	posY += 1;
	this->content.insert(this->content.begin() + posY, std::string(spaceCount, ' '));
	posX = spaceCount;
	return true;
}

bool Content::actionRemove(int& posX, int& posY)
{
	if (posX > 0) {
		std::string beforeRemove = this->content[posY].substr(0, posX - 1);
		std::string afterRemove = this->content[posY].substr(posX);
		this->content[posY] = beforeRemove + afterRemove;
		posX -= 1;
		return true;
	}
	else if (posY > 0) {
		posX = this->content[posY - 1].size();
		this->content[posY - 1] += this->content[posY];
		this->content.erase(this->content.begin() + posY);
		posY -= 1;
		return true;
	}
}

bool Content::actionRemoveWord(int& posX, int& posY)
{
	if (posX == 0) {
		return this->actionRemove(posX, posY);
	}
	int firstType = isalnum(this->content[posY][posX - 1]);
	int count = 0;
	while (posX - count >= 1 && isalnum(this->content[posY][posX - count - 1]) == firstType) {
		count++;
	}
	this->content[posY].erase(posX - count, count);
	posX -= count;
	return true;
}

bool Content::actionWrite(int& posX, int& posY, char character)
{
	if (character == '\t') {
		for (int i = 0; i < TAB_SIZE; i++) {
			this->content[posY].insert(this->content[posY].begin() + posX, ' ');
		}
		posX += TAB_SIZE;
		return true;
	}
	this->content[posY].insert(this->content[posY].begin() + posX, character);
	posX += 1;
	return true;
}

bool Content::actionLeftKey(int& posX, int& posY)
{
	if (posX > 0) {
		posX -= 1;
	}
	else if (posY > 0) {
		posY -= 1;
		posX = this->content[posY].size();
	}
	return false;
}

bool Content::actionRightKey(int& posX, int& posY)
{
	if (posX < this->content[posY].size()) {
		posX += 1;
	}
	else if (posY < this->content.size() - 1) {
		posY += 1;
		posX = 0;
	}
	return false;
}

bool Content::actionUpKey(int& posX, int& posY)
{
	if (posY > 0) {
		posY -= 1;
		if (this->content[posY].size() < posX) {
			posX = this->content[posY].size();
		}
	}
	else {
		posX = 0;
	}
	return false;
}

bool Content::actionDownKey(int& posX, int& posY)
{
	if (posY < this->content.size() - 1) {
		posY += 1;
		if (this->content[posY].size() < posX) {
			posX = this->content[posY].size();
		}
	}
	else {
		posX = this->content[posY].size();
	}
	return false;
}

bool Content::actionWordRight(int& posX, int& posY)
{
	if (posX == this->content[posY].size() && posY < this->content.size() - 1) {
		posX = 0;
		posY++;
	}
	else {
		int firstType = isalnum(this->content[posY][posX]);
		while (isalnum(this->content[posY][posX]) == firstType) {
			if (posX == this->content[posY].size()) break;
			posX++;
		}
	}
	return false;
}

bool Content::actionSaveFile(int& posX, int& posY)
{
	Helper::writeFile(this->fileName, this->getContent());
	return false;
}

bool Content::actionWordLeft(int& posX, int& posY)
{
	if (posX == 0) {
		if (posY > 0) {
			posY--;
			posX = this->content[posY].size();
		}
	}
	else {
		int firstType = isalnum(this->content[posY][posX - 1]);
		do {
			posX--;
		} while (posX > 1 && isalnum(this->content[posY][posX - 1]) == firstType);
	}
	return false;
}

#define COMMAND_SAVE "s"
#define COMMAND_OPEN "o"
#define COMMAND_QUIT "q"
#define COMMAND_QUIT_AND_SAVE "qs"
std::string Content::runCommand(std::string command, int& posX, int& posY, void* arg)
{
	command = Helper::trim(command);
	if (command.size() == 0) {
		return "";
	}

	std::string commandName = command.substr(0, command.find_first_of(' '));
	std::string afterSpace =  Helper::trim(command.substr(commandName.size()));

	if (commandName == COMMAND_SAVE) {
		*(bool*)arg = this->actionSaveFile(posX, posY);
		return "File Saved.";
	}
	else if (commandName == COMMAND_QUIT) {
		system("cls");
		exit(0);
	}
	else if (commandName == "qs") {
		this->actionSaveFile(posX, posY);
		system("cls");
		exit(0);
	}
	else if (commandName == COMMAND_OPEN) {
		*this = Content(afterSpace);
		return "Opened " + afterSpace;
	}
	return "Command not found.";
}
