#include "Content.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctype.h>
#include <chrono>

const std::map<char, void (Content::*)(int&, int&)> Content::utilActions = {
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
	{ ACTION_FN_RIGHT, &Content::actionJumpToLineEnd },
	{ ACTION_FN_LEFT, &Content::actionJumpToLineStart },
	{ ACTION_PAGE_UP, &Content::actionPageUp },
	{ ACTION_PAGE_DOWN, &Content::actionPageDown },
};

const std::map<char, void (Content::*)(int&, int&)> Content::oneClickActions = {
	{ ACTION_ENTER, &Content::actionEnter },
	{ ACTION_CTRL_ENTER, &Content::actionEnterNewline },
	{ ACTION_REMOVE, &Content::actionRemove },
	{ ACTION_CTRL_REMOVE, &Content::actionRemoveWord },
	{ ACTION_CTRL_S, &Content::actionSaveFile },
	{ ACTION_CTRL_D, &Content::actionDuplicateLine },
	{ ACTION_CTRL_L, &Content::actionDeleteLine },
	{ ACTION_TABIFY, &Content::actionTabify },
	{ ACTION_UNTABIFY, &Content::actionUntabify },
};

const std::map<std::string, std::string(Content::*)(std::string, int&, int&)> Content::calledCommands = {
	{COMMAND_OPEN, &Content::commandOpen},
	{COMMAND_FIND, &Content::commandFind},
	{COMMAND_FIND_AND_REPLACE, &Content::commandFindAndReplace},
};

const std::map<std::string, void(Content::*)(int&, int&)> Content::instantCommands = {
	{ COMMAND_SAVE, &Content::actionSaveFile },
	{ COMMAND_QUIT, &Content::actionQuit },
	{ COMMAND_QUIT_AND_SAVE, &Content::actionQuitAndSave },
	{ COMMAND_PASTE, &Content::actionPaste },
	{ COMMAND_DELETE_WORD, &Content::actionDeleteWord },
	{ COMMAND_REMOVE_WORD, &Content::actionRemoveWord },
	{ COMMAND_DELETE_LINE, &Content::actionDeleteLine },
	{ COMMAND_MOVE_WORD, &Content::actionWordRight },
	{ COMMAND_BACK_WORD, &Content::actionWordLeft },
	{ COMMAND_TABIFY, &Content::actionTabify },
	{ COMMAND_UNTABIFY, &Content::actionUntabify },
	{ COMMAND_LEFT_KEY, &Content::actionLeftKey },
	{ COMMAND_RIGHT_KEY, &Content::actionRightKey },
	{ COMMAND_UP_KEY, &Content::actionUpKey },
	{ COMMAND_DOWN_KEY, &Content::actionDownKey },
	{ COMMAND_COPY_WORD, &Content::actionCopyWord },
	{ COMMAND_COPY_LINE, &Content::actionCopyLine },
	{ COMMAND_COPY_WORD_BACK, &Content::actionCopyWordBack },
};

const std::map<char, void(Content::*)(int&, int&, int&, int&)> Content::visualCommands = {
	{ ACTION_COPY_SELECTION, &Content::actionCopySelection },
	{ ACTION_PASTE_SELECTION, &Content::actionPasteSelection },
	{ ACTION_REMOVE, &Content::actionDeleteSelection },
	{ ACTION_REMOVE_SELECTION, &Content::actionDeleteSelection },
	{ ACTION_REMOVE_SELECTION_ALT, &Content::actionDeleteSelection },
	{ ACTION_TABIFY, &Content::actionTabifySelection },
	{ ACTION_UNTABIFY, &Content::actionUntabifySelection },
	{ ACTION_LEFT_KEY, &Content::actionLeftKeySelection },
	{ ACTION_RIGHT_KEY, &Content::actionRightKeySelection },
	{ ACTION_UP_KEY, &Content::actionUpKeySelection },
	{ ACTION_DOWN_KEY, &Content::actionDownKeySelection },
	{ ACTION_FN_RIGHT, &Content::actionJumpToLineEndSelection  },
	{ ACTION_FN_LEFT, &Content::actionJumpToLineStartSelection  },
	{ ACTION_CTRL_RIGHT_KEY, &Content::actionWordRightSelection  },
	{ ACTION_CTRL_LEFT_KEY, &Content::actionWordLeftSelection  },
	{ ACTION_ALT_UP, &Content::actionMoveLineUpSelection },
	{ ACTION_ALT_DOWN, &Content::actionMoveLineDownSelection },
	{ ACTION_SELECT_LINES, &Content::actionSelectLinesSelection },
	{ ACTION_PAGE_UP, &Content::actionPageUpSelection },
	{ ACTION_PAGE_DOWN, &Content::actionPageDownSelection },
};

