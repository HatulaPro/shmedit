#include <fstream>
#include <string>
#include <vector>
#include "Windows.h"

enum Style {
	RESET,
	UNDERLINE,
	BLACK,
	WHITE,
	RED,
	BLUE,
	MAGENTA,
	CURSOR,
	BACKGROUND,
	VISUAL_STYLE,
	FIND_HIGHLIGHTING,
	LINE_NUMBER,
};

typedef struct Colorizer {
	size_t begin;
	size_t count;
	Style style;
} Colorizer;

typedef std::vector<Colorizer> Colorizers;

class Helper {
public:
	static void hideCursor(HANDLE hConsole);
	static void getCursorPosition(HANDLE hConsole, COORD& pcoord);
	static void setCursorPosition(HANDLE hConsole, COORD coord);
	static void getTerminalSize(HANDLE hConsole, short* x, short* y);
	static std::string readFile(std::string fileName);
	static void writeFile(std::string fileName, std::string content);
	static std::string colorize(std::string text, int style);
	static std::string colorize(std::string text, Colorizers colorizers);
	static std::string setCursor(std::string line, int x);
	static size_t getDisplayLength(std::string str, size_t begin = 0, size_t end = std::string::npos);
	static size_t getDisplayIndex(std::string str, size_t index);
	static std::string replace(std::string subject, const std::string& search, const std::string& replace);
	static std::string getTimeString();
	static bool isPrintable(char c);
	static bool isAlphanumeric(char c);
	static std::string trim(std::string s);
	static std::string getFileName(std::string fileName);
	static std::vector<std::string> getFilesInDirectory(std::string dir);
	static bool insStrCompare(std::string a, std::string b);
};