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
	//Display d = Display("mytest.txt");
	while (true) {
		d.show();
		d.callAction(_getch()); 
		//std::this_thread::sleep_for(std::chrono::milliseconds(200));
		//std::cout << _getch() << std::endl;
	}

	return 0;
}