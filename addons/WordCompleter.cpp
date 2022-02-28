#include "WordCompleter.h"
#include "../helpers/Helper.h"
#include "../helpers/ConsoleUtils.h"



std::vector<std::string> WordCompleter::getWords(std::string prefix) const
{
	std::vector<std::string> res;
	for (auto word : this->words) {
		if (prefix.size() <= word.size() && Helper::insStrCompare(word.substr(0, prefix.size()), prefix)) {
			res.push_back(word);
		}
	}
	return res;
}

WordCompleter::WordCompleter(Display& d) : Addon(d)
{
	this->words.push_back("hello");
	this->words.push_back("help me");
	this->words.push_back("test");
	this->words.push_back("12346");
}

void WordCompleter::show(int left, int top, int width, int height)
{
	std::vector<std::string> words = this->getWords("he");
	for (size_t i = 0; i < min(words.size(), height); i++) {
		ConsoleUtils::setCursorPosition(left, top + i);
		std::cout << Helper::colorize(StringsVector(std::string(3, ' '), words[i], std::string(12 - words[i].size(), ' ')), StylesVector(Style::FILE_EXPLORER, Style::FILE_EXPLORER, Style::FILE_EXPLORER), 15);
	}
}

void WordCompleter::callAction(int x, std::string& lastKeys, std::string& commandOutput)
{
	this->display.closeAddon();
}

AddonType WordCompleter::type()
{
	return AddonType::WordCompleter;
}
