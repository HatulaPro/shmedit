#include "Content.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctype.h>
#include <chrono>

Content::Content(std::string c)
{
	setContent(c);
}

void Content::setContent(std::string c)
{
	std::string tmp;
	std::stringstream cc(Helper::readFile(c).c_str());
	this->content = std::vector<std::string>();
	while (std::getline(cc, tmp, '\n')) {
		this->content.push_back(Helper::replace(tmp, "\t", "  "));
	}
}

std::vector<std::string> Content::getLines() const
{
	return this->content;
}

std::string Content::getLine(size_t index) const
{
	return this->content[index];
}

size_t Content::size() const
{
	return this->content.size();
}

std::string Content::getContent() const
{
	std::string result;
	for (auto i = this->content.begin(); i != this->content.end(); i++) {
		result += *i + '\n';
	}
	return result;
}

bool Content::actionDelete(int* posX, int* posY)
{
	if (*posX < this->content[*posY].size()) {
		this->content[*posY] = this->content[*posY].substr(0, *posX) + this->content[*posY].substr(*posX + 1);
		return true;
	}
	else if (*posY + 1 < this->content.size()) {
		this->content[*posY] += this->content[*posY + 1];
		this->content.erase(this->content.begin() + (*posY) + 1);
		return true;
	}
	return false;
}

bool Content::actionMoveLineUp(int* posX, int* posY)
{
	if (*posY > 0) {
		std::string tmp = this->content[*posY];
		this->content.erase(this->content.begin() + (*posY));
		this->content.insert(this->content.begin() + (*posY) - 1, tmp);
		(*posY)--;
		return true;
	}
	return false;
}

bool Content::actionMoveLineDown(int* posX, int* posY)
{
	if (*posY < this->content.size() - 1) {
		std::string tmp = this->content[*posY];
		this->content.erase(this->content.begin() + (*posY));
		this->content.insert(this->content.begin() + (*posY) + 1, tmp);
		(*posY)++;
		return true;
	}
}

bool Content::actionEnter(int* posX, int* posY)
{
	std::string beforeEnter = this->content[*posY].substr(0, *posX);
	int spaceCount = 0;
	while (spaceCount < beforeEnter.size() && beforeEnter[spaceCount] == ' ') spaceCount += 2;
	std::string afterEnter = this->content[*posY].substr(*posX);
	this->content[*posY] = beforeEnter;
	*posY += 1;
	this->content.insert(this->content.begin() + *posY, std::string(spaceCount, ' ') + afterEnter);
	*posX = spaceCount;
	return true;
}

bool Content::actionRemove(int* posX, int* posY)
{
	if (*posX > 0) {
		std::string beforeRemove = this->content[*posY].substr(0, *posX - 1);
		std::string afterRemove = this->content[*posY].substr(*posX);
		this->content[*posY] = beforeRemove + afterRemove;
		*posX -= 1;
		return true;
	}
	else if (*posY > 0) {
		this->content[*posY - 1] += this->content[*posY];
		this->content.erase(this->content.begin() + *posY);
		*posX = this->content[*posY - 1].size();
		*posY -= 1;
		return true;
	}
}

bool Content::actionWrite(int* posX, int* posY, char character)
{
	if (character == '\t') {
		for (int i = 0; i < TAB_SIZE; i++) {
			this->content[*posY].insert(this->content[*posY].begin() + *posX, ' ');
		}
		*posX += TAB_SIZE;
		return true;
	}
	this->content[*posY].insert(this->content[*posY].begin() + *posX, character);
	*posX += 1;
	return true;
}
