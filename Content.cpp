#include "Content.h"
#include "helpers/Helper.h"
#include "helpers/FilesUtil.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctype.h>
#include <chrono>
#include "Config.h"
#include "helpers/ConsoleUtils.h"



Content::Content(std::string c, Display& d) : display(d)
{
	for (auto i : FilesUtil::getFilesInDirectory(c)) {
		if (Helper::insStrCompare(c, i)) {
			this->fileName = i;
			setContent(i);
			this->wasEdited = false;
			return;
		}
	}
	this->wasEdited = true;
	this->fileName = c;
	setContent(c);
}

void Content::setContent(std::string c)
{
	std::string tmp;
	std::stringstream cc(FilesUtil::readFile(c).c_str());
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

std::string Content::getCommandInfo() const
{
	return this->commandInfo;
}

std::string Content::getCommandArgs(std::string lastKeys) const
{
	if (this->state == DEAFULT) {
		return "";
	}
	else if (this->state == COMMAND) {
		return lastKeys;
	}
	else if (this->state == FIND) {
		return this->commandInfo;
	}
	else if (this->isInFindState()) {
		return this->commandInfo + '~' + this->commandInfo2;
	}
	else if (this->state == VISUAL) {
		return "#";
	}
	throw std::exception("Unkown state");
}

void Content::callAction(int x, std::string& lastKeys, std::string& commandOutput)
{
	commandOutput = "";

	if (x == ACTION_START_FIND) { // Ctrl + F for find
		lastKeys = std::string(1, BEGIN_CALLED_COMMAND) + COMMAND_FIND + ' ';
		this->setState(COMMAND);
		return;
	}
	else if (x == ACTION_START_FIND_AND_REPLACE) { // Ctrl + R for find and replace 
		lastKeys = std::string(1, BEGIN_CALLED_COMMAND) + COMMAND_FIND_AND_REPLACE + ' ';
		this->setState(COMMAND);
		return;
	}
	else if (x == ACTION_START_VISUAL) { // Ctrl + V for visual mode
		this->startX = this->posX;
		this->startY = this->posY;
		this->actionRightKey();
		lastKeys = "";
		this->setState(VISUAL);
		return;
	}
	if (x == ACTION_START_OPEN) { // Ctrl + O for find
		lastKeys = std::string(1, BEGIN_CALLED_COMMAND) + COMMAND_OPEN + ' ';
		this->setState(COMMAND);
		return;
	}
	else if (x == ACTION_START_COMMAND) { // Starting command
		this->setState(COMMAND);
		lastKeys = "";
		return;
	}
	else if (x == ACTION_START_DEAFULT) { // Starting default
		this->setState(DEAFULT);
		lastKeys = "";
		return;
	}

	if (this->getState() == VISUAL) { // Visual mode
		if (x == EXIT_CMD_MODE || x == ACTION_NEWLINE) {
			this->setState(DEAFULT);
		}
		if (Config::visualCommands.count(x)) { // All visual commands
			auto f = Config::visualCommands.find(x);
			(this->*(f->second.second))();
		}
		return;
	}
	else if (this->getState() == FIND) { // Find
		if (x == FIND_NEXT || x == ACTION_NEWLINE) {
			commandOutput = this->runCommand(COMMAND_FIND);
		}
		return;
	}
	if (this->isInFindState()) { // Find and replace
		if (x == FIND_NEXT || x == ACTION_NEWLINE) {
			commandOutput = this->runCommand(COMMAND_FIND_AND_REPLACE);
		}
		else if (x == FIND_AND_REPLACE_SKIP) {
			this->setState(FIND_AND_REPLACE_F);
			commandOutput = this->runCommand(COMMAND_FIND_AND_REPLACE);
		}
		return;
	}
	if (this->getState() == COMMAND) { // Command mode
		// Leave cmd mode
		if (lastKeys.size() == 0 && x == EXIT_CMD_MODE) {
			this->setState(DEAFULT);
		}
		else if (x == ACTION_NEWLINE) {
			this->setState(DEAFULT);
			// No command
			if (!lastKeys.size()) return;

			// Called command was... called
			if (lastKeys.find_first_not_of("0123456789") == std::string::npos) { // Just a number
				std::stringstream s(lastKeys);
				s >> this->posY;
				this->posY = min(this->posY, (int)this->size() - 1);
				this->posX = min(this->posX, (int)this->getLine(this->posY).size());
			}
			else if (lastKeys[0] == BEGIN_CALLED_COMMAND) {
				commandOutput = this->runCommand(lastKeys.substr(1));
			}
			// Command does not exist
			else {
				commandOutput = "`" + lastKeys + "` is not a called command";
			}
			lastKeys = "";
		}
		// Remove last character
		else if (x == ACTION_REMOVE && lastKeys.size() > 0) {
			lastKeys = lastKeys.substr(0, lastKeys.size() - 1);
		}
		// Normal character
		else if (Helper::isPrintable(x)) {
			lastKeys += x;
			int count = 1; // Number of times to run
			std::string command;
			std::stringstream tmp(lastKeys);
			if (isdigit(lastKeys[0])) {
				tmp >> count;
				count = max(1, count);
			}
			std::getline(tmp, command);

			if (Config::instantCommands.count(command)) {
				for (int i = 0; i < count; i++) {
					commandOutput = this->runCommand(command);
					lastKeys = "";
				}
			}
		}
		return;
	}
	if (Config::oneClickActions.count(x)) { // One click actions
		auto f = Config::oneClickActions.find(x);
		(this->*(f->second.second))();
	}
	else if (Helper::isPrintable(x)) { // A normal character
		this->actionWrite(x);
	}
}

void Content::getCursorPositions(int& posX, int& posY, int& startX, int& startY) const
{
	posX = this->posX;
	posY = this->posY;
	startX = this->startX;
	startY = this->startY;
}

int Content::getState() const
{
	return this->state;
}

bool Content::isInFindState() const
{
	return this->state == FIND_AND_REPLACE_F || this->state == FIND_AND_REPLACE_R || this->state == FIND;
}

std::string Content::getStateString() const
{
	if (this->state == DEAFULT) {
		return "key| ";
	}
	else if (this->state == COMMAND) {
		return "cmd| ";
	}
	else if (this->state == FIND) {
		return "find| ";
	}
	else if (this->isInFindState()) {
		return "find&rep| ";
	}
	else if (this->state == VISUAL) {
		return "visual| ";
	}
	throw std::exception("Unkown state");
}

void Content::setState(int state)
{
	this->state = state;
}

bool Content::getEditStatus() const
{
	return this->wasEdited;
}

void Content::actionDelete()
{
	if (posX < this->content[posY].size()) {
		this->content[posY] = this->content[posY].substr(0, posX) + this->content[posY].substr(posX + 1);
		this->wasEdited = true;
		return;
	}
	else if (posY + 1 < this->content.size()) {
		this->content[posY] += this->content[posY + 1];
		this->content.erase(this->content.begin() + (posY)+1);
		this->wasEdited = true;
		return;
	}
}

void Content::actionDeleteWord()
{
	if (posX == this->content[posY].size()) {
		this->actionDelete();
		return;
	}
	int firstType = Helper::isAlphanumeric(this->content[posY][posX]);
	int count = 0;
	while (Helper::isAlphanumeric(this->content[posY][posX + count]) == firstType) {
		if (posX + count == this->content[posY].size()) break;
		count++;
	}
	this->commandInfo = this->content[posY].substr(posX, count);
	this->content[posY].erase(posX, count);
	this->wasEdited = true;
}

void Content::actionMoveLineUp()
{
	if (posY > 0) {
		std::string tmp = this->content[posY];
		this->content.erase(this->content.begin() + (posY));
		this->content.insert(this->content.begin() + (posY)-1, tmp);
		(posY)--;
		this->wasEdited = true;
	}
}

void Content::actionMoveLineDown()
{
	if (posY < this->content.size() - 1) {
		std::string tmp = this->content[posY];
		this->content.erase(this->content.begin() + (posY));
		this->content.insert(this->content.begin() + (posY)+1, tmp);
		(posY)++;
		this->wasEdited = true;
	}
}

void Content::actionEnter()
{
	std::string beforeEnter = this->content[posY].substr(0, posX);
	int spaceCount = 0;
	while (spaceCount < beforeEnter.size() && beforeEnter[spaceCount] == ' ') spaceCount += 2;
	std::string afterEnter = this->content[posY].substr(posX);
	this->content[posY] = beforeEnter;
	posY += 1;
	this->content.insert(this->content.begin() + posY, std::string(spaceCount, ' ') + afterEnter);
	posX = spaceCount;
	this->wasEdited = true;
}

void Content::actionEnterNoSpacing()
{
	std::string beforeEnter = this->content[posY].substr(0, posX);
	std::string afterEnter = this->content[posY].substr(posX);
	this->content[posY] = beforeEnter;
	posY += 1;
	this->content.insert(this->content.begin() + posY, afterEnter);
	posX = 0;
	this->wasEdited = true;
}

void Content::actionEnterNewline()
{
	int spaceCount = 0;
	while (spaceCount < this->content[posY].size() && this->content[posY][spaceCount] == ' ') spaceCount += TAB_SIZE;
	posY += 1;
	this->content.insert(this->content.begin() + posY, std::string(spaceCount, ' '));
	posX = spaceCount;
	this->wasEdited = true;
}

void Content::actionRemove()
{
	if (posX > 0) {
		std::string beforeRemove = this->content[posY].substr(0, posX - 1);
		std::string afterRemove = this->content[posY].substr(posX);
		this->content[posY] = beforeRemove + afterRemove;
		posX -= 1;
		this->wasEdited = true;
	}
	else if (posY > 0) {
		posX = this->content[posY - 1].size();
		this->content[posY - 1] += this->content[posY];
		this->content.erase(this->content.begin() + posY);
		posY -= 1;
		this->wasEdited = true;
	}
}

void Content::actionRemoveWord()
{
	if (posX == 0) {
		this->actionRemove();
		return;
	}
	bool firstType = Helper::isAlphanumeric(this->content[posY][posX - 1]);
	int count = 0;
	while (posX - count >= 1 && Helper::isAlphanumeric(this->content[posY][posX - count - 1]) == firstType) {
		count++;
	}
	this->commandInfo = this->content[posY].substr(posX - count, count);
	this->content[posY].erase(posX - count, count);
	posX -= count;
	this->wasEdited = true;
}

void Content::actionWrite(char character)
{
	if (character == '\t') {
		for (int i = 0; i < TAB_SIZE; i++) {
			this->content[posY].insert(this->content[posY].begin() + posX, ' ');
		}
		posX += TAB_SIZE;
		this->wasEdited = true;
		return;
	}
	this->content[posY].insert(this->content[posY].begin() + posX, character);
	posX += 1;
	this->wasEdited = true;
}

void Content::actionLeftKey()
{
	if (posX > 0) {
		posX -= 1;
	}
	else if (posY > 0) {
		posY -= 1;
		posX = this->content[posY].size();
	}
}

void Content::actionRightKey()
{
	if (posX < this->content[posY].size()) {
		posX += 1;
	}
	else if (posY < this->content.size() - 1) {
		posY += 1;
		posX = 0;
	}
}

void Content::actionUpKey()
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
}