Content::Content(std::string c)
{
	for (auto i : Helper::getFilesInDirectory(c)) {
		if (Helper::insStrCompare(c, i)) {
			this->fileName = i;
			setContent(i);
			return;
		}
	}
	throw std::exception("File not found");
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

void Content::actionDelete(int& posX, int& posY)
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

void Content::actionDeleteWord(int& posX, int& posY)
{
	if (posX == this->content[posY].size()) {
		this->actionDelete(posX, posY);
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

void Content::actionMoveLineUp(int& posX, int& posY)
{
	if (posY > 0) {
		std::string tmp = this->content[posY];
		this->content.erase(this->content.begin() + (posY));
		this->content.insert(this->content.begin() + (posY)-1, tmp);
		(posY)--;
		this->wasEdited = true;
	}
}

void Content::actionMoveLineDown(int& posX, int& posY)
{
	if (posY < this->content.size() - 1) {
		std::string tmp = this->content[posY];
		this->content.erase(this->content.begin() + (posY));
		this->content.insert(this->content.begin() + (posY)+1, tmp);
		(posY)++;
		this->wasEdited = true;
	}
}

void Content::actionEnter(int& posX, int& posY)
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

void Content::actionEnterNoSpacing(int& posX, int& posY)
{
	std::string beforeEnter = this->content[posY].substr(0, posX);
	std::string afterEnter = this->content[posY].substr(posX);
	this->content[posY] = beforeEnter;
	posY += 1;
	this->content.insert(this->content.begin() + posY, afterEnter);
	posX = 0;
	this->wasEdited = true;
}

void Content::actionEnterNewline(int& posX, int& posY)
{
	int spaceCount = 0;
	while (spaceCount < this->content[posY].size() && this->content[posY][spaceCount] == ' ') spaceCount += 2;
	posY += 1;
	this->content.insert(this->content.begin() + posY, std::string(spaceCount, ' '));
	posX = spaceCount;
	this->wasEdited = true;
}

void Content::actionRemove(int& posX, int& posY)
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

void Content::actionRemoveWord(int& posX, int& posY)
{
	if (posX == 0) {
		this->actionRemove(posX, posY);
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

void Content::actionWrite(int& posX, int& posY, char character)
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

void Content::actionLeftKey(int& posX, int& posY)
{
	if (posX > 0) {
		posX -= 1;
	}
	else if (posY > 0) {
		posY -= 1;
		posX = this->content[posY].size();
	}
}

void Content::actionRightKey(int& posX, int& posY)
{
	if (posX < this->content[posY].size()) {
		posX += 1;
	}
	else if (posY < this->content.size() - 1) {
		posY += 1;
		posX = 0;
	}
}

void Content::actionUpKey(int& posX, int& posY)
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

void Content::actionDownKey(int& posX, int& posY)
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

void Content::actionWordRight(int& posX, int& posY)
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

void Content::actionCopyWord(int& posX, int& posY)
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

void Content::actionCopyLine(int& posX, int& posY)
{
	this->commandInfo = this->content[posY];
}

void Content::actionCopyWordBack(int& posX, int& posY)
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

void Content::actionCopySelection(int& posX, int& posY, int& startX, int& startY)
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

void Content::actionPasteSelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionPaste(posX, posY);
}

void Content::actionDeleteSelection(int& posX, int& posY, int& startX, int& startY)
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

void Content::actionLeftKeySelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionLeftKey(posX, posY);
	if (posY < startY) {
		this->state = DEAFULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEAFULT;
	}
}

void Content::actionRightKeySelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionRightKey(posX, posY);
}

void Content::actionUpKeySelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionUpKey(posX, posY);
	if (posY < startY) {
		this->state = DEAFULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEAFULT;
	}
}

void Content::actionDownKeySelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionDownKey(posX, posY);
}

void Content::actionJumpToLineEndSelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionJumpToLineEnd(posX, posY);
}

void Content::actionJumpToLineStartSelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionJumpToLineStart(posX, posY);
	if (posY < startY) {
		this->state = DEAFULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEAFULT;
	}
}

void Content::actionTabifySelection(int& posX, int& posY, int& startX, int& startY)
{
	for (size_t i = startY; i <= posY; i++) {
		this->content[i] = "    " + this->content[i];
	}
	this->wasEdited = true;
}

void Content::actionUntabifySelection(int& posX, int& posY, int& startX, int& startY)
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

void Content::actionWordRightSelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionWordRight(posX, posY);
}

void Content::actionWordLeftSelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionWordLeft(posX, posY);
	if (posY < startY) {
		this->state = DEAFULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEAFULT;
	}
}

