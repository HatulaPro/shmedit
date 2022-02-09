#include <iostream>
#include <vector>
#include "Content.h"
#include "Windows.h"


class Display {
private:
	Content c;
	
	int posX;
	int posY;

	int startX = -1;
	int startY = -1;

	std::string commandOutput;
	std::string lastKeys;
	int cmdCount = 0;

	void showTopBar(short width, bool wasEdited) const;

	HANDLE hConsole;
	COORD cursorPosition;
public:
	Display(std::string fname);
	void show() const;
	std::string padToLine(std::string line, short width) const;
	void callAction(int x);


};