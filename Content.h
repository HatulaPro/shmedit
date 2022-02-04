#pragma once
#include <vector>
#include <string>
#include <map>
#include "KeyBinds.h"
#define TAB_SIZE 4

class Content {
private:
	std::vector<std::string> content;
	std::string fileName;
	bool wasEdited = false;

	std::string commandInfo;
public:
	Content(std::string c);
	void setContent(std::string c);
	std::vector<std::string> getLines() const;
	std::string getLine(size_t index) const;
	size_t size() const;
	std::string getContent() const;
	std::string getFileName() const;
	bool getEditStatus() const;

	void actionDelete(int& posX, int& posY);
	void actionDeleteWord(int& posX, int& posY);
	void actionMoveLineUp(int& posX, int& posY);
	void actionMoveLineDown(int& posX, int& posY);
	void actionEnter(int& posX, int& posY);
	void actionEnterNewline(int& posX, int& posY);
	void actionRemove(int& posX, int& posY);
	void actionRemoveWord(int& posX, int& posY);
	void actionWrite(int& posX, int& posY, char character);
	void actionLeftKey(int& posX, int& posY);
	void actionRightKey(int& posX, int& posY);
	void actionUpKey(int& posX, int& posY);
	void actionDownKey(int& posX, int& posY);
	void actionWordRight(int& posX, int& posY);
	void actionCopyLine(int& posX, int& posY);
	void actionJumpToLineEnd(int& posX, int& posY);
	void actionJumpToLineStart(int& posX, int& posY);

	void actionSaveFile(int& posX, int& posY);
	void actionWordLeft(int& posX, int& posY);

	std::string commandOpen(std::string command, int& posX, int& posY);

	void actionQuit(int& posX, int& posY);
	void actionQuitAndSave(int& posX, int& posY);
	void actionPaste(int& posX, int& posY);
	void actionDeleteLine(int& posX, int& posY);

	std::string runCommand(std::string command, int& posX, int& posY);


	static const std::map<char, void (Content::*)(int&, int&)> utilActions; 
	static const std::map<char, void (Content::*)(int&, int&)> oneClickActions; 

	static const std::map<std::string, void(Content::*)(int&, int&)> instantCommands;
	static const std::map<std::string, std::string (Content::*)(std::string, int&, int&)> calledCommands; 
	//{ACTION_CTRL_S, Content::actionSave},
};