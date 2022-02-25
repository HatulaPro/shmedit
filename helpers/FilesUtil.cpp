#include "FilesUtil.h"
#include <iostream>
#include <istream>
#include <fstream>
#include "Windows.h"


std::string FilesUtil::readFile(std::string fileName)
{
	std::string content;
	std::string tmp;
	std::ifstream f(fileName);
	while (getline(f, tmp)) {
		content += tmp + '\n';
	}
	f.close();
	return content;
}

void FilesUtil::writeFile(std::string fileName, std::string content)
{
	std::ofstream f(fileName);
	f << content;
	f.close();
}

std::string FilesUtil::getFullPath(std::string fileName)
{
	char filename[] = "test.txt";
	char fullFileName[MAX_PATH];

	GetFullPathNameA(fileName.c_str(), MAX_PATH, fullFileName, nullptr);
	return fullFileName;
}

std::vector<std::string> FilesUtil::getRealFileName(std::string dir)
{
	std::vector<std::wstring> names;
	std::wstring search_path(dir.begin(), dir.end());
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			//if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			names.push_back(fd.cFileName);
			//}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	std::vector<std::string> res;
	for (auto i : names) {
		res.push_back(std::string(i.begin(), i.end()));
	}
	return res;
}

std::vector<std::pair<bool, std::string>> FilesUtil::getDirectoryListings(std::string directory)
{
	HANDLE dir;
	WIN32_FIND_DATAA file_data;
	
	std::vector<std::pair<bool, std::string>> result;

	if ((dir = FindFirstFileA((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return result; 

	do {
		std::string fileName = file_data.cFileName;
		std::string fullFileName = directory + "/" + fileName;
		bool isDirectory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (fileName[0] == '.')
			continue;

		result.push_back({ isDirectory, fileName });
	} while (FindNextFileA(dir, &file_data));

	FindClose(dir);
	return result;
}

std::string FilesUtil::getDirectoryName(std::string fileName)
{
	fileName = FilesUtil::getFullPath(fileName);
	const size_t last_slash_idx = fileName.rfind('\\');
	if (std::string::npos != last_slash_idx)
	{
		return fileName.substr(0, last_slash_idx);
	}

	throw std::exception("Can't get directory name");
}
