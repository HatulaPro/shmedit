#include <iostream>
#include <vector>
#include "Content.h"

enum STATE
{
	DEAFULT,
	COMMAND,
};


class Display {
private:
	Content c;
	int posX;
	int posY;

	std::string commandOutput;
	std::string lastKeys;
	int state = DEAFULT;

	void showTopBar(short width, bool wasEdited) const;
public:
	Display(std::string fname);
	void show() const;
	std::string padToLine(std::string line, short width) const;
	void callAction(char x);


};