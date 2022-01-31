#include <fstream>
#include <string>


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
	LINE_NUMBER,
};

class Helper {
public:
	static void hideCursor();
	static std::string readFile(std::string fileName);
	static void writeFile(std::string fileName, std::string content);
	static std::string colorize(std::string text, int style);
	static std::string setCursor(std::string line, int x);
	static size_t getDisplayLength(std::string str);
	static size_t getDisplayIndex(std::string str, size_t index);
	static void getTerminalSize(short* x, short* y);
	static std::string replace(std::string subject, const std::string& search, const std::string& replace);
};