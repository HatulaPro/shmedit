#pragma once
#include <vector>
#include <string>
#include <stack>
#include <map>
#include "KeyBinds.h"
#include "Display.h"

enum STATE
{
	DEFAULT,
	COMMAND,
	FIND,
	FIND_AND_REPLACE_F,
	FIND_AND_REPLACE_R,
	VISUAL,
};

enum class HistoryAction {
	WRITE,
	PASTE,
	REMOVE,
	LINES_UP,
	LINES_DOWN,
	TABIFY,
	UNTABIFY,
};

struct HistoryItem {
	HistoryAction action;
	int posX;
	int posY;
	int startX;
	int startY;
	std::string op;
};

class Content {
private:
	Display& display;
	std::vector<std::string> content;
	std::stack<HistoryItem> history;

	std::string fileName;
	bool wasEdited = false;

	int state = DEFAULT;

	std::string commandInfo;
	std::string commandInfo2;

	int posX;
	int posY;
	int startX;
	int startY;
public:
	Content(std::string c, Display& d);
	void setContent(std::string c);
	std::vector<std::string> getLines() const;
	std::string getLine(size_t index) const;
	size_t size() const;
	std::string getContent() const;
	std::string getFileName() const;
	std::string getCommandInfo() const;
	std::string getCommandArgs(std::string lastKeys) const;

	void callAction(int x, std::string& lastKeys, std::string& commandOutput);

	void getCursorPositions(int& posX, int& posY, int& startX, int& startY) const;

	int getState() const;
	bool isInFindState() const;
	std::string getStateString() const;
	void setState(int state);
	bool getEditStatus() const;

	// One click actions:
	void actionWrite(char character);
	void actionDelete();
	void actionDeleteWord();
	void actionMoveLineUp();
	void actionMoveLineDown();
	void actionEnter();
	void actionEnterNoSpacing(); // TODO: Will there be somthing weird when pasting?
	void actionEnterNewline();
	void actionRemove();
	void actionRemoveWord();
	void actionLeftKey();
	void actionRightKey();
	void actionUpKey();
	void actionDownKey();
	void actionWordRight();
	void actionWordLeft();
	void actionDuplicateLine();
	void actionJumpToLineEnd();
	void actionJumpToLineStart();

	void actionTabify();
	void actionUntabify();
	void actionPageUp();
	void actionPageDown();
	void actionSaveFile();
	void actionQuit();
	void actionQuitAndSave();
	void actionPaste();
	void actionDeleteLine();
	void actionCopyWord();
	void actionCopyLine();
	void actionCopyWordBack();
	void actionPasteFromClipboard();
	void actionUndo();

	void openNextFile();
	void openPreviousFile();

	// Visual Commands:
	void actionCopySelection();
	void actionPasteSelection();
	void actionDeleteSelection();
	void actionLeftKeySelection();
	void actionRightKeySelection();
	void actionUpKeySelection();
	void actionDownKeySelection();
	void actionJumpToLineEndSelection();
	void actionJumpToLineStartSelection();
	void actionTabifySelection();
	void actionUntabifySelection();
	void actionWordRightSelection();
	void actionWordLeftSelection();
	void actionMoveLineUpSelection();
	void actionMoveLineDownSelection();
	void actionSelectLinesSelection();
	void actionPageUpSelection();
	void actionPageDownSelection();

	// Commands:
	std::string commandOpen(std::string command);
	std::string commandExit(std::string command);
	std::string commandSaveAndExit(std::string command);
	std::string commandRenameFile(std::string command);
	std::string commandFind(std::string command);
	std::string commandFindAndReplace(std::string command);
	std::string commandOpenFileExplorer(std::string command);
	std::string runCommand(std::string command);
};