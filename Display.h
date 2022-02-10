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

	void showTopBar(short width, bool wasEdited) const;

	HANDLE hConsole;
	COORD cursorPosition;
public:
	Display(std::string fname);
	void show() const;
	void callAction(int x);


};