void Content::actionDownKey()
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
}

void Content::actionWordRight()
{
	if (posX == this->content[posY].size() && posY < this->content.size() - 1) {
		posX = 0;
		posY++;
	}
	else {
		int firstType = Helper::isAlphanumeric(this->content[posY][posX]);
		while (Helper::isAlphanumeric(this->content[posY][posX]) == firstType) {
			if (posX == this->content[posY].size()) break;
			posX++;
		}
	}
}

void Content::actionCopyWord()
{
	if (posX == this->content[posY].size() && posY < this->content.size() - 1) {
		this->commandInfo = "\n";
		return;
	}
	bool firstType = Helper::isAlphanumeric(this->content[posY][posX]);
	size_t count = 0;
	while (Helper::isAlphanumeric(this->content[posY][posX + count]) == firstType) {
		if (posX + count == this->content[posY].size()) break;
		count++;
	}
	this->commandInfo = this->content[posY].substr(posX, count);
}

void Content::actionCopyLine()
{
	this->commandInfo = this->content[posY];
}

void Content::actionCopyWordBack()
{
	if (posX == 0) {
		this->commandInfo = "\n";
		return;
	}
	bool firstType = Helper::isAlphanumeric(this->content[posY][posX - 1]);
	size_t count = 0;
	while (posX - count >= 1 && Helper::isAlphanumeric(this->content[posY][posX - count - 1]) == firstType) {
		count++;
	}
	this->commandInfo = this->content[posY].substr(posX - count, count);
}

