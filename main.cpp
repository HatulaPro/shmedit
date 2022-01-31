#include "Display.h"
#include "Helper.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <conio.h>

int main() {
	Helper::hideCursor();
	Display d = Display("README.md");
	while (true) {
		d.show();
		std::this_thread::sleep_for(std::chrono::milliseconds(17));
		d.callAction(_getch());
		//std::cout << _getch();
	}

	return 0;
}