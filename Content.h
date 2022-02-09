#pragma once
#include <vector>
#include <string>
#include <map>
#include "KeyBinds.h"
#include <stack>
#define TAB_SIZE 4

enum STATE
{
	DEAFULT,
	COMMAND,
	FIND,
	FIND_AND_REPLACE_F,
	FIND_AND_REPLACE_R,
	VISUAL,
};


class Content {
private:
	std::vector<std::string> content;
	std::string fileName;
	bool wasEdited = false;

	int state = DEAFULT;

	std::string commandInfo;
	std::string commandInfo2;

public:
	Content(std::string c);
	void setContent(std::string c);
	std::vector<std::string> getLines() const;
	std::string getLine(size_t index) const;
	size_t size() const;
	std::string getContent() const;
	std::string getFileName() const;
	std::string getCommandInfo() const;
	std::string getCommandArgs(std::string lastKeys) const;

	int getState() const;
	bool isInFindState() const;
	std::string getStateString() const;
	void setState(int state);
	bool getEditStatus() const;

	void actionDelete(int& posX, int& posY);
	void actionDeleteWord(int& posX, int& posY);
	void actionMoveLineUp(int& posX, int& posY);
	void actionMoveLineDown(int& posX, int& posY);
	void actionEnter(int& posX, int& posY);
	void actionEnterNoSpacing(int& posX, int& posY);
	void actionEnterNewline(int& posX, int& posY);
	void actionRemove(int& posX, int& posY);
	void actionRemoveWord(int& posX, int& posY);
	void actionWrite(int& posX, int& posY, char character);
	void actionLeftKey(int& posX, int& posY);
	void actionRightKey(int& posX, int& posY);
	void actionUpKey(int& posX, int& posY);
	void actionDownKey(int& posX, int& posY);
	void actionWordRight(int& posX, int& posY);
	void actionWordLeft(int& posX, int& posY);
	void actionDuplicateLine(int& posX, int& posY);
	void actionJumpToLineEnd(int& posX, int& posY);
	void actionJumpToLineStart(int& posX, int& posY);
	void actionTabify(int& posX, int& posY);
	void actionUntabify(int& posX, int& posY);
	void actionPageUp(int& posX, int& posY);
	void actionPageDown(int& posX, int& posY);

	void actionSaveFile(int& posX, int& posY);


	void actionQuit(int& posX, int& posY);
	void actionQuitAndSave(int& posX, int& posY);
	void actionPaste(int& posX, int& posY);
	void actionDeleteLine(int& posX, int& posY);

	void actionCopyWord(int& posX, int& posY);
	void actionCopyLine(int& posX, int& posY);
	void actionCopyWordBack(int& posX, int& posY);

	void actionCopySelection(int& posX, int& posY, int& startX, int& startY);
	void actionPasteSelection(int& posX, int& posY, int& startX, int& startY);
	void actionDeleteSelection(int& posX, int& posY, int& startX, int& startY);
	void actionLeftKeySelection(int& posX, int& posY, int& startX, int& startY);
	void actionRightKeySelection(int& posX, int& posY, int& startX, int& startY);
	void actionUpKeySelection(int& posX, int& posY, int& startX, int& startY);
	void actionDownKeySelection(int& posX, int& posY, int& startX, int& startY);
	void actionJumpToLineEndSelection(int& posX, int& posY, int& startX, int& startY);
	void actionJumpToLineStartSelection(int& posX, int& posY, int& startX, int& startY);
	void actionTabifySelection(int& posX, int& posY, int& startX, int& startY);
	void actionUntabifySelection(int& posX, int& posY, int& startX, int& startY);
	void actionWordRightSelection(int& posX, int& posY, int& startX, int& startY);
	void actionWordLeftSelection(int& posX, int& posY, int& startX, int& startY);
	void actionMoveLineUpSelection(int& posX, int& posY, int& startX, int& startY);
	void actionMoveLineDownSelection(int& posX, int& posY, int& startX, int& startY);
	void actionSelectLinesSelection(int& posX, int& posY, int& startX, int& startY);
	void actionPageUpSelection(int& posX, int& posY, int& startX, int& startY);
	void actionPageDownSelection(int& posX, int& posY, int& startX, int& startY);


	std::string commandOpen(std::string command, int& posX, int& posY);
	std::string commandFind(std::string command, int& posX, int& posY);
	std::string commandFindAndReplace(std::string command, int& posX, int& posY);
	std::string runCommand(std::string command, int& posX, int& posY);


	
	//{ACTION_CTRL_S, Content::actionSave},
};