void Content::actionPasteFromClipboard()
{
	std::string text = ConsoleUtils::getClipboardText();
	if (!text.size()) return;

	std::string tmp = this->commandInfo;
	this->commandInfo = text;
	this->actionPaste();
	this->commandInfo = tmp;
	this->wasEdited = true;
}

void Content::openNextFile()
{
	this->display.openNext();
}

void Content::openPreviousFile()
{
	this->display.openPrev();
}

void Content::actionCopySelection()
{
	if (posY == startY) {
		this->commandInfo = this->content[posY].substr(startX, posX - startX);
		return;
	}
	this->commandInfo = this->content[startY].substr(startX) + '\n';
	for (size_t i = startY + 1; i < posY; i++) {
		this->commandInfo += this->content[i] + '\n';
	}
	this->commandInfo += this->content[posY].substr(0, posX);
}

void Content::actionPasteSelection()
{
	this->actionPaste();
}

void Content::actionDeleteSelection()
{
	if (posY == startY) {
		this->content[posY].erase(startX, posX - startX);
		if (startX == this->content[posY].size()) startX--;
		posX = startX + 1;
		this->wasEdited = true;
		return;
	}

	this->content[startY] = this->content[startY].substr(0, startX) + this->content[posY].substr(posX);
	if (startY + 1 < posY) {
		this->content.erase(this->content.begin() + startY + 1, this->content.begin() + posY + 1);
	}
	else {
		this->content.erase(this->content.begin() + posY);
	}
	posY = startY;
	if (startX == this->content[posY].size() && startX > 0) startX--;
	posX = startX + 1;
	this->wasEdited = true;
}

