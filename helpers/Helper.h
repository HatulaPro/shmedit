#include "../Config.h"

#include <string>
#include <vector>
#include "Windows.h"

#define StringsVector(...) std::vector<std::string>{__VA_ARGS__}
#define StylesVector(...) std::vector<Style>{__VA_ARGS__}

enum class Style {
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
	DIMMED,
	FILE_EXPLORER,
	FILE_EXPLORER_ACTIVE,
};


class Helper {
public:
	
	static std::string colorize(std::string text, Style style);
	static std::string colorize(std::vector<std::string> text, std::vector<Style> styles, short width);
	static std::string setCursor(std::string line, int x);
	static size_t getDisplayLength(std::string str, size_t begin = 0, size_t end = std::string::npos);
	static size_t getDisplayIndex(std::string str, size_t index);
	static std::string replace(std::string subject, const std::string& search, const std::string& replace);
	static std::string getTimeString();
	static bool isPrintable(char c);
	static bool isAlphanumeric(char c);
	static std::string trim(std::string s);
	static bool insStrCompare(std::string a, std::string b);

	static int getNextKey();
};