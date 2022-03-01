#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include "Addon.h"

class Display;

class WordCompleter : public Addon {

private:
	std::unordered_set<std::string> words;
	std::vector<std::string> activeWords;

	std::string prefix;
	size_t index = 0;
	bool isEndOfWord = false;

	std::vector<std::string> getWords(std::string prefix) const;
	std::string getActiveWord(size_t index) const;
public:
	WordCompleter(Display& d);

	void show(int left, int top, int width, int height);
	void callAction(int x, std::string& lastKeys, std::string& commandOutput);
	AddonType type();
};