void Content::actionLeftKeySelection()
{
	this->actionLeftKey();
	if (posY < startY) {
		this->state = DEAFULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEAFULT;
	}
}

void Content::actionRightKeySelection()
{
	this->actionRightKey();
}

void Content::actionUpKeySelection()
{
	this->actionUpKey();
	if (posY < startY) {
		this->state = DEAFULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEAFULT;
	}
}

void Content::actionDownKeySelection()
{
	this->actionDownKey();
}

void Content::actionJumpToLineEndSelection()
{
	this->actionJumpToLineEnd();
}

void Content::actionJumpToLineStartSelection()
{
	this->actionJumpToLineStart();
	if (posY < startY) {
		this->state = DEAFULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEAFULT;
	}
}

void Content::actionTabifySelection()
{
	for (size_t i = startY; i <= posY; i++) {
		this->content[i] = "    " + this->content[i];
	}
	this->wasEdited = true;
}

void Content::actionUntabifySelection()
{
	for (size_t i = startY; i <= posY; i++) {
		size_t count = 0;

		while (count < this->content[i].size() && isspace(this->content[i][count]) && count < TAB_SIZE) count++;

		if (count > 0) {
			if (i == startY) {
				startX = max(startX - (int)count, 0);
			}
			this->content[i] = this->content[i].substr(count);
			this->wasEdited = true;
		}
	}
}

void Content::actionWordRightSelection()
{
	this->actionWordRight();
}

void Content::actionWordLeftSelection()
{
	this->actionWordLeft();
	if (posY < startY) {
		this->state = DEAFULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEAFULT;
	}
}

void Content::actionMoveLineUpSelection()
{
	if (startY == 0) return;
	if (posY == startY) this->actionMoveLineUp();
	std::string tmp = this->content[startY - 1];
	this->content.erase(this->content.begin() + startY - 1);
	this->content.insert(this->content.begin() + posY, tmp);
	startY--;
	posY--;
	this->wasEdited = true;
}

void Content::actionMoveLineDownSelection()
{
	if (posY == this->content.size() - 1) return;
	if (posY == startY) this->actionMoveLineDown();

	std::string tmp = this->content[posY + 1];
	this->content.erase(this->content.begin() + posY + 1);
	this->content.insert(this->content.begin() + startY, tmp);
	startY++;
	posY++;
	this->wasEdited = true;
}

void Content::actionSelectLinesSelection()
{
	startX = 0;
	posX = this->content[posY].size();
}

void Content::actionPageUpSelection()
{
	this->actionPageUp();
}

void Content::actionPageDownSelection()
{
	this->actionPageDown();
}

void Content::actionDuplicateLine()
{
	this->content.insert(this->content.begin() + posY, this->content[posY]);
	posY++;
	this->wasEdited = true;
}

void Content::actionJumpToLineEnd()
{
	posX = this->content[posY].size();
}

void Content::actionJumpToLineStart()
{
	posX = 0;
}

void Content::actionTabify()
{
	this->content[posY] = "    " + this->content[posY];
	this->wasEdited = true;
}

void Content::actionUntabify()
{
	size_t count = 0;
	while (count < this->content[posY].size() && isspace(this->content[posY][count]) && count < TAB_SIZE) count++;

	if (count > 0) {
		posX = max(posX - (int)count, 0);
		this->content[posY] = this->content[posY].substr(count);
		this->wasEdited = true;
		return;
	}
}

void Content::actionPageUp()
{
	posY = max(posY - PAGE_UP_DOWN_SIZE, 0);
	posX = min(this->content[posY].size(), posX);
}

void Content::actionPageDown()
{
	posY = min(posY + PAGE_UP_DOWN_SIZE, this->content.size() - 1);
	posX = min(this->content[posY].size(), posX);
}

