#pragma once
#include <vector>
#include <string>
#include <map>
#include "KeyBinds.h"
#define TAB_SIZE 4

class Content {
private:
	std::vector<std::string> content;
public:
	Content(std::string c);
	void setContent(std::string c);
	std::vector<std::string> getLines() const;
	std::string getLine(size_t index) const;
	size_t size() const;
	std::string getContent() const;

	bool actionDelete(int& posX, int& posY);
	bool actionDeleteWord(int& posX, int& posY);
	bool actionMoveLineUp(int& posX, int& posY);
	bool actionMoveLineDown(int& posX, int& posY);
	bool actionEnter(int& posX, int& posY);
	bool actionEnterNewline(int& posX, int& posY);
	bool actionRemove(int& posX, int& posY);
	bool actionRemoveWord(int& posX, int& posY);
	bool actionWrite(int& posX, int& posY, char character);
	bool actionLeftKey(int& posX, int& posY);
	bool actionRightKey(int& posX, int& posY);
	bool actionUpKey(int& posX, int& posY);
	bool actionDownKey(int& posX, int& posY);
	bool actionWordRight(int& posX, int& posY);
	bool actionWordLeft(int& posX, int& posY);


	static const std::map<char, bool (Content::*)(int&, int&)> utilActions; 
	static const std::map<char, bool (Content::*)(int&, int&)> oneClickActions; 
	//{ACTION_CTRL_S, Content::actionSave},
};