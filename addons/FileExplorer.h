#pragma once
#include <string>
#include "Addon.h"

class Display;

class FileExplorer : public Addon {
private:
	std::string currentPath;
	std::string query;

	int activeIndex;
	std::vector<std::pair<bool, std::string>> files;
public:
	FileExplorer(Display& d, std::string path);

	void setCurrentPath(std::string path);
	void show(int left, int top, int width, int height);
	void callAction(int x, std::string& lastKeys, std::string& commandOutput);
};