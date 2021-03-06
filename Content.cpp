#include "Content.h"
#include "helpers/Helper.h"
#include "helpers/FilesUtil.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctype.h>
#include <errno.h>
#include <chrono>
#include "Config.h"
#include "helpers/ConsoleUtils.h"



Content::Content(std::string c, Display& d) : display(d)
{
	this->fileName = c;
	for (auto i : FilesUtil::getRealFileName(c)) {
		if (Helper::insStrCompare(this->getFileName(), i)) {
			setContent(i);
			this->wasEdited = false;
			return;
		}
	}
	this->wasEdited = true;
	setContent(c);
}

void Content::setContent(std::string c)
{
	std::string tmp;
	std::stringstream cc(FilesUtil::readFile(c).c_str());
	this->content = std::vector<std::string>();
	while (std::getline(cc, tmp, '\n')) {
		this->content.push_back(tmp);
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
	size_t startIndex = this->fileName.find_last_of("\\/");
	if (startIndex == std::string::npos) {
		return this->fileName;
	}
	else {
		return this->fileName.substr(startIndex + 1);
	}
}

std::string Content::getCommandInfo() const
{
	return this->commandInfo;
}

std::string Content::getCommandArgs(std::string lastKeys) const
{
	if (this->state == DEFAULT) {
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
		return "";
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
		this->setState(DEFAULT);
		lastKeys = "";
		return;
	}

	if (this->getState() == VISUAL) { // Visual mode
		if (x == EXIT_CMD_MODE || x == ACTION_NEWLINE) {
			this->setState(DEFAULT);
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
		else {
			this->state = DEFAULT;
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
		else {
			this->state = DEFAULT;
		}
		return;
	}
	if (this->getState() == COMMAND) { // Command mode
		// Leave cmd mode
		if (lastKeys.size() == 0 && x == EXIT_CMD_MODE) {
			this->setState(DEFAULT);
		}
		else if (x == ACTION_NEWLINE) {
			this->setState(DEFAULT);
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
	if (this->state == DEFAULT) {
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
	if (this->posX < this->content[this->posY].size()) {
		this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				this->posX,
				this->posY,
				-1, -1,
				std::string(1, this->content[this->posY][this->posX])
			});

		this->content[this->posY] = this->content[this->posY].substr(0, this->posX) + this->content[this->posY].substr(this->posX + 1);
		this->wasEdited = true;


		return;
	}
	else if (this->posY + 1 < this->content.size()) {

		this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				this->posX,
				this->posY,
				-1, -1,
				"\n"
			});

		this->content[this->posY] += this->content[this->posY + 1];
		this->content.erase(this->content.begin() + this->posY + 1);
		this->wasEdited = true;
		return;
	}
}

void Content::actionDeleteWord()
{
	if (this->posX == this->content[this->posY].size()) {
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

	this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				this->posX,
				this->posY,
				-1, -1,
				this->commandInfo
		});

	this->content[posY].erase(posX, count);
	this->wasEdited = true;
}

void Content::actionMoveLineUp()
{
	if (this->posY > 0) {
		std::string tmp = this->content[this->posY];
		this->content.erase(this->content.begin() + this->posY);

		this->history.push(HistoryItem{
				HistoryAction::LINES_UP,
				0,
				this->posY - 1,
				-1, -1,
				""
			});

		this->content.insert(this->content.begin() + this->posY - 1, tmp);
		this->posY--;
		this->wasEdited = true;
	}
}

void Content::actionMoveLineDown()
{
	if (this->posY < this->content.size() - 1) {
		std::string tmp = this->content[this->posY];

		this->history.push(HistoryItem{
				HistoryAction::LINES_DOWN,
				0,
				this->posY + 1,
				-1, -1,
				""
			});

		this->content.erase(this->content.begin() + this->posY);
		this->content.insert(this->content.begin() + this->posY + 1, tmp);
		this->posY++;
		this->wasEdited = true;
	}
}

void Content::actionEnter()
{
	std::string beforeEnter = this->content[this->posY].substr(0, this->posX);
	int spaceCount = 0;
	while (spaceCount < beforeEnter.size() && beforeEnter[spaceCount] == ' ') spaceCount += 2;
	std::string afterEnter = this->content[this->posY].substr(this->posX);
	this->content[this->posY] = beforeEnter;
	std::string spaces(spaceCount, ' ');
	this->history.push(HistoryItem{
				HistoryAction::WRITE,
				this->posX,
				this->posY,
				-1, -1,
				'\n' + spaces
		});

	this->posY += 1;
	this->content.insert(this->content.begin() + this->posY, spaces + afterEnter);
	this->posX = spaceCount;
	this->wasEdited = true;
}

void Content::actionEnterNoSpacing()
{
	std::string beforeEnter = this->content[this->posY].substr(0, this->posX);
	std::string afterEnter = this->content[this->posY].substr(this->posX);
	this->content[this->posY] = beforeEnter;

	this->history.push(HistoryItem{
				HistoryAction::WRITE,
				this->posX,
				this->posY,
				-1, -1,
				"\n"
		});

	this->posY += 1;
	this->content.insert(this->content.begin() + this->posY, afterEnter);
	this->posX = 0;
	this->wasEdited = true;
}

void Content::actionEnterNewline()
{
	int spaceCount = 0;
	std::string spaces;
	if (Config::settings["USE_TABS"]) {
		while (spaceCount < this->content[this->posY].size() && this->content[this->posY][spaceCount] == '\t') spaceCount++;
		spaces = std::string(spaceCount, '\t');
	}
	else {
		while (spaceCount < this->content[this->posY].size() && this->content[this->posY][spaceCount] == ' ') spaceCount += Config::settings["TAB_SIZE"];
		spaces = std::string(spaceCount, ' ');
	}

	this->history.push(HistoryItem{
				HistoryAction::WRITE,
				(int)this->content[this->posY].size(),
				this->posY,
				-1, -1,
				"\n" + spaces
		});
	this->posY += 1;
	this->posX = spaceCount;

	this->content.insert(this->content.begin() + this->posY, spaces);
	this->wasEdited = true;
}

void Content::actionRemove()
{
	if (this->posX > 0) {
		std::string beforeRemove = this->content[this->posY].substr(0, this->posX - 1);
		std::string afterRemove = this->content[this->posY].substr(this->posX);

		this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				this->posX - 1,
				this->posY,
				-1, -1,
				std::string(1, this->content[this->posY][this->posX - 1])
			});


		this->content[this->posY] = beforeRemove + afterRemove;
		this->posX -= 1;
		this->wasEdited = true;
	}
	else if (this->posY > 0) {
		this->posX = this->content[this->posY - 1].size();
		this->content[this->posY - 1] += this->content[this->posY];


		this->content.erase(this->content.begin() + this->posY);
		this->posY -= 1;

		this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				this->posX,
				this->posY,
				-1, -1,
				"\n"
			});
		this->wasEdited = true;
	}
}

