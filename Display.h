#include <iostream>
#include <vector>
#include "Content.h"


class Display {
private:
	Content c;
	int posX;
	int posY;

	std::string commandOutput;
	std::string lastKeys;
	int cmdCount = 0;

	void showTopBar(short width, bool wasEdited) const;
public:
	Display(std::string fname);
	void show() const;
	std::string padToLine(std::string line, short width) const;
	void callAction(char x);


};