void Content::actionMoveLineUpSelection(int& posX, int& posY, int& startX, int& startY)
{
	if (startY == 0) return;
	if (posY == startY) this->actionMoveLineUp(posX, posY);
	std::string tmp = this->content[startY - 1];
	this->content.erase(this->content.begin() + startY - 1);
	this->content.insert(this->content.begin() + posY, tmp);
	startY--;
	posY--;
	this->wasEdited = true;
}

void Content::actionMoveLineDownSelection(int& posX, int& posY, int& startX, int& startY)
{
	if (posY == this->content.size() - 1) return;
	if (posY == startY) this->actionMoveLineDown(posX, posY);

	std::string tmp = this->content[posY + 1];
	this->content.erase(this->content.begin() + posY + 1);
	this->content.insert(this->content.begin() + startY, tmp);
	startY++;
	posY++;
	this->wasEdited = true;
}

void Content::actionSelectLinesSelection(int& posX, int& posY, int& startX, int& startY)
{
	startX = 0;
	posX = this->content[posY].size();
}

void Content::actionPageUpSelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionPageUp(posX, posY);
}

void Content::actionPageDownSelection(int& posX, int& posY, int& startX, int& startY)
{
	this->actionPageDown(posX, posY);
}

void Content::actionDuplicateLine(int& posX, int& posY)
{
	this->content.insert(this->content.begin() + posY, this->content[posY]);
	posY++;
	this->wasEdited = true;
}

void Content::actionJumpToLineEnd(int& posX, int& posY)
{
	posX = this->content[posY].size();
}

void Content::actionJumpToLineStart(int& posX, int& posY)
{
	posX = 0;
}

void Content::actionTabify(int& posX, int& posY)
{
	this->content[posY] = "    " + this->content[posY];
	this->wasEdited = true;
}

void Content::actionUntabify(int& posX, int& posY)
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

void Content::actionPageUp(int& posX, int& posY)
{
	posY = min(posY + PAGE_UP_DOWN_SIZE, this->content.size() - 1);
}

void Content::actionPageDown(int& posX, int& posY)
{
	posY = max(posY - PAGE_UP_DOWN_SIZE, 0);
}

void Content::actionSaveFile(int& posX, int& posY)
{
	Helper::writeFile(this->fileName, this->getContent());
	this->wasEdited = false;
}

void Content::actionWordLeft(int& posX, int& posY)
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

void Content::actionQuit(int& posX, int& posY)
{
	system("cls");
	exit(0);
}

void Content::actionQuitAndSave(int& posX, int& posY)
{
	this->actionSaveFile(posX, posY);
	system("cls");
	exit(0);
}

std::string Content::commandOpen(std::string command, int& posX, int& posY)
{
	*this = Content(command);
	this->wasEdited = true;
	return "Opened " + command;
}

std::string Content::commandFind(std::string command, int& posX, int& posY)
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

std::string Content::commandFindAndReplace(std::string command, int& posX, int& posY)
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
		this->commandFind(beforeTilda, posX, posY);

		this->state = FIND_AND_REPLACE_R;
		this->commandInfo = beforeTilda;
		return "Found.";
	}
	else if (this->state == FIND_AND_REPLACE_F) {
		this->commandFind(this->commandInfo, posX, posY);

		this->state = FIND_AND_REPLACE_R;
		return "Found.";
	}
}

void Content::actionPaste(int& posX, int& posY)
{
	if (!this->commandInfo.size()) return;
	
	for (size_t i = 0; i < this->commandInfo.size(); i++) {
		if (commandInfo[i] == '\n') {
			this->actionEnterNoSpacing(posX, posY);
		}
		else {
			this->actionWrite(posX, posY, commandInfo[i]);
		}
	}
}

void Content::actionDeleteLine(int& posX, int& posY)
{
	if (this->content.size() == 1) {
		this->content[0] = "";
		posX = 0;
	}
	this->commandInfo = this->content[posY] + '\n';
	this->content.erase(this->content.begin() + posY);
	posY = min((int)posY, (int)this->content.size() - 1);
	posX = min((int)posX, (int)this->content[posY].size());
}

std::string Content::runCommand(std::string command, int& posX, int& posY)
{
	command = Helper::trim(command);
	if (command.size() == 0) {
		return "";
	}

	if (Content::instantCommands.count(command)) {
		auto f = Content::instantCommands.find(command);
		(this->*(f->second))(posX, posY);
		return "";
	}

	std::string commandName = command.substr(0, command.find_first_of(' '));
	std::string afterSpace = Helper::trim(command.substr(commandName.size()));


	if (Content::calledCommands.count(commandName)) {
		auto f = Content::calledCommands.find(commandName);
		return (this->*(f->second))(afterSpace, posX, posY);
	}
	return "Command not found";
}
