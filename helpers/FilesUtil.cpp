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

std::vector<std::string> FilesUtil::getFilesInDirectory(std::string dir)
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