void Content::actionRemoveWord()
{
	if (this->posX == 0) {
		this->actionRemove();
		return;
	}
	bool firstType = Helper::isAlphanumeric(this->content[this->posY][this->posX - 1]);
	int count = 0;
	while (this->posX - count >= 1 && Helper::isAlphanumeric(this->content[this->posY][this->posX - count - 1]) == firstType) {
		count++;
	}
	this->commandInfo = this->content[this->posY].substr(this->posX - count, count);
	this->content[this->posY].erase(this->posX - count, count);
	this->posX -= count;

	this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				this->posX,
				this->posY,
				-1, -1,
				this->commandInfo
		});

	this->wasEdited = true;
}

void Content::actionWrite(char character)
{
	this->content[posY].insert(this->content[posY].begin() + posX, character);


	this->history.push(HistoryItem{
				HistoryAction::WRITE,
				this->posX,
				this->posY,
				-1, -1,
				std::string(1, character)
		});
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

void Content::actionUndo()
{
	if (this->history.empty()) return;
	HistoryItem lastAction = this->history.top();
	this->history.pop();

	if (lastAction.action == HistoryAction::WRITE) {
		while (true) {
			size_t count = 0;
			for (char ch : lastAction.op) {
				if (ch == '\n') {
					this->content[lastAction.posY] = this->content[lastAction.posY].substr(0, lastAction.posX) + this->content[lastAction.posY + 1];
					this->content.erase(this->content.begin() + lastAction.posY + 1);
					count = 0;
				}
				else {
					count++;
				}
			}
			this->content[lastAction.posY].erase(lastAction.posX, count);
			this->posX = lastAction.posX;
			this->posY = lastAction.posY;

			if (this->history.empty()) break;
			lastAction = this->history.top();
			if (lastAction.action == HistoryAction::WRITE && lastAction.posX == this->posX - 1 && lastAction.posY == this->posY && lastAction.op != " ") {
				this->history.pop();
			}
			else {
				break;
			}
		}

	}
	else if (lastAction.action == HistoryAction::REMOVE) {
		do {
			this->posX = lastAction.posX;
			this->posY = lastAction.posY;
			std::string tmp = this->commandInfo;
			this->commandInfo = lastAction.op;
			this->actionPaste();
			this->history.pop();
			this->commandInfo = tmp;

			if (this->history.empty()) break;

			lastAction = this->history.top();
			if (lastAction.action == HistoryAction::REMOVE && (lastAction.posX == this->posX || lastAction.posX - this->posX == -1) && lastAction.posY == lastAction.posY && lastAction.op.find(" ") == std::string::npos) {
				this->history.pop();
			}
			else {
				break;
			}

		} while (true);
	}
	else if (lastAction.action == HistoryAction::LINES_UP) {
		this->posY = lastAction.posY;
		this->posX = lastAction.posX;
		if (lastAction.startY != -1) {
			this->startY = lastAction.startY;
			this->startX = lastAction.startX;
			this->actionMoveLineDownSelection();
		}
		else {
			this->actionMoveLineDown();
		}
		this->history.pop();
	}
	else if (lastAction.action == HistoryAction::LINES_DOWN) {
		this->posY = lastAction.posY;
		this->posX = lastAction.posX;
		if (lastAction.startY != -1) {
			this->startY = lastAction.startY;
			this->startX = lastAction.startX;
			this->actionMoveLineUpSelection();
		}
		else {
			this->actionMoveLineUp();
		}
		this->history.pop();
	}
	else if (lastAction.action == HistoryAction::TABIFY) {
		this->posX = lastAction.posX;
		this->posY = lastAction.posY;
		if (lastAction.startX != -1) {
			this->startX = lastAction.startX;
			this->startY = lastAction.startY;
			this->actionUntabifySelection();
		}
		else {
			this->actionUntabify();
		}
		this->history.pop();
	}
	else if (lastAction.action == HistoryAction::UNTABIFY) {
		this->posX = lastAction.posX;
		this->posY = lastAction.posY;
		if (lastAction.startX != -1) {
			this->startX = lastAction.startX;
			this->startY = lastAction.startY;
			this->actionTabifySelection();
		}
		else {
			this->actionTabify();
		}
		this->history.pop();
	}
	else {
		throw std::exception("Unreachable");
	}
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

void Content::actionOpenCompleteWord()
{
	this->display.openWordCompleter();
}

void Content::actionCopySelection()
{
	if (this->posY == this->startY) {
		this->commandInfo = this->content[this->posY].substr(this->startX, this->posX - this->startX);
		return;
	}
	this->commandInfo = this->content[this->startY].substr(this->startX) + '\n';
	for (size_t i = this->startY + 1; i < this->posY; i++) {
		this->commandInfo += this->content[i] + '\n';
	}
	this->commandInfo += this->content[this->posY].substr(0, this->posX);
}

void Content::actionPasteSelection()
{
	this->actionPaste();
}

void Content::actionDeleteSelection()
{
	if (this->posY == startY) {
		std::string tmp = this->content[this->posY].substr(this->startX, this->posX - this->startX);
		this->content[this->posY].erase(this->startX, this->posX - this->startX);
		if (this->startX == this->content[this->posY].size()) this->startX--;

		this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				this->startX,
				this->posY,
				-1, -1,
				tmp
			});

		this->posX = this->startX + 1;
		this->wasEdited = true;
		return;
	}

	std::string toDelete = this->content[this->startY].substr(this->startX) + '\n';
	for (size_t i = this->startY + 1; i < this->posY; i++) {
		toDelete += this->content[i] + '\n';
	}
	toDelete += this->content[this->posY].substr(0, this->posX);

	this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				this->startX,
				this->startY,
				-1, -1,
				toDelete
		});

	this->content[this->startY] = this->content[this->startY].substr(0, this->startX) + this->content[this->posY].substr(this->posX);
	if (this->startY + 1 < this->posY) {
		this->content.erase(this->content.begin() + this->startY + 1, this->content.begin() + this->posY + 1);
	}
	else {
		this->content.erase(this->content.begin() + this->posY);
	}
	this->posY = this->startY;
	if (this->startX == this->content[this->posY].size() && this->startX > 0) this->startX--;
	this->posX = this->startX + 1;
	this->wasEdited = true;
}

