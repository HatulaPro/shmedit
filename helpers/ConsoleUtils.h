#include <string>
#include <vector>
#include "Windows.h"

class ConsoleUtils {
public:
	static void hideCursor();
	static void getCursorPosition(HANDLE hConsole, COORD& pcoord);
	static void setConsoleName(std::string consoleName);
	static void setCursorPosition(int x, int y);
	static void getTerminalSize(HANDLE hConsole, short* x, short* y);
	static std::string getClipboardText();
};