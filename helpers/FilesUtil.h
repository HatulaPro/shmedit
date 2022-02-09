#pragma once
#include <string>
#include <vector>

class FilesUtil {
public:
	static std::string getFileName(std::string fileName);
	static std::vector<std::string> getFilesInDirectory(std::string dir);
	static std::string readFile(std::string fileName);
	static void writeFile(std::string fileName, std::string content);
};