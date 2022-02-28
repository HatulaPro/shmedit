#include <iostream>
#include <vector>
#include <algorithm>
#include "FileExplorer.h"
#include "../helpers/ConsoleUtils.h"
#include "../helpers/FilesUtil.h"
#include "../helpers/Helper.h"

FileExplorer::FileExplorer(Display& d, std::string path) : Addon(d)
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
	if (offset == 0) {
		std::cout << Helper::colorize(StringsVector(std::string(width, ' ')), StylesVector(Style::FILE_EXPLORER), width);
	}
	else {
		std::cout << Helper::colorize(StringsVector(prePadding, dots, afterDots), StylesVector(Style::FILE_EXPLORER, Style::FILE_EXPLORER, Style::FILE_EXPLORER), width);
	}
	int count = 0;
	for (int i = offset; count <= effectiveHeight; i++) {
		ConsoleUtils::setCursorPosition(left, top + count + 1);
		if (this->files.size()) {

			std::string fileName;
			std::string fileType;
			std::string queriedFileName;
			if (i < this->files.size()) {
				fileName = this->files[i].second;
				fileType = (this->files[i].first ? "d) " : "   ");
			}
			if (this->query.size() && fileName.size()) {
				queriedFileName = fileName.substr(0, this->query.size());
				fileName = fileName.substr(this->query.size());
			}

			std::string afterPadding(width - prePadding.size() - fileName.size() - fileType.size() - queriedFileName.size(), ' ');
			Style fileColor = i == this->activeIndex ? Style::FILE_EXPLORER_ACTIVE : Style::FILE_EXPLORER;

			std::cout << Helper::colorize(StringsVector(prePadding, fileType, queriedFileName, fileName, afterPadding), StylesVector(Style::FILE_EXPLORER, Style::FILE_EXPLORER, Style::FIND_HIGHLIGHTING, fileColor, Style::FILE_EXPLORER), width);
		}
		else if (count == 0) {
			std::string notFoundMessage = this->query.size() ? "No files found for `" + this->query + "`" : "Empty Directory";
			std::string afterPadding(width - prePadding.size() - notFoundMessage.size(), ' ');
			std::cout << Helper::colorize(StringsVector(prePadding, notFoundMessage, afterPadding), StylesVector(Style::FILE_EXPLORER, Style::FILE_EXPLORER, Style::FILE_EXPLORER), width);
		}
		else {
			std::cout << Helper::colorize(StringsVector(std::string(width, ' ')), StylesVector(Style::FILE_EXPLORER), width);
		}
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
		this->display.closeAddon();
	}
	else if (x == ACTION_MOVE_DOWN) {
		this->activeIndex++;
		if (this->activeIndex >= this->files.size()) this->activeIndex = 0;
	}
	else if (x == ACTION_MOVE_UP) {
		this->activeIndex--;
		if (this->activeIndex < 0) this->activeIndex = this->files.size() - 1;
	}
	else if (x == ACTION_MOVE_RIGHT || x == ACTION_NEWLINE) {
		if (this->files.size() && this->files[this->activeIndex].first) {
			this->setCurrentPath(this->currentPath + this->files[this->activeIndex].second);
			this->files = FilesUtil::getDirectoryListings(this->currentPath);
			this->activeIndex = 0;
		}
		else if (this->files.size()) {
			this->display.open(this->currentPath + this->files[this->activeIndex].second);
			this->display.closeAddon();
		}
	}
	else if (x == ACTION_MOVE_LEFT) {
		this->currentPath = this->currentPath.substr(0, this->currentPath.size() - 1);
		std::string lastFileName = this->currentPath.substr(this->currentPath.find_last_of("\\/") + 1);
		this->setCurrentPath(this->currentPath.substr(0, this->currentPath.size() - lastFileName.size() - 1));
		this->files = FilesUtil::getDirectoryListings(this->currentPath);

		for (int i = 0; i < this->files.size(); i++) {
			if (this->files[i].second == lastFileName) this->activeIndex = i;
		}
	}
	else if (Helper::isPrintable(x)) {
		this->query += x;
		if (this->files.size()) {
			std::vector<std::pair<bool, std::string>> filteredFiles;
			std::copy_if(this->files.begin(), this->files.end(), std::back_inserter(filteredFiles), [=](std::pair<bool, std::string>& f) {
				return Helper::insStrCompare(this->query, f.second.substr(0, this->query.size()));
			});
			std::string lastFileName = this->files[activeIndex].second;
			this->files = filteredFiles;
			this->activeIndex = 0;
			for (int i = 0; i < this->files.size(); i++) {
				if (this->files[i].second == lastFileName) this->activeIndex = i;
			}
		}
	}
	else {
		this->query = "";
		this->files = FilesUtil::getDirectoryListings(this->currentPath);
	}
}

AddonType FileExplorer::type()
{
	return AddonType::FileExplorer;
}
