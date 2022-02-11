#pragma once
#include <iostream>
#include <vector>
#include "Windows.h"

class Content;

class Display {
private:
	std::vector<Content*> contents;
	size_t activeContent = 0;

	std::string commandOutput;
	std::string lastKeys;

	void showTopBar(short width, bool wasEdited) const;

	HANDLE hConsole;
	COORD cursorPosition;
public:
	Display(std::string fname);
	~Display();

	void open(std::string fname);
	void openNext();
	void openPrev();
	void closeActiveContent();

	void show() const;
	void callAction(int x);


};