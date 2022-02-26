#include "ConsoleUtils.h"
#include <iostream>

void ConsoleUtils::hideCursor()
{
	std::cout << "\033[?25l";
}
void ConsoleUtils::getCursorPosition(HANDLE hConsole, COORD& pcoord)
{
	CONSOLE_SCREEN_BUFFER_INFO  info;
	GetConsoleScreenBufferInfo(hConsole, &info);
	pcoord = info.dwCursorPosition;
}
void ConsoleUtils::setConsoleName(std::string consoleName)
{
	SetConsoleTitleA(consoleName.c_str());
}
void ConsoleUtils::setCursorPosition(int x, int y)
{
	std::cout << "\033[" << y << ';' << x << 'H';
}


void ConsoleUtils::getTerminalSize(HANDLE hConsole, short* x, short* y)
{
	// Only works on windows!
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(hConsole, &csbi);
	*x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

std::string ConsoleUtils::getClipboardText()
{
	if (!OpenClipboard(nullptr)) return "";

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr) return "";

	char* pszText = static_cast<char*>(GlobalLock(hData));
	if (pszText == nullptr) return "";

	std::string text(pszText);

	GlobalUnlock(hData);
	CloseClipboard();
	
	return text;
}

