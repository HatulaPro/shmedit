#pragma once
#include "../Display.h"

class Display;

enum class AddonType {
	FileExplorer,
	WordCompleter,
};

class Addon
{
protected:
	Display& display;
public:
	Addon(Display& d);
	virtual void show(int left, int top, int width, int height) = 0;
	virtual void callAction(int x, std::string& lastKeys, std::string& commandOutput) = 0;
	virtual AddonType type() = 0;
};

