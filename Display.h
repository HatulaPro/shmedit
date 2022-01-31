#include <iostream>
#include <vector>
#include "Content.h"

enum STATE
{
	DEAFULT,
	FIND,
};


class Display {
private:
	Content c;
	int posX;
	int posY;
	std::string lastKeys;
	std::string fileName;
	bool wasEdited = false;
	int state = DEAFULT;

	void showTopBar(short width) const;
public:
	Display(std::string fname);
	void show() const;
	std::string padToLine(std::string line, short width) const;
	void callAction(char x);
};