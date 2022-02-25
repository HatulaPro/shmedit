#include <iostream>
#include <vector>
#include "FileExplorer.h"
#include "../helpers/ConsoleUtils.h"
#include "../helpers/FilesUtil.h"
#include "../helpers/Helper.h"

FileExplorer::FileExplorer(Display& d, std::string path) : display(d)
{
	this->setCurrentPath(path);
	this->activeIndex = 0;
	this->files = FilesUtil::getDirectoryListings(this->currentPath);
}

void FileExplorer::setCurrentPath(std::string path)
{
	this->currentPath = path;
	if (this->currentPath[this->currentPath.size() - 1] != '\\' && this->currentPath[this->currentPath.size() - 1] != '/') {
		this->currentPath += '\\';
	}
}

void FileExplorer::show(int left, int top, int width, int height)
{
	std::string prePadding(20, ' ');
	std::string dots = "   ...";
	std::string afterDots(width - prePadding.size() - dots.size(), ' ');
	ConsoleUtils::setCursorPosition(left, top);
	int effectiveHeight = height - 2;
	
	int offset = max(0, this->activeIndex - effectiveHeight + 1);
	if(offset == 0){
		std::cout << Helper::colorize(StringsVector(std::string(width, ' ')), StylesVector(Style::FILE_EXPLORER), width);
	}
	else {
		std::cout << Helper::colorize(StringsVector(prePadding, dots, afterDots), StylesVector(Style::FILE_EXPLORER, Style::FILE_EXPLORER, Style::FILE_EXPLORER), width);
	}
	int count = 0;
	for (int i = offset; count <= effectiveHeight; i++) {
		ConsoleUtils::setCursorPosition(left, top + count + 1);

		std::string fileName;
		if (i < this->files.size()) fileName = (this->files[i].first ? "d) " : "   ") + this->files[i].second;
		std::string afterPadding(width - prePadding.size() - fileName.size(), ' ');
		Style fileColor = i == this->activeIndex ? Style::FILE_EXPLORER_ACTIVE : Style::FILE_EXPLORER;
		std::cout << Helper::colorize(StringsVector(prePadding, fileName, afterPadding), StylesVector(Style::FILE_EXPLORER, fileColor, Style::FILE_EXPLORER), width);

		count++;
	}
	ConsoleUtils::setCursorPosition(left, top + height - 1);
	if (this->files.size() <= effectiveHeight || this->activeIndex == this->files.size() - 1) {
		std::cout << Helper::colorize(StringsVector(std::string(width, ' ')), StylesVector(Style::FILE_EXPLORER), width);
	}
	else {
		std::cout << Helper::colorize(StringsVector(prePadding, dots, afterDots), StylesVector(Style::FILE_EXPLORER, Style::FILE_EXPLORER, Style::FILE_EXPLORER), width);
	}
}

void FileExplorer::callAction(int x, std::string& lastKeys, std::string& commandOutput)
{
	if (x == ACTION_START_COMMAND) {
		this->display.closeFileExplorer();
	}
	else if (x == ACTION_MOVE_DOWN) {
		this->activeIndex++;
		if(this->activeIndex >= this->files.size()) this->activeIndex = 0;
	}
	else if (x == ACTION_MOVE_UP) {
		this->activeIndex--;
		if(this->activeIndex < 0) this->activeIndex = this->files.size() - 1;
	}
	else if (x == ACTION_MOVE_RIGHT) {
		if (this->files[this->activeIndex].first) {
			this->setCurrentPath(this->currentPath + this->files[this->activeIndex].second);
			this->files = FilesUtil::getDirectoryListings(this->currentPath);
			this->activeIndex = 0;
		}
	}
	else if (x == ACTION_MOVE_LEFT) {
		this->currentPath = this->currentPath.substr(0, this->currentPath.size() - 1);
		this->setCurrentPath(this->currentPath.substr(0, this->currentPath.find_last_of("\\/")));
        this->files = FilesUtil::getDirectoryListings(this->currentPath);
        this->activeIndex = 0;
	}
}
