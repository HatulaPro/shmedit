#pragma once
#include <string>

class Display;

class FileExplorer {
private:
	Display& display;
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