void Content::actionSaveFile()
{
	FilesUtil::writeFile(this->fileName, this->getContent());
	this->wasEdited = false;
}

void Content::actionWordLeft()
{
	if (posX == 0) {
		if (posY > 0) {
			posY--;
			posX = this->content[posY].size();
		}
	}
	else {
		int firstType = Helper::isAlphanumeric(this->content[posY][posX - 1]);
		do {
			posX--;
		} while (posX > 1 && Helper::isAlphanumeric(this->content[posY][posX - 1]) == firstType);
	}
}

void Content::actionQuit()
{
	this->display.closeActiveContent();
}

void Content::actionQuitAndSave()
{
	this->actionSaveFile();
	this->display.closeActiveContent();
}

std::string Content::commandOpen(std::string command)
{
	this->display.open(command);
	//*this = Content(command);
	return "Opened " + command;
}

std::string Content::commandFind(std::string command)
{
	if (this->state == FIND) {
		command = this->commandInfo;
	}
	else {
		this->commandInfo = command;
	}
	this->state = FIND;

	if (posX < this->content[posY].size()) {
		size_t found = this->content[posY].find(command);
		if (found != std::string::npos && found > posX) {
			posX = found;
			return "Found in " + std::to_string(posY) + ':' + std::to_string(posX);
		}
	}

	for (size_t i = 0; i < this->content.size(); i++) {
		size_t index = (posY + i + 1) % this->content.size();
		size_t found = this->content[index].find(command);
		if (found != std::string::npos) {
			posX = found;
			posY = index;
			return "Found in " + std::to_string(posY) + ':' + std::to_string(posX);
		}
	}
	this->state = DEAFULT;
	return "String not found.";
}

std::string Content::commandFindAndReplace(std::string command)
{
	if (this->state == FIND_AND_REPLACE_R) {
		std::string beforeTilda = this->commandInfo;
		std::string afterTilda = this->commandInfo2;

		this->content[posY].erase(posX, beforeTilda.size());
		this->content[posY].insert(posX, afterTilda);
		this->state = FIND_AND_REPLACE_F;
		return "Found.";
	}
	else if (this->state == DEAFULT) {
		size_t index = command.find_first_of('~');
		if (index == std::string::npos) {
			return "'~' sign not found. Can not parse request.";
		}

		std::string beforeTilda = command.substr(0, index);
		if (!beforeTilda.size()) {
			return "Can not find an empty string.";
		}
		std::string afterTilda = command.substr(index + 1);

		this->commandInfo = beforeTilda;
		this->commandInfo2 = afterTilda;

		// :fr test~not test
		this->commandFind(beforeTilda);

		this->state = FIND_AND_REPLACE_R;
		this->commandInfo = beforeTilda;
		return "Found.";
	}
	else if (this->state == FIND_AND_REPLACE_F) {
		this->commandFind(this->commandInfo);

		this->state = FIND_AND_REPLACE_R;
		return "Found.";
	}
	return "Unreachable";
}

void Content::actionPaste()
{
	if (!this->commandInfo.size()) return;

	bool gotSlashR = false;
	for (size_t i = 0; i < this->commandInfo.size(); i++) {
		if (commandInfo[i] == '\r') {
			gotSlashR = true;
		}
		else if (gotSlashR && commandInfo[i] == '\n') {
			this->actionEnterNoSpacing();
			gotSlashR = false;
		}
		else {
			this->actionWrite(commandInfo[i]);
			gotSlashR = false;
		}
	}
}

void Content::actionDeleteLine()
{
	this->wasEdited = true;
	if (this->content.size() == 1) {
		this->content[0] = "";
		posX = 0;
		return;
	}
	this->commandInfo = this->content[posY] + '\n';
	this->content.erase(this->content.begin() + posY);
	posY = min((int)posY, (int)this->content.size() - 1);
	posX = min((int)posX, (int)this->content[posY].size());
}

std::string Content::runCommand(std::string command)
{
	command = Helper::trim(command);
	if (command.size() == 0) {
		return "";
	}

	if (Config::instantCommands.count(command)) {
		auto f = Config::instantCommands.find(command);
		(this->*(f->second))();
		return "";
	}

	std::string commandName = command.substr(0, command.find_first_of(' '));
	std::string afterSpace = Helper::trim(command.substr(commandName.size()));


	if (Config::calledCommands.count(commandName)) {
		auto f = Config::calledCommands.find(commandName);
		return (this->*(f->second))(afterSpace);
	}
	return "Command not found";
}
