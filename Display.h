#pragma once
#include <iostream>
#include <vector>
#include "Windows.h"

class Content;
class FileExplorer;

class Display {
private:
	std::vector<Content*> contents;
	size_t activeContent = 0;

	std::string commandOutput;
	std::string lastKeys;

	FileExplorer* fileExplorer;

	void showTopBar(short width, bool wasEdited) const;

	HANDLE hConsole;
	COORD cursorPosition;
public:
	Display(std::string fname);
	~Display();

	void openFileExplorer();
	void closeFileExplorer();
	void open(std::string fname);
	void openNext();
	void openPrev();
	void closeActiveContent();
	void closeAll();
	void saveAll();

	void show() const;
	void callAction(int x);


};