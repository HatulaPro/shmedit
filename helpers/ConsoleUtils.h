#include <string>
#include <vector>
#include "Windows.h"

class ConsoleUtils {
public:
	static void hideCursor(HANDLE hConsole);
	static void getCursorPosition(HANDLE hConsole, COORD& pcoord);
	static void setCursorPosition(HANDLE hConsole, COORD coord);
	static void getTerminalSize(HANDLE hConsole, short* x, short* y);
	static std::string getClipboardText();
};