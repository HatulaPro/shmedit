#include <sstream>
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

std::string WordCompleter::getActiveWord(size_t index) const
{
	return this->activeWords[index % this->activeWords.size()];
}

WordCompleter::WordCompleter(Display& d) : Addon(d)
{
	Content* c = this->display.getActiveContent();
	int posX, posY, startX, startY;
	c->getCursorPositions(posX, posY, startX, startY);

	std::string line = c->getLine(posY);
	if (line[posX] == ' ' || line.size() == posX) {
		this->isEndOfWord = true;
		posX--;
	}
	while (posX > 0 && Helper::isAlphanumeric(line[posX])) posX--;
	std::stringstream s(line.substr(posX));
	s >> this->prefix;
	while (this->prefix.size() > 1 && !Helper::isAlphanumeric(this->prefix[this->prefix.size() - 1])) this->prefix = this->prefix.substr(0, this->prefix.size() - 1);
	if (!this->prefix.size() || !Helper::isAlphanumeric(this->prefix[0])) {
		throw std::exception("No word to complete");
	}
	std::string tmp;
	for (auto line : c->getLines()) {
		s = std::stringstream(line);
		while (s >> tmp) {
			while (tmp.size() > 1 && !Helper::isAlphanumeric(tmp[tmp.size() - 1])) tmp = tmp.substr(0, tmp.size() - 1);
			this->words.insert(tmp);
		}
	}

	this->activeWords = this->getWords(this->prefix);

	if (!this->activeWords.size()) throw std::exception("No words");
}

void WordCompleter::show(int left, int top, int width, int height)
{
	height = min(this->activeWords.size(), height);
	std::string background(18, ' ');
	for (size_t i = 0; i < height; i++) {
		Style wordStyle = i == 0 ? Style::FILE_EXPLORER_ACTIVE : Style::FILE_EXPLORER;

		ConsoleUtils::setCursorPosition(left, top + i);
		std::cout << Helper::colorize(StringsVector(background), StylesVector(wordStyle), background.size());
		ConsoleUtils::setCursorPosition(left + 1, top + i);
		std::string word = this->getActiveWord(i + this->index);
		if (word.size() > background.size() - 2) word = word.substr(0, background.size() - 4) + "...";
		std::cout << Helper::colorize(StringsVector(word), StylesVector(wordStyle), word.size());
	}
}

void WordCompleter::callAction(int x, std::string& lastKeys, std::string& commandOutput)
{
	if (x == ACTION_MOVE_UP) {
		if (this->index == 0) {
			this->index = this->activeWords.size() - 1;
		}
		else {
			this->index--;
		}
	}
	else if (x == ACTION_MOVE_DOWN) {
		this->index++;
	}
	else if (x == ACTION_NEWLINE) {
		std::string toComplete = this->getActiveWord(this->index).substr(this->prefix.size());
		this->display.getActiveContent()->actionWordRight();
		if (this->isEndOfWord) {
			this->display.getActiveContent()->actionLeftKey();
		}
		for (char c : toComplete) {
			this->display.getActiveContent()->actionWrite(c);
		}
		this->display.closeAddon();
	}
	else {
		this->display.closeAddon();
	}
}

AddonType WordCompleter::type()
{
	return AddonType::WordCompleter;
}
