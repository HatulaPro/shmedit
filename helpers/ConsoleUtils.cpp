#include "ConsoleUtils.h"

void ConsoleUtils::hideCursor(HANDLE hConsole)
{
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = false;
	lpCursor.dwSize = 100;
	SetConsoleCursorInfo(hConsole, &lpCursor);
}
void ConsoleUtils::getCursorPosition(HANDLE hConsole, COORD& pcoord)
{
	CONSOLE_SCREEN_BUFFER_INFO  info;
	GetConsoleScreenBufferInfo(hConsole, &info);
	pcoord = info.dwCursorPosition;
}
void ConsoleUtils::setCursorPosition(HANDLE hConsole, COORD coord)
{
	SetConsoleCursorPosition(hConsole, coord);
}


void ConsoleUtils::getTerminalSize(HANDLE hConsole, short* x, short* y)
{
	// Only works on windows!
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(hConsole, &csbi);
	*x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
