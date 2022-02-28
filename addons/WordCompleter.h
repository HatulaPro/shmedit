#pragma once
#include <string>
#include <vector>
#include "Addon.h"

class Display;

class WordCompleter : public Addon {

private:
	std::vector<std::string> words;
	
	std::vector<std::string> getWords(std::string prefix) const;

public:
	WordCompleter(Display& d);

	void show(int left, int top, int width, int height);
	void callAction(int x, std::string& lastKeys, std::string& commandOutput);
	AddonType type();
};