void Content::actionLeftKeySelection()
{
	this->actionLeftKey();
	if (posY < startY) {
		this->state = DEFAULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEFAULT;
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
		this->state = DEFAULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEFAULT;
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
		this->state = DEFAULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEFAULT;
	}
}

void Content::actionTabifySelection()
{
	for (size_t i = startY; i <= posY; i++) {
		if (Config::settings["USE_TABS"]) {
			this->content[i] = '\t' + this->content[i];
		}
		else {
			this->content[i] = std::string(Config::settings["TAB_SIZE"], ' ') + this->content[i];
		}
	}
	this->history.push(HistoryItem{
				HistoryAction::TABIFY,
				this->posX,
				this->posY,
				this->startX,
				this->startY,
				""
		});

	this->wasEdited = true;
}

void Content::actionUntabifySelection()
{
	this->history.push(HistoryItem{
				HistoryAction::UNTABIFY,
				this->posX,
				this->posY,
				this->startX,
				this->startY,
				""
		});
	for (size_t i = this->startY; i <= this->posY; i++) {
		if (this->content[i].size() > 0 && this->content[i][0] == '\t') {
			this->content[i] = this->content[i].substr(1);
			this->wasEdited = true;
			continue;
		}
		size_t count = 0;

		while (count < this->content[i].size() && isspace(this->content[i][count]) && count < Config::settings["TAB_SIZE"]) count++;

		if (count > 0) {
			if (i == this->startY) {
				this->startX = max(this->startX - (int)count, 0);
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
		this->state = DEFAULT;
	}
	else if (posY == startY && posX < startX) {
		this->state = DEFAULT;
	}
}

void Content::actionMoveLineUpSelection()
{
	if (this->startY == 0) return;


	std::string tmp = this->content[this->startY - 1];
	this->content.erase(this->content.begin() + this->startY - 1);
	this->content.insert(this->content.begin() + this->posY, tmp);
	this->startY--;
	this->posY--;
	
	this->history.push(HistoryItem{
				HistoryAction::LINES_UP,
				this->posX,
				this->posY,
				this->startX,
				this->startY,
				""
		});
	this->wasEdited = true;
}

void Content::actionMoveLineDownSelection()
{
	if (this->posY == this->content.size() - 1) return;


	std::string tmp = this->content[this->posY + 1];
	this->content.erase(this->content.begin() + this->posY + 1);
	this->content.insert(this->content.begin() + this->startY, tmp);
	this->startY++;
	this->posY++;
	this->history.push(HistoryItem{
				HistoryAction::LINES_DOWN,
				this->posX,
				this->posY,
				this->startX,
				this->startY,
				""
		});
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
	this->content.insert(this->content.begin() + this->posY, this->content[this->posY]);

	this->history.push(HistoryItem{
				HistoryAction::WRITE,
				0,
				this->posY,
				-1, -1,
				this->content[this->posY] + '\n'
		});

	this->posY++;
	this->wasEdited = true;
}

void Content::actionJumpToLineEnd()
{
	this->posX = this->content[this->posY].size();
}

void Content::actionJumpToLineStart()
{
	int initial = this->posX;
	this->posX = 0;
	while (this->posX < this->content[this->posY].size() && isspace(this->content[this->posY][this->posX])) this->posX++;
	if (initial <= this->posX) this->posX = 0;
}

void Content::actionTabify()
{
	std::string spaces = Config::settings["USE_TABS"] ? "\t" : "    ";
	this->content[this->posY] = spaces + this->content[this->posY];
	this->posX += spaces.size();
	this->history.push(HistoryItem{
				HistoryAction::TABIFY,
				this->posX,
				this->posY,
				-1, -1,
				spaces
		});

	this->wasEdited = true;
}

void Content::actionUntabify()
{
	this->history.push(HistoryItem{
				HistoryAction::UNTABIFY,
				this->posX,
				this->posY,
				-1, -1,
				""
		});
	if (this->content[this->posY].size() > 0 && this->content[this->posY][0] == '\t') {
		this->content[this->posY] = this->content[this->posY].substr(1);
		if (this->posX > 0) this->posX -= 1;
		this->wasEdited = true;
		return;
	}
	size_t count = 0;
	while (count < this->content[this->posY].size() && isspace(this->content[this->posY][count]) && count < Config::settings["TAB_SIZE"]) count++;

	if (count > 0) {
		this->posX = max(this->posX - (int)count, 0);
		this->content[this->posY] = this->content[posY].substr(count);
		this->wasEdited = true;
		return;
	}
}

void Content::actionPageUp()
{
	this->posY = max(this->posY - PAGE_UP_DOWN_SIZE, 0);
	this->posX = min(this->content[this->posY].size(), this->posX);
}

void Content::actionPageDown()
{
	this->posY = min(this->posY + PAGE_UP_DOWN_SIZE, this->content.size() - 1);
	this->posX = min(this->content[this->posY].size(), this->posX);
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
	return "Opened " + command;
}

std::string Content::commandExit(std::string command)
{
	this->display.closeAll();
	throw std::exception("Unreachable");
}

std::string Content::commandSaveAndExit(std::string command)
{
	this->display.saveAll();
	this->display.closeAll();
	throw std::exception("Unreachable");
}

std::string Content::commandRenameFile(std::string command)
{
	char err[256] = {};
	command = Helper::trim(command);
	if (rename(this->fileName.c_str(), command.c_str()) == 0) {
		this->fileName = command;
		this->actionSaveFile();
		return "File name changed";
	}
	else {
		strerror_s(err, errno);
		return std::string("Error: ") + err;
	}
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

	if (command[0] == '^') {
		// Look up:
		std::string lookingFor = command.substr(1);
		size_t found = this->content[posY].substr(0, posX).rfind(lookingFor);

		if (found != std::string::npos) {
			posX = found;
			return "Found in " + std::to_string(posY) + ':' + std::to_string(posX);
		}

		for (int i = this->content.size() - 1; i >= 0; i--) {
			size_t index = (posY + i) % this->content.size();
			found = this->content[index].rfind(lookingFor);
			if (found != std::string::npos) {
				posX = found;
				posY = index;
				return "Found in " + std::to_string(posY) + ':' + std::to_string(posX);
			}
		}
	}
	else {
		// Look down:
		if (posX < this->content[posY].size()) {
			size_t found = this->content[posY].substr(posX + 1).find(command);
			if (found != std::string::npos) {
				posX += found + 1;
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
	}

	this->state = DEFAULT;
	return "String not found.";
}

std::string Content::commandFindAndReplace(std::string command)
{
	if (this->state == FIND_AND_REPLACE_R) {
		std::string beforeTilda = this->commandInfo;
		size_t beforeTildaSize = beforeTilda.size();
		if (beforeTildaSize && beforeTilda[0] == '^') beforeTildaSize--;
		std::string afterTilda = this->commandInfo2;

		this->content[posY].erase(posX, beforeTildaSize);
		this->content[posY].insert(posX, afterTilda);
		this->state = FIND_AND_REPLACE_F;
		return "Found.";
	}
	else if (this->state == DEFAULT) {
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

std::string Content::commandOpenFileExplorer(std::string command)
{
	this->display.openFileExplorer();
	return "Opened.";
}

void Content::actionPaste()
{
	if (!this->commandInfo.size()) return;

	this->history.push(HistoryItem{
				HistoryAction::WRITE,
				this->posX,
				this->posY,
				-1, -1,
				this->commandInfo
		});

	bool gotSlashR = false;
	for (size_t i = 0; i < this->commandInfo.size(); i++) {
		if (this->commandInfo[i] == '\r') {
			continue;
		}
		else if (this->commandInfo[i] == '\n') {
			this->actionEnterNoSpacing();
			this->history.pop();
		}
		else {
			this->actionWrite(this->commandInfo[i]);
			this->history.pop();
		}
	}
}

void Content::actionDeleteLine()
{
	this->wasEdited = true;
	this->history.push(HistoryItem{
				HistoryAction::REMOVE,
				0,
				this->posY,
				-1, -1,
				this->content[this->posY] + '\n'
		});
	if (this->content.size() == 1) {
		this->content[0] = "";
		this->posX = 0;
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
