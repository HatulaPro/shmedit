#include <Windows.h>
#include "Helper.h"


void Helper::hideCursor()
{
    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = false;
    lpCursor.dwSize = 50;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &lpCursor);
}

std::string Helper::readFile(std::string fileName)
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

void Helper::writeFile(std::string fileName, std::string content)
{
    std::ofstream f(fileName);
    f << content;
    f.close();
}

std::string Helper::colorize(std::string text, int style)
{
    switch (style) {
    case RESET:
        return "\033[0m" + text + "\033[0m";
    case UNDERLINE:
        return "\033[4m" + text + "\033[0m";
    case BLACK:
        return "\033[30m" + text + "\033[0m";
    case WHITE:
        return "\033[37m" + text + "\033[0m";
    case RED:
        return "\033[31m" + text + "\033[0m";
    case BLUE:
        return "\033[34m" + text + "\033[0m";
    case MAGENTA:
        return "\033[35m" + text + "\033[0m";
    case CURSOR:
        return "\033[42m" + text + "\033[0m";
    case BACKGROUND:
        return "\033[43m" + text + "\033[0m";
    case LINE_NUMBER:
        return "\033[46m\033[34m" + text + "\033[0m";
    }
}

std::string Helper::setCursor(std::string line, int x)
{
    if (x > line.size()) {
        return line;
    }
    if (line.size() == 0) {
        return Helper::colorize(" ", CURSOR);
    }
    if (x == 0) {
        return Helper::colorize(line.substr(0, 1), CURSOR) + line.substr(1);
    }
    if (x == line.size()) {
        return line + Helper::colorize(" ", CURSOR);
    }
    return line.substr(0, x) + Helper::colorize(line.substr(x, 1), CURSOR) + line.substr(x + 1);
}


size_t Helper::getDisplayLength(std::string str)
{
    size_t s = 0;
    bool inStyle = false;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '\033') {
            inStyle = true;
        } else if (inStyle && str[i] == 'm') {
            inStyle = false;
        }
        else if (!inStyle) {
            s += 1;
        }
    }
    return s;
}

size_t Helper::getDisplayIndex(std::string str, size_t index)
{
    if (index >= str.size()) {
        throw std::exception("Invalid index");
    }
    size_t s = 0;
    bool inStyle = false;
    for (size_t i = 0; i < str.size(); i++) {
        if (i == index) {
            return s;
        }
        if (str[i] == '\033') {
            inStyle = true;
        }
        else if (inStyle && str[i] == 'm') {
            inStyle = false;
        }
        else if (!inStyle) {
            s += 1;
        }
    }
    return s;
}

void Helper::getTerminalSize(short* x, short* y)
{
    // Only works on windows!
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

std::string Helper::replace(std::string subject, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}
