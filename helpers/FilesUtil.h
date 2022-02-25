#pragma once
#include <string>
#include <vector>

class FilesUtil {
public:
	static std::string getFullPath(std::string fileName);
	static std::vector<std::string> getRealFileName(std::string dir);
	static std::vector<std::pair<bool, std::string>> getDirectoryListings(std::string dir);
	static std::string getDirectoryName(std::string fileName);
	static std::string readFile(std::string fileName);
	static void writeFile(std::string fileName, std